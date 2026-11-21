#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
class TerminalWidget;
class EditorWidget;
class ProjectWidget;
class CyberLabWidget;
class PluginStoreWidget;
class QLabel;

class MainWindow : public QMainWindow {
 Q_OBJECT
public:
 explicit MainWindow(QWidget *parent=nullptr);
private:
 QStackedWidget *pages;
 QListWidget *nav;
 TerminalWidget *terminal;
 EditorWidget *editor;
 ProjectWidget *projects;
 CyberLabWidget *cyber;
 PluginStoreWidget *plugins;
 QLabel *activePage;
 QWidget *dashboard();
 QWidget *filePage();
 QWidget *gitPage();
 QWidget *settingsPage();
 void addPage(QWidget *page);
 void showPage(int row);
};
