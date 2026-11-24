#include "mainwindow.h"
#include "terminalwidget.h"
#include "editorwidget.h"
#include "projectwidget.h"
#include "cyberlabwidget.h"
#include "pluginstorewidget.h"
#include "modulepage.h"
#include "startupwidget.h"
#include "systeminfo.h"
#include "systemmonitorwidget.h"
#include "lamiaaiwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QFileSystemModel>
#include <QTreeView>
#include <QProcess>
#include <QPlainTextEdit>
#include <QDir>
#include <QListWidgetItem>
#include <QSizePolicy>
#include <QIcon>
#include <QStyle>
#include <QSettings>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QColorDialog>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QPalette>
#include <QFont>

MainWindow::MainWindow(QWidget *p):QMainWindow(p){
 setWindowTitle("EVILWARE | The Unholy Core 3");
 setWindowIcon(QIcon(":/assets/evilware_app_icon.png"));
 resize(1440,900);
 setMinimumSize(1050,680);
 auto *root=new QWidget;root->setObjectName("appRoot");
 auto *outer=new QHBoxLayout(root);outer->setContentsMargins(0,0,0,0);outer->setSpacing(0);
 auto *sidebar=new QFrame;sidebar->setObjectName("sidebar");sidebar->setFixedWidth(258);
 auto *side=new QVBoxLayout(sidebar);side->setContentsMargins(18,20,18,16);side->setSpacing(8);
 auto *brandRow=new QHBoxLayout;
 auto *mark=new QLabel("E");mark->setObjectName("brandMark");
 auto *brandText=new QVBoxLayout;
 auto *brand=new QLabel("EVILWARE");brand->setObjectName("brandName");
 auto *tagline=new QLabel("THE UNHOLY CORE 3");tagline->setObjectName("brandCaption");
 brandText->addWidget(brand);brandText->addWidget(tagline);brandRow->addWidget(mark);brandRow->addLayout(brandText);brandRow->addStretch();side->addLayout(brandRow);
 auto *workspace=new QLabel("WORKSPACE");workspace->setObjectName("sectionCaption");workspace->setContentsMargins(4,20,0,6);side->addWidget(workspace);
 nav=new QListWidget;nav->setObjectName("mainNavigation");nav->setFrameShape(QFrame::NoFrame);nav->setSpacing(3);
 const QStringList items={"Dashboard","Build Apps","Code Studio","Linux Terminal","Windows Terminal","Cyber Lab","Network Tools","Web Security","Digital Forensics","Reverse Engineering","Database Studio","AI Lab","Docker & DevOps","Cloud Manager","File Manager","Git Manager","Plugin Store","System Monitor","Settings","Documentation"};
 const QStringList symbols={"⌂","▦","</>","›_","$_","◇","⌁","◎","▤","⌘","▥","✦","◈","☁","▣","⑂","⊞","◷","⚙","≡"};
 for(int i=0;i<items.size();++i){
  auto *item=new QListWidgetItem(symbols.value(i,"•")+"   "+items[i]);
  item->setData(Qt::UserRole,items[i]);
  item->setSizeHint(QSize(205,40));
  item->setToolTip(items[i]);
  nav->addItem(item);
 }
 side->addWidget(nav,1);
 auto *footer=new QLabel("EVILWARE  ·  THE UNHOLY CORE 3");footer->setObjectName("sidebarFooter");side->addWidget(footer);
 auto *mainArea=new QWidget;mainArea->setAttribute(Qt::WA_TranslucentBackground);mainArea->setObjectName("mainArea");mainArea->setStyleSheet("QWidget#mainArea{background-image:url(:/assets/evilware_core_banner.png);background-position:center;background-repeat:no-repeat;background-color:#080b10;}");auto *mainLayout=new QVBoxLayout(mainArea);mainLayout->setContentsMargins(0,0,0,0);mainLayout->setSpacing(0);
 auto *topbar=new QFrame;topbar->setObjectName("topbar");topbar->setFixedHeight(66);auto *top=new QHBoxLayout(topbar);top->setContentsMargins(26,0,26,0);
 activePage=new QLabel("Dashboard");activePage->setObjectName("topbarTitle");
 auto *workspaceLabel=new QLabel("THE UNHOLY CORE 3  /  LOCAL WORKSPACE");workspaceLabel->setObjectName("mutedText");top->addWidget(activePage);top->addStretch();top->addWidget(workspaceLabel);mainLayout->addWidget(topbar);
 pages=new QStackedWidget;pages->setObjectName("pageStack");pages->setStyleSheet("QStackedWidget#pageStack{background:rgba(7,10,15,178);}");mainLayout->addWidget(pages,1);outer->addWidget(sidebar);outer->addWidget(mainArea,1);setCentralWidget(root);

 windowsTerminal=new TerminalWidget;windowsTerminal->setShellIndex(0);
 linuxTerminal=new TerminalWidget;linuxTerminal->setShellIndex(2);
 editor=new EditorWidget;projects=new ProjectWidget;cyber=new CyberLabWidget;plugins=new PluginStoreWidget;
 connect(projects,&ProjectWidget::projectCreated,editor,&EditorWidget::setProjectFolder);
 addPage(dashboard());                         // 01 Dashboard
 addPage(projects);                             // 02 Build Apps
 addPage(editor);                               // 03 Code Studio
 addPage(linuxTerminal);                        // 04 Linux Terminal
 addPage(windowsTerminal);                      // 05 Windows Terminal
 addPage(cyber);                                // 06 Cyber Lab
 auto addModule=[this](const QString &name,const QString &desc,const QStringList &caps,const QStringList &deps,const QString &state){
  auto *page=new ModulePage(name,desc,caps,deps,state);
  connect(page,&ModulePage::openPluginStoreRequested,this,[this]{nav->setCurrentRow(16);});
  addPage(page);
 };
 addModule("Network Tools","Network discovery and troubleshooting workspace for networks you own or are authorized to assess.",{"DNS and host lookup workflows","Ping, route and interface diagnostics","Network inventory and service visibility (authorized scope)"},{"Nmap","Wireshark","Npcap"},"FOUNDATION / IN DEVELOPMENT"); // 07
 addModule("Web Security","A controlled web application security testing workspace for your own applications and approved test targets.",{"HTTP request inspection","TLS and security-header checks","Safe test plans and findings notes"},{"OWASP ZAP","Burp Suite Community","Browser developer tools"},"FOUNDATION / IN DEVELOPMENT"); // 08
 addModule("Digital Forensics","Evidence-handling workspace for lawful incident response and forensic learning.",{"Case notes and evidence register","File hash calculation and verification","Timeline and artifact review workflows"},{"Autopsy","FTK Imager","Python"},"FOUNDATION / IN DEVELOPMENT"); // 09
 addModule("Reverse Engineering","Binary inspection and software analysis workspace for authorized research.",{"File metadata and hash inspection","Strings and PE metadata workflows","Disassembly and debugging tool launch points"},{"Ghidra","x64dbg","Detect It Easy"},"FOUNDATION / IN DEVELOPMENT"); // 10
 addModule("Database Studio","Database connection and query workspace for development and administration.",{"Connection profiles (planned)","SQL editor and query execution (planned)","Schema browsing and export (planned)"},{"SQLite","PostgreSQL","MySQL / MariaDB"},"FOUNDATION / IN DEVELOPMENT"); // 11
 addPage(new LamiaAIWidget);                    // 12 — LAMIA AI provider and chat workspace
 addModule("Docker & DevOps","Container, build and delivery workspace for software projects.",{"Docker environment checks","Container and image workflows (planned)","Compose and CI/CD helpers (planned)"},{"Docker Desktop","WSL 2","Git"},"FOUNDATION / IN DEVELOPMENT"); // 13
 addModule("Cloud Manager","A unified place to configure and work with cloud provider command-line tools.",{"Provider profile management (planned)","CLI status and account checks (planned)","Deployment workflow helpers (planned)"},{"Azure CLI","AWS CLI","Google Cloud CLI"},"FOUNDATION / IN DEVELOPMENT"); // 14
 addPage(filePage());                           // 15 File Manager
 addPage(gitPage());                            // 16 Git Manager
 addPage(plugins);                              // 17 Plugin Store
 addPage(new SystemMonitorWidget);               // 18 — read-only process inspection
 addPage(settingsPage());                       // 19 Settings
 addModule("Documentation","Product guides, module references, setup instructions and troubleshooting.",{"Open user guide, roadmap and test matrix","Open build and installation guides","Open security policy"},{"Bundled EVILWARE documentation"},"AVAILABLE / LOCAL GUIDES"); // 20

 connect(nav,&QListWidget::currentRowChanged,this,&MainWindow::showPage);nav->setCurrentRow(0);
 setStyleSheet(R"(
 QMainWindow,QWidget#mainArea{background:transparent;color:#e7ebf1;font-family:"Segoe UI",Arial;font-size:10pt}
 QWidget#appRoot{background:#0d1117;color:#e7ebf1}
 QFrame#sidebar{background:#11151b;border-right:1px solid #2a3039}
 QLabel#brandMark{background:#a90014;color:white;font-size:23px;font-weight:700;border-radius:9px;min-width:42px;max-width:42px;min-height:42px;max-height:42px;qproperty-alignment:AlignCenter}
 QLabel#brandName{color:#f5f7fa;font-size:20px;font-weight:700;letter-spacing:1px}
 QLabel#brandCaption,QLabel#sectionCaption,QLabel#sidebarFooter{color:#7f8a99;font-size:8pt;letter-spacing:1px}
 QListWidget#mainNavigation{background:transparent;color:#aeb8c6;border:0;font-size:10pt;outline:0}
 QListWidget#mainNavigation::item{padding:9px 12px;border-radius:7px}
 QListWidget#mainNavigation::item:hover{background:#222a35;color:#f4f7fb}
 QListWidget#mainNavigation::item:selected{background:#43151b;color:#ffffff;border-left:3px solid #e12636;padding-left:9px}
 QFrame#topbar{background:rgba(17,20,26,235);border-bottom:1px solid #2a3039}
 QLabel#topbarTitle{font-size:12pt;font-weight:600;color:#f3f5f8}
 QLabel#mutedText{color:#909baa}
 QLabel#pageTitle{font-size:23px;font-weight:650;color:#f4f6f9}
 QLabel#cardTitle{font-size:12pt;font-weight:600;color:#f0f3f7}
 QLabel#bodyText{color:#c4ccd6}
 QFrame#pluginCard,QFrame#infoCard{background:rgba(20,24,31,225);border:1px solid #2c3541;border-radius:10px}
 QPushButton{background:#283342;color:#e8edf4;border:1px solid #394657;border-radius:6px;padding:8px 13px}
 QPushButton:hover{background:#344357}
 QPushButton#primaryButton{background:#a90014;border:1px solid #e12636;color:white;font-weight:600}
 QPushButton#primaryButton:hover{background:#c81729}
 QPushButton#secondaryButton{background:#202833;color:#cbd5e1}
 QLineEdit,QComboBox,QPlainTextEdit,QTreeView{background:#141a22;color:#e7edf5;border:1px solid #35404e;border-radius:6px;padding:8px;selection-background-color:#315d9a}
 QPlainTextEdit{font-family:Consolas,monospace}
 QTreeView::item{padding:5px}
 QTreeView::item:selected{background:#263d5e}
 QScrollBar:vertical{background:#171c23;width:10px;margin:0}
 QScrollBar::handle:vertical{background:#3a4655;border-radius:4px;min-height:24px}
 QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0}
 )");
 setProperty("baseStyleSheet", styleSheet());
 QSettings saved("EVILWARE","TheUnholyCore3");
 applyAppearance(saved.value("appearance/theme","Graphite Red").toString(),
                 saved.value("appearance/background","Original EVILWARE artwork").toString(),
                 QColor(saved.value("appearance/accent","#e12636").toString()),
                 saved.value("appearance/fontSize",10).toInt());
}
void MainWindow::addPage(QWidget *page){pages->addWidget(page);}
void MainWindow::showPage(int row){if(row<0||row>=pages->count())return;pages->setCurrentIndex(row);activePage->setText(nav->item(row)->data(Qt::UserRole).toString());}
void MainWindow::applyAppearance(const QString &theme,const QString &background,const QColor &accent,int fontSize){
 QColor window,sidebar,panel,text,muted,border,field,selection;
 if(theme=="Light"){
  window=QColor("#eef2f6");sidebar=QColor("#e2e8ef");panel=QColor("#ffffff");text=QColor("#172333");muted=QColor("#566579");border=QColor("#c6d0dc");field=QColor("#ffffff");selection=QColor("#dbeafe");
 }else if(theme=="Midnight Blue"){
  window=QColor("#081321");sidebar=QColor("#0c1b2d");panel=QColor("#102238");text=QColor("#e4efff");muted=QColor("#91a8c4");border=QColor("#1e3a5c");field=QColor("#0b1929");selection=QColor("#173b60");
 }else{
  window=QColor("#0d1117");sidebar=QColor("#11151b");panel=QColor("#141a22");text=QColor("#e7ebf1");muted=QColor("#909baa");border=QColor("#2a3039");field=QColor("#141a22");selection=QColor("#43151b");
 }
 if(!accent.isValid())return;
 QPalette pal=qApp->palette();
 pal.setColor(QPalette::Window,window);pal.setColor(QPalette::WindowText,text);
 pal.setColor(QPalette::Base,field);pal.setColor(QPalette::AlternateBase,panel);
 pal.setColor(QPalette::Text,text);pal.setColor(QPalette::Button,panel);
 pal.setColor(QPalette::ButtonText,text);pal.setColor(QPalette::Highlight,accent);
 pal.setColor(QPalette::HighlightedText,Qt::white);pal.setColor(QPalette::ToolTipBase,panel);
 pal.setColor(QPalette::ToolTipText,text);qApp->setPalette(pal);
 qApp->setFont(QFont("Segoe UI",qBound(8,fontSize,16)));
 const QString css=property("baseStyleSheet").toString()+QString(R"(
 QWidget{color:%1;}
 QWidget#appRoot{background:%2;color:%1;}
 QFrame#sidebar{background:%3;border-right:1px solid %4;}
 QFrame#topbar{background:%3;border-bottom:1px solid %4;}
 QLabel#topbarTitle,QLabel#pageTitle,QLabel#cardTitle{color:%1;}
 QLabel#mutedText,QLabel#sectionCaption,QLabel#sidebarFooter{color:%5;}
 QFrame#infoCard,QFrame#pluginCard{background:%6;border:1px solid %4;}
 QPushButton{background:%6;color:%1;border:1px solid %4;border-radius:6px;padding:8px 13px;}
 QPushButton:hover{border-color:%7;background:%8;}
 QPushButton#primaryButton{background:%7;border:1px solid %7;color:white;font-weight:600;}
 QPushButton#primaryButton:hover{background:%9;}
 QLineEdit,QComboBox,QPlainTextEdit,QTreeView,QSpinBox{background:%10;color:%1;border:1px solid %4;border-radius:6px;padding:8px;selection-background-color:%7;}
 QGroupBox{border:1px solid %4;border-radius:8px;margin-top:12px;padding:12px 10px 10px;font-weight:600;}
 QGroupBox::title{subcontrol-origin:margin;left:12px;padding:0 5px;color:%5;}
 QListWidget#mainNavigation{background:transparent;color:%1;}
 QListWidget#mainNavigation::item{padding:9px 12px;border-radius:7px;}
 QListWidget#mainNavigation::item:hover{background:%8;}
 QListWidget#mainNavigation::item:selected{background:%8;color:%1;border-left:3px solid %7;padding-left:9px;}
 QTabWidget::pane{border:1px solid %4;background:%6;}
 QMenu{background:%6;color:%1;border:1px solid %4;}
 QMenu::item:selected{background:%8;}
 QScrollBar:vertical{background:%2;width:10px;margin:0;}
 QScrollBar::handle:vertical{background:%4;border-radius:4px;min-height:24px;}
 )").arg(text.name()).arg(window.name()).arg(sidebar.name()).arg(border.name()).arg(muted.name()).arg(panel.name()).arg(accent.name()).arg(selection.name()).arg(accent.darker(125).name()).arg(field.name());
 setStyleSheet(css);
 const QString bg=(theme=="Light" && background=="Original EVILWARE artwork")?"background:#eef2f6;":background=="Original EVILWARE artwork"?"background-image:url(:/assets/evilware_core_banner.png);background-position:center;background-repeat:no-repeat;":background=="Deep red"?"background:#21090e;":"background:#10151c;";
 if(auto *area=findChild<QWidget*>("mainArea"))area->setStyleSheet("QWidget#mainArea{"+bg+"}");
 if(pages)pages->setStyleSheet(theme=="Light"?"QStackedWidget#pageStack{background:rgba(255,255,255,220);}":"QStackedWidget#pageStack{background:rgba(7,10,15,178);}");
}
QWidget* MainWindow::dashboard(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(28,26,28,28);v->setSpacing(18);
 auto *title=new QLabel("Welcome to EVILWARE");title->setObjectName("pageTitle");v->addWidget(title);
 auto *sub=new QLabel("The Unholy Core 3 — your development, systems and security workspaces in one desktop command centre.");sub->setObjectName("mutedText");sub->setWordWrap(true);v->addWidget(sub);
 auto s=readSystemInfo();auto *grid=new QGridLayout;grid->setSpacing(12);
 QStringList labels={"Operating system","Architecture","Device","Processor","Memory","Edition"};
 QStringList values={s.os,s.architecture,s.host,s.cpu,s.memory,"The Unholy Core 3"};
 for(int i=0;i<labels.size();++i){auto *card=new QFrame;card->setObjectName("infoCard");auto *l=new QVBoxLayout(card);l->setContentsMargins(16,14,16,14);auto *a=new QLabel(labels[i]);a->setObjectName("mutedText");auto *b=new QLabel(values[i]);b->setStyleSheet("font-size:13pt;font-weight:600");b->setWordWrap(true);l->addWidget(a);l->addWidget(b);grid->addWidget(card,i/3,i%3);}
 v->addLayout(grid);
 auto *section=new QLabel("All workspaces");section->setObjectName("pageTitle");section->setStyleSheet("font-size:16pt;font-weight:600");v->addWidget(section);
 auto *list=new QLabel("20 dedicated sections are available in the navigation. Existing working tools include project creation, source editing, local terminals, basic network diagnostics, file browsing, Git commands and the plugin installer. Other workspaces are being implemented and are clearly marked.");list->setObjectName("bodyText");list->setWordWrap(true);v->addWidget(list);v->addStretch();return w;
}
QWidget* MainWindow::filePage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);auto *title=new QLabel("File Manager");title->setObjectName("pageTitle");v->addWidget(title);
 auto *model=new QFileSystemModel(w);model->setRootPath(QDir::homePath());auto *tree=new QTreeView;tree->setModel(model);tree->setRootIndex(model->index(QDir::homePath()));tree->setColumnWidth(0,380);v->addWidget(tree,1);return w;
}
QWidget* MainWindow::gitPage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);v->setSpacing(10);
 auto *title=new QLabel("Git Manager");title->setObjectName("pageTitle");v->addWidget(title);
 auto *label=new QLabel("Run explicit Git operations in a selected working folder. Fetch, pull, push, staging and commits only run when you click their controls.");label->setObjectName("mutedText");label->setWordWrap(true);v->addWidget(label);
 auto *path=new QLineEdit(QDir::homePath());auto *browse=new QPushButton("Choose repository…");auto *pathRow=new QHBoxLayout;pathRow->addWidget(path,1);pathRow->addWidget(browse);v->addLayout(pathRow);
 auto *status=new QLabel("Ready");status->setObjectName("mutedText");v->addWidget(status);
 auto *out=new QPlainTextEdit;out->setReadOnly(true);out->setPlaceholderText("Git command output appears here…");v->addWidget(out,1);
 auto runGit=[w,path,out,status](const QStringList &args){
  const QString folder=QDir::cleanPath(path->text().trimmed());
  if(folder.isEmpty()||!QDir(folder).exists()){status->setText("Choose an existing repository folder.");return;}
  auto *process=new QProcess(w);process->setWorkingDirectory(folder);
  out->appendPlainText("\n> git "+args.join(" "));
  status->setText("Running Git…");
  QObject::connect(process,&QProcess::readyReadStandardOutput,[process,out]{const auto data=process->readAllStandardOutput();if(!data.isEmpty())out->appendPlainText(QString::fromLocal8Bit(data));});
  QObject::connect(process,&QProcess::readyReadStandardError,[process,out]{const auto data=process->readAllStandardError();if(!data.isEmpty())out->appendPlainText(QString::fromLocal8Bit(data));});
  QObject::connect(process,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),w,[process,status,out](int code,QProcess::ExitStatus state){
   const bool ok=state==QProcess::NormalExit&&code==0;status->setText(ok?"Completed successfully":QString("Git failed (exit %1)").arg(code));
   const auto a=process->readAllStandardOutput(),b=process->readAllStandardError();if(!a.isEmpty())out->appendPlainText(QString::fromLocal8Bit(a));if(!b.isEmpty())out->appendPlainText(QString::fromLocal8Bit(b));
   process->deleteLater();
  });
  QObject::connect(process,&QProcess::errorOccurred,w,[process,status,out](QProcess::ProcessError){status->setText("Git could not start");out->appendPlainText("[EVILWARE] Git was not found. Install Git and ensure git.exe is on PATH.");process->deleteLater();});
  process->start("git",args);
 };
 connect(browse,&QPushButton::clicked,w,[path,w]{const QString folder=QFileDialog::getExistingDirectory(w,"Choose Git repository",path->text());if(!folder.isEmpty())path->setText(folder);});
 auto *grid=new QGridLayout;grid->setSpacing(7);
 const QList<QPair<QString,QStringList>> actions={
  {"Status",{"status","--short","--branch"}},
  {"Recent commits",{"log","--oneline","-15"}},
  {"Branches",{"branch","--all"}},
  {"Diff",{"diff","--"}},
  {"Stage all changes",{"add","-A"}},
  {"Fetch",{"fetch","--all","--prune"}},
  {"Pull",{"pull","--ff-only"}},
  {"Push",{"push"}}
 };
 for(int i=0;i<actions.size();++i){auto *button=new QPushButton(actions[i].first);grid->addWidget(button,i/4,i%4);connect(button,&QPushButton::clicked,w,[runGit,args=actions[i].second]{runGit(args);});}
 auto *commit=new QPushButton("Commit staged changes");commit->setObjectName("primaryButton");
 connect(commit,&QPushButton::clicked,w,[w,runGit]{bool ok=false;const QString message=QInputDialog::getText(w,"Git commit","Commit message:",QLineEdit::Normal,QString(),&ok);if(!ok)return;if(message.trimmed().isEmpty()){QMessageBox::warning(w,"Commit","Enter a non-empty commit message.");return;}runGit({"commit","-m",message});});
 auto *refresh=new QPushButton("Refresh status");connect(refresh,&QPushButton::clicked,w,[runGit]{runGit({"status","--short","--branch"});});
 auto *extra=new QHBoxLayout;extra->addWidget(commit);extra->addWidget(refresh);extra->addStretch();v->addLayout(grid);v->addLayout(extra);
 return w;
}
QWidget* MainWindow::settingsPage(){
 auto *w=new QWidget;auto *outer=new QVBoxLayout(w);outer->setContentsMargins(24,20,24,20);
 auto *scroll=new QScrollArea;scroll->setWidgetResizable(true);scroll->setFrameShape(QFrame::NoFrame);outer->addWidget(scroll);
 auto *content=new QWidget;auto *v=new QVBoxLayout(content);v->setSpacing(14);scroll->setWidget(content);
 auto *title=new QLabel("Settings Center");title->setObjectName("pageTitle");v->addWidget(title);
 auto *hint=new QLabel("Configure the complete EVILWARE workstation. Appearance changes apply across all workspaces and persist after restart.");hint->setObjectName("mutedText");hint->setWordWrap(true);v->addWidget(hint);
 QSettings prefs("EVILWARE","TheUnholyCore3");
 auto *ownerBox=new QGroupBox("LOCAL PROFILE");auto *ownerForm=new QFormLayout(ownerBox);
 auto *ownerName=new QLineEdit(prefs.value("owner/name","Local Owner").toString());
 auto *ownerEmail=new QLineEdit(prefs.value("owner/email").toString());
 auto *ownerRole=new QLineEdit(prefs.value("owner/role","Local owner").toString());
 ownerForm->addRow("Display name",ownerName);ownerForm->addRow("Email / contact",ownerEmail);ownerForm->addRow("Role",ownerRole);v->addWidget(ownerBox);
 auto *appearance=new QGroupBox("APPEARANCE");auto *af=new QFormLayout(appearance);
 auto *theme=new QComboBox;theme->addItems({"Graphite Red","Midnight Blue","Light"});theme->setCurrentText(prefs.value("appearance/theme","Graphite Red").toString());
 auto *background=new QComboBox;background->addItems({"Original EVILWARE artwork","Plain graphite","Deep red"});background->setCurrentText(prefs.value("appearance/background","Original EVILWARE artwork").toString());
 auto *fontSize=new QSpinBox;fontSize->setRange(8,16);fontSize->setValue(prefs.value("appearance/fontSize",10).toInt());
 auto *accent=new QPushButton("Choose accent colour");auto *accentColor=new QColor(prefs.value("appearance/accent","#e12636").toString());accent->setStyleSheet("background:"+accentColor->name()+";color:white;font-weight:600;");
 connect(accent,&QPushButton::clicked,w,[accent,accentColor,w]{QColor chosen=QColorDialog::getColor(*accentColor,w,"Choose EVILWARE accent");if(chosen.isValid()){*accentColor=chosen;accent->setStyleSheet("background:"+accentColor->name()+";color:white;font-weight:600;");}});
 af->addRow("Colour theme",theme);af->addRow("Workspace background",background);af->addRow("Accent colour",accent);af->addRow("Interface font size",fontSize);v->addWidget(appearance);
 auto *behaviour=new QGroupBox("APPLICATION BEHAVIOUR");auto *bf=new QFormLayout(behaviour);
 auto *defaultShell=new QComboBox;defaultShell->addItems({"PowerShell","Command Prompt","WSL"});defaultShell->setCurrentText(prefs.value("application/defaultShell","PowerShell").toString());
 auto *autosave=new QCheckBox("Enable editor autosave");autosave->setChecked(prefs.value("application/autosave",false).toBool());
 auto *confirm=new QCheckBox("Confirm potentially destructive actions");confirm->setChecked(prefs.value("security/confirmDestructive",true).toBool());
 auto *notifications=new QCheckBox("Enable application notifications");notifications->setChecked(prefs.value("application/notifications",true).toBool());
 bf->addRow("Default terminal",defaultShell);bf->addRow(autosave);bf->addRow(confirm);bf->addRow(notifications);v->addWidget(behaviour);
 auto *security=new QGroupBox("SECURITY & PRIVACY");auto *sv=new QVBoxLayout(security);
 auto *securityHint=new QLabel("API credentials are configured in LAMIA AI. EVILWARE will not silently execute commands, install dependencies or modify project files without an explicit action.");securityHint->setWordWrap(true);securityHint->setObjectName("bodyText");sv->addWidget(securityHint);v->addWidget(security);
 auto *save=new QPushButton("Save and apply all settings");save->setObjectName("primaryButton");v->addWidget(save,0,Qt::AlignLeft);
 connect(save,&QPushButton::clicked,w,[this,&prefs,ownerName,ownerEmail,ownerRole,theme,background,fontSize,accentColor,defaultShell,autosave,confirm,notifications]{
  prefs.setValue("owner/name",ownerName->text().trimmed());prefs.setValue("owner/email",ownerEmail->text().trimmed());prefs.setValue("owner/role",ownerRole->text().trimmed());
  prefs.setValue("appearance/theme",theme->currentText());prefs.setValue("appearance/background",background->currentText());prefs.setValue("appearance/accent",accentColor->name());prefs.setValue("appearance/fontSize",fontSize->value());
  prefs.setValue("application/defaultShell",defaultShell->currentText());prefs.setValue("application/autosave",autosave->isChecked());prefs.setValue("security/confirmDestructive",confirm->isChecked());prefs.setValue("application/notifications",notifications->isChecked());prefs.sync();
  applyAppearance(theme->currentText(),background->currentText(),*accentColor,fontSize->value());
  QMessageBox::information(this,"Settings saved","EVILWARE preferences have been saved and applied across the application.");
 });
 v->addStretch();return w;
}
