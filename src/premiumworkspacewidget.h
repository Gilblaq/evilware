#pragma once
#include <QWidget>
#include <QJsonObject>
#include <functional>
class QLineEdit;
class QLabel;
class QTableWidget;
class QNetworkAccessManager;
class PremiumWorkspaceWidget : public QWidget {
 Q_OBJECT
public:
 explicit PremiumWorkspaceWidget(QWidget *parent=nullptr);
private:
 QLineEdit *apiUrl;
 QLineEdit *email;
 QLineEdit *password;
 QLabel *status;
 QTableWidget *customers;
 QNetworkAccessManager *network;
 QString token;
 QString role;
 void request(const QByteArray &method,const QString &path,const QJsonObject &body,
              const std::function<void(QJsonObject)> &done,bool authenticated=true);
 void refreshAccount();
 void refreshCustomers();
 void showMessage(const QString &message);
};