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

MainWindow::MainWindow(QWidget *p):QMainWindow(p){
 setWindowTitle("EVILWARE | The Unholy Core 3");
 setWindowIcon(QIcon());
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
 const QList<QStyle::StandardPixmap> glyphs={QStyle::SP_ComputerIcon,QStyle::SP_DirOpenIcon,QStyle::SP_FileDialogDetailedView,QStyle::SP_DesktopIcon,QStyle::SP_CommandLink,QStyle::SP_ComputerIcon,QStyle::SP_DriveNetIcon,QStyle::SP_DialogApplyButton,QStyle::SP_FileDialogInfoView,QStyle::SP_FileIcon,QStyle::SP_DriveHDIcon,QStyle::SP_MediaPlay,QStyle::SP_DriveDVDIcon,QStyle::SP_DriveNetIcon,QStyle::SP_DirIcon,QStyle::SP_BrowserReload,QStyle::SP_DialogOpenButton,QStyle::SP_ComputerIcon,QStyle::SP_FileDialogContentsView,QStyle::SP_MessageBoxInformation};
 for(int i=0;i<items.size();++i){auto*item=new QListWidgetItem(style()->standardIcon(glyphs.value(i,QStyle::SP_FileIcon)),items[i]);item->setSizeHint(QSize(205,40));item->setToolTip(items[i]);nav->addItem(item);}
 side->addWidget(nav,1);
 auto *footer=new QLabel("EVILWARE  ·  THE UNHOLY CORE 3");footer->setObjectName("sidebarFooter");side->addWidget(footer);
 auto *mainArea=new QWidget;mainArea->setAttribute(Qt::WA_TranslucentBackground);mainArea->setObjectName("mainArea");mainArea->setStyleSheet("QWidget#mainArea{background-image:url(:/assets/evilware_reverse_engineering_banner.jpg);background-position:center;background-repeat:no-repeat;background-color:#080b10;}");auto *mainLayout=new QVBoxLayout(mainArea);mainLayout->setContentsMargins(0,0,0,0);mainLayout->setSpacing(0);
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
 addModule("AI Lab","Local and hosted AI development workspace with model and API integrations.",{"Model endpoint configuration (planned)","Prompt and chat workbench (planned)","Local model runtime integration (planned)"},{"Ollama","Python","API provider credentials"},"FOUNDATION / IN DEVELOPMENT"); // 12
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
 QMainWindow,QWidget#mainArea{background:transparent;color:#e7ebf1;font-family:"Segoe UI",Arial;font-size:10pt}\n QWidget#appRoot{background:#0d1117;color:#e7ebf1}
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
 const QString savedTheme=saved.value("appearance/theme","Graphite Red").toString();
 const QString savedBackground=saved.value("appearance/background","Original EVILWARE artwork").toString();
 QString persistedTheme;
 if(savedTheme=="Light") persistedTheme="QWidget#appRoot{background:#edf1f5;color:#17202b;} QFrame#sidebar{background:#e0e6ed;border-right:1px solid #bac4d0;} QFrame#topbar{background:rgba(225,231,238,245);border-bottom:1px solid #bac4d0;} QLabel#topbarTitle,QLabel#pageTitle,QLabel#cardTitle{color:#182333;} QLabel#bodyText{color:#344154;} QFrame#infoCard,QFrame#pluginCard{background:rgba(255,255,255,235);border:1px solid #c6d0dc;} QLineEdit,QComboBox,QPlainTextEdit,QTreeView{background:#fff;color:#182333;border:1px solid #bac4d0;}";
 else if(savedTheme=="Midnight Blue") persistedTheme="QWidget#appRoot{background:#081321;color:#e4efff;} QFrame#sidebar{background:#0c1b2d;border-right:1px solid #1e3a5c;} QFrame#topbar{background:rgba(8,22,39,240);border-bottom:1px solid #1e3a5c;} QListWidget#mainNavigation::item:selected{background:#173b60;border-left:3px solid #4da3ff;} QPushButton#primaryButton{background:#155da0;border:1px solid #4da3ff;color:white;}";
 else persistedTheme="QWidget#appRoot{background:#0d1117;color:#e7ebf1;} QFrame#sidebar{background:#11151b;border-right:1px solid #2a3039;} QFrame#topbar{background:rgba(17,20,26,235);border-bottom:1px solid #2a3039;} QListWidget#mainNavigation::item:selected{background:#43151b;border-left:3px solid #e12636;} QPushButton#primaryButton{background:#a90014;border:1px solid #e12636;color:white;}";
 const QString persistedBackground=savedBackground=="Original EVILWARE artwork"?"background-image:url(:/assets/evilware_reverse_engineering_banner.jpg);background-position:center;background-repeat:no-repeat;":savedBackground=="Deep red"?"background:#21090e;":"background:#10151c;";
 setStyleSheet(property("baseStyleSheet").toString()+persistedTheme+" QWidget#mainArea{"+persistedBackground+"}");
}
void MainWindow::addPage(QWidget *page){pages->addWidget(page);}
void MainWindow::showPage(int row){if(row<0||row>=pages->count())return;pages->setCurrentIndex(row);activePage->setText(nav->item(row)->text());}
QWidget* MainWindow::dashboard(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(28,26,28,28);v->setSpacing(18);
 auto *title=new QLabel("Welcome to EVILWARE");title->setObjectName("pageTitle");v->addWidget(title);
 auto *sub=new QLabel("The Unholy Core 3 — your development, systems and security workspaces in one desktop command centre.");sub->setObjectName("mutedText");sub->setWordWrap(true);v->addWidget(sub);
 auto s=readSystemInfo();auto *grid=new QGridLayout;grid->setSpacing(12);
 QStringList labels={"Operating system","Architecture","Device","Processor","Memory","Edition"};
 QStringList values={s.os,s.architecture,s.host,s.cpu,s.memory,"The Unholy Core 3"};
 for(int i=0;i<labels.size();++i){auto *card=new QFrame;card->setObjectName("infoCard");auto *l=new QVBoxLayout(card);l->setContentsMargins(16,14,16,14);auto *a=new QLabel(labels[i]);a->setObjectName("mutedText");auto *b=new QLabel(values[i]);b->setStyleSheet("font-size:13pt;font-weight:600;color:#f0f3f7");b->setWordWrap(true);l->addWidget(a);l->addWidget(b);grid->addWidget(card,i/3,i%3);}
 v->addLayout(grid);
 auto *section=new QLabel("All workspaces");section->setObjectName("pageTitle");section->setStyleSheet("font-size:16pt;font-weight:600");v->addWidget(section);
 auto *list=new QLabel("20 dedicated sections are available in the navigation. Existing working tools include project creation, source editing, local terminals, basic network diagnostics, file browsing, Git commands and the plugin installer. Other workspaces are being implemented and are clearly marked.");list->setObjectName("bodyText");list->setWordWrap(true);v->addWidget(list);v->addStretch();return w;
}
QWidget* MainWindow::filePage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);auto *title=new QLabel("File Manager");title->setObjectName("pageTitle");v->addWidget(title);
 auto *model=new QFileSystemModel(w);model->setRootPath(QDir::homePath());auto *tree=new QTreeView;tree->setModel(model);tree->setRootIndex(model->index(QDir::homePath()));tree->setColumnWidth(0,380);v->addWidget(tree,1);return w;
}
QWidget* MainWindow::gitPage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);auto *title=new QLabel("Git Manager");title->setObjectName("pageTitle");v->addWidget(title);
 auto *label=new QLabel("Run Git commands in a selected working folder.");label->setObjectName("mutedText");v->addWidget(label);auto *path=new QLineEdit(QDir::homePath());v->addWidget(path);auto *out=new QPlainTextEdit;out->setReadOnly(true);v->addWidget(out,1);
 auto *row=new QHBoxLayout;for(QString cmd:{"status","log --oneline -10","branch --all"}){auto *b=new QPushButton("git "+cmd);row->addWidget(b);connect(b,&QPushButton::clicked,w,[path,out,cmd,w]{auto *p=new QProcess(w);p->setWorkingDirectory(path->text());p->start("git",cmd.split(' '));QObject::connect(p,&QProcess::readyReadStandardOutput,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardOutput()));});QObject::connect(p,&QProcess::readyReadStandardError,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardError()));});QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),p,&QObject::deleteLater);});}v->addLayout(row);return w;
}
QWidget* MainWindow::settingsPage(){
 auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);v->setSpacing(14);
 auto*title=new QLabel("Owner & Application Settings");title->setObjectName("pageTitle");v->addWidget(title);
 auto*hint=new QLabel("Local owner profile and preferences. This is a local profile, not online authentication or a security boundary.");hint->setObjectName("mutedText");hint->setWordWrap(true);v->addWidget(hint);
 QSettings prefs("EVILWARE","TheUnholyCore3");
 auto*ownerBox=new QGroupBox("OWNER PROFILE");auto*form=new QFormLayout(ownerBox);
 auto*ownerName=new QLineEdit(prefs.value("owner/name","Local Owner").toString());auto*ownerEmail=new QLineEdit(prefs.value("owner/email").toString());auto*ownerRole=new QLineEdit(prefs.value("owner/role","Local owner").toString());
 form->addRow("Display name",ownerName);form->addRow("Email / contact",ownerEmail);form->addRow("Role",ownerRole);v->addWidget(ownerBox);
 auto*appearance=new QGroupBox("APPEARANCE");auto*af=new QFormLayout(appearance);
 auto*theme=new QComboBox;theme->addItems({"Graphite Red","Midnight Blue","Light"});theme->setCurrentText(prefs.value("appearance/theme","Graphite Red").toString());
 auto*background=new QComboBox;background->addItems({"Original EVILWARE artwork","Plain graphite","Deep red"});background->setCurrentText(prefs.value("appearance/background","Original EVILWARE artwork").toString());af->addRow("Colour theme",theme);af->addRow("Workspace background",background);v->addWidget(appearance);
 auto*info=new QLabel("Local commands run under your current Windows user. No online owner login is configured. Do not store passwords or API secrets in this profile.");info->setWordWrap(true);info->setObjectName("bodyText");v->addWidget(info);
 auto*save=new QPushButton("Save and apply preferences");save->setObjectName("primaryButton");v->addWidget(save,0,Qt::AlignLeft);
 auto apply=[this,theme,background](){QString t=theme->currentText(),b=background->currentText();
 QString colors=t=="Light"?"QWidget#appRoot{background:#edf1f5;color:#17202b;} QFrame#sidebar{background:#e0e6ed;border-right:1px solid #bac4d0;} QFrame#topbar{background:rgba(225,231,238,245);border-bottom:1px solid #bac4d0;} QLabel#topbarTitle,QLabel#pageTitle,QLabel#cardTitle{color:#182333;} QLabel#bodyText{color:#344154;} QFrame#infoCard,QFrame#pluginCard{background:rgba(255,255,255,235);border:1px solid #c6d0dc;} QLineEdit,QComboBox,QPlainTextEdit,QTreeView{background:#fff;color:#182333;border:1px solid #bac4d0;}":t=="Midnight Blue"?"QWidget#appRoot{background:#081321;color:#e4efff;} QFrame#sidebar{background:#0c1b2d;border-right:1px solid #1e3a5c;} QFrame#topbar{background:rgba(8,22,39,240);border-bottom:1px solid #1e3a5c;} QListWidget#mainNavigation::item:selected{background:#173b60;border-left:3px solid #4da3ff;} QPushButton#primaryButton{background:#155da0;border:1px solid #4da3ff;color:white;}":"QWidget#appRoot{background:#0d1117;color:#e7ebf1;} QFrame#sidebar{background:#11151b;border-right:1px solid #2a3039;} QFrame#topbar{background:rgba(17,20,26,235);border-bottom:1px solid #2a3039;} QListWidget#mainNavigation::item:selected{background:#43151b;border-left:3px solid #e12636;} QPushButton#primaryButton{background:#a90014;border:1px solid #e12636;color:white;}";
 QString bg=b=="Original EVILWARE artwork"?"background-image:url(:/assets/evilware_reverse_engineering_banner.jpg);background-position:center;background-repeat:no-repeat;":b=="Deep red"?"background:#21090e;":"background:#10151c;";
 setStyleSheet(property("baseStyleSheet").toString()+colors+" QWidget#mainArea{"+bg+"}");};
 connect(save,&QPushButton::clicked,w,[&prefs,ownerName,ownerEmail,ownerRole,theme,background,apply]{prefs.setValue("owner/name",ownerName->text().trimmed());prefs.setValue("owner/email",ownerEmail->text().trimmed());prefs.setValue("owner/role",ownerRole->text().trimmed());prefs.setValue("appearance/theme",theme->currentText());prefs.setValue("appearance/background",background->currentText());prefs.sync();apply();});
 apply();v->addStretch();return w;
}
