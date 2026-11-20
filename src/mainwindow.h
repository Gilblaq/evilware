#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
class TerminalWidget; class EditorWidget; class ProjectWidget; class CyberLabWidget;
class MainWindow : public QMainWindow {
 Q_OBJECT
public: explicit MainWindow(QWidget *parent=nullptr);
private:
 QStackedWidget *pages; QListWidget *nav; TerminalWidget *terminal;
 EditorWidget *editor; ProjectWidget *projects; CyberLabWidget *cyber;
 QWidget *dashboard(); QWidget *filePage(); QWidget *gitPage(); QWidget *settingsPage();
 void addPage(const QString &name, QWidget *page); void showPage(int row);
};
