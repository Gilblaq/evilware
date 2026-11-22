#include "startupwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QtMath>

static void drawRedFace(QPainter &p,const QRectF &r){
 p.save();p.setRenderHint(QPainter::Antialiasing);
 QRadialGradient glow(r.center(),r.width()*.55);glow.setColorAt(0,QColor(130,0,0,100));glow.setColorAt(.55,QColor(55,0,0,75));glow.setColorAt(1,QColor(0,0,0,0));p.fillRect(r,glow);
 QPointF c(r.center().x(),r.center().y()*.91);QPainterPath hood;hood.moveTo(c.x()-r.width()*.38,r.top()+r.height()*.08);hood.quadTo(c.x(),r.top()-r.height()*.16,c.x()+r.width()*.38,r.top()+r.height()*.08);hood.quadTo(r.right(),c.y(),c.x()+r.width()*.28,r.bottom());hood.quadTo(c.x(),r.bottom()+r.height()*.08,c.x()-r.width()*.28,r.bottom());hood.quadTo(r.left(),c.y(),c.x()-r.width()*.38,r.top()+r.height()*.08);p.fillPath(hood,QColor(3,2,4,245));
 QPainterPath face;face.moveTo(c.x()-r.width()*.19,c.y()-r.height()*.22);face.quadTo(c.x(),c.y()-r.height()*.34,c.x()+r.width()*.19,c.y()-r.height()*.22);face.lineTo(c.x()+r.width()*.16,c.y()+r.height()*.25);face.quadTo(c.x(),c.y()+r.height()*.48,c.x()-r.width()*.16,c.y()+r.height()*.25);face.closeSubpath();p.fillPath(face,QColor(74,10,12,230));
 p.setPen(QPen(QColor(180,0,0,125),r.width()*.009));p.drawPath(face);
 for(int s:{-1,1}){QPointF eye(c.x()+s*r.width()*.09,c.y()-r.height()*.055);p.setPen(Qt::NoPen);p.setBrush(QColor(15,0,0));p.drawEllipse(QRectF(eye.x()-r.width()*.066,eye.y()-r.height()*.035,r.width()*.132,r.height()*.070));p.setBrush(QColor(255,0,12));p.drawEllipse(QRectF(eye.x()-r.width()*.017,eye.y()-r.height()*.014,r.width()*.034,r.height()*.028));}
 p.setPen(QPen(QColor(12,0,0),r.width()*.022));p.drawLine(QPointF(c.x(),c.y()+r.height()*.03),QPointF(c.x()-r.width()*.018,c.y()+r.height()*.15));p.drawLine(QPointF(c.x()-r.width()*.07,c.y()+r.height()*.24),QPointF(c.x()+r.width()*.07,c.y()+r.height()*.24));
 p.setPen(QPen(QColor(155,0,0,65),2));for(int i=0;i<14;i++){qreal x=r.left()+((i*73)%int(r.width()));qreal y=r.top()+((i*47)%int(r.height()));p.drawLine(QPointF(x,y),QPointF(x+18,y+26));}
 p.restore();
}
static void drawSnake(QPainter &p,const QRectF &r,int tick){
 p.save();p.setRenderHint(QPainter::Antialiasing);
 const QPointF c=r.center();const qreal rx=r.width()*.29,ry=r.height()*.31;
 const qreal phase=tick*.035;
 QPainterPath body;
 for(int i=0;i<=180;i++){
  const qreal a=phase+i*(2*M_PI/180.0);
  const qreal wobble=1.0+.025*qSin(i*.19+phase);
  const QPointF pt(c.x()+rx*wobble*qCos(a),c.y()+ry*wobble*qSin(a));
  if(i==0)body.moveTo(pt);else body.lineTo(pt);
 }
 p.setPen(QPen(QColor(5,0,0,235),r.height()*.052,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));p.drawPath(body);
 QLinearGradient scales(r.left(),r.top(),r.right(),r.bottom());scales.setColorAt(0,QColor(75,0,5));scales.setColorAt(.45,QColor(225,0,18));scales.setColorAt(1,QColor(92,0,8));
 p.setPen(QPen(QBrush(scales),r.height()*.031,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));p.drawPath(body);
 p.setPen(QPen(QColor(255,75,82,145),r.height()*.006,Qt::SolidLine,Qt::RoundCap));p.drawPath(body);
 // Fine cross-bands give the coil a scaled, segmented surface.
 p.setPen(QPen(QColor(35,0,2,150),1.2));
 for(int i=8;i<175;i+=5){
  const qreal a=phase+i*(2*M_PI/180.0),w=1.0+.025*qSin(i*.19+phase);
  const QPointF pt(c.x()+rx*w*qCos(a),c.y()+ry*w*qSin(a));
  const QPointF n(qCos(a),qSin(a));
  p.drawLine(pt-n*5,pt+n*5);
 }
 const qreal a=phase;
 const QPointF head(c.x()+rx*qCos(a),c.y()+ry*qSin(a));
 const qreal angle=(a+M_PI/2.0)*180.0/M_PI;
 p.translate(head);p.rotate(angle);
 QRadialGradient headShade(-5,-4,24);headShade.setColorAt(0,QColor(255,72,78));headShade.setColorAt(.55,QColor(174,0,12));headShade.setColorAt(1,QColor(48,0,4));
 p.setPen(QPen(QColor(30,0,2),2));p.setBrush(headShade);p.drawEllipse(QRectF(-17,-12,34,24));
 p.setPen(Qt::NoPen);p.setBrush(QColor(255,215,190));p.drawEllipse(QRectF(3,-7,5,5));p.drawEllipse(QRectF(3,3,5,5));
 p.setBrush(QColor(12,0,0));p.drawEllipse(QRectF(4,-6,2,3));p.drawEllipse(QRectF(4,4,2,3));
 p.setPen(QPen(QColor(255,30,45),2));p.drawLine(QPointF(16,0),QPointF(27,0));p.drawLine(QPointF(27,0),QPointF(34,-5));p.drawLine(QPointF(27,0),QPointF(34,5));
 p.restore();
}
SplashScreen::SplashScreen(QWidget *parent):QWidget(parent){
 setWindowFlags(Qt::Window|Qt::FramelessWindowHint);setAttribute(Qt::WA_TranslucentBackground,false);setFixedSize(1000,620);
 auto *timer=new QTimer(this);connect(timer,&QTimer::timeout,this,[this]{++tick;update();});timer->start(35);
 QTimer::singleShot(4600,this,[this]{emit finished();});
}
void SplashScreen::paintEvent(QPaintEvent *){
 QPainter p(this);p.fillRect(rect(),QColor(2,1,3));drawRedFace(p,QRectF(0,0,width(),height()));drawSnake(p,QRectF(0,0,width(),height()),tick);
 p.fillRect(QRect(0,height()-95,width(),95),QColor(0,0,0,135));p.setPen(QColor(245,240,240));QFont f("Segoe UI",31,QFont::Bold);p.setFont(f);p.drawText(QRect(0,height()-220,width(),52),Qt::AlignCenter,"EVILWARE");p.setPen(QColor(255,35,45));p.setFont(QFont("Segoe UI",14,QFont::DemiBold));p.drawText(QRect(0,height()-170,width(),30),Qt::AlignCenter,"THE UNHOLY CORE 3");p.setPen(QColor(195,195,205));p.setFont(QFont("Segoe UI",10));p.drawText(QRect(0,height()-135,width(),24),Qt::AlignCenter,"DEVELOPER COMMAND CENTRE");QRect bar(width()/2-190,height()-72,380,7);p.setPen(QPen(QColor(115,0,8),1));p.setBrush(QColor(25,12,15));p.drawRoundedRect(bar,4,4);QRect fill=bar;fill.setWidth(qMax(4,(tick%125)*bar.width()/125));p.setPen(Qt::NoPen);p.setBrush(QColor(225,0,18));p.drawRoundedRect(fill,4,4);p.setPen(QColor(205,190,195));p.drawText(QRect(0,height()-58,width(),20),Qt::AlignCenter,"INITIALIZING CORE SYSTEMS...");
}
AccessScreen::AccessScreen(QWidget *parent):QWidget(parent){
 setWindowFlags(Qt::Window);resize(1120,700);setMinimumSize(850,560);
 auto *outer=new QHBoxLayout(this);outer->setContentsMargins(0,0,0,0);outer->addStretch(3);
 auto *panel=new QWidget;panel->setObjectName("accessPanel");panel->setFixedWidth(390);auto *v=new QVBoxLayout(panel);v->setContentsMargins(30,28,30,28);v->setSpacing(13);
 auto *brand=new QLabel("EVILWARE");brand->setAlignment(Qt::AlignCenter);brand->setStyleSheet("font-size:27px;font-weight:800;letter-spacing:3px;color:#f2eeee");v->addWidget(brand);
 auto *edition=new QLabel("THE UNHOLY CORE 3");edition->setAlignment(Qt::AlignCenter);edition->setStyleSheet("color:#f22b38;font-weight:700;letter-spacing:3px");v->addWidget(edition);
 auto *tag=new QLabel("DEVELOPER COMMAND CENTRE");tag->setAlignment(Qt::AlignCenter);tag->setStyleSheet("color:#aaa1a5;font-size:10px;letter-spacing:2px");v->addWidget(tag);v->addSpacing(18);
 auto *tabs=new QHBoxLayout;loginTab=new QPushButton("Login");signupTab=new QPushButton("Sign Up");tabs->addWidget(loginTab);tabs->addWidget(signupTab);v->addLayout(tabs);
 heading=new QLabel("Welcome back");heading->setStyleSheet("font-size:18px;font-weight:650;color:#f5f1f2");v->addWidget(heading);
 auto *hint=new QLabel("Enter a display name to open your local workspace.");hint->setWordWrap(true);hint->setStyleSheet("color:#aaa1a5");v->addWidget(hint);
 name=new QLineEdit;name->setPlaceholderText("Your name");name->setMinimumHeight(44);v->addWidget(name);
 auto *enter=new QPushButton("ENTER EVILWARE");enter->setObjectName("enterButton");enter->setMinimumHeight(46);v->addWidget(enter);
 auto *guest=new QPushButton("Continue as Guest");guest->setObjectName("guestButton");v->addWidget(guest);v->addStretch();
 auto *foot=new QLabel("LOCAL PROFILE ONLY  •  NO PASSWORD  •  NO ONLINE AUTHENTICATION");foot->setAlignment(Qt::AlignCenter);foot->setWordWrap(true);foot->setStyleSheet("color:#766b70;font-size:8px");v->addWidget(foot);
 outer->addWidget(panel,0,Qt::AlignRight|Qt::AlignVCenter);outer->addSpacing(7);
 setStyleSheet("QWidget{background:transparent;color:#eee} QWidget#accessPanel{background:rgba(10,8,10,235);border:1px solid #6d1018;border-radius:14px} QPushButton{background:#241417;color:#eee;border:1px solid #4c2025;border-radius:7px;padding:10px} QPushButton:hover{background:#43151b} QPushButton#enterButton{background:#b90012;border:1px solid #f12a39;font-weight:700} QPushButton#guestButton{background:transparent;border:0;color:#d8aeb2} QLineEdit{background:#171316;color:#fff;border:1px solid #593139;border-radius:7px;padding:10px}");
 connect(loginTab,&QPushButton::clicked,this,[this]{setMode(false);});connect(signupTab,&QPushButton::clicked,this,[this]{setMode(true);});setMode(false);
 auto go=[this]{QString value=name->text().trimmed();if(value.isEmpty())value="Guest";emit enterRequested(value);};connect(enter,&QPushButton::clicked,this,go);connect(guest,&QPushButton::clicked,this,[this]{emit enterRequested("Guest");});
}
void AccessScreen::setMode(bool create){signup=create;heading->setText(create?"Create your local profile":"Welcome back");loginTab->setStyleSheet(create?"color:#999":"color:#fff;border-bottom:2px solid #e21b2b");signupTab->setStyleSheet(create?"color:#fff;border-bottom:2px solid #e21b2b":"color:#999");name->setPlaceholderText(create?"Choose a display name":"Your name");}
void AccessScreen::paintEvent(QPaintEvent *){QPainter p(this);p.fillRect(rect(),QColor(3,2,4));drawRedFace(p,QRectF(0,0,width()*.72,height()));drawSnake(p,QRectF(0,0,width()*.72,height()),20);}
SkullBackdrop::SkullBackdrop(QWidget *parent):QWidget(parent){setAttribute(Qt::WA_StyledBackground,false);}
void SkullBackdrop::paintEvent(QPaintEvent *){QPainter p(this);p.setRenderHint(QPainter::Antialiasing);p.fillRect(rect(),QColor(8,11,16));QRectF r(width()*.35,height()*.12,width()*.43,height()*.78);QRadialGradient fog(r.center(),r.width()*.8);fog.setColorAt(0,QColor(54,76,87,100));fog.setColorAt(1,QColor(5,8,13,0));p.fillRect(rect(),fog);QPointF c=r.center();QPainterPath skull;skull.moveTo(c.x()-r.width()*.29,c.y()-r.height()*.12);skull.cubicTo(c.x()-r.width()*.36,c.y()-r.height()*.55,c.x()+r.width()*.36,c.y()-r.height()*.55,c.x()+r.width()*.29,c.y()-r.height()*.12);skull.cubicTo(c.x()+r.width()*.27,c.y()+r.height()*.18,c.x()+r.width()*.17,c.y()+r.height()*.24,c.x()+r.width()*.15,c.y()+r.height()*.38);skull.lineTo(c.x()+r.width()*.12,c.y()+r.height()*.48);skull.lineTo(c.x()-r.width()*.12,c.y()+r.height()*.48);skull.lineTo(c.x()-r.width()*.15,c.y()+r.height()*.38);skull.cubicTo(c.x()-r.width()*.17,c.y()+r.height()*.24,c.x()-r.width()*.27,c.y()+r.height()*.18,c.x()-r.width()*.29,c.y()-r.height()*.12);skull.closeSubpath();p.setBrush(QColor(150,166,168,35));p.setPen(QPen(QColor(180,198,199,45),2));p.drawPath(skull);p.setBrush(QColor(4,8,13,190));p.setPen(Qt::NoPen);p.drawEllipse(QPointF(c.x()-r.width()*.13,c.y()+r.height()*.06),r.width()*.085,r.height()*.075);p.drawEllipse(QPointF(c.x()+r.width()*.13,c.y()+r.height()*.06),r.width()*.085,r.height()*.075);QPainterPath nose;nose.moveTo(c.x(),c.y()+r.height()*.17);nose.lineTo(c.x()-r.width()*.035,c.y()+r.height()*.28);nose.lineTo(c.x()+r.width()*.035,c.y()+r.height()*.28);nose.closeSubpath();p.drawPath(nose);p.setPen(QPen(QColor(205,0,20,45),2));p.drawLine(QPointF(0,height()*.92),QPointF(width(),height()*.92));}
