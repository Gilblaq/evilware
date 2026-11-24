#pragma once
#include <QWidget>
#include <QProcess>
class QComboBox;class QLabel;class TerminalConsole;
class TerminalWidget:public QWidget{Q_OBJECT
public:explicit TerminalWidget(QWidget*parent=nullptr);void setShellIndex(int index);
private slots:void startShell();void readOutput();void processEnded(int,QProcess::ExitStatus);
private:QComboBox*shell=nullptr;TerminalConsole*console=nullptr;QProcess*proc=nullptr;QLabel*status=nullptr;void sendCommand(const QString&command);
};
