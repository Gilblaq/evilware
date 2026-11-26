#include "ownerdashboardwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDesktopServices>

OwnerDashboardWidget::OwnerDashboardWidget(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24,20,24,24);
    root->setSpacing(14);
    auto *title = new QLabel("EVILWARE OWNER CONSOLE"); title->setObjectName("pageTitle");
    auto *intro = new QLabel("Private local overview of your app portfolio and public EVILWARE release performance. Portfolio data stays on this computer.");
    intro->setObjectName("mutedText"); intro->setWordWrap(true);
    root->addWidget(title); root->addWidget(intro);
    auto *grid = new QGridLayout; grid->setSpacing(12);
    auto addMetric = [grid](const QString &name, QLabel **value, int col) {
        auto *card = new QFrame; card->setObjectName("infoCard");
        auto *layout = new QVBoxLayout(card); layout->setContentsMargins(16,14,16,14);
        auto *label = new QLabel(name); label->setObjectName("mutedText");
        *value = new QLabel("—"); (*value)->setObjectName("cardTitle");
        (*value)->setStyleSheet("font-size:20pt;font-weight:700;");
        layout->addWidget(label); layout->addWidget(*value); grid->addWidget(card,0,col);
    };
    addMetric("APPLICATIONS TRACKED",&appCount,0);
    addMetric("ACTIVE / RELEASED",&activeCount,1);
    addMetric("PUBLIC RELEASES",&releaseCount,2);
    addMetric("TOTAL DOWNLOADS",&downloadCount,3);
    root->addLayout(grid);
    auto *row = new QHBoxLayout;
    auto *refreshButton = new QPushButton("Refresh dashboard"); refreshButton->setObjectName("primaryButton");
    auto *releasesButton = new QPushButton("Open GitHub Releases");
    row->addWidget(refreshButton); row->addWidget(releasesButton); row->addStretch(); root->addLayout(row);
    table = new QTableWidget(0,4);
    table->setHorizontalHeaderLabels({"Application","Version","Stage","Platform"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    root->addWidget(table,1);
    connect(refreshButton,&QPushButton::clicked,this,[this]{refresh();loadReleaseStats();});
    connect(releasesButton,&QPushButton::clicked,this,[]{QDesktopServices::openUrl(QUrl("https://github.com/Gilblaq/evilware/releases"));});
    refresh(); loadReleaseStats();
}
void OwnerDashboardWidget::refresh() {
    const QString filePath = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("my_apps.json");
    QFile file(filePath); QJsonArray apps;
    if(file.open(QIODevice::ReadOnly)) { const auto doc=QJsonDocument::fromJson(file.readAll()); if(doc.isArray()) apps=doc.array(); }
    int active=0; table->setRowCount(0);
    for(const auto &entry:apps) {
        const auto app=entry.toObject(); const QString stage=app.value("stage").toString();
        if(stage=="Active"||stage=="Released") ++active;
        const int r=table->rowCount(); table->insertRow(r);
        const QStringList values={app.value("name").toString(),app.value("version").toString(),stage,app.value("platform").toString()};
        for(int c=0;c<values.size();++c) table->setItem(r,c,new QTableWidgetItem(values[c]));
    }
    appCount->setText(QString::number(apps.size()));
    activeCount->setText(QString::number(active));
}
void OwnerDashboardWidget::loadReleaseStats() {
    auto *manager=new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.github.com/repos/Gilblaq/evilware/releases"));
    request.setHeader(QNetworkRequest::UserAgentHeader,"EVILWARE-Owner-Console");
    request.setRawHeader("Accept","application/vnd.github+json");
    auto *reply=manager->get(request);
    connect(reply,&QNetworkReply::finished,this,[this,reply,manager]{
        int releases=0; qint64 downloads=0;
        if(reply->error()==QNetworkReply::NoError) {
            const auto doc=QJsonDocument::fromJson(reply->readAll());
            if(doc.isArray()) { releases=doc.array().size(); for(const auto &item:doc.array()) for(const auto &asset:item.toObject().value("assets").toArray()) downloads+=asset.toObject().value("download_count").toInteger(); }
            releaseCount->setText(QString::number(releases));
            downloadCount->setText(QString::number(downloads));
        } else {
            releaseCount->setText("Unavailable"); downloadCount->setText("Unavailable");
        }
        reply->deleteLater(); manager->deleteLater();
    });
}
