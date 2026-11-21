#pragma once
#include <QWidget>
class QLineEdit; class QComboBox; class QLabel;
class ProjectWidget:public QWidget{
 Q_OBJECT
public: explicit ProjectWidget(QWidget *parent=nullptr);
private slots: void createProject(); void chooseFolder();
private:
 QLineEdit *name,*location; QComboBox *kind; QLabel *message;
 void ensureDependency(const QString &label,const QString &executable,const QString &packageId);
};
