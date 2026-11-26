#include "premiumworkspacewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

PremiumWorkspaceWidget::PremiumWorkspaceWidget(QWidget *parent):QWidget(parent),network(new QNetworkAccessManager(this)){
 auto *root=new QVBoxLayout(this);root->setContentsMargins(26,24,26,24);root->setSpacing(14);
 auto *title=new QLabel("EVILWARE PREMIUM WORKSPACE");title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel("A subscription workspace for premium tools and services. Free EVILWARE workspaces remain available without a subscription. Premium access is checked by the EVILWARE account service.");intro->setObjectName("mutedText");intro->setWordWrap(true);root->addWidget(intro);
 auto *connection=new QHBoxLayout;apiUrl=new QLineEdit(QSettings("EVILWARE","TheUnholyCore3").value("premium/apiUrl","https://YOUR-API.onrender.com").toString());apiUrl->setPlaceholderText("Your deployed EVILWARE API URL");
 auto *saveApi=new QPushButton("Save API");connection->addWidget(new QLabel("Service"));connection->addWidget(apiUrl,1);connection->addWidget(saveApi);root->addLayout(connection);
 connect(saveApi,&QPushButton::clicked,this,[this]{QSettings("EVILWARE","TheUnholyCore3").setValue("premium/apiUrl",apiUrl->text().trimmed());showMessage("Subscription service address saved.");});
 auto *account=new QGridLayout;email=new QLineEdit;email->setPlaceholderText("Email address");password=new QLineEdit;password->setPlaceholderText("Password (10+ characters)");password->setEchoMode(QLineEdit::Password);
 auto *login=new QPushButton("Sign In");login->setObjectName("primaryButton");auto *registerButton=new QPushButton("Create Account");
 account->addWidget(new QLabel("Account email"),0,0);account->addWidget(email,0,1);account->addWidget(new QLabel("Password"),1,0);account->addWidget(password,1,1);account->addWidget(login,2,0);account->addWidget(registerButton,2,1);root->addLayout(account);
 status=new QLabel("Not signed in. Configure the API address after deploying the subscription service.");status->setObjectName("bodyText");status->setWordWrap(true);root->addWidget(status);
 auto *billing=new QHBoxLayout;auto *monthly=new QPushButton("Subscribe — 30 days");monthly->setObjectName("primaryButton");auto *annual=new QPushButton("Subscribe — 365 days");billing->addWidget(monthly);billing->addWidget(annual);auto *refresh=new QPushButton("Refresh access");billing->addWidget(refresh);billing->addStretch();root->addLayout(billing);
 connect(login,&QPushButton::clicked,this,[this]{request("POST","/api/auth/login",{{"email",email->text().trimmed()},{"password",password->text()}},[this](QJsonObject o){token=o.value("token").toString();role=o.value("user").toObject().value("role").toString();QSettings("EVILWARE","TheUnholyCore3").setValue("premium/token",token);refreshAccount();});});
 connect(registerButton,&QPushButton::clicked,this,[this]{request("POST","/api/auth/register",{{"email",email->text().trimmed()},{"password",password->text()}},[this](QJsonObject o){token=o.value("token").toString();role=o.value("user").toObject().value("role").toString();QSettings("EVILWARE","TheUnholyCore3").setValue("premium/token",token);refreshAccount();});});
 connect(refresh,&QPushButton::clicked,this,&PremiumWorkspaceWidget::refreshAccount);
 auto checkout=[this](const QString &plan){request("POST","/api/billing/checkout",{{"plan",plan}},[this](QJsonObject o){const QUrl url(o.value("authorization_url").toString());if(url.isValid()&&!url.isEmpty())QDesktopServices::openUrl(url);else showMessage("The payment provider did not return a checkout link.");});};
 connect(monthly,&QPushButton::clicked,this,[checkout]{checkout("monthly");});connect(annual,&QPushButton::clicked,this,[checkout]{checkout("annual");});
 auto *ownerTitle=new QLabel("OWNER CUSTOMER MANAGEMENT");ownerTitle->setObjectName("pageTitle");root->addWidget(ownerTitle);
 auto *ownerHint=new QLabel("Visible only to the owner account. Revoke immediately blocks the customer's active premium entitlement on the next server check.");ownerHint->setObjectName("mutedText");ownerHint->setWordWrap(true);root->addWidget(ownerHint);
 auto *ownerActions=new QHBoxLayout;auto *loadCustomers=new QPushButton("Load customers");ownerActions->addWidget(loadCustomers);ownerActions->addStretch();root->addLayout(ownerActions);
 customers=new QTableWidget(0,5);customers->setHorizontalHeaderLabels({"Customer","Plan","Status","Expires","Action"});customers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);customers->setEditTriggers(QAbstractItemView::NoEditTriggers);customers->setSelectionBehavior(QAbstractItemView::SelectRows);root->addWidget(customers,1);
 connect(loadCustomers,&QPushButton::clicked,this,&PremiumWorkspaceWidget::refreshCustomers);
 token=QSettings("EVILWARE","TheUnholyCore3").value("premium/token").toString();
 if(!token.isEmpty())refreshAccount();
}
void PremiumWorkspaceWidget::showMessage(const QString &message){QMessageBox::information(this,"EVILWARE Premium",message);}
void PremiumWorkspaceWidget::request(const QByteArray &method,const QString &path,const QJsonObject &body,const std::function<void(QJsonObject)> &done,bool authenticated){
 QString base=apiUrl->text().trimmed();while(base.endsWith('/'))base.chop(1);
 if(base.isEmpty()||base.contains("YOUR-API")){showMessage("Deploy the subscription API and enter its HTTPS URL first. See server/README.md.");return;}
 QNetworkRequest req(QUrl(base+path));req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
 if(authenticated&&!token.isEmpty())req.setRawHeader("Authorization",("Bearer "+token).toUtf8());
 QNetworkReply *reply=nullptr;const QByteArray payload=QJsonDocument(body).toJson(QJsonDocument::Compact);
 if(method=="POST")reply=network->post(req,payload);else if(method=="GET")reply=network->get(req);else reply=network->sendCustomRequest(req,method,payload);
 connect(reply,&QNetworkReply::finished,this,[this,reply,done]{
  const QByteArray bytes=reply->readAll();const auto doc=QJsonDocument::fromJson(bytes);const QJsonObject obj=doc.object();
  if(reply->error()!=QNetworkReply::NoError){const QString error=obj.value("error").toString(reply->errorString());status->setText("Service response: "+error);reply->deleteLater();return;}
  reply->deleteLater();if(done)done(obj);
 });
}
void PremiumWorkspaceWidget::refreshAccount(){
 request("GET","/api/me",{},[this](QJsonObject o){
  const auto user=o.value("user").toObject(),sub=o.value("subscription").toObject();role=user.value("role").toString();
  if(sub.isEmpty())status->setText("Signed in as "+user.value("email").toString()+". Free account — subscribe to unlock Premium Workspace.");
  else status->setText("Signed in as "+user.value("email").toString()+". PREMIUM ACTIVE · "+sub.value("plan").toString().toUpper()+" · Expires "+sub.value("expires_at").toString());
  if(role=="owner")status->setText("OWNER ACCOUNT · "+user.value("email").toString()+" · "+(sub.isEmpty()?"Owner access is permanent.": "Premium entitlement active."));
  if(role=="owner")refreshCustomers();
 });
}
void PremiumWorkspaceWidget::refreshCustomers(){
 if(role!="owner"){status->setText("Customer management is restricted to the verified owner account.");return;}
 request("GET","/api/admin/customers",{},[this](QJsonObject o){
  const auto list=o.value("customers").toArray();customers->setRowCount(0);
  for(const auto &v:list){const auto c=v.toObject();const int row=customers->rowCount();customers->insertRow(row);
   const QStringList values={c.value("email").toString(),c.value("plan").toString(),c.value("status").toString(),c.value("expires_at").toString()};
   for(int col=0;col<4;++col)customers->setItem(row,col,new QTableWidgetItem(values[col]));
   auto *revoke=new QPushButton("Revoke");customers->setCellWidget(row,4,revoke);const QString id=QString::number(c.value("id").toInteger());
   connect(revoke,&QPushButton::clicked,this,[this,id]{if(QMessageBox::question(this,"Revoke premium access","Remove this customer's active premium access?")!=QMessageBox::Yes)return;
    request("POST","/api/admin/customers/"+id+"/revoke",{},[this](QJsonObject){refreshCustomers();});
   });
  }
 });
}
