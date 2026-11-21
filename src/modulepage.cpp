#include "modulepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QListWidget>
ModulePage::ModulePage(const QString &name,const QString &description,const QStringList &capabilities,const QStringList &dependencies,const QString &status,QWidget *parent):QWidget(parent){
 auto *root=new QVBoxLayout(this);root->setContentsMargins(28,26,28,26);root->setSpacing(16);
 auto *eyebrow=new QLabel("EVILWARE WORKSPACE MODULE");eyebrow->setObjectName("mutedText");root->addWidget(eyebrow);
 auto *title=new QLabel(name);title->setObjectName("pageTitle");root->addWidget(title);
 auto *intro=new QLabel(description);intro->setObjectName("bodyText");intro->setWordWrap(true);root->addWidget(intro);
 auto *statusCard=new QFrame;statusCard->setObjectName("infoCard");auto *statusLayout=new QHBoxLayout(statusCard);statusLayout->setContentsMargins(16,12,16,12);
 auto *statusLabel=new QLabel("MODULE STATUS");statusLabel->setObjectName("mutedText");
 auto *statusValue=new QLabel(status);statusValue->setObjectName("cardTitle");statusLayout->addWidget(statusLabel);statusLayout->addStretch();statusLayout->addWidget(statusValue);root->addWidget(statusCard);
 auto *grid=new QHBoxLayout;grid->setSpacing(14);
 auto makeCard=[&](const QString &heading,const QStringList &items){
  auto *card=new QFrame;card->setObjectName("infoCard");auto *v=new QVBoxLayout(card);v->setContentsMargins(16,14,16,16);
  auto *h=new QLabel(heading);h->setObjectName("cardTitle");v->addWidget(h);
  for(const QString &item:items){auto *line=new QLabel(QString("•  %1").arg(item));line->setObjectName("bodyText");line->setWordWrap(true);v->addWidget(line);}
  v->addStretch();return card;
 };
 grid->addWidget(makeCard("WORKSPACE CAPABILITIES",capabilities),3);
 grid->addWidget(makeCard("INTEGRATIONS & DEPENDENCIES",dependencies),2);
 root->addLayout(grid);
 auto *actions=new QHBoxLayout;auto *plugins=new QPushButton("Open Plugin Store");plugins->setObjectName("primaryButton");actions->addWidget(plugins);actions->addStretch();root->addLayout(actions);
 auto *note=new QLabel("EVILWARE only marks a module ready when its tools and actions are implemented and verified. A listed capability is a planned workspace area, not a claim that an external tool is already installed.");note->setObjectName("mutedText");note->setWordWrap(true);root->addWidget(note);root->addStretch();
 connect(plugins,&QPushButton::clicked,this,&ModulePage::openPluginStoreRequested);
}
