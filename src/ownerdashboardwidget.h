#pragma once
#include <QWidget>
#include <QJsonArray>
class QLabel;
class QTableWidget;
class OwnerDashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit OwnerDashboardWidget(QWidget *parent = nullptr);
private:
    QLabel *appCount;
    QLabel *activeCount;
    QLabel *releaseCount;
    QLabel *downloadCount;
    QTableWidget *table;
    void refresh();
    void loadReleaseStats();
};
