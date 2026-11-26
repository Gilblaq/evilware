#include "appportfoliowidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <QAbstractItemView>

AppPortfolioWidget::AppPortfolioWidget(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 20, 24, 24);
    root->setSpacing(12);

    auto *title = new QLabel("My Apps");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    auto *intro = new QLabel("Your private, local app portfolio. Track projects, versions, release stages, repositories and working folders. Data stays on this computer.");
    intro->setObjectName("mutedText");
    intro->setWordWrap(true);
    root->addWidget(intro);

    auto *summaryCard = new QWidget;
    summaryCard->setObjectName("infoCard");
    auto *summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(14, 10, 14, 10);
    summary = new QLabel("0 applications tracked");
    summary->setObjectName("cardTitle");
    summaryLayout->addWidget(summary);
    root->addWidget(summaryCard);

    auto *toolbar = new QHBoxLayout;
    search = new QLineEdit;
    search->setPlaceholderText("Search your apps...");
    auto *add = new QPushButton("Add app");
    add->setObjectName("primaryButton");
    auto *edit = new QPushButton("Edit");
    auto *remove = new QPushButton("Remove");
    auto *open = new QPushButton("Open folder");
    toolbar->addWidget(search, 1);
    toolbar->addWidget(add);
    toolbar->addWidget(edit);
    toolbar->addWidget(remove);
    toolbar->addWidget(open);
    root->addLayout(toolbar);

    table = new QTableWidget(0, 5);
    table->setObjectName("appPortfolioTable");
    table->setHorizontalHeaderLabels({"Application", "Version", "Stage", "Platform", "Working folder"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    root->addWidget(table, 1);

    auto *subscriptionTitle = new QLabel("SUBSCRIPTIONS — FUTURE MODULE");
    subscriptionTitle->setObjectName("cardTitle");
    root->addWidget(subscriptionTitle);
    auto *subscriptionNote = new QLabel("Subscription plans, pricing, billing cycles, renewal reminders and customer status are reserved for a later update. This version does not collect payments or store payment-card details.");
    subscriptionNote->setObjectName("mutedText");
    subscriptionNote->setWordWrap(true);
    root->addWidget(subscriptionNote);

    connect(add, &QPushButton::clicked, this, &AppPortfolioWidget::addApp);
    connect(edit, &QPushButton::clicked, this, &AppPortfolioWidget::editApp);
    connect(remove, &QPushButton::clicked, this, &AppPortfolioWidget::removeApp);
    connect(open, &QPushButton::clicked, this, &AppPortfolioWidget::openAppFolder);
    connect(search, &QLineEdit::textChanged, this, &AppPortfolioWidget::refresh);
    connect(table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { openAppFolder(); });

    load();
    refresh();
}

QString AppPortfolioWidget::dataFile() const {
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return QDir(base).filePath("my_apps.json");
}

void AppPortfolioWidget::load() {
    QFile file(dataFile());
    if (!file.exists() || !file.open(QIODevice::ReadOnly))
        return;
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error == QJsonParseError::NoError && doc.isArray())
        apps = doc.array();
}

void AppPortfolioWidget::save() {
    QSaveFile file(dataFile());
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Save failed", "EVILWARE could not save your app portfolio locally.");
        return;
    }
    file.write(QJsonDocument(apps).toJson(QJsonDocument::Indented));
    if (!file.commit())
        QMessageBox::warning(this, "Save failed", "EVILWARE could not safely finish saving your app portfolio.");
}

void AppPortfolioWidget::refresh() {
    const QString filter = search->text().trimmed();
    table->setRowCount(0);
    int active = 0;
    for (int i = 0; i < apps.size(); ++i) {
        const QJsonObject app = apps.at(i).toObject();
        if (app.value("stage").toString() == "Active" || app.value("stage").toString() == "Released")
            ++active;
        const QStringList values = {
            app.value("name").toString(),
            app.value("version").toString(),
            app.value("stage").toString(),
            app.value("platform").toString(),
            app.value("folder").toString()
        };
        bool matches = filter.isEmpty();
        for (const QString &value : values)
            if (value.contains(filter, Qt::CaseInsensitive)) matches = true;
        if (!matches) continue;
        const int row = table->rowCount();
        table->insertRow(row);
        for (int col = 0; col < values.size(); ++col) {
            auto *item = new QTableWidgetItem(values[col]);
            item->setData(Qt::UserRole, i);
            table->setItem(row, col, item);
        }
    }
    summary->setText(QString("%1 applications tracked  ·  %2 active or released")
                     .arg(apps.size()).arg(active));
}

void AppPortfolioWidget::addApp() {
    QDialog dialog(this);
    dialog.setWindowTitle("Add application");
    dialog.resize(520, 460);
    auto *form = new QFormLayout(&dialog);
    auto *name = new QLineEdit;
    auto *version = new QLineEdit("0.1.0");
    auto *stage = new QComboBox;
    stage->addItems({"In development", "Active", "Released", "Paused", "Archived"});
    auto *platform = new QComboBox;
    platform->addItems({"Windows", "Web", "Android", "iOS", "Linux", "Cross-platform", "Other"});
    auto *folder = new QLineEdit;
    auto *browse = new QPushButton("Browse...");
    auto *folderRow = new QHBoxLayout;
    folderRow->addWidget(folder, 1);
    folderRow->addWidget(browse);
    auto *repo = new QLineEdit;
    repo->setPlaceholderText("https://github.com/...");
    auto *notes = new QPlainTextEdit;
    notes->setMaximumHeight(90);
    form->addRow("Application name *", name);
    form->addRow("Current version", version);
    form->addRow("Development stage", stage);
    form->addRow("Platform", platform);
    form->addRow("Working folder", folderRow);
    form->addRow("Website / repository", repo);
    form->addRow("Notes", notes);
    auto *buttons = new QHBoxLayout;
    auto *saveButton = new QPushButton("Save application");
    saveButton->setObjectName("primaryButton");
    auto *cancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(saveButton);
    form->addRow(buttons);
    connect(browse, &QPushButton::clicked, &dialog, [&] {
        const QString selected = QFileDialog::getExistingDirectory(&dialog, "Choose application folder", folder->text());
        if (!selected.isEmpty()) folder->setText(selected);
    });
    connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, &dialog, [&] {
        if (name->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Application name required", "Enter a name for this application.");
            return;
        }
        dialog.accept();
    });
    if (dialog.exec() != QDialog::Accepted) return;
    QJsonObject item;
    item["name"] = name->text().trimmed();
    item["version"] = version->text().trimmed();
    item["stage"] = stage->currentText();
    item["platform"] = platform->currentText();
    item["folder"] = folder->text().trimmed();
    item["repository"] = repo->text().trimmed();
    item["notes"] = notes->toPlainText().trimmed();
    item["created"] = QDate::currentDate().toString(Qt::ISODate);
    item["subscription"] = QJsonObject{{"enabled", false}, {"plan", ""}, {"price", ""}, {"cycle", ""}, {"renewal", ""}};
    apps.append(item);
    save();
    refresh();
}

void AppPortfolioWidget::editApp() {
    const int row = table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Select an app", "Select an application to edit."); return; }
    const int index = table->item(row, 0)->data(Qt::UserRole).toInt();
    QJsonObject item = apps.at(index).toObject();
    QDialog dialog(this);
    dialog.setWindowTitle("Edit application");
    dialog.resize(520, 460);
    auto *form = new QFormLayout(&dialog);
    auto *name = new QLineEdit(item.value("name").toString());
    auto *version = new QLineEdit(item.value("version").toString());
    auto *stage = new QComboBox;
    stage->addItems({"In development", "Active", "Released", "Paused", "Archived"});
    stage->setCurrentText(item.value("stage").toString());
    auto *platform = new QComboBox;
    platform->addItems({"Windows", "Web", "Android", "iOS", "Linux", "Cross-platform", "Other"});
    platform->setCurrentText(item.value("platform").toString());
    auto *folder = new QLineEdit(item.value("folder").toString());
    auto *browse = new QPushButton("Browse...");
    auto *folderRow = new QHBoxLayout;
    folderRow->addWidget(folder, 1);
    folderRow->addWidget(browse);
    auto *repo = new QLineEdit(item.value("repository").toString());
    auto *notes = new QPlainTextEdit(item.value("notes").toString());
    notes->setMaximumHeight(90);
    form->addRow("Application name *", name);
    form->addRow("Current version", version);
    form->addRow("Development stage", stage);
    form->addRow("Platform", platform);
    form->addRow("Working folder", folderRow);
    form->addRow("Website / repository", repo);
    form->addRow("Notes", notes);
    auto *buttons = new QHBoxLayout;
    auto *saveButton = new QPushButton("Save changes");
    saveButton->setObjectName("primaryButton");
    auto *cancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(cancel);
    buttons->addWidget(saveButton);
    form->addRow(buttons);
    connect(browse, &QPushButton::clicked, &dialog, [&] {
        const QString selected = QFileDialog::getExistingDirectory(&dialog, "Choose application folder", folder->text());
        if (!selected.isEmpty()) folder->setText(selected);
    });
    connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, &dialog, [&] {
        if (name->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Application name required", "Enter a name for this application.");
            return;
        }
        dialog.accept();
    });
    if (dialog.exec() != QDialog::Accepted) return;
    item["name"] = name->text().trimmed();
    item["version"] = version->text().trimmed();
    item["stage"] = stage->currentText();
    item["platform"] = platform->currentText();
    item["folder"] = folder->text().trimmed();
    item["repository"] = repo->text().trimmed();
    item["notes"] = notes->toPlainText().trimmed();
    apps[index] = item;
    save();
    refresh();
}

void AppPortfolioWidget::removeApp() {
    const int row = table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Select an app", "Select an application to remove."); return; }
    const int index = table->item(row, 0)->data(Qt::UserRole).toInt();
    const QString name = apps.at(index).toObject().value("name").toString();
    if (QMessageBox::question(this, "Remove application", "Remove " + name + " from your portfolio? This does not delete its project files.", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;
    apps.removeAt(index);
    save();
    refresh();
}

void AppPortfolioWidget::openAppFolder() {
    const int row = table->currentRow();
    if (row < 0) { QMessageBox::information(this, "Select an app", "Select an application first."); return; }
    const int index = table->item(row, 0)->data(Qt::UserRole).toInt();
    const QString folder = apps.at(index).toObject().value("folder").toString();
    if (folder.isEmpty() || !QDir(folder).exists()) {
        QMessageBox::information(this, "Folder unavailable", "No valid working folder has been saved for this application.");
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(folder));
}
