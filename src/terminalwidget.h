#pragma once
#include <QWidget>
class QTabWidget;
class TerminalWidget : public QWidget {
public:
    explicit TerminalWidget(QWidget *parent = nullptr);
    void setShellIndex(int index);
private:
    void addTab();
    QTabWidget *tabs = nullptr;
    int defaultShell = 0;
    int tabCounter = 0;
};
