#include "mainwindow.h"
#include "startupwidget.h"
#include <QApplication>
#include <QFont>
#include <QGuiApplication>
#include <QScreen>
#include <QListWidget>
#include <QStackedWidget>
#include <QDebug>
#include <QTreeView>
#include <QTabWidget>

int main(int argc,char *argv[]){
 QApplication app(argc,argv);
 QApplication::setApplicationName("EVILWARE");
 QApplication::setOrganizationName("EVILWARE");
 QApplication::setApplicationVersion("3.0.0");
 app.setStyle("Fusion");
 app.setFont(QFont("Segoe UI",10));

 // Headless CI smoke test: construct the real workspace and verify that all
 // 20 navigation entries map to 20 distinct pages without showing a window.
 if(app.arguments().contains("--self-test")){
  MainWindow testWindow;
  auto *navigation=testWindow.findChild<QListWidget*>("mainNavigation");
  auto *pageStack=testWindow.findChild<QStackedWidget*>("pageStack");
  if(!navigation || !pageStack){
   qCritical() << "SELF-TEST FAILED: workspace navigation or page stack is missing";
   return 2;
  }
  auto *projectExplorer=testWindow.findChild<QTreeView*>("projectExplorer");
  const auto terminalTabWidgets=testWindow.findChildren<QTabWidget*>("terminalTabs");
  if(terminalTabWidgets.size()!=2 || terminalTabWidgets[0]->count()<1 || terminalTabWidgets[1]->count()<1){
   qCritical() << "SELF-TEST FAILED: Windows and Linux terminal tab containers are missing";
   return 6;
  }
  if(!projectExplorer){
   qCritical() << "SELF-TEST FAILED: Code Studio project explorer is missing";
   return 5;
  }
  if(navigation->count()!=20 || pageStack->count()!=20){
   qCritical() << "SELF-TEST FAILED: expected 20 navigation entries and pages; got"
               << navigation->count() << pageStack->count();
   return 3;
  }
  for(int i=0;i<20;++i){
   navigation->setCurrentRow(i);
   if(pageStack->currentIndex()!=i){
    qCritical() << "SELF-TEST FAILED: navigation entry" << i
                << "did not select its matching page";
    return 4;
   }
  }
  qInfo() << "SELF-TEST PASSED: all 20 workspaces construct, navigation maps correctly, Code Studio explorer exists and both terminal tab containers are initialized";
  return 0;
 }

 MainWindow *workspace=nullptr;
 AccessScreen access;
 SplashScreen splash;
 QObject::connect(&splash,&SplashScreen::finished,&splash,[&]{
  splash.hide();
  if(QScreen *screen=QGuiApplication::primaryScreen()){
   QRect g=screen->availableGeometry();access.move(g.center()-QPoint(access.width()/2,access.height()/2));
  }
  access.show();
 });
 QObject::connect(&access,&AccessScreen::enterRequested,&app,[&](const QString &displayName){
  if(!workspace)workspace=new MainWindow;
  workspace->setWindowTitle(QString("EVILWARE | The Unholy Core 3 — %1").arg(displayName));
  access.hide();workspace->showMaximized();
 });
 if(QScreen *screen=QGuiApplication::primaryScreen()){
  QRect g=screen->availableGeometry();splash.move(g.center()-QPoint(splash.width()/2,splash.height()/2));
 }
 splash.show();
 const int result=app.exec();
 delete workspace;
 return result;
}
