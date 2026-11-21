#pragma once
#include <QWidget>
#include <QString>
class QLineEdit; class QComboBox; class QLabel; class QPushButton; class QPlainTextEdit; class QProcess;
class ProjectWidget:public QWidget{
 Q_OBJECT
public: explicit ProjectWidget(QWidget *parent=nullptr);
private slots: void createProject(); void chooseFolder(); void buildOrRun();
private:
 QLineEdit *name,*location; QComboBox *kind; QLabel *message;
 QPushButton *buildRunButton; QPlainTextEdit *output;
 QString projectPath; int projectKind=-1; QProcess *activeProcess=nullptr; int buildStage=0;
 void ensureDependency(const QString &label,const QString &executable,const QString &packageId);
 void startProcess(const QString &program,const QStringList &arguments,int stage);
 void appendOutput(const QString &text);
 void finishBuildStage(int exitCode,QProcess::ExitStatus status);
};
