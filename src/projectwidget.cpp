#include "projectwidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
ProjectWidget::ProjectWidget(QWidget*p):QWidget(p){
 auto*v=new QVBoxLayout(this);auto*form=new QFormLayout;name=new QLineEdit("MyNexusProject");location=new QLineEdit(QDir::homePath()+"/NEXUSProjects");
 kind=new QComboBox;kind->addItems({"C++ Console (CMake)","Python application","Web starter (HTML/CSS/JS)"});
 auto*browse=new QPushButton("Browse…");auto*locrow=new QWidget;auto*lr=new QHBoxLayout(locrow);lr->setContentsMargins(0,0,0,0);lr->addWidget(location,1);lr->addWidget(browse);
 form->addRow("Project name",name);form->addRow("Template",kind);form->addRow("Parent folder",locrow);v->addLayout(form);
 auto*create=new QPushButton("Create project");create->setMinimumHeight(42);v->addWidget(create);message=new QLabel("Creates a real folder and starter source files on this computer.");v->addWidget(message);v->addStretch();
 connect(browse,&QPushButton::clicked,this,&ProjectWidget::chooseFolder);connect(create,&QPushButton::clicked,this,&ProjectWidget::createProject);
}
void ProjectWidget::chooseFolder(){auto p=QFileDialog::getExistingDirectory(this,"Choose project folder",location->text());if(!p.isEmpty())location->setText(p);}
void ProjectWidget::createProject(){
 QString n=name->text().trimmed();if(n.isEmpty()||n.contains('/')||n.contains('\\')){QMessageBox::warning(this,"Invalid name","Enter a simple project name.");return;}
 QDir parent(location->text());if(!parent.exists()&&!parent.mkpath(".")){message->setText("Could not create parent folder.");return;}
 if(parent.exists(n)){QMessageBox::warning(this,"Already exists","That project folder already exists.");return;}
 if(!parent.mkdir(n)){message->setText("Could not create project folder.");return;}
 QDir d(parent.filePath(n));auto put=[&](const QString&file,const QString&data){QFile f(d.filePath(file));if(!f.open(QIODevice::WriteOnly|QIODevice::Text))return false;f.write(data.toUtf8());return true;};
 bool ok=true;
 if(kind->currentIndex()==0){ok&=put("CMakeLists.txt","cmake_minimum_required(VERSION 3.20)\nproject("+n+" LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 20)\nadd_executable("+n+" main.cpp)\n");ok&=put("main.cpp","#include <iostream>\nint main(){ std::cout << \"Hello from "+n+"!\\n\"; }\n");}
 else if(kind->currentIndex()==1){ok&=put("main.py","def main():\n    print('Hello from "+n+"!')\n\nif __name__ == '__main__':\n    main()\n");}
 else {ok&=put("index.html","<!doctype html>\n<html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>"+n+"</title><link rel=\"stylesheet\" href=\"style.css\"></head><body><main><h1>"+n+"</h1><p>Your project starts here.</p></main><script src=\"app.js\"></script></body></html>\n");ok&=put("style.css","body{font-family:system-ui;margin:3rem;background:#101722;color:#e8f0fa}\n");ok&=put("app.js","console.log('"+n+" ready');\n");}
 message->setText(ok?"Created: "+d.absolutePath():"Project folder created, but one or more files could not be written.");
}
