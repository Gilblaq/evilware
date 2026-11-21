#include "mainwindow.h"
#include <QApplication>
#include <QFont>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("EVILWARE");
    QApplication::setOrganizationName("EVILWARE");
    QApplication::setApplicationVersion("3.0.0");
    app.setStyle("Fusion");
    QFont f("Segoe UI", 10); app.setFont(f);
    MainWindow window; window.show();
    return app.exec();
}
