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
#include <QFont>

struct PluginInfo { const char *name; const char *category; const char *description; const char *packageId; const char *website; };
static const PluginInfo plugins[] = {
 {"Git","Source control","Version control and Git command-line tools.","Git.Git","https://git-scm.com/download/win"},
 {"Visual Studio Code","Code editor","Lightweight editor with extensions and integrated terminal.","Microsoft.VisualStudioCode","https://code.visualstudio.com/"},
 {"Python 3","Language runtime","Python runtime for scripts, automation and applications.","Python.Python.3.12","https://www.python.org/downloads/"},
 {"Node.js LTS","Language runtime","JavaScript runtime and npm package manager.","OpenJS.NodeJS.LTS","https://nodejs.org/"},
 {"Docker Desktop","Containers","Build and run containers on Windows. Requires WSL 2 or Hyper-V.","Docker.DockerDesktop","https://www.docker.com/products/docker-desktop/"},
 {"Ubuntu on WSL","Linux environment","Ubuntu distribution for the Windows Subsystem for Linux.","Canonical.Ubuntu.2404","https://learn.microsoft.com/windows/wsl/install"},
 {"Nmap","Network diagnostics","Network discovery and security auditing for systems you are authorized to assess.","Insecure.Nmap","https://nmap.org/download.html"},
 {"Wireshark","Network analysis","Inspect network traffic on networks where you have permission.","WiresharkFoundation.Wireshark","https://www.wireshark.org/download.html"}
};
PluginStoreWidget::PluginStoreWidget(QWidget *parent):QWidget(parent){
 auto *root=new QVBoxLayout(this);root->setContentsMargins(24,20,24,24);root->setSpacing(14);
 auto *title=new QLabel("Plugin Store");title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel("Add trusted developer tools and connect them to your EVILWARE workspace. Installations are performed by Windows Package Manager (winget).");intro->setWordWrap(true);intro->setObjectName("mutedText");root->addWidget(intro);
 search=new QLineEdit;search->setPlaceholderText("Search tools and integrations…");root->addWidget(search);
 auto *hint=new QLabel("Install buttons run winget on this PC. Review each tool's publisher and license before installing. Some tools require administrator approval or a restart.");hint->setWordWrap(true);hint->setObjectName("mutedText");root->addWidget(hint);
 auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);
 listHost=new QWidget;listLayout=new QVBoxLayout(listHost);listLayout->setContentsMargins(0,0,8,0);listLayout->setSpacing(10);scroll->setWidget(listHost);root->addWidget(scroll,1);
 connect(search,&QLineEdit::textChanged,this,&PluginStoreWidget::rebuild);rebuild();
}
void PluginStoreWidget::rebuild(const QString &filter){
 while(auto *item=listLayout->takeAt(0)){if(item->widget())item->widget()->deleteLater();delete item;}
 const QString needle=filter.trimmed();
 for(const auto &p:plugins){
  const QString name=p.name,category=p.category,description=p.description,id=p.packageId;
  if(!needle.isEmpty()&&!name.contains(needle,Qt::CaseInsensitive)&&!category.contains(needle,Qt::CaseInsensitive)&&!description.contains(needle,Qt::CaseInsensitive))continue;
  auto *card=new QFrame;card->setObjectName("pluginCard");auto *row=new QHBoxLayout(card);row->setContentsMargins(16,14,16,14);row->setSpacing(14);
  auto *details=new QVBoxLayout;auto *nameLabel=new QLabel(name);nameLabel->setObjectName("cardTitle");auto *categoryLabel=new QLabel(category+"  ·  "+id);categoryLabel->setObjectName("mutedText");auto *desc=new QLabel(description);desc->setWordWrap(true);desc->setObjectName("bodyText");details->addWidget(nameLabel);details->addWidget(categoryLabel);details->addWidget(desc);row->addLayout(details,1);
  auto *actions=new QVBoxLayout;auto *install=new QPushButton("Install with winget");install->setObjectName("primaryButton");auto *website=new QPushButton("Publisher website");website->setObjectName("secondaryButton");actions->addWidget(install);actions->addWidget(website);actions->addStretch();row->addLayout(actions);
  connect(install,&QPushButton::clicked,this,[this,name,id]{installTool(name,id);});
  const QString url=p.website;connect(website,&QPushButton::clicked,this,[url]{QDesktopServices::openUrl(QUrl(url));});
  listLayout->addWidget(card);
 }
 listLayout->addStretch();
}
void PluginStoreWidget::installTool(const QString &name,const QString &packageId){
 if(!QProcess::startDetached("winget",{"install","--id",packageId,"--exact","--source","winget","--accept-package-agreements","--accept-source-agreements"})){
  QMessageBox::warning(this,"Windows Package Manager unavailable","EVILWARE could not start winget. Update or install App Installer from Microsoft Store, then try again. You can also use the publisher website.");
  return;
 }
 QMessageBox::information(this,"Installation started",name+" installation was handed to Windows Package Manager. Follow any prompts in the installer and restart NEXUS if needed.");
}
