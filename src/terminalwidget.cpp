#include "terminalwidget.h"
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QKeyEvent>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <functional>
class TerminalConsole final:public QPlainTextEdit{
public:
 explicit TerminalConsole(QWidget*p=nullptr):QPlainTextEdit(p){setReadOnly(false);setUndoRedoEnabled(false);setLineWrapMode(QPlainTextEdit::NoWrap);setFont(QFont("Consolas",10));}
 std::function<void(const QString&)> submit;QStringList history;int historyIndex=0;
 void writeOutput(const QString&s){QTextCursor c(document());c.movePosition(QTextCursor::End);c.insertText(s);setTextCursor(c);ensureCursorVisible();}
protected:
 void keyPressEvent(QKeyEvent*e)override{
  if(e->key()==Qt::Key_Return||e->key()==Qt::Key_Enter){QTextCursor c(document());c.movePosition(QTextCursor::End);c.movePosition(QTextCursor::StartOfBlock);QString cmd=c.block().text();QTextCursor z(document());z.movePosition(QTextCursor::End);z.insertText("\n");setTextCursor(z);if(!cmd.trimmed().isEmpty()){history.append(cmd);historyIndex=history.size();}if(submit)submit(cmd);return;}
  if(e->key()==Qt::Key_Backspace&&textCursor().position()<=document()->lastBlock().position())return;
  if(e->key()==Qt::Key_Up&&!history.isEmpty()){historyIndex=qMax(0,historyIndex-1);replaceLine(history.value(historyIndex));return;}
  if(e->key()==Qt::Key_Down&&!history.isEmpty()){historyIndex=qMin(history.size(),historyIndex+1);replaceLine(history.value(historyIndex));return;}
  QTextCursor c=textCursor();QTextCursor lineStart(document());lineStart.movePosition(QTextCursor::End);lineStart.movePosition(QTextCursor::StartOfBlock);if(c.position()<lineStart.position()){QTextCursor end(document());end.movePosition(QTextCursor::End);setTextCursor(end);}QPlainTextEdit::keyPressEvent(e);
 }
 void replaceLine(const QString&s){QTextCursor c(document());c.movePosition(QTextCursor::End);c.movePosition(QTextCursor::StartOfBlock,QTextCursor::KeepAnchor);c.insertText(s);QTextCursor end(document());end.movePosition(QTextCursor::End);setTextCursor(end);}
};
TerminalWidget::TerminalWidget(QWidget*p):QWidget(p),proc(new QProcess(this)){
 auto*v=new QVBoxLayout(this);auto*bar=new QHBoxLayout;shell=new QComboBox;shell->addItems({"PowerShell","Command Prompt","WSL (if installed)"});auto*restart=new QPushButton("Restart shell");status=new QLabel("Shell stopped");bar->addWidget(new QLabel("Environment"));bar->addWidget(shell);bar->addWidget(restart);bar->addWidget(status);v->addLayout(bar);
 console=new TerminalConsole;console->setObjectName("terminalConsole");console->setPlaceholderText("Type directly here and press Enter to run a command.");v->addWidget(console,1);
 connect(restart,&QPushButton::clicked,this,&TerminalWidget::startShell);connect(shell,qOverload<int>(&QComboBox::currentIndexChanged),this,&TerminalWidget::startShell);
 connect(proc,&QProcess::readyReadStandardOutput,this,&TerminalWidget::readOutput);connect(proc,&QProcess::readyReadStandardError,this,&TerminalWidget::readOutput);connect(proc,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,&TerminalWidget::processEnded);
 console->submit=[this](const QString&cmd){sendCommand(cmd);};startShell();
}
void TerminalWidget::setShellIndex(int i){if(i>=0&&i<shell->count()){shell->setCurrentIndex(i);startShell();}}
void TerminalWidget::startShell(){if(proc->state()!=QProcess::NotRunning){proc->kill();proc->waitForFinished(1000);}QString program;QStringList args;
#ifdef Q_OS_WIN
 if(shell->currentIndex()==0){program="powershell.exe";args={"-NoLogo","-NoProfile","-NoExit","-Command","-"};}else if(shell->currentIndex()==1){program="cmd.exe";args={"/Q","/K"};}else{program="wsl.exe";args={"--exec","bash","-l"};}
#else
 program="bash";args={"-i"};
#endif
 proc->setWorkingDirectory(QDir::homePath());proc->start(program,args);status->setText("Starting shell…");console->clear();console->writeOutput("EVILWARE interactive terminal\nStarting "+program+"…\n");console->setFocus();}
void TerminalWidget::sendCommand(const QString&cmd){if(cmd.trimmed().isEmpty())return;if(proc->state()!=QProcess::Running){console->writeOutput("[Shell is not running. Restart it and try again.]\n");return;}proc->write((cmd+"\n").toLocal8Bit());status->setText("Command sent");}
void TerminalWidget::readOutput(){auto a=proc->readAllStandardOutput(),b=proc->readAllStandardError();if(!a.isEmpty())console->writeOutput(QString::fromLocal8Bit(a));if(!b.isEmpty())console->writeOutput(QString::fromLocal8Bit(b));status->setText("Running");}
void TerminalWidget::processEnded(int code,QProcess::ExitStatus){status->setText(QString("Exited (%1)").arg(code));console->writeOutput(QString("\n[Shell exited: %1]\n").arg(code));}
