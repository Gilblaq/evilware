#!/usr/bin/env python3
"""Generate EVILWARE's original offline artwork and multi-size Windows icon.

Uses only Python's standard library so clean Windows CI and local builds need no
image-processing package. Outputs PNG files plus a Windows ICO containing
multiple PNG-backed resolutions.
"""
from __future__ import annotations
import math, os, struct, sys, zlib

OUT = os.path.abspath(sys.argv[1] if len(sys.argv) > 1 else "assets")
os.makedirs(OUT, exist_ok=True)

def png(path, w, h, pixels):
    raw = bytearray()
    for y in range(h):
        raw.append(0)
        for x in range(w):
            raw.extend(pixels[y*w+x])
    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag+data) & 0xffffffff)
    data = b"\x89PNG\r\n\x1a\n"
    data += chunk(b"IHDR", struct.pack(">2I5B", w, h, 8, 6, 0, 0, 0))
    data += chunk(b"IDAT", zlib.compress(bytes(raw), 9))
    data += chunk(b"IEND", b"")
    with open(path, "wb") as f: f.write(data)

def blend(dst, src, a):
    return tuple(max(0, min(255, int(dst[i]*(1-a)+src[i]*a))) for i in range(3)) + (255,)

def line(buf, w, h, x0, y0, x1, y1, color, width=2):
    dx,dy=x1-x0,y1-y0
    steps=max(int(math.ceil(abs(dx))),int(math.ceil(abs(dy))),1)
    r=max(0,width//2)
    for i in range(steps+1):
        x=round(x0+dx*i/steps); y=round(y0+dy*i/steps)
        for yy in range(y-r,y+r+1):
            for xx in range(x-r,x+r+1):
                if 0<=xx<w and 0<=yy<h and (xx-x)**2+(yy-y)**2 <= (r+0.8)**2:
                    buf[yy*w+xx]=color

def polygon(buf,w,h,pts,color):
    ymin=max(0,int(math.floor(min(p[1] for p in pts)))); ymax=min(h-1,int(math.ceil(max(p[1] for p in pts))))
    for y in range(ymin,ymax+1):
        xs=[]
        for i in range(len(pts)):
            x1,y1=pts[i]; x2,y2=pts[(i+1)%len(pts)]
            if y1==y2: continue
            if min(y1,y2)<=y<max(y1,y2):
                xs.append(round(x1+(y-y1)*(x2-x1)/(y2-y1)))
        xs.sort()
        for j in range(0,len(xs)-1,2):
            for x in range(max(0,xs[j]),min(w-1,xs[j+1])+1):
                buf[y*w+x]=color

def circle(buf,w,h,cx,cy,r,color):
    rr=r*r
    for y in range(max(0,cy-r),min(h,cy+r+1)):
        for x in range(max(0,cx-r),min(w,cx+r+1)):
            if (x-cx)**2+(y-cy)**2<=rr: buf[y*w+x]=color

# Wide dark graphite artwork with red circuitry and a luminous geometric core.
W,H=1280,720
pixels=[]
for y in range(H):
    for x in range(W):
        glow=max(0.0,1.0-math.sqrt(((x-930)/780)**2+((y-360)/600)**2))
        vignette=min(1.0,math.sqrt(((x-W/2)/(W*.78))**2+((y-H/2)/(H*.82))**2))
        pixels.append((int(5+glow*14-vignette*2),int(7+glow*2),int(11+glow*3),255))
# soft red ambient pools
for cy,cx,rad,amount in [(570,110,330,.20),(230,1110,300,.18),(650,700,260,.10)]:
    for y in range(max(0,cy-rad),min(H,cy+rad)):
        for x in range(max(0,cx-rad),min(W,cx+rad)):
            d=math.sqrt((x-cx)**2+(y-cy)**2)/rad
            if d<1:
                i=y*W+x; pixels[i]=blend(pixels[i],(90,0,12,255),(1-d)**2*amount)
red=(185,8,28,255); hot=(245,25,45,255); dim=(65,8,17,255); steel=(55,63,73,255)
# Angular circuit traces and fine highlights.
paths=[
 [(0,110),(210,110),(285,35),(470,35),(520,0)],
 [(0,480),(170,480),(250,555),(440,555),(510,625),(650,625)],
 [(1280,95),(1110,95),(1040,165),(1010,165)],
 [(1280,575),(1115,575),(1050,510),(900,510),(845,455)],
 [(70,0),(70,130),(160,220),(160,315)],
 [(1210,0),(1210,150),(1150,210),(1150,285)],
 [(0,635),(220,635),(280,690),(490,690)],
 [(1280,665),(1090,665),(1025,610),(900,610)]
]
for pts in paths:
    for a,b in zip(pts,pts[1:]):
        line(pixels,W,H,*a,*b,dim,5); line(pixels,W,H,*a,*b,red,2)
        line(pixels,W,H,a[0],a[1]-2,b[0],b[1]-2,hot,1)
# Main core: concentric angular diamonds on the right.
cx,cy=930,355
for scale,col in [(1.0,steel),(.88,red),(.70,dim),(.58,hot),(.43,dim)]:
    rx,ry=260*scale,255*scale
    pts=[(cx,cy-ry),(cx+rx,cy),(cx,cy+ry),(cx-rx,cy)]
    for a,b in zip(pts,pts[1:]+pts[:1]):
        line(pixels,W,H,*a,*b,col,5 if scale==1 else 3)
polygon(pixels,W,H,[(cx,cy-115),(cx+120,cy),(cx,cy+115),(cx-120,cy)],(12,8,13,255))
polygon(pixels,W,H,[(cx,cy-78),(cx+80,cy),(cx,cy+78),(cx-80,cy)],(25,5,12,255))
# Core glow and circuit nodes
for r,col in [(95,(65,0,10,255)),(58,(115,0,15,255)),(28,(235,12,35,255))]:
    circle(pixels,W,H,cx,cy,r,col)
for x,y in [(210,110),(250,555),(1110,95),(1115,575),(70,130),(1150,210),(280,690),(1025,610)]:
    circle(pixels,W,H,x,y,5,hot); circle(pixels,W,H,x,y,2,(255,120,130,255))
# Horizontal ground reflection / baseline
for y in range(665,720):
    a=(y-665)/55
    for x in range(W):
        i=y*W+x
        if (x%90)<2:
            pixels[i]=blend(pixels[i],(90,8,18,255),.28*(1-a))
png(os.path.join(OUT,"evilware_core_banner.png"),W,H,pixels)

# Icon renderer: simple trident mark on a rounded graphite tile.
def make_icon(n):
    buf=[(7,9,14,255)]*(n*n)
    # red radial centre glow
    for y in range(n):
        for x in range(n):
            d=math.sqrt(((x-n*.5)/(n*.8))**2+((y-n*.5)/(n*.8))**2)
            if d<1: buf[y*n+x]=blend(buf[y*n+x],(55,0,10,255),(1-d)*.55)
    def poly(points,c): polygon(buf,n,n,[(round(x*n/256),round(y*n/256)) for x,y in points],c)
    # inset border
    for x in range(n):
        for y in range(n):
            if x<n*.045 or y<n*.045 or x>=n*.955 or y>=n*.955:
                buf[y*n+x]=(65,28,37,255)
    poly([(128,35),(153,89),(139,84),(139,174),(128,194),(117,174),(117,84),(103,89)],(235,18,42,255))
    poly([(70,72),(105,96),(96,112),(82,106),(94,176),(75,193),(61,112),(52,92)],(205,10,34,255))
    poly([(186,72),(204,92),(195,112),(181,106),(167,193),(148,176),(160,112),(151,96)],(205,10,34,255))
    poly([(96,178),(128,202),(160,178),(151,220),(128,239),(105,220)],(145,5,24,255))
    return buf
sizes=[16,24,32,48,64,128,256]
png_images=[]
for n in sizes:
    p=os.path.join(OUT,f".icon_{n}.png")
    png(p,n,n,make_icon(n))
    with open(p,"rb") as f: png_images.append(f.read())
    os.remove(p)
# ICO directory followed by PNG payloads.
count=len(sizes); header=struct.pack("<HHH",0,1,count)
entries=[]; offset=6+16*count
for n,data in zip(sizes,png_images):
    dim=0 if n==256 else n
    entries.append(struct.pack("<BBBBHHII",dim,dim,0,0,1,32,len(data),offset))
    offset+=len(data)
with open(os.path.join(OUT,"evilware_app_icon.ico"),"wb") as f:
    f.write(header+b"".join(entries)+b"".join(png_images))
# Also include a PNG copy for Qt's application/window icon.
png(os.path.join(OUT,"evilware_app_icon.png"),256,256,make_icon(256))
print("Generated EVILWARE banner PNG and multi-resolution application icon in",OUT)
