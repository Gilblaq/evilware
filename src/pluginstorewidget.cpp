#include "pluginstorewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>

struct PluginInfo { const char *name; const char *category; const char *description; const char *id; const char *url; };
static const PluginInfo plugins[] = {
 {"Git","Source control","Version control and Git command-line tools.","Git.Git","https://git-scm.com/download/win"},
 {"Visual Studio Code","Code editor","Code editor with extensions and integrated terminal.","Microsoft.VisualStudioCode","https://code.visualstudio.com/"},
 {"Python 3","Language runtime","Python runtime for scripts, automation and applications.","Python.Python.3.12","https://www.python.org/downloads/"},
 {"Node.js LTS","Language runtime","JavaScript runtime and npm package manager.","OpenJS.NodeJS.LTS","https://nodejs.org/"},
 {"Docker Desktop","Containers","Build and run containers on Windows; requires WSL 2 or Hyper-V.","Docker.DockerDesktop","https://www.docker.com/products/docker-desktop/"},
 {"Ubuntu on WSL","Linux environment","Ubuntu distribution for Windows Subsystem for Linux.","Canonical.Ubuntu.2404","https://learn.microsoft.com/windows/wsl/install"},
 {"Nmap","Network diagnostics","Network discovery and security auditing on authorized systems.","Insecure.Nmap","https://nmap.org/download.html"},
 {"Wireshark","Network analysis","Inspect traffic on networks where you have permission.","WiresharkFoundation.Wireshark","https://www.wireshark.org/download.html"}
};
PluginStoreWidget::PluginStoreWidget(QWidget *parent):QWidget(parent){
 auto *root=new QVBoxLayout(this);root->setContentsMargins(24,20,24,24);root->setSpacing(14);
 auto *title=new QLabel("Plugin & Software Manager");title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel("Manage approved developer tools on this Windows device. EVILWARE uses Windows Package Manager (winget). Every install, update and removal requires your confirmation.");intro->setWordWrap(true);intro->setObjectName("mutedText");root->addWidget(intro);
 search=new QLineEdit;search->setPlaceholderText("Search tools and integrations…");root->addWidget(search);
 status=new QLabel("Choose Manage to check installation status and available actions.");status->setWordWrap(true);status->setObjectName("mutedText");root->addWidget(status);
 auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);
 listHost=new QWidget;listLayout=new QVBoxLayout(listHost);listLayout->setContentsMargins(0,0,8,0);listLayout->setSpacing(10);scroll->setWidget(listHost);root->addWidget(scroll,1);
 connect(search,&QLineEdit::textChanged,this,&PluginStoreWidget::rebuild);rebuild();
}
void PluginStoreWidget::rebuild(const QString &filter){
 while(auto *item=listLayout->takeAt(0)){if(item->widget())item->widget()->deleteLater();delete item;}
 const QString needle=filter.trimmed();
 for(const auto &p:plugins){
  const QString name=p.name,category=p.category,description=p.description,id=p.id;
  if(!needle.isEmpty()&&!name.contains(needle,Qt::CaseInsensitive)&&!category.contains(needle,Qt::CaseInsensitive)&&!description.contains(needle,Qt::CaseInsensitive))continue;
  auto *card=new QFrame;card->setObjectName("pluginCard");auto *row=new QHBoxLayout(card);row->setContentsMargins(16,14,16,14);row->setSpacing(14);
  auto *details=new QVBoxLayout;auto *heading=new QLabel(name);heading->setObjectName("cardTitle");auto *meta=new QLabel(category+"  ·  "+id);meta->setObjectName("mutedText");auto *desc=new QLabel(description);desc->setWordWrap(true);desc->setObjectName("bodyText");details->addWidget(heading);details->addWidget(meta);details->addWidget(desc);row->addLayout(details,1);
  auto *actions=new QVBoxLayout;auto *manage=new QPushButton("Manage");manage->setObjectName("primaryButton");auto *website=new QPushButton("Publisher website");website->setObjectName("secondaryButton");actions->addWidget(manage);actions->addWidget(website);actions->addStretch();row->addLayout(actions);
  connect(manage,&QPushButton::clicked,this,[this,name,id]{checkTool(name,id);});
  const QString url=p.url;connect(website,&QPushButton::clicked,this,[url]{QDesktopServices::openUrl(QUrl(url));});
  listLayout->addWidget(card);
 }
 listLayout->addStretch();
}
void PluginStoreWidget::checkTool(const QString &name,const QString &id){
 const QString winget=QStandardPaths::findExecutable("winget");
 if(winget.isEmpty()){QMessageBox::warning(this,"winget unavailable","Install or update App Installer from Microsoft Store, then reopen EVILWARE.");return;}
 status->setText("Checking "+name+"…");
 auto *p=new QProcess(this);p->setProgram(winget);p->setArguments({"list","--id",id,"--exact","--source","winget","--disable-interactivity"});
 connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this,p,name,id](int,QProcess::ExitStatus){
  const QString out=QString::fromLocal8Bit(p->readAllStandardOutput())+"\n"+QString::fromLocal8Bit(p->readAllStandardError());p->deleteLater();
  const bool installed=out.contains(id,Qt::CaseInsensitive)&&!out.contains("No installed package found",Qt::CaseInsensitive);
  QMessageBox box(this);box.setWindowTitle("Manage "+name);box.setText(installed?name+" appears installed. Choose an action.":name+" was not found as installed. Install it?");
  auto *install=box.addButton(installed?"Reinstall":"Install",QMessageBox::AcceptRole);
  auto *update=installed?box.addButton("Update",QMessageBox::ActionRole):nullptr;
  auto *remove=installed?box.addButton("Uninstall",QMessageBox::DestructiveRole):nullptr;
  box.addButton("Cancel",QMessageBox::RejectRole);box.exec();
  if(box.clickedButton()==install)runPackageAction(name,id,"install");
  else if(update&&box.clickedButton()==update)runPackageAction(name,id,"upgrade");
  else if(remove&&box.clickedButton()==remove)runPackageAction(name,id,"uninstall");
 });
 connect(p,&QProcess::errorOccurred,this,[this,p,name](QProcess::ProcessError){status->setText("Could not run winget to check "+name+".");p->deleteLater();});
 p->start();
}
void PluginStoreWidget::runPackageAction(const QString &name,const QString &id,const QString &action){
 const QString winget=QStandardPaths::findExecutable("winget");if(winget.isEmpty()){QMessageBox::warning(this,"winget unavailable","Install or update App Installer, then try again.");return;}
 const QString prompt=action=="install"?"Install "+name+" from winget?":action=="upgrade"?"Update "+name+" using winget?":"Uninstall "+name+" using winget?";
 if(QMessageBox::question(this,"Confirm package operation",prompt,QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
 status->setText(action.toUpper()+" running for "+name+"…");
 auto *p=new QProcess(this);p->setProgram(winget);QStringList args={action,"--id",id,"--exact","--source","winget","--disable-interactivity"};
 if(action!="uninstall")args<<"--accept-package-agreements"<<"--accept-source-agreements";p->setArguments(args);
 connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this,p,name,action](int code,QProcess::ExitStatus state){
  const QString output=QString::fromLocal8Bit(p->readAllStandardOutput())+"\n"+QString::fromLocal8Bit(p->readAllStandardError());p->deleteLater();
  const bool ok=state==QProcess::NormalExit&&code==0;status->setText(ok?name+" "+action+" completed.":name+" "+action+" failed.");
  QMessageBox box(this);box.setWindowTitle(ok?"Operation complete":"Operation failed");box.setIcon(ok?QMessageBox::Information:QMessageBox::Warning);box.setText(ok?name+" "+action+" completed successfully.":name+" "+action+" did not complete successfully.");box.setDetailedText(output.trimmed().isEmpty()?"winget returned no additional details.":output);box.exec();
 });
 connect(p,&QProcess::errorOccurred,this,[this,p,name](QProcess::ProcessError){status->setText("Could not start winget for "+name+".");QMessageBox::warning(this,"Package operation failed","Check that App Installer is installed and winget is available.");p->deleteLater();});
 p->start();
}
