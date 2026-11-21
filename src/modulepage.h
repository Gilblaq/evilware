#pragma once
#include <QWidget>
#include <QStringList>
class QLabel;
class ModulePage : public QWidget {
 Q_OBJECT
public:
 ModulePage(const QString &name,const QString &description,const QStringList &capabilities,const QStringList &dependencies,const QString &status,QWidget *parent=nullptr);
signals:
 void openPluginStoreRequested();
};
