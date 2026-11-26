import express from "express";
import helmet from "helmet";
import cors from "cors";
import pg from "pg";
import bcrypt from "bcryptjs";
import jwt from "jsonwebtoken";
import crypto from "node:crypto";
import rateLimit from "express-rate-limit";

const { Pool } = pg;
const app = express();
const port = Number(process.env.PORT || 3000);
const pool = new Pool({ connectionString: process.env.DATABASE_URL, ssl: process.env.DATABASE_URL?.includes("render.com") ? { rejectUnauthorized: false } : undefined });
const required = ["DATABASE_URL", "JWT_SECRET", "PAYSTACK_SECRET_KEY", "OWNER_EMAIL"];
for (const key of required) if (!process.env[key]) console.warn(`Missing environment variable: ${key}`);

app.use(helmet());
app.use(cors({ origin: process.env.ALLOWED_ORIGINS ? process.env.ALLOWED_ORIGINS.split(",").map(x => x.trim()) : true }));
app.use(express.json({ limit: "32kb", verify: (req, _res, buf) => { req.rawBody = Buffer.from(buf); } }));

await pool.query(`
CREATE TABLE IF NOT EXISTS users (
 id BIGSERIAL PRIMARY KEY, email TEXT UNIQUE NOT NULL, password_hash TEXT NOT NULL,
 role TEXT NOT NULL DEFAULT 'customer', created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
CREATE TABLE IF NOT EXISTS subscriptions (
 id BIGSERIAL PRIMARY KEY, user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
 plan TEXT NOT NULL, status TEXT NOT NULL, starts_at TIMESTAMPTZ NOT NULL,
 expires_at TIMESTAMPTZ NOT NULL, payment_reference TEXT UNIQUE NOT NULL,
 created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
`);

const authLimiter = rateLimit({ windowMs: 15 * 60 * 1000, limit: 12, standardHeaders: "draft-7", legacyHeaders: false });
const checkoutLimiter = rateLimit({ windowMs: 60 * 60 * 1000, limit: 6, standardHeaders: "draft-7", legacyHeaders: false });
const tokenFor = user => jwt.sign({ sub: String(user.id), role: user.role }, process.env.JWT_SECRET, { expiresIn: "7d" });
async function auth(req, res, next) {
 try {
  const token = (req.headers.authorization || "").replace(/^Bearer\s+/i, "");
  const claims = jwt.verify(token, process.env.JWT_SECRET);
  const result = await pool.query("SELECT id,email,role FROM users WHERE id=$1", [claims.sub]);
  if (!result.rowCount) return res.status(401).json({ error: "Account not found" });
  req.user = result.rows[0]; next();
 } catch { res.status(401).json({ error: "Please sign in again" }); }
}
const owner = (req,res,next) => req.user?.role === "owner" ? next() : res.status(403).json({error:"Owner access required"});
async function entitlement(userId) {
 const r = await pool.query("SELECT plan,status,expires_at FROM subscriptions WHERE user_id=$1 AND status='active' AND expires_at>NOW() ORDER BY expires_at DESC LIMIT 1",[userId]);
 return r.rows[0] || null;
}
app.get("/api/health", (_req,res) => res.json({ ok:true, service:"EVILWARE Subscription API" }));

app.post("/api/auth/register", async (req,res) => {
 try {
  const email=String(req.body.email||"").trim().toLowerCase(), password=String(req.body.password||"");
  if (!/^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email) || password.length<10) return res.status(400).json({error:"Use a valid email and a password of at least 10 characters"});
  const role=email===String(process.env.OWNER_EMAIL).trim().toLowerCase()?"owner":"customer";
  const hash=await bcrypt.hash(password,12);
  const r=await pool.query("INSERT INTO users(email,password_hash,role) VALUES($1,$2,$3) RETURNING id,email,role",[email,hash,role]);
  const user=r.rows[0]; res.status(201).json({token:tokenFor(user),user,subscription:await entitlement(user.id)});
 } catch(e) { if(e.code==="23505") return res.status(409).json({error:"An account with that email already exists"}); console.error(e); res.status(500).json({error:"Could not create account"}); }
});
app.post("/api/auth/login", async (req,res) => {
 const email=String(req.body.email||"").trim().toLowerCase(), password=String(req.body.password||"");
 const r=await pool.query("SELECT id,email,role,password_hash FROM users WHERE email=$1",[email]);
 if(!r.rowCount || !(await bcrypt.compare(password,r.rows[0].password_hash))) return res.status(401).json({error:"Email or password is incorrect"});
 const {password_hash,...user}=r.rows[0]; res.json({token:tokenFor(user),user,subscription:await entitlement(user.id)});
});
app.get("/api/me",auth,async(req,res)=>res.json({user:req.user,subscription:await entitlement(req.user.id)}));
app.get("/api/premium/access",auth,async(req,res)=>{
 const subscription=await entitlement(req.user.id);
 const allowed=req.user.role==="owner"||Boolean(subscription);
 res.json({allowed,role:req.user.role,subscription});
});

app.post("/api/billing/checkout",auth,checkoutLimiter,async(req,res)=>{
 try {
  const plan=req.body.plan==="annual"?"annual":"monthly";
  const amount=Number(plan==="annual"?process.env.ANNUAL_PRICE_GHS:process.env.MONTHLY_PRICE_GHS);
  if(!Number.isFinite(amount)||amount<=0) return res.status(503).json({error:"Subscription pricing is not configured yet"});
  const reference="ew_"+crypto.randomBytes(16).toString("hex");
  const response=await fetch("https://api.paystack.co/transaction/initialize",{method:"POST",headers:{Authorization:`Bearer ${process.env.PAYSTACK_SECRET_KEY}`,"Content-Type":"application/json"},body:JSON.stringify({email:req.user.email,amount:Math.round(amount*100),currency:"GHS",reference,callback_url:process.env.PAYSTACK_CALLBACK_URL,metadata:{user_id:String(req.user.id),plan,product:"EVILWARE Premium Workspace"}})});
  const data=await response.json();
  if(!response.ok||!data.status) return res.status(502).json({error:"Payment provider could not start checkout"});
  res.json({authorization_url:data.data.authorization_url,reference,plan,amount});
 } catch(e){console.error(e);res.status(502).json({error:"Payment service unavailable"});}
});
async function activatePayment(reference) {
 const verify=await fetch(`https://api.paystack.co/transaction/verify/${encodeURIComponent(reference)}`,{headers:{Authorization:`Bearer ${process.env.PAYSTACK_SECRET_KEY}`}});
 const result=await verify.json();
 if(!verify.ok||!result.status||result.data.status!=="success"||result.data.currency!=="GHS") return false;
 const meta=result.data.metadata||{}, userId=Number(meta.user_id), plan=meta.plan==="annual"?"annual":"monthly";
 const expected=Number(plan==="annual"?process.env.ANNUAL_PRICE_GHS:process.env.MONTHLY_PRICE_GHS)*100;
 if(!userId||!Number.isFinite(expected)||Number(result.data.amount)!==Math.round(expected)) return false;
 await pool.query(`INSERT INTO subscriptions(user_id,plan,status,starts_at,expires_at,payment_reference)
 VALUES($1,$2,'active',
 GREATEST(NOW(),COALESCE((SELECT MAX(expires_at) FROM subscriptions WHERE user_id=$1 AND status='active' AND expires_at>NOW()),NOW())),
 GREATEST(NOW(),COALESCE((SELECT MAX(expires_at) FROM subscriptions WHERE user_id=$1 AND status='active' AND expires_at>NOW()),NOW()))+
 CASE WHEN $2='annual' THEN INTERVAL '365 days' ELSE INTERVAL '30 days' END,$3)
 ON CONFLICT(payment_reference) DO NOTHING`,[userId,plan,reference]);
 return true;
}
app.post("/api/billing/verify/:reference",auth,async(req,res)=>{
 try {
  const r=await fetch(`https://api.paystack.co/transaction/verify/${encodeURIComponent(req.params.reference)}`,{headers:{Authorization:`Bearer ${process.env.PAYSTACK_SECRET_KEY}`}});
  const data=await r.json();
  if(!data.status||Number(data.data?.metadata?.user_id)!==Number(req.user.id)) return res.status(400).json({error:"Payment could not be verified for this account"});
  const ok=await activatePayment(req.params.reference);
  if(!ok) return res.status(402).json({error:"Payment is not confirmed yet"});
  res.json({subscription:await entitlement(req.user.id)});
 } catch(e){console.error(e);res.status(502).json({error:"Could not verify payment"});}
});
app.post("/api/payments/paystack-webhook",async(req,res)=>{
 const signature=req.headers["x-paystack-signature"];
 const raw=req.rawBody||Buffer.from("");
 const expected=crypto.createHmac("sha512",process.env.PAYSTACK_SECRET_KEY).update(raw).digest("hex");
 const supplied=Buffer.from(String(signature||""));
 const calculated=Buffer.from(expected);
 if(supplied.length!==calculated.length||!crypto.timingSafeEqual(supplied,calculated)) return res.sendStatus(401);
 const event=req.body;
 if(event.event==="charge.success"&&event.data?.reference) { try { await activatePayment(event.data.reference); } catch(e){console.error(e);return res.sendStatus(500);} }
 res.sendStatus(200);
});
app.get("/api/admin/customers",auth,owner,async(_req,res)=>{
 const r=await pool.query(`SELECT u.id,u.email,u.created_at,s.plan,s.status,s.expires_at
 FROM users u LEFT JOIN LATERAL (SELECT plan,status,expires_at FROM subscriptions WHERE user_id=u.id ORDER BY expires_at DESC LIMIT 1) s ON true
 WHERE u.role='customer' ORDER BY u.created_at DESC`);
 res.json({customers:r.rows});
});
app.post("/api/admin/customers/:id/revoke",auth,owner,async(req,res)=>{
 await pool.query("UPDATE subscriptions SET status='revoked' WHERE user_id=$1 AND status='active'",[req.params.id]);
 res.json({ok:true});
});
app.listen(port,()=>console.log(`EVILWARE Subscription API listening on ${port}`));
