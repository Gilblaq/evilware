# EVILWARE Subscription API

This service is the authority for customer accounts and Premium Workspace entitlements. The desktop application must never decide premium access from a local flag.

## Deploy on Render
Create a PostgreSQL database and a Node web service using this folder as the service root. Build command: `npm install`. Start command: `npm start`. Use Node 20+.

Configure environment variables in the hosting dashboard (never commit secrets):
- `DATABASE_URL`: Render PostgreSQL internal connection string
- `JWT_SECRET`: long random secret (at least 32 random bytes)
- `PAYSTACK_SECRET_KEY`: Paystack secret key
- `OWNER_EMAIL`: your account email; register this email to receive owner role
- `MONTHLY_PRICE_GHS`: monthly price in GHS
- `ANNUAL_PRICE_GHS`: annual price in GHS
- `PAYSTACK_CALLBACK_URL`: optional return URL
- `ALLOWED_ORIGINS`: optional comma-separated allowed web origins

In Paystack, configure the webhook URL as `https://YOUR-API/api/payments/paystack-webhook`. Keep the secret key only in Render environment variables. Paystack verifies the payment; the API checks amount, currency, user and reference before granting 30-day or 365-day access.

## Important launch checklist
1. Set prices and owner email.
2. Deploy and confirm `/api/health`.
3. Configure the Paystack webhook and complete a low-value live/test-mode transaction.
4. Test registration, login, successful payment, duplicate webhook delivery, expiry and owner revocation.
5. Add account recovery, email verification, abuse monitoring, database backups and production rate limiting before a public paid launch.

The API is an initial production-oriented foundation, not a claim of payment certification. Access is granted only after server-side Paystack verification. Mobile money checkout is one-time; customers renew when their access expires. No recurring charge is initiated without a separate explicit recurring-payment implementation.
