#pragma once
#include <QWidget>
class QLineEdit;
class QVBoxLayout;
class PluginStoreWidget : public QWidget {
 Q_OBJECT
public:
 explicit PluginStoreWidget(QWidget *parent=nullptr);
private:
 QLineEdit *search;
 QWidget *listHost;
 QVBoxLayout *listLayout;
 void rebuild(const QString &filter={});
 void installTool(const QString &name,const QString &packageId);
};
