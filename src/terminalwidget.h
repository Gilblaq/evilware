#pragma once
#include <QWidget>
#include <QProcess>

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QLabel;

class TerminalWidget : public QWidget {
 Q_OBJECT
public:
 explicit TerminalWidget(QWidget *parent=nullptr);
 void setShellIndex(int index);
private slots:
 void startShell();
 void sendCommand();
 void readOutput();
 void processEnded(int,QProcess::ExitStatus);
private:
 QComboBox *shell;
 QLineEdit *input;
 QPlainTextEdit *output;
 QProcess *proc;
 QLabel *status;
 void append(const QString &text);
};
