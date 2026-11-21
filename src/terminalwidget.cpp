#include "terminalwidget.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDir>
TerminalWidget::TerminalWidget(QWidget *p):QWidget(p),proc(new QProcess(this)) {
 auto *v=new QVBoxLayout(this); auto *bar=new QHBoxLayout;
 shell=new QComboBox; shell->addItems({"PowerShell","Command Prompt","WSL (if installed)"});
 auto *restart=new QPushButton("Restart shell"); status=new QLabel("Shell stopped");
 bar->addWidget(new QLabel("Environment")); bar->addWidget(shell); bar->addWidget(restart); bar->addWidget(status); v->addLayout(bar);
 output=new QPlainTextEdit; output->setReadOnly(true); output->setMaximumBlockCount(5000);
 output->setObjectName("terminalOutput");
 v->addWidget(output,1); auto *row=new QHBoxLayout; input=new QLineEdit; input->setPlaceholderText("Enter a command and press Enter…");
 auto *send=new QPushButton("Run"); row->addWidget(input,1); row->addWidget(send); v->addLayout(row);
 connect(restart,&QPushButton::clicked,this,&TerminalWidget::startShell); connect(send,&QPushButton::clicked,this,&TerminalWidget::sendCommand);
 connect(input,&QLineEdit::returnPressed,this,&TerminalWidget::sendCommand);
 connect(proc,&QProcess::readyReadStandardOutput,this,&TerminalWidget::readOutput);
 connect(proc,&QProcess::readyReadStandardError,this,&TerminalWidget::readOutput);
 connect(proc,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,&TerminalWidget::processEnded);
 startShell();
}
void TerminalWidget::setShellIndex(int index){ if(index>=0 && index<shell->count()) shell->setCurrentIndex(index); }
void TerminalWidget::append(const QString &t){ output->moveCursor(QTextCursor::End); output->insertPlainText(t); output->moveCursor(QTextCursor::End); }
void TerminalWidget::startShell(){
 if(proc->state()!=QProcess::NotRunning) proc->kill();
 QString program; QStringList args;
#ifdef Q_OS_WIN
 if(shell->currentIndex()==0){program="powershell.exe";args={"-NoLogo","-NoProfile","-NoExit","-Command","-"};}
 else if(shell->currentIndex()==1){program="cmd.exe";args={"/Q","/K"};}
 else {program="wsl.exe";args={"--exec","bash","-l"};}
#else
 program="bash";args={"-l"};
#endif
 proc->setWorkingDirectory(QDir::homePath()); proc->start(program,args);
 status->setText("Starting…"); append("\n[NEXUS] Starting "+program+"\n");
}
void TerminalWidget::sendCommand(){
 const QString cmd=input->text().trimmed(); if(cmd.isEmpty())return;
 if(proc->state()!=QProcess::Running){append("\n[Terminal is not running. Restart the shell.]\n");return;}
 append("\n> "+cmd+"\n"); proc->write((cmd+"\n").toUtf8()); input->clear();
}
void TerminalWidget::readOutput(){append(QString::fromLocal8Bit(proc->readAllStandardOutput()));append(QString::fromLocal8Bit(proc->readAllStandardError()));status->setText("Running");}
void TerminalWidget::processEnded(int code,QProcess::ExitStatus){status->setText(QString("Exited (%1)").arg(code));append(QString("\n[Shell exited: %1]\n").arg(code));}
