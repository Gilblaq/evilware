#include "projectwidget.h"
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QDesktopServices>
#include <QUrl>

ProjectWidget::ProjectWidget(QWidget*p):QWidget(p){
 auto*v=new QVBoxLayout(this);auto*form=new QFormLayout;
 name=new QLineEdit("MyEVILWAREProject");location=new QLineEdit(QDir::homePath()+"/EVILWAREProjects");
 kind=new QComboBox;kind->addItems({"C++ Console (CMake)","Python application","Web starter (HTML/CSS/JS)","Node.js application"});
 auto*browse=new QPushButton("Browse…");auto*locrow=new QWidget;auto*lr=new QHBoxLayout(locrow);lr->setContentsMargins(0,0,0,0);lr->addWidget(location,1);lr->addWidget(browse);
 form->addRow("Project name",name);form->addRow("Template",kind);form->addRow("Parent folder",locrow);v->addLayout(form);
 auto*actions=new QHBoxLayout;auto*create=new QPushButton("Create project");create->setMinimumHeight(42);create->setObjectName("primaryButton");
 buildRunButton=new QPushButton("Build / Run");buildRunButton->setMinimumHeight(42);buildRunButton->setEnabled(false);
 actions->addWidget(create);actions->addWidget(buildRunButton);v->addLayout(actions);
 message=new QLabel("Create a project, then build or run it here. Build output and errors appear below.");message->setWordWrap(true);v->addWidget(message);
 output=new QPlainTextEdit;output->setReadOnly(true);output->setPlaceholderText("Build and run output will appear here…");output->setMinimumHeight(180);v->addWidget(output,1);
 connect(browse,&QPushButton::clicked,this,&ProjectWidget::chooseFolder);connect(create,&QPushButton::clicked,this,&ProjectWidget::createProject);connect(buildRunButton,&QPushButton::clicked,this,&ProjectWidget::buildOrRun);
}
void ProjectWidget::chooseFolder(){auto p=QFileDialog::getExistingDirectory(this,"Choose project folder",location->text());if(!p.isEmpty())location->setText(p);}
void ProjectWidget::appendOutput(const QString&t){if(!t.isEmpty()){output->moveCursor(QTextCursor::End);output->insertPlainText(t);output->ensureCursorVisible();}}
void ProjectWidget::ensureDependency(const QString &label,const QString &executable,const QString &packageId){
 if(!QStandardPaths::findExecutable(executable).isEmpty()){message->setText(label+" is already available.");return;}
 const QString winget=QStandardPaths::findExecutable("winget");
 if(winget.isEmpty()){message->setText("Missing "+label+". Windows Package Manager (winget) was not found; install App Installer, then use the Plugin & Software Manager.");return;}
 if(QMessageBox::question(this,"Missing dependency",label+" is required for this project, but was not found. Install it now from the configured winget source?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes){message->setText("Project created. "+label+" was skipped; install it later from Plugin & Software Manager.");return;}
 message->setText("Installing "+label+"… EVILWARE is waiting for Windows Package Manager.");
 auto *process=new QProcess(this);process->setProgram(winget);process->setArguments({"install","--id",packageId,"--exact","--source","winget","--accept-package-agreements","--accept-source-agreements","--disable-interactivity"});
 connect(process,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,[this,process,label](int code,QProcess::ExitStatus state){
  const QString details=QString::fromLocal8Bit(process->readAllStandardOutput())+"\n"+QString::fromLocal8Bit(process->readAllStandardError());process->deleteLater();
  const bool ok=state==QProcess::NormalExit&&code==0;
  message->setText(ok?label+" installation completed. Restart the relevant terminal or workspace if it does not appear immediately.":label+" installation failed. Open Plugin & Software Manager for details and retry.");
  if(!ok){QMessageBox box(this);box.setWindowTitle("Dependency installation failed");box.setText("EVILWARE could not install "+label+". Your project files are safe.");box.setDetailedText(details);box.exec();}
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
 bool ok=true;projectKind=kind->currentIndex();
 if(projectKind==0){ok&=put("CMakeLists.txt","cmake_minimum_required(VERSION 3.20)\nproject("+n+" LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 20)\nadd_executable("+n+" main.cpp)\n");ok&=put("main.cpp","#include <iostream>\nint main(){ std::cout << \"Hello from "+n+"!\\n\"; }\n");}
 else if(projectKind==1){ok&=put("main.py","def main():\n    print('Hello from "+n+"!')\n\nif __name__ == '__main__':\n    main()\n");}
 else if(projectKind==2){ok&=put("index.html","<!doctype html>\n<html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>"+n+"</title><link rel=\"stylesheet\" href=\"style.css\"></head><body><main><h1>"+n+"</h1><p>Your project starts here.</p></main><script src=\"app.js\"></script></body></html>\n");ok&=put("style.css","body{font-family:system-ui;margin:3rem;background:#101722;color:#e8f0fa}\n");ok&=put("app.js","console.log('"+n+" ready');\n");}
 else {ok&=put("package.json","{\n  \"name\": \""+n.toLower()+"\",\n  \"version\": \"1.0.0\",\n  \"private\": true,\n  \"scripts\": { \"start\": \"node index.js\" }\n}\n");ok&=put("index.js","console.log('Hello from "+n+"!');\n");}
 if(!ok){message->setText("Project folder created, but one or more files could not be written. Check folder permissions.");return;}
 projectPath=d.absolutePath();buildRunButton->setEnabled(true);output->clear();message->setText("Project created: "+projectPath+" — use Build / Run when ready.");emit projectCreated(projectPath);
 if(projectKind==0){
  if(QStandardPaths::findExecutable("cmake").isEmpty())ensureDependency("CMake","cmake","Kitware.CMake");
  QMessageBox::information(this,"C++ toolchain","EVILWARE will ask CMake to detect the installed compiler. If configuration fails, install Visual Studio Build Tools with Desktop development with C++, or configure a compatible MinGW/Clang toolchain.");
 }else if(projectKind==1&&QStandardPaths::findExecutable("python").isEmpty()&&QStandardPaths::findExecutable("py").isEmpty()){
  ensureDependency("Python 3","python","Python.Python.3.12");
 }else if(projectKind==3&&QStandardPaths::findExecutable("node").isEmpty()){
  ensureDependency("Node.js LTS","node","OpenJS.NodeJS.LTS");
 }
}
void ProjectWidget::startProcess(const QString&program,const QStringList&arguments,int stage){
 if(activeProcess)return;
 activeProcess=new QProcess(this);activeProcess->setWorkingDirectory(projectPath);buildStage=stage;
 connect(activeProcess,&QProcess::readyReadStandardOutput,this,[this]{appendOutput(QString::fromLocal8Bit(activeProcess->readAllStandardOutput()));});
 connect(activeProcess,&QProcess::readyReadStandardError,this,[this]{appendOutput(QString::fromLocal8Bit(activeProcess->readAllStandardError()));});
 connect(activeProcess,qOverload<int,QProcess::ExitStatus>(&QProcess::finished),this,&ProjectWidget::finishBuildStage);
 connect(activeProcess,&QProcess::errorOccurred,this,[this](QProcess::ProcessError){appendOutput("\n[EVILWARE] Could not start the selected tool. Check that it is installed and available on PATH.\n");message->setText("Could not start the build/run process.");buildRunButton->setEnabled(true);if(activeProcess){activeProcess->deleteLater();activeProcess=nullptr;}});
 appendOutput("\n> "+program+" "+arguments.join(" ")+"\n");message->setText("Running "+program+"…");buildRunButton->setEnabled(false);activeProcess->start(program,arguments);
}
void ProjectWidget::finishBuildStage(int code,QProcess::ExitStatus status){
 if(!activeProcess)return;
 appendOutput(QString::fromLocal8Bit(activeProcess->readAllStandardOutput()));appendOutput(QString::fromLocal8Bit(activeProcess->readAllStandardError()));
 const bool ok=status==QProcess::NormalExit&&code==0;activeProcess->deleteLater();activeProcess=nullptr;
 if(!ok){appendOutput("\n[EVILWARE] Process failed (exit code "+QString::number(code)+").\n");message->setText("Build/run failed. Review the output for diagnostics.");buildRunButton->setEnabled(true);return;}
 if(buildStage==1){appendOutput("\n[EVILWARE] CMake configuration completed.\n");startProcess(QStandardPaths::findExecutable("cmake"),{"--build",QDir(projectPath).filePath("build"),"--config","Release"},2);return;}
 if(buildStage==2){
  appendOutput("\n[EVILWARE] Build completed successfully. Launching the program…\n");
  const QString executableName=QFileInfo(projectPath).fileName()+".exe";
  QString exe;
  QDirIterator files(QDir(projectPath).filePath("build"),QStringList{executableName},QDir::Files,QDirIterator::Subdirectories);
  if(files.hasNext())exe=files.next();
  if(exe.isEmpty()){appendOutput("[EVILWARE] Build succeeded, but "+executableName+" was not found under the build directory. Check the build output and CMake target settings.\n");message->setText("Build succeeded; executable location needs checking.");buildRunButton->setEnabled(true);return;}
  startProcess(exe,{},3);return;
 }
 appendOutput("\n[EVILWARE] Run completed successfully.\n");message->setText("Project finished successfully.");buildRunButton->setEnabled(true);
}
void ProjectWidget::buildOrRun(){
 if(projectPath.isEmpty()||!QDir(projectPath).exists()){message->setText("Create a project first.");return;}
 output->clear();
 if(projectKind==2){const QString page=QDir(projectPath).filePath("index.html");if(!QFileInfo::exists(page)){message->setText("index.html was not found.");return;}QDesktopServices::openUrl(QUrl::fromLocalFile(page));message->setText("Opened the web project in your default browser.");appendOutput("Opened "+page+" in the default browser.\n");return;}
 if(projectKind==3){const QString node=QStandardPaths::findExecutable("node");if(node.isEmpty()){message->setText("Node.js is not installed or not available on PATH. Use Plugin & Software Manager.");return;}startProcess(node,{QDir(projectPath).filePath("index.js")},3);return;}
 if(projectKind==1){QString python=QStandardPaths::findExecutable("python");QStringList args;if(python.isEmpty()){python=QStandardPaths::findExecutable("py");args<<"-3";}if(python.isEmpty()){message->setText("Python is not installed or not available on PATH. Use Plugin & Software Manager.");return;}args<<QDir(projectPath).filePath("main.py");startProcess(python,args,3);return;}
 const QString cmake=QStandardPaths::findExecutable("cmake");if(cmake.isEmpty()){message->setText("CMake is missing. Install it using Plugin & Software Manager, then retry.");return;}
 startProcess(cmake,{"-S",projectPath,"-B",QDir(projectPath).filePath("build")},1);
}
