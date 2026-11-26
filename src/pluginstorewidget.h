#pragma once
#include <QWidget>
class QLineEdit;
class QVBoxLayout;
class QLabel;
class QNetworkAccessManager;
class PluginStoreWidget : public QWidget {
 Q_OBJECT
public:
 explicit PluginStoreWidget(QWidget *parent=nullptr);
private:
 QLineEdit *search;
 QWidget *listHost;
 QVBoxLayout *listLayout;
 QLabel *status;
 QNetworkAccessManager *network;
 void rebuild(const QString &filter={});
 void checkTool(const QString &name,const QString &packageId);
 void runPackageAction(const QString &name,const QString &packageId,const QString &action);
 void installPlugin(const QString &id,const QString &name,const QString &relativePath);
 void removePlugin(const QString &id,const QString &name);
 QString pluginDirectory(const QString &id) const;
};
