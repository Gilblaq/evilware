#include "systemmonitorwidget.h"

#include <QCheckBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#endif

SystemMonitorWidget::SystemMonitorWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(12);

    auto *title = new QLabel("System Monitor");
    title->setObjectName("pageTitle");
    layout->addWidget(title);

    auto *description = new QLabel(
        "Read-only view of running processes on this Windows device. "
        "Some protected or elevated processes may hide memory details.");
    description->setObjectName("mutedText");
    description->setWordWrap(true);
    layout->addWidget(description);

    resourceLabel = new QLabel("CPU: collecting sample...    Memory: reading...");
    resourceLabel->setObjectName("cardTitle");
    layout->addWidget(resourceLabel);

    auto *toolbar = new QHBoxLayout;
    filterEdit = new QLineEdit;
    filterEdit->setPlaceholderText("Filter by process name or PID...");
    toolbar->addWidget(filterEdit, 1);

    auto *refreshButton = new QPushButton("Refresh");
    refreshButton->setObjectName("primaryButton");
    toolbar->addWidget(refreshButton);

    autoRefresh = new QCheckBox("Auto-refresh (3 sec)");
    autoRefresh->setChecked(true);
    toolbar->addWidget(autoRefresh);
    layout->addLayout(toolbar);

    summaryLabel = new QLabel("Reading process information...");
    summaryLabel->setObjectName("mutedText");
    layout->addWidget(summaryLabel);

    processTable = new QTableWidget(this);
    processTable->setColumnCount(5);
    processTable->setHorizontalHeaderLabels(
        {"Process", "PID", "Parent PID", "Threads", "Working set"});
    processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    processTable->setSelectionMode(QAbstractItemView::SingleSelection);
    processTable->setAlternatingRowColors(true);
    processTable->setSortingEnabled(false);
    processTable->verticalHeader()->setVisible(false);
    processTable->horizontalHeader()->setStretchLastSection(true);
    processTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    processTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    processTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    processTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    layout->addWidget(processTable, 1);

    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(3000);
    resourceTimer = new QTimer(this);
    resourceTimer->setInterval(1000);
    connect(refreshButton, &QPushButton::clicked, this, &SystemMonitorWidget::refreshProcesses);
    connect(filterEdit, &QLineEdit::textChanged, this, &SystemMonitorWidget::filterProcesses);
    connect(autoRefresh, &QCheckBox::toggled, this, [this](bool enabled) {
        if (enabled) {
            refreshTimer->start();
            refreshProcesses();
        } else {
            refreshTimer->stop();
        }
    });
    connect(refreshTimer, &QTimer::timeout, this, &SystemMonitorWidget::refreshProcesses);
    connect(resourceTimer, &QTimer::timeout, this, &SystemMonitorWidget::refreshResources);
    refreshTimer->start();
    resourceTimer->start();
    refreshResources();
    refreshProcesses();
}

void SystemMonitorWidget::refreshProcesses() {
#ifdef Q_OS_WIN
    struct ProcessRow {
        QString name;
        DWORD pid;
        DWORD parentPid;
        DWORD threads;
        quint64 workingSet;
    };

    QList<ProcessRow> rows;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        summaryLabel->setText("Windows could not provide the process snapshot. Try Refresh again.");
        return;
    }

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) {
        do {
            ProcessRow row;
            row.name = QString::fromWCharArray(entry.szExeFile);
            row.pid = entry.th32ProcessID;
            row.parentPid = entry.th32ParentProcessID;
            row.threads = entry.cntThreads;
            row.workingSet = 0;

            HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                         FALSE, entry.th32ProcessID);
            if (process) {
                PROCESS_MEMORY_COUNTERS memory{};
                memory.cb = sizeof(memory);
                if (GetProcessMemoryInfo(process, &memory, sizeof(memory)))
                    row.workingSet = static_cast<quint64>(memory.WorkingSetSize);
                CloseHandle(process);
            }
            rows.append(row);
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);

    processTable->setUpdatesEnabled(false);
    processTable->setRowCount(0);
    for (const ProcessRow &row : rows) {
        const int index = processTable->rowCount();
        processTable->insertRow(index);
        processTable->setItem(index, 0, new QTableWidgetItem(row.name));
        processTable->setItem(index, 1, new QTableWidgetItem(QString::number(row.pid)));
        processTable->setItem(index, 2, new QTableWidgetItem(QString::number(row.parentPid)));
        processTable->setItem(index, 3, new QTableWidgetItem(QString::number(row.threads)));
        const QString memoryText = row.workingSet
            ? QString("%1 MB").arg(double(row.workingSet) / (1024.0 * 1024.0), 0, 'f', 1)
            : QString("Unavailable");
        processTable->setItem(index, 4, new QTableWidgetItem(memoryText));
    }
    processTable->setUpdatesEnabled(true);
    summaryLabel->setText(QString("%1 processes found • Read-only inspection • Refreshed just now")
                              .arg(rows.size()));
    filterProcesses(filterEdit->text());
#else
    summaryLabel->setText("Process inspection is currently implemented for Windows.");
#endif
}

void SystemMonitorWidget::refreshResources() {
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    QString memoryText = "Memory: unavailable";
    if (GlobalMemoryStatusEx(&memory)) {
        const double totalGb = double(memory.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0);
        const double usedGb = double(memory.ullTotalPhys - memory.ullAvailPhys) / (1024.0 * 1024.0 * 1024.0);
        memoryText = QString("Memory: %1%  (%2 / %3 GB)")
            .arg(memory.dwMemoryLoad)
            .arg(usedGb, 0, 'f', 1)
            .arg(totalGb, 0, 'f', 1);
    }

    FILETIME idleTime{}, kernelTime{}, userTime{};
    QString cpuText = "CPU: sampling...";
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        auto toValue = [](const FILETIME &value) -> quint64 {
            ULARGE_INTEGER result{};
            result.LowPart = value.dwLowDateTime;
            result.HighPart = value.dwHighDateTime;
            return result.QuadPart;
        };
        const quint64 idle = toValue(idleTime);
        const quint64 kernel = toValue(kernelTime);
        const quint64 user = toValue(userTime);
        if (hasCpuSample) {
            const quint64 idleDelta = idle - previousIdle;
            const quint64 totalDelta = (kernel - previousKernel) + (user - previousUser);
            if (totalDelta > 0) {
                const double busy = 100.0 * double(totalDelta - qMin(idleDelta, totalDelta)) / double(totalDelta);
                cpuText = QString("CPU: %1%").arg(busy, 0, 'f', 1);
            }
        }
        previousIdle = idle;
        previousKernel = kernel;
        previousUser = user;
        hasCpuSample = true;
    }
    resourceLabel->setText(cpuText + "     •     " + memoryText);
#else
    resourceLabel->setText("Live CPU and memory metrics are currently supported on Windows.");
#endif
}

void SystemMonitorWidget::filterProcesses(const QString &text) {
    const QString needle = text.trimmed();
    for (int row = 0; row < processTable->rowCount(); ++row) {
        const bool match = needle.isEmpty()
            || processTable->item(row, 0)->text().contains(needle, Qt::CaseInsensitive)
            || processTable->item(row, 1)->text().contains(needle, Qt::CaseInsensitive);
        processTable->setRowHidden(row, !match);
    }
}
