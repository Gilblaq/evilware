#pragma once
#include <QWidget>

class QLineEdit;
class QLabel;
class QTableWidget;
class QTimer;
class QCheckBox;

class SystemMonitorWidget : public QWidget {
    Q_OBJECT
public:
    explicit SystemMonitorWidget(QWidget *parent = nullptr);

private:
    QLineEdit *filterEdit = nullptr;
    QLabel *summaryLabel = nullptr;
    QTableWidget *processTable = nullptr;
    QTimer *refreshTimer = nullptr;
    QCheckBox *autoRefresh = nullptr;

    void refreshProcesses();
    void filterProcesses(const QString &text);
};
