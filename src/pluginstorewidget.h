#pragma once
#include <QWidget>
class QLineEdit;
class QVBoxLayout;
class QLabel;
class PluginStoreWidget : public QWidget {
 Q_OBJECT
public:
 explicit PluginStoreWidget(QWidget *parent=nullptr);
private:
 QLineEdit *search;
 QWidget *listHost;
 QVBoxLayout *listLayout;
 QLabel *status;
 void rebuild(const QString &filter={});
 void checkTool(const QString &name,const QString &packageId);
 void runPackageAction(const QString &name,const QString &packageId,const QString &action);
};
