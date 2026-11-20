#pragma once
#include <QWidget>
class QComboBox; class QLineEdit; class QPlainTextEdit; class QProcess; class QLabel;
class TerminalWidget : public QWidget {
 Q_OBJECT
public: explicit TerminalWidget(QWidget *parent=nullptr);
private slots: void startShell(); void sendCommand(); void readOutput(); void processEnded(int,QProcess::ExitStatus);
private: QComboBox *shell; QLineEdit *input; QPlainTextEdit *output; QProcess *proc; QLabel *status;
 void append(const QString &text);
};
