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
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSaveFile>

struct PluginInfo { const char *id; const char *name; const char *category; const char *description; const char *file; };
static const PluginInfo extensionPlugins[] = {
 {"dev-project-audit","Project Audit Toolkit","Developer","Read-only project structure, language, Git status and TODO report.","dev/project-audit/project-audit.ps1"},
 {"dev-port-inspector","Developer Port Inspector","Developer","Inspect listening TCP ports and owning processes on this Windows computer.","dev/port-inspector/port-inspector.ps1"},
 {"sec-dns-inspector","DNS Inspector","Cybersecurity","Look up DNS records for a domain using Windows DNS tools.","security/dns-inspector/dns-inspector.ps1"},
 {"sec-http-header-audit","HTTP Security Header Audit","Cybersecurity","Review HTTP security headers on a site you own or are authorized to assess.","security/http-header-audit/http-header-audit.ps1"},
 {"sec-local-network-inventory","Local Network Inventory","Cybersecurity","View this computer's IP configuration and cached ARP neighbours; no network scan.","security/local-network-inventory/local-network-inventory.ps1"}
};
struct SoftwareInfo { const char *name; const char *category; const char *description; const char *id; const char *url; };
static const SoftwareInfo software[] = {
 {"Git","Source control","Version control and Git command-line tools.","Git.Git","https://git-scm.com/download/win"},
 {"Visual Studio Code","Code editor","Code editor with extensions and integrated terminal.","Microsoft.VisualStudioCode","https://code.visualstudio.com/"},
 {"Python 3","Language runtime","Python runtime for scripts, automation and applications.","Python.Python.3.12","https://www.python.org/downloads/"},
 {"Node.js LTS","Language runtime","JavaScript runtime and npm package manager.","OpenJS.NodeJS.LTS","https://nodejs.org/"},
 {"Docker Desktop","Containers","Build and run containers on Windows; requires WSL 2 or Hyper-V.","Docker.DockerDesktop","https://www.docker.com/products/docker-desktop/"},
 {"Ubuntu on WSL","Linux environment","Ubuntu distribution for Windows Subsystem for Linux.","Canonical.Ubuntu.2404","https://learn.microsoft.com/windows/wsl/install"},
 {"Nmap","Network diagnostics","Network discovery and security auditing on authorized systems.","Insecure.Nmap","https://nmap.org/download.html"},
 {"Wireshark","Network analysis","Inspect traffic on networks where you have permission.","WiresharkFoundation.Wireshark","https://www.wireshark.org/download.html"}
};

PluginStoreWidget::PluginStoreWidget(QWidget *parent):QWidget(parent),network(new QNetworkAccessManager(this)){
 auto *root=new QVBoxLayout(this);root->setContentsMargins(24,20,24,24);root->setSpacing(14);
 auto *title=new QLabel("Plugin & Software Manager");title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel("Download EVILWARE extensions for developer and authorized cybersecurity workflows, or manage trusted Windows software through winget. Extensions are saved locally and are never executed automatically.");intro->setWordWrap(true);intro->setObjectName("mutedText");root->addWidget(intro);
 search=new QLineEdit;search->setPlaceholderText("Search plugins and software…");root->addWidget(search);
 status=new QLabel("EVILWARE extensions are downloaded from the official GitHub repository. Review scripts before running them.");status->setWordWrap(true);status->setObjectName("mutedText");root->addWidget(status);
 auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);
 listHost=new QWidget;listLayout=new QVBoxLayout(listHost);listLayout->setContentsMargins(0,0,8,0);listLayout->setSpacing(10);scroll->setWidget(listHost);root->addWidget(scroll,1);
 connect(search,&QLineEdit::textChanged,this,&PluginStoreWidget::rebuild);rebuild();
}
QString PluginStoreWidget::pluginDirectory(const QString &id) const {
 return QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("plugins/"+id);
}
void PluginStoreWidget::rebuild(const QString &filter){
 while(auto *item=listLayout->takeAt(0)){if(item->widget())item->widget()->deleteLater();delete item;}
 const QString needle=filter.trimmed();
 auto *extensionsTitle=new QLabel("EVILWARE EXTENSIONS  ·  DOWNLOADABLE PLUGINS");extensionsTitle->setObjectName("cardTitle");listLayout->addWidget(extensionsTitle);
 for(const auto &p:extensionPlugins){
  const QString id=p.id,name=p.name,category=p.category,description=p.description,file=p.file;
  if(!needle.isEmpty()&&!name.contains(needle,Qt::CaseInsensitive)&&!category.contains(needle,Qt::CaseInsensitive)&&!description.contains(needle,Qt::CaseInsensitive))continue;
  const QString folder=pluginDirectory(id);const bool installed=QFileInfo::exists(QDir(folder).filePath(QFileInfo(file).fileName()));
  auto *card=new QFrame;card->setObjectName("pluginCard");auto *row=new QHBoxLayout(card);row->setContentsMargins(16,14,16,14);row->setSpacing(14);
  auto *details=new QVBoxLayout;auto *heading=new QLabel(name);heading->setObjectName("cardTitle");auto *meta=new QLabel(category+"  ·  EVILWARE plugin  ·  v1.0.0");meta->setObjectName("mutedText");auto *desc=new QLabel(description);desc->setWordWrap(true);desc->setObjectName("bodyText");details->addWidget(heading);details->addWidget(meta);details->addWidget(desc);row->addLayout(details,1);
  auto *actions=new QVBoxLayout;auto *manage=new QPushButton(installed?"Open plugin folder":"Download & install");manage->setObjectName("primaryButton");actions->addWidget(manage);
  auto *remove=new QPushButton("Uninstall");remove->setObjectName("secondaryButton");remove->setEnabled(installed);actions->addWidget(remove);actions->addStretch();row->addLayout(actions);
  connect(manage,&QPushButton::clicked,this,[this,id,name,file,installed]{
   if(installed)QDesktopServices::openUrl(QUrl::fromLocalFile(pluginDirectory(id)));
   else installPlugin(id,name,file);
  });
  connect(remove,&QPushButton::clicked,this,[this,id,name]{removePlugin(id,name);});
  listLayout->addWidget(card);
 }
 auto *softwareTitle=new QLabel("WINDOWS SOFTWARE  ·  WINGET");softwareTitle->setObjectName("cardTitle");listLayout->addSpacing(12);listLayout->addWidget(softwareTitle);
 for(const auto &p:software){
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
void PluginStoreWidget::installPlugin(const QString &id,const QString &name,const QString &relativePath){
 if(!QMessageBox::question(this,"Install EVILWARE plugin","Download "+name+" from the official EVILWARE GitHub repository? The script will be saved locally and will not run automatically.",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes)return;
 const QUrl url("https://raw.githubusercontent.com/Gilblaq/evilware/main/plugins/"+relativePath);
 if(url.host()!="raw.githubusercontent.com"||!url.path().startsWith("/Gilblaq/evilware/main/plugins/")||!url.path().endsWith(".ps1")){QMessageBox::warning(this,"Blocked plugin URL","The plugin download URL did not pass the repository allowlist.");return;}
 status->setText("Downloading "+name+" from GitHub…");
 QNetworkRequest request(url);request.setTransferTimeout(30000);request.setHeader(QNetworkRequest::UserAgentHeader,"EVILWARE-Unholy-Core-3");
 auto *reply=network->get(request);
 connect(reply,&QNetworkReply::finished,this,[this,reply,id,name,relativePath]{
  const QByteArray data=reply->readAll();const auto error=reply->error();const int http=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();reply->deleteLater();
  if(error!=QNetworkReply::NoError||http!=200||data.isEmpty()||data.size()>1024*1024){status->setText("Download failed for "+name+".");QMessageBox::warning(this,"Plugin download failed","GitHub did not return a valid plugin file. Check your connection and try again.");return;}
  const QString folder=pluginDirectory(id);if(!QDir().mkpath(folder)){QMessageBox::warning(this,"Install failed","Could not create the local plugin directory.");return;}
  const QString target=QDir(folder).filePath(QFileInfo(relativePath).fileName());QSaveFile file(target);
  if(!file.open(QIODevice::WriteOnly)||file.write(data)!=data.size()||!file.commit()){QMessageBox::warning(this,"Install failed","Could not save the downloaded plugin file.");return;}
  status->setText(name+" installed locally. Review the PowerShell script before running it.");
  QMessageBox::information(this,"Plugin installed",name+" was downloaded to:\n"+target+"\n\nIt has not been executed. Review it before running in PowerShell.");
  rebuild(search->text());
 });
}
void PluginStoreWidget::removePlugin(const QString &id,const QString &name){
 if(QMessageBox::question(this,"Uninstall plugin","Remove "+name+" from this computer?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
 QDir dir(pluginDirectory(id));if(!dir.removeRecursively()){QMessageBox::warning(this,"Uninstall failed","Could not remove the plugin folder.");return;}
 status->setText(name+" removed.");rebuild(search->text());
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
