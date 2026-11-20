#pragma once
#include <QWidget>
class QComboBox; class QLineEdit; class QPlainTextEdit; class QProcess;
class CyberLabWidget:public QWidget{
 Q_OBJECT
public: explicit CyberLabWidget(QWidget *parent=nullptr);
private slots: void runDiagnostic();
private: QComboBox *tool; QLineEdit *target; QPlainTextEdit *result; QProcess *process;
};
