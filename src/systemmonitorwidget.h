#pragma once
#include <QWidget>
#include <QtGlobal>

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
    QLabel *resourceLabel = nullptr;
    QTableWidget *processTable = nullptr;
    QTimer *refreshTimer = nullptr;
    QTimer *resourceTimer = nullptr;
    QCheckBox *autoRefresh = nullptr;
    quint64 previousIdle = 0;
    quint64 previousKernel = 0;
    quint64 previousUser = 0;
    bool hasCpuSample = false;

    void refreshProcesses();
    void refreshResources();
    void filterProcesses(const QString &text);
};
