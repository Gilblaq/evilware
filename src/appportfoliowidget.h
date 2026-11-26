#pragma once
#include <QWidget>
#include <QJsonArray>

class QTableWidget;
class QLabel;
class QLineEdit;

class AppPortfolioWidget : public QWidget {
    Q_OBJECT
public:
    explicit AppPortfolioWidget(QWidget *parent = nullptr);
private:
    QTableWidget *table;
    QLabel *summary;
    QLineEdit *search;
    QJsonArray apps;
    QString dataFile() const;
    void load();
    void save();
    void refresh();
    void addApp();
    void editApp();
    void removeApp();
    void openAppFolder();
};
