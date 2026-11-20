#include "cyberlabwidget.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QRegularExpression>
#include <QLabel>
CyberLabWidget::CyberLabWidget(QWidget*p):QWidget(p),process(new QProcess(this)){
 auto*v=new QVBoxLayout(this);auto*r=new QHBoxLayout;tool=new QComboBox;tool->addItems({"Ping","DNS lookup (nslookup)","Route trace","Network configuration"});target=new QLineEdit("example.com");auto*b=new QPushButton("Run diagnostic");r->addWidget(tool);r->addWidget(target,1);r->addWidget(b);v->addLayout(r);
 result=new QPlainTextEdit;result->setReadOnly(true);result->setStyleSheet("background:#0b1018;color:#cde4d8;font-family:Consolas,monospace;");v->addWidget(result,1);
 v->addWidget(new QLabel("Authorized diagnostics only. Use targets you own or have permission to assess. This lab does not perform exploitation or stealth actions."));
 connect(b,&QPushButton::clicked,this,&CyberLabWidget::runDiagnostic);connect(process,&QProcess::readyReadStandardOutput,this,[this]{result->appendPlainText(QString::fromLocal8Bit(process->readAllStandardOutput()));});connect(process,&QProcess::readyReadStandardError,this,[this]{result->appendPlainText(QString::fromLocal8Bit(process->readAllStandardError()));});
}
void CyberLabWidget::runDiagnostic(){
 if(process->state()!=QProcess::NotRunning){result->appendPlainText("A diagnostic is already running.");return;}
 QString t=target->text().trimmed();QRegularExpression safe("^[A-Za-z0-9._:-]{1,253}$");
 if(tool->currentIndex()!=3&&!safe.match(t).hasMatch()){result->appendPlainText("Invalid target. Use a hostname or IP address only.");return;}
 QString program;QStringList args;
#ifdef Q_OS_WIN
 if(tool->currentIndex()==0){program="ping";args={"-n","4",t};}
 else if(tool->currentIndex()==1){program="nslookup";args={t};}
 else if(tool->currentIndex()==2){program="tracert";args={"-d",t};}
 else {program="ipconfig";args={"/all"};}
#else
 if(tool->currentIndex()==0){program="ping";args={"-c","4",t};}
 else if(tool->currentIndex()==1){program="nslookup";args={t};}
 else if(tool->currentIndex()==2){program="traceroute";args={t};}
 else {program="ip";args={"address"};}
#endif
 result->appendPlainText("\n> "+program+" "+args.join(" ")+"\n");process->start(program,args);
}
