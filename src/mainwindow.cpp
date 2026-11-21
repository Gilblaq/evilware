#include "mainwindow.h"
#include "terminalwidget.h"
#include "editorwidget.h"
#include "projectwidget.h"
#include "cyberlabwidget.h"
#include "pluginstorewidget.h"
#include "systeminfo.h"
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

MainWindow::MainWindow(QWidget *p):QMainWindow(p){
 setWindowTitle("NEXUS | Developer Workspace");
 resize(1360,860);
 setMinimumSize(980,640);

 auto *root=new QWidget;
 root->setObjectName("appRoot");
 auto *outer=new QHBoxLayout(root);
 outer->setContentsMargins(0,0,0,0);
 outer->setSpacing(0);

 auto *sidebar=new QFrame;
 sidebar->setObjectName("sidebar");
 sidebar->setFixedWidth(248);
 auto *side=new QVBoxLayout(sidebar);
 side->setContentsMargins(18,20,18,16);
 side->setSpacing(10);

 auto *brandRow=new QHBoxLayout;
 auto *mark=new QLabel("N");
 mark->setObjectName("brandMark");
 auto *brandText=new QVBoxLayout;
 auto *brand=new QLabel("NEXUS");
 brand->setObjectName("brandName");
 auto *tagline=new QLabel("DEVELOPER WORKSPACE");
 tagline->setObjectName("brandCaption");
 brandText->addWidget(brand);brandText->addWidget(tagline);
 brandRow->addWidget(mark);brandRow->addLayout(brandText);brandRow->addStretch();
 side->addLayout(brandRow);
 auto *workspace=new QLabel("PERSONAL WORKSPACE");
 workspace->setObjectName("sectionCaption");
 workspace->setContentsMargins(4,20,0,6);
 side->addWidget(workspace);

 nav=new QListWidget;
 nav->setObjectName("mainNavigation");
 nav->setFrameShape(QFrame::NoFrame);
 nav->setSpacing(4);
 const QStringList items={"Dashboard","Build Apps","Code Studio","Terminal","Cyber Lab","Plugin Store","File Explorer","Git Manager","Settings"};
 for(const QString &name:items){auto *item=new QListWidgetItem(name);item->setSizeHint(QSize(190,42));nav->addItem(item);}
 side->addWidget(nav,1);
 auto *footer=new QLabel("NEXUS CORE  ·  v1.1");
 footer->setObjectName("sidebarFooter");
 side->addWidget(footer);

 auto *mainArea=new QWidget;
 mainArea->setObjectName("mainArea");
 auto *mainLayout=new QVBoxLayout(mainArea);
 mainLayout->setContentsMargins(0,0,0,0);
 mainLayout->setSpacing(0);
 auto *topbar=new QFrame;topbar->setObjectName("topbar");topbar->setFixedHeight(66);
 auto *top=new QHBoxLayout(topbar);top->setContentsMargins(26,0,26,0);
 activePage=new QLabel("Dashboard");activePage->setObjectName("topbarTitle");
 auto *workspaceLabel=new QLabel("Personal workspace");workspaceLabel->setObjectName("mutedText");
 top->addWidget(activePage);top->addStretch();top->addWidget(workspaceLabel);
 mainLayout->addWidget(topbar);

 pages=new QStackedWidget;pages->setObjectName("pageStack");mainLayout->addWidget(pages,1);
 outer->addWidget(sidebar);outer->addWidget(mainArea,1);
 setCentralWidget(root);

 terminal=new TerminalWidget;
 editor=new EditorWidget;
 projects=new ProjectWidget;
 cyber=new CyberLabWidget;
 plugins=new PluginStoreWidget;
 addPage(dashboard());
 addPage(projects);
 addPage(editor);
 addPage(terminal);
 addPage(cyber);
 addPage(plugins);
 addPage(filePage());
 addPage(gitPage());
 addPage(settingsPage());
 connect(nav,&QListWidget::currentRowChanged,this,&MainWindow::showPage);
 nav->setCurrentRow(0);

 setStyleSheet(R"(
 QMainWindow,QWidget#appRoot,QWidget#mainArea{background:#101318;color:#e7ebf1;font-family:"Segoe UI",Arial;font-size:10pt}
 QFrame#sidebar{background:#171b21;border-right:1px solid #2a3039}
 QLabel#brandMark{background:#2563eb;color:white;font-size:23px;font-weight:700;border-radius:9px;min-width:42px;max-width:42px;min-height:42px;max-height:42px;qproperty-alignment:AlignCenter}
 QLabel#brandName{color:#f5f7fa;font-size:20px;font-weight:700;letter-spacing:1px}
 QLabel#brandCaption,QLabel#sectionCaption,QLabel#sidebarFooter{color:#7f8a99;font-size:8pt;letter-spacing:1px}
 QListWidget#mainNavigation{background:transparent;color:#aeb8c6;border:0;font-size:10pt;outline:0}
 QListWidget#mainNavigation::item{padding:10px 12px;border-radius:7px}
 QListWidget#mainNavigation::item:hover{background:#222a35;color:#f4f7fb}
 QListWidget#mainNavigation::item:selected{background:#253b5b;color:#ffffff;border-left:3px solid #4b8df8;padding-left:9px}
 QFrame#topbar{background:#171b21;border-bottom:1px solid #2a3039}
 QLabel#topbarTitle{font-size:12pt;font-weight:600;color:#f3f5f8}
 QLabel#mutedText{color:#909baa}
 QLabel#pageTitle{font-size:23px;font-weight:650;color:#f4f6f9}
 QLabel#cardTitle{font-size:12pt;font-weight:600;color:#f0f3f7}
 QLabel#bodyText{color:#c4ccd6}
 QFrame#pluginCard,QFrame#infoCard{background:#191f27;border:1px solid #2c3541;border-radius:10px}
 QPushButton{background:#283342;color:#e8edf4;border:1px solid #394657;border-radius:6px;padding:8px 13px}
 QPushButton:hover{background:#344357}
 QPushButton#primaryButton{background:#2864c7;border:1px solid #3475df;color:white;font-weight:600}
 QPushButton#primaryButton:hover{background:#3475df}
 QPushButton#secondaryButton{background:#202833;color:#cbd5e1}
 QLineEdit,QComboBox,QPlainTextEdit,QTreeView{background:#141a22;color:#e7edf5;border:1px solid #35404e;border-radius:6px;padding:8px;selection-background-color:#315d9a}
 QPlainTextEdit{font-family:Consolas,monospace}
 QTreeView::item{padding:5px}
 QTreeView::item:selected{background:#263d5e}
 QScrollBar:vertical{background:#171c23;width:10px;margin:0}
 QScrollBar::handle:vertical{background:#3a4655;border-radius:4px;min-height:24px}
 QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0}
 )");
}

void MainWindow::addPage(QWidget *page){pages->addWidget(page);}
void MainWindow::showPage(int row){
 if(row<0||row>=pages->count())return;
 pages->setCurrentIndex(row);
 QString name=nav->item(row)->text();
 activePage->setText(name); 
}

QWidget* MainWindow::dashboard(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(28,26,28,28);v->setSpacing(18);
 auto *title=new QLabel("Welcome to NEXUS");title->setObjectName("pageTitle");v->addWidget(title);
 auto *sub=new QLabel("Your development tools, projects and system integrations in one workspace.");sub->setObjectName("mutedText");v->addWidget(sub);
 auto s=readSystemInfo();
 auto *grid=new QGridLayout;grid->setSpacing(12);
 QStringList labels={"Operating system","Architecture","Device","Processor","Memory","Application"};
 QStringList values={s.os,s.architecture,s.host,s.cpu,s.memory,s.appVersion};
 for(int i=0;i<labels.size();++i){
  auto *card=new QFrame;card->setObjectName("infoCard");auto *l=new QVBoxLayout(card);l->setContentsMargins(16,14,16,14);
  auto *a=new QLabel(labels[i]);a->setObjectName("mutedText");auto *b=new QLabel(values[i]);b->setStyleSheet("font-size:13pt;font-weight:600;color:#f0f3f7");b->setWordWrap(true);l->addWidget(a);l->addWidget(b);grid->addWidget(card,i/3,i%3);
 }
 v->addLayout(grid);
 auto *section=new QLabel("Workspace");section->setObjectName("pageTitle");section->setStyleSheet("font-size:16pt;font-weight:600");v->addWidget(section);
 auto *modules=new QGridLayout;modules->setSpacing(12);
 const QStringList names={"Build Apps","Code Studio","Terminals","Cyber Lab","Plugin Store","Git Manager"};
 const QStringList desc={"Create starter projects for popular languages and web apps.","Open, edit and save source files on this computer.","Use PowerShell, Command Prompt or WSL.","Run basic network diagnostics on authorized systems.","Install and connect optional developer tools.","Inspect branches, commits and working-tree status."};
 for(int i=0;i<names.size();++i){auto *card=new QFrame;card->setObjectName("infoCard");auto *l=new QVBoxLayout(card);l->setContentsMargins(16,14,16,14);auto *a=new QLabel(names[i]);a->setObjectName("cardTitle");auto *b=new QLabel(desc[i]);b->setObjectName("mutedText");b->setWordWrap(true);l->addWidget(a);l->addWidget(b);modules->addWidget(card,i/3,i%3);}
 v->addLayout(modules);v->addStretch();return w;
}
QWidget* MainWindow::filePage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);
 auto *title=new QLabel("File Explorer");title->setObjectName("pageTitle");v->addWidget(title);
 auto *model=new QFileSystemModel(w);model->setRootPath(QDir::homePath());auto *tree=new QTreeView;tree->setModel(model);tree->setRootIndex(model->index(QDir::homePath()));tree->setColumnWidth(0,380);v->addWidget(tree,1);return w;
}
QWidget* MainWindow::gitPage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);
 auto *title=new QLabel("Git Manager");title->setObjectName("pageTitle");v->addWidget(title);
 auto *label=new QLabel("Run Git commands in a selected working folder.");label->setObjectName("mutedText");v->addWidget(label);
 auto *path=new QLineEdit(QDir::homePath());v->addWidget(path);auto *out=new QPlainTextEdit;out->setReadOnly(true);v->addWidget(out,1);
 auto *row=new QHBoxLayout;for(QString cmd:{"status","log --oneline -10","branch --all"}){auto *b=new QPushButton("git "+cmd);row->addWidget(b);connect(b,&QPushButton::clicked,w,[path,out,cmd,w]{auto *p=new QProcess(w);p->setWorkingDirectory(path->text());p->start("git",cmd.split(' '));QObject::connect(p,&QProcess::readyReadStandardOutput,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardOutput()));});QObject::connect(p,&QProcess::readyReadStandardError,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardError()));});QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),p,&QObject::deleteLater);});}v->addLayout(row);return w;
}
QWidget* MainWindow::settingsPage(){
 auto *w=new QWidget;auto *v=new QVBoxLayout(w);v->setContentsMargins(24,20,24,20);
 auto *title=new QLabel("Settings");title->setObjectName("pageTitle");v->addWidget(title);
 auto *details=new QLabel("NEXUS Developer Command Centre\nVersion 1.1.0\nTheme: Professional Graphite\nTerminal commands run locally under your Windows user account.\nOptional integrations can be installed from Plugin Store using Windows Package Manager.\nCyber Lab provides basic network diagnostics for authorized testing.");details->setWordWrap(true);details->setObjectName("bodyText");v->addWidget(details);v->addStretch();return w;
}
