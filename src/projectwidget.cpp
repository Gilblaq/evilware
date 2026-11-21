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
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
ProjectWidget::ProjectWidget(QWidget*p):QWidget(p){
 auto*v=new QVBoxLayout(this);auto*form=new QFormLayout;name=new QLineEdit("MyEVILWAREProject");location=new QLineEdit(QDir::homePath()+"/EVILWAREProjects");
 kind=new QComboBox;kind->addItems({"C++ Console (CMake)","Python application","Web starter (HTML/CSS/JS)"});
 auto*browse=new QPushButton("Browse…");auto*locrow=new QWidget;auto*lr=new QHBoxLayout(locrow);lr->setContentsMargins(0,0,0,0);lr->addWidget(location,1);lr->addWidget(browse);
 form->addRow("Project name",name);form->addRow("Template",kind);form->addRow("Parent folder",locrow);v->addLayout(form);
 auto*create=new QPushButton("Create project");create->setMinimumHeight(42);create->setObjectName("primaryButton");v->addWidget(create);message=new QLabel("Creates a real project folder and checks the tools required to work with the selected template.");message->setWordWrap(true);v->addWidget(message);v->addStretch();
 connect(browse,&QPushButton::clicked,this,&ProjectWidget::chooseFolder);connect(create,&QPushButton::clicked,this,&ProjectWidget::createProject);
}
void ProjectWidget::chooseFolder(){auto p=QFileDialog::getExistingDirectory(this,"Choose project folder",location->text());if(!p.isEmpty())location->setText(p);}
void ProjectWidget::ensureDependency(const QString &label,const QString &executable,const QString &packageId){
 if(!QStandardPaths::findExecutable(executable).isEmpty()){message->setText(label+" is already available.");return;}
 const QString winget=QStandardPaths::findExecutable("winget");
 if(winget.isEmpty()){message->setText("Missing "+label+". Windows Package Manager (winget) was not found; install App Installer, then use the Plugin & Software Manager.");return;}
 if(QMessageBox::question(this,"Missing dependency",label+" is required or recommended for this project, but was not found. Install it now from the configured winget source?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes){message->setText("Project created. "+label+" was skipped; install it later from Plugin & Software Manager.");return;}
 message->setText("Installing "+label+"… EVILWARE is waiting for Windows Package Manager.");
 auto *process=new QProcess(this);process->setProgram(winget);process->setArguments({"install","--id",packageId,"--exact","--source","winget","--accept-package-agreements","--accept-source-agreements","--disable-interactivity"});
 connect(process,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this,process,label](int code,QProcess::ExitStatus state){
  const QString output=QString::fromLocal8Bit(process->readAllStandardOutput())+"\n"+QString::fromLocal8Bit(process->readAllStandardError());process->deleteLater();
  const bool ok=state==QProcess::NormalExit&&code==0;
  message->setText(ok?label+" installation completed. Restart the relevant terminal or workspace if it does not appear immediately.":label+" installation failed. Open Plugin & Software Manager for details and retry.");
  if(!ok){QMessageBox box(this);box.setWindowTitle("Dependency installation failed");box.setText("EVILWARE could not install "+label+". Your project files are safe.");box.setDetailedText(output);box.exec();}
 });
 connect(process,&QProcess::errorOccurred,this,[this,process,label](QProcess::ProcessError){message->setText("Could not start winget to install "+label+". Your project files are safe.");process->deleteLater();});
 process->start();
}
void ProjectWidget::createProject(){
 QString n=name->text().trimmed();static const QRegularExpression safeName("^[A-Za-z0-9][A-Za-z0-9_-]{0,63}$");if(!safeName.match(n).hasMatch()){QMessageBox::warning(this,"Invalid name","Use 1–64 characters: letters, numbers, hyphens and underscores. The first character must be a letter or number.");return;}
 QDir parent(location->text());if(!parent.exists()&&!parent.mkpath(".")){message->setText("Could not create parent folder.");return;}
 if(parent.exists(n)){QMessageBox::warning(this,"Already exists","That project folder already exists.");return;}
 if(!parent.mkdir(n)){message->setText("Could not create project folder.");return;}
 QDir d(parent.filePath(n));auto put=[&](const QString&file,const QString&data){QFile f(d.filePath(file));if(!f.open(QIODevice::WriteOnly|QIODevice::Text))return false;return f.write(data.toUtf8())==data.toUtf8().size();};
 bool ok=true;
 if(kind->currentIndex()==0){ok&=put("CMakeLists.txt","cmake_minimum_required(VERSION 3.20)\nproject("+n+" LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 20)\nadd_executable("+n+" main.cpp)\n");ok&=put("main.cpp","#include <iostream>\nint main(){ std::cout << \"Hello from "+n+"!\\n\"; }\n");}
 else if(kind->currentIndex()==1){ok&=put("main.py","def main():\n    print('Hello from "+n+"!')\n\nif __name__ == '__main__':\n    main()\n");}
 else {ok&=put("index.html","<!doctype html>\n<html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>"+n+"</title><link rel=\"stylesheet\" href=\"style.css\"></head><body><main><h1>"+n+"</h1><p>Your project starts here.</p></main><script src=\"app.js\"></script></body></html>\n");ok&=put("style.css","body{font-family:system-ui;margin:3rem;background:#101722;color:#e8f0fa}\n");ok&=put("app.js","console.log('"+n+" ready');\n");}
 if(!ok){message->setText("Project folder created, but one or more files could not be written. Check folder permissions.");return;}
 message->setText("Project created successfully: "+d.absolutePath());
 if(kind->currentIndex()==0){
  ensureDependency("CMake","cmake","Kitware.CMake");
  const bool compiler=!QStandardPaths::findExecutable("cl").isEmpty()||!QStandardPaths::findExecutable("g++").isEmpty()||!QStandardPaths::findExecutable("clang++").isEmpty();
  if(!compiler)QMessageBox::information(this,"C++ compiler required","The project files are ready, but no C++ compiler was detected. Install Visual Studio Build Tools with the Desktop development with C++ workload, or configure an existing MinGW/Clang toolchain before building.");
 }else if(kind->currentIndex()==1){
  if(QStandardPaths::findExecutable("python").isEmpty()&&QStandardPaths::findExecutable("py").isEmpty())ensureDependency("Python 3","python","Python.Python.3.12");
 }else if(QStandardPaths::findExecutable("node").isEmpty()){
  ensureDependency("Node.js LTS","node","OpenJS.NodeJS.LTS");
 }
}
