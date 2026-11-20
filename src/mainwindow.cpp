#include "mainwindow.h"
#include "terminalwidget.h"
#include "editorwidget.h"
#include "projectwidget.h"
#include "cyberlabwidget.h"
#include "systeminfo.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QFileSystemModel>
#include <QTreeView>
#include <QProcess>
#include <QPlainTextEdit>
#include <QDir>
#include <QMessageBox>
MainWindow::MainWindow(QWidget*p):QMainWindow(p){
 setWindowTitle("NEXUS Developer Command Centre");resize(1280,820);setMinimumSize(900,600);
 auto*root=new QWidget;auto*layout=new QHBoxLayout(root);layout->setContentsMargins(0,0,0,0);layout->setSpacing(0);
 nav=new QListWidget;nav->setFixedWidth(210);nav->addItems({"Overview","Build Apps","Code Studio","Terminal","Cyber Lab","File Explorer","Git Manager","Settings"});
 nav->setStyleSheet("QListWidget{background:#0b111b;color:#aebed0;border:0;padding:12px;font-size:14px}QListWidget::item{padding:13px;border-radius:7px}QListWidget::item:selected{background:#173b60;color:white}");
 pages=new QStackedWidget;layout->addWidget(nav);layout->addWidget(pages,1);setCentralWidget(root);
 terminal=new TerminalWidget;editor=new EditorWidget;projects=new ProjectWidget;cyber=new CyberLabWidget;
 addPage("Overview",dashboard());addPage("Build Apps",projects);addPage("Code Studio",editor);addPage("Terminal",terminal);addPage("Cyber Lab",cyber);addPage("File Explorer",filePage());addPage("Git Manager",gitPage());addPage("Settings",settingsPage());
 connect(nav,&QListWidget::currentRowChanged,this,&MainWindow::showPage);nav->setCurrentRow(0);
 setStyleSheet("QMainWindow,QWidget{background:#111a27;color:#e7eef8}QPushButton{background:#1e4c77;color:white;border:0;border-radius:6px;padding:9px 14px}QPushButton:hover{background:#28659c}QLineEdit,QComboBox{background:#0c1420;color:#e7eef8;border:1px solid #2a3b50;border-radius:5px;padding:8px}QLabel{color:#dce7f4}");
}
void MainWindow::addPage(const QString&,QWidget*w){pages->addWidget(w);}
void MainWindow::showPage(int r){if(r>=0&&r<pages->count())pages->setCurrentIndex(r);}
QWidget* MainWindow::dashboard(){
 auto*w=new QWidget;auto*v=new QVBoxLayout(w);auto*title=new QLabel("NEXUS  /  Developer Command Centre");title->setStyleSheet("font-size:25px;font-weight:700;color:#f4f8ff");v->addWidget(title);
 auto*sub=new QLabel("Your workspace for building, debugging, shipping and learning.");sub->setStyleSheet("color:#8fa6bf");v->addWidget(sub);
 auto s=readSystemInfo();auto*grid=new QGridLayout;QStringList labels={"Operating system","Architecture","Host","Processor","Memory","Version"};QStringList values={s.os,s.architecture,s.host,s.cpu,s.memory,s.appVersion};
 for(int i=0;i<labels.size();++i){auto*c=new QFrame;c->setStyleSheet("QFrame{background:#182638;border:1px solid #263b52;border-radius:9px}");auto*l=new QVBoxLayout(c);auto*a=new QLabel(labels[i]);a->setStyleSheet("color:#83a5c8");auto*b=new QLabel(values[i]);b->setWordWrap(true);b->setStyleSheet("font-size:15px;font-weight:600");l->addWidget(a);l->addWidget(b);grid->addWidget(c,i/2,i%2);}
 v->addLayout(grid);auto*heading=new QLabel("Workspace modules");heading->setStyleSheet("font-size:18px;font-weight:600");v->addWidget(heading);
 auto*info=new QLabel("Build Apps • Code Studio • Windows PowerShell/CMD • WSL terminal • Git Manager • File Explorer • Network diagnostics • Preferences");info->setWordWrap(true);v->addWidget(info);
 auto*note=new QLabel("Integrations such as WSL, Git and Docker depend on the corresponding tools being installed on Windows. NEXUS will not silently install external software.");note->setWordWrap(true);note->setStyleSheet("color:#90a3b8");v->addWidget(note);v->addStretch();return w;
}
QWidget* MainWindow::filePage(){
 auto*w=new QWidget;auto*v=new QVBoxLayout(w);auto*model=new QFileSystemModel(w);model->setRootPath(QDir::homePath());auto*tree=new QTreeView;tree->setModel(model);tree->setRootIndex(model->index(QDir::homePath()));tree->setColumnWidth(0,380);v->addWidget(new QLabel("Home folder"));v->addWidget(tree);return w;
}
QWidget* MainWindow::gitPage(){
 auto*w=new QWidget;auto*v=new QVBoxLayout(w);auto*label=new QLabel("Git Manager — run Git commands in a selected working folder.");v->addWidget(label);
 auto*path=new QLineEdit(QDir::homePath());v->addWidget(path);auto*out=new QPlainTextEdit;out->setReadOnly(true);v->addWidget(out,1);
 auto*row=new QHBoxLayout;for(QString cmd:{"status","log --oneline -10","branch --all"}){auto*b=new QPushButton("git "+cmd);row->addWidget(b);connect(b,&QPushButton::clicked,w,[path,out,cmd,w]{auto*p=new QProcess(w);p->setWorkingDirectory(path->text());p->start("git",cmd.split(' '));QObject::connect(p,&QProcess::readyReadStandardOutput,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardOutput()));});QObject::connect(p,&QProcess::readyReadStandardError,[p,out]{out->appendPlainText(QString::fromLocal8Bit(p->readAllStandardError()));});QObject::connect(p,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),p,&QObject::deleteLater);});}v->addLayout(row);return w;
}
QWidget* MainWindow::settingsPage(){
 auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->addWidget(new QLabel("NEXUS Settings"));v->addWidget(new QLabel("Version 1.0.0\nTheme: Midnight Graphite\nTerminal commands run locally under your Windows user account.\nOptional integrations are not bundled.\nCyber Lab is restricted to basic authorized network diagnostics."));v->addStretch();return w;
}
