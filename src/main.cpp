#include "mainwindow.h"
#include "startupwidget.h"
#include <QApplication>
#include <QFont>
#include <QGuiApplication>
#include <QScreen>
int main(int argc,char *argv[]){
 QApplication app(argc,argv);
 QApplication::setApplicationName("EVILWARE");
 QApplication::setOrganizationName("EVILWARE");
 QApplication::setApplicationVersion("3.0.0");
 app.setStyle("Fusion");
 app.setFont(QFont("Segoe UI",10));
 MainWindow workspace;
 AccessScreen access;
 SplashScreen splash;
 QObject::connect(&splash,&SplashScreen::finished,&splash,[&]{
  splash.hide();
  if(QScreen *screen=QGuiApplication::primaryScreen()){
   QRect g=screen->availableGeometry();access.move(g.center()-QPoint(access.width()/2,access.height()/2));
  }
  access.show();
 });
 QObject::connect(&access,&AccessScreen::enterRequested,&workspace,[&](const QString &displayName){
  workspace.setWindowTitle(QString("EVILWARE | The Unholy Core 3 — %1").arg(displayName));
  access.hide();workspace.showMaximized();
 });
 if(QScreen *screen=QGuiApplication::primaryScreen()){
  QRect g=screen->availableGeometry();splash.move(g.center()-QPoint(splash.width()/2,splash.height()/2));
 }
 splash.show();
 return app.exec();
}
