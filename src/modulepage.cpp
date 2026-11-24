#include "modulepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QPixmap>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QCryptographicHash>
#include <QProcess>
#include <QRegularExpression>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QUrl>
#include <functional>

ModulePage::ModulePage(const QString &name,const QString &description,const QStringList &capabilities,const QStringList &dependencies,const QString &status,QWidget *parent):QWidget(parent){
 auto *outer=new QVBoxLayout(this);outer->setContentsMargins(0,0,0,0);outer->setSpacing(0);
 auto *scroll=new QScrollArea(this);scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
 auto *content=new QWidget;content->setObjectName("moduleScrollContent");scroll->setWidget(content);outer->addWidget(scroll);
 auto *root=new QVBoxLayout(content);root->setContentsMargins(28,26,28,28);root->setSpacing(14);
 auto *eyebrow=new QLabel("EVILWARE WORKSPACE MODULE");eyebrow->setObjectName("mutedText");root->addWidget(eyebrow);
 auto *title=new QLabel(name);title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel(description);intro->setObjectName("bodyText");intro->setWordWrap(true);root->addWidget(intro);
 if(name=="Reverse Engineering"){
  auto *banner=new QLabel;banner->setObjectName("workspaceArtwork");banner->setPixmap(QPixmap(":/assets/evilware_reverse_engineering_banner.jpg"));banner->setScaledContents(true);banner->setFixedHeight(164);banner->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);root->addWidget(banner);
 }
 auto *statusCard=new QFrame;statusCard->setObjectName("infoCard");auto *statusLayout=new QHBoxLayout(statusCard);statusLayout->setContentsMargins(16,12,16,12);
 auto *statusLabel=new QLabel("MODULE STATUS");statusLabel->setObjectName("mutedText");
 auto *statusValue=new QLabel(status);statusValue->setObjectName("cardTitle");statusLayout->addWidget(statusLabel);statusLayout->addStretch();statusLayout->addWidget(statusValue);root->addWidget(statusCard);
 auto *grid=new QHBoxLayout;grid->setSpacing(14);
 auto makeCard=[&](const QString &heading,const QStringList &items){
  auto *card=new QFrame;card->setObjectName("infoCard");auto *v=new QVBoxLayout(card);v->setContentsMargins(16,14,16,16);
  auto *h=new QLabel(heading);h->setObjectName("cardTitle");v->addWidget(h);
  for(const QString &item:items){auto *line=new QLabel(QString("•  %1").arg(item));line->setObjectName("bodyText");line->setWordWrap(true);v->addWidget(line);}
  v->addStretch();return card;
 };
 grid->addWidget(makeCard("WORKSPACE CAPABILITIES",capabilities),3);
 grid->addWidget(makeCard("INTEGRATIONS & DEPENDENCIES",dependencies),2);
 root->addLayout(grid);

 auto *toolsCard=new QFrame;toolsCard->setObjectName("infoCard");auto *toolsLayout=new QVBoxLayout(toolsCard);toolsLayout->setContentsMargins(16,14,16,14);
 auto *toolsTitle=new QLabel("AVAILABLE LOCAL TOOLS");toolsTitle->setObjectName("cardTitle");toolsLayout->addWidget(toolsTitle);
 auto *descriptionLabel=new QLabel("Actions run locally under your Windows account. External tools are optional and are never installed automatically.");descriptionLabel->setObjectName("mutedText");descriptionLabel->setWordWrap(true);toolsLayout->addWidget(descriptionLabel);
 auto *toolbar=new QHBoxLayout;auto *target=new QLineEdit;target->setPlaceholderText("Hostname, URL, database path or command input");toolbar->addWidget(target,1);
 auto *output=new QPlainTextEdit;output->setReadOnly(true);output->setPlaceholderText("Tool output will appear here...");output->setMinimumHeight(150);toolsLayout->addLayout(toolbar);toolsLayout->addWidget(output);
 auto *buttons=new QHBoxLayout;toolsLayout->addLayout(buttons);root->addWidget(toolsCard);
 auto *process=new QProcess(this);
 auto append=[output](const QString &s){if(!s.isEmpty())output->appendPlainText(s);};
 connect(process,&QProcess::readyReadStandardOutput,this,[process,append]{append(QString::fromLocal8Bit(process->readAllStandardOutput()));});
 connect(process,&QProcess::readyReadStandardError,this,[process,append]{append(QString::fromLocal8Bit(process->readAllStandardError()));});
 connect(process,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[append](int code,QProcess::ExitStatus state){append(QString("\n[Process %1, exit code %2]").arg(state==QProcess::NormalExit?"finished":"crashed").arg(code));});
 connect(process,&QProcess::errorOccurred,this,[append](QProcess::ProcessError){append("[Process could not start. Check that the selected tool is installed and available on PATH.]");});
 auto run=[this,process,output,append](const QString &program,const QStringList &args){
  if(process->state()!=QProcess::NotRunning){append("[A command is already running.]");return;}
  output->clear();append("> "+program+" "+args.join(" "));
  process->start(program,args);
 };
 auto addButton=[buttons](const QString &label,const std::function<void()> &action,bool primary=false){
  auto *b=new QPushButton(label);if(primary)b->setObjectName("primaryButton");buttons->addWidget(b);QObject::connect(b,&QPushButton::clicked,b,action);
 };

 if(name=="Network Tools"){
  target->setPlaceholderText("Hostname or IP address");
  auto validHost=[target,append](){const QString t=target->text().trimmed();static const QRegularExpression re("^[A-Za-z0-9][A-Za-z0-9.:-]{0,252}$");if(!re.match(t).hasMatch()){append("Enter a valid hostname or IP address.");return QString();}return t;};
#ifdef Q_OS_WIN
  addButton("Ping",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("ping",{"-n","4",t});},true);
  addButton("DNS lookup",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("nslookup",{t});});
  addButton("Trace route",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("tracert",{"-d",t});});
  addButton("IP configuration",[run]{run("ipconfig",{"/all"});});
#else
  addButton("Ping",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("ping",{"-c","4",t});},true);
  addButton("DNS lookup",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("nslookup",{t});});
  addButton("Trace route",[run,validHost]{auto t=validHost();if(!t.isEmpty())run("traceroute",{t});});
  addButton("Interfaces",[run]{run("ip",{"address"});});
#endif
 }else if(name=="Web Security"){
  target->setPlaceholderText("https://your-domain.example");
  addButton("Inspect HTTP headers",[run,target,append]{
   QString url=target->text().trimmed();if(!url.startsWith("https://")&&!url.startsWith("http://")){append("Enter a full http:// or https:// URL.");return;}
   if(url.contains(QRegularExpression("[\\s\\r\\n]"))){append("URL contains invalid whitespace.");return;}
#ifdef Q_OS_WIN
   run("curl.exe",{"--head","--location","--max-time","15","--max-redirs","5","--",url});
#else
   run("curl",{"--head","--location","--max-time","15","--max-redirs","5","--",url});
#endif
  },true);
  addButton("TLS certificate summary",[run,target,append]{
   QString url=target->text().trimmed();QRegularExpression re("^https://([A-Za-z0-9.-]+)(?::[0-9]+)?(?:/.*)?$");auto m=re.match(url);if(!m.hasMatch()){append("Enter an HTTPS URL with a hostname.");return;}
#ifdef Q_OS_WIN
   run("curl.exe",{"-Iv","--max-time","15","--",url});
#else
   run("curl",{"-Iv","--max-time","15","--",url});
#endif
  });
 }else if(name=="Digital Forensics"||name=="Reverse Engineering"){
  target->setReadOnly(true);target->setPlaceholderText("Choose a file to inspect");
  addButton("Choose evidence/file",[this,target,output,name]{
   QString file=QFileDialog::getOpenFileName(this,"Select a file to inspect");if(file.isEmpty())return;
   target->setText(file);QFileInfo info(file);QFile f(file);output->clear();
   output->appendPlainText("FILE: "+info.fileName());output->appendPlainText("PATH: "+info.absoluteFilePath());
   output->appendPlainText("SIZE: "+QString::number(info.size())+" bytes");output->appendPlainText("MODIFIED: "+info.lastModified().toString(Qt::ISODate));
   if(!f.open(QIODevice::ReadOnly)){output->appendPlainText("ERROR: unable to read file.");return;}
   QCryptographicHash hash(QCryptographicHash::Sha256);QByteArray sample;quint64 printable=0;
   while(!f.atEnd()){
    QByteArray chunk=f.read(1024*1024);hash.addData(chunk);
    if(name=="Reverse Engineering"){
     for(unsigned char c:chunk)if((c>=32&&c<=126)||c=='\\n'||c=='\\r'||c=='\\t')++printable;
     if(sample.size()<1024*1024)sample+=chunk.left(1024*1024-sample.size());
    }
   }
   output->appendPlainText("SHA-256: "+hash.result().toHex());
   if(name=="Digital Forensics")output->appendPlainText("Evidence note: hash recorded for integrity verification. Work on a copy and preserve the original.");
   else{
    output->appendPlainText("Printable-byte estimate: "+QString::number(printable));
    output->appendPlainText("\n--- Printable strings (first 100) ---");
    const auto matches=QRegularExpression("[ -~]{5,}").globalMatch(QString::fromLatin1(sample));
    int count=0;auto it=matches;while(it.hasNext()&&count<100){output->appendPlainText(it.next().captured());++count;}
    output->appendPlainText("\nStatic metadata/string inspection only; no code is executed.");
   }
  },true);
  addButton("Clear",[output,target]{output->clear();target->clear();});
 }else if(name=="Database Studio"){
  target->setPlaceholderText("Path to an SQLite database file");
  addButton("Open SQLite database",[this,target,output,append]{
   QString file=QFileDialog::getOpenFileName(this,"Select SQLite database",QDir::homePath(),"SQLite databases (*.db *.sqlite *.sqlite3);;All files (*.*)");
   if(file.isEmpty())return;target->setText(file);QFileInfo info(file);
   output->setPlainText("Selected database: "+info.absoluteFilePath()+"\nSize: "+QString::number(info.size())+" bytes\n\nUse the sqlite3 command-line tool to query it. This workspace currently supports read-only query templates only.");
  },true);
  addButton("List tables (sqlite3)",[run,target,append]{
   const QString file=target->text().trimmed();if(file.isEmpty()){append("Choose a database file first.");return;}
   run("sqlite3",{"-readonly",file,"SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;"});
  });
  addButton("SQLite version",[run]{run("sqlite3",{"--version"});});
 }else if(name=="AI Lab"){
  target->setPlaceholderText("Local Ollama model name, e.g. llama3.2");
  auto *prompt=new QLineEdit;prompt->setPlaceholderText("Enter a prompt to send to the selected local model");toolsLayout->addWidget(prompt);
  addButton("Ollama version",[run]{run("ollama",{"--version"});},true);
  addButton("List local models",[run]{run("ollama",{"list"});});
  addButton("Running models",[run]{run("ollama",{"ps"});});
  addButton("Send prompt",[run,target,prompt,append]{QString model=target->text().trimmed();QString text=prompt->text().trimmed();if(!QRegularExpression("^[A-Za-z0-9._:-]{1,100}$").match(model).hasMatch()){append("Enter a valid local Ollama model tag.");return;}if(text.isEmpty()){append("Enter a prompt first.");return;}run("ollama",{"run",model,text});});
 }else if(name=="Docker & DevOps"){
  addButton("Docker version",[run]{run("docker",{"version"});},true);
  addButton("Containers",[run]{run("docker",{"ps","-a"});});
  addButton("Images",[run]{run("docker",{"images"});});
  addButton("Compose version",[run]{run("docker",{"compose","version"});});
 }else if(name=="Cloud Manager"){
  addButton("Azure CLI",[run]{run("az",{"version"});},true);
  addButton("AWS CLI",[run]{run("aws",{"--version"});});
  addButton("Google Cloud CLI",[run]{run("gcloud",{"version"});});
  addButton("Azure account status",[run]{run("az",{"account","show","--output","table"});});
 }else if(name=="Documentation"){
  auto openGuide=[output,append](const QString &relativePath){
   const QString path=QDir(QCoreApplication::applicationDirPath()).filePath(relativePath);
   if(!QFileInfo::exists(path)){append("Guide is not included in this installation: "+relativePath);return;}
   if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))append("Could not open guide: "+path);
  };
  addButton("User guide",[openGuide]{openGuide("docs/USER_GUIDE.md");},true);
  addButton("Engineering roadmap",[openGuide]{openGuide("ROADMAP.md");});
  addButton("Testing matrix",[openGuide]{openGuide("TESTING.md");});
  addButton("Build guide",[openGuide]{openGuide("BUILD.md");});
  addButton("Installation guide",[openGuide]{openGuide("INSTALLATION.md");});
  addButton("Security policy",[openGuide]{openGuide("SECURITY.md");});
 }else{
  addButton("Check tool availability",[run]{run("git",{"--version"});},true);
 }
 root->addStretch();
 auto *note=new QLabel("Safety: commands are launched as separate processes with fixed argument lists. Review targets and output. No elevated privileges or background installation are requested.");note->setObjectName("mutedText");note->setWordWrap(true);root->addWidget(note);
 auto *plugins=new QPushButton("Open Plugin Store");plugins->setObjectName("secondaryButton");root->addWidget(plugins,0,Qt::AlignLeft);
 connect(plugins,&QPushButton::clicked,this,&ModulePage::openPluginStoreRequested);
}
