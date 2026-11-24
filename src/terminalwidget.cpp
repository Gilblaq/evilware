#include "terminalwidget.h"
#include <QComboBox>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QKeyEvent>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDir>
#include <QFont>
#include <QMessageBox>
#include <functional>

class TerminalConsole final : public QPlainTextEdit {
public:
    explicit TerminalConsole(QWidget *parent = nullptr) : QPlainTextEdit(parent) {
        setReadOnly(false);
        setUndoRedoEnabled(false);
        setLineWrapMode(QPlainTextEdit::NoWrap);
        setFont(QFont("Consolas", 10));
        inputStart = document()->characterCount() - 1;
    }

    std::function<void(const QString &)> submit;
    QStringList history;
    int historyIndex = 0;

    void resetInputRegion() {
        inputStart = document()->characterCount() - 1;
        QTextCursor cursor(document());
        cursor.movePosition(QTextCursor::End);
        setTextCursor(cursor);
    }

    void writeOutput(const QString &output) {
        if (output.isEmpty()) return;
        QTextCursor current = textCursor();
        const int oldInputStart = qBound(0, inputStart, document()->characterCount() - 1);
        const int cursorOffset = qMax(0, current.position() - oldInputStart);
        QTextCursor edit(document());
        edit.setPosition(oldInputStart);
        edit.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        const QString pendingInput = edit.selectedText();
        edit.removeSelectedText();
        edit.insertText(output);
        inputStart = edit.position();
        edit.insertText(pendingInput);
        QTextCursor restored(document());
        restored.setPosition(qMin(inputStart + cursorOffset, document()->characterCount() - 1));
        setTextCursor(restored);
        ensureCursorVisible();
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        QTextCursor cursor = textCursor();
        const int endPosition = document()->characterCount() - 1;
        if (cursor.position() < inputStart || cursor.anchor() < inputStart) {
            cursor.movePosition(QTextCursor::End);
            setTextCursor(cursor);
        }

        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            QTextCursor commandCursor(document());
            commandCursor.setPosition(inputStart);
            commandCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
            const QString command = commandCursor.selectedText();
            commandCursor.movePosition(QTextCursor::End);
            commandCursor.insertText("\n");
            inputStart = commandCursor.position();
            if (!command.trimmed().isEmpty()) {
                history.append(command);
                historyIndex = history.size();
            }
            if (submit) submit(command);
            return;
        }

        if (event->key() == Qt::Key_Backspace && cursor.position() <= inputStart) return;
        if (event->key() == Qt::Key_Delete && cursor.position() < inputStart) return;

        if (event->key() == Qt::Key_Up && !history.isEmpty()) {
            historyIndex = qMax(0, historyIndex - 1);
            replaceInput(history.value(historyIndex));
            return;
        }
        if (event->key() == Qt::Key_Down && !history.isEmpty()) {
            historyIndex = qMin(history.size(), historyIndex + 1);
            replaceInput(historyIndex == history.size() ? QString() : history.value(historyIndex));
            return;
        }

        if (event->key() == Qt::Key_Home) {
            cursor.setPosition(inputStart);
            setTextCursor(cursor);
            return;
        }
        if (event->key() == Qt::Key_L && event->modifiers().testFlag(Qt::ControlModifier)) {
            clear();
            resetInputRegion();
            return;
        }
        QPlainTextEdit::keyPressEvent(event);
        Q_UNUSED(endPosition);
    }

private:
    int inputStart = 0;

    void replaceInput(const QString &value) {
        QTextCursor edit(document());
        edit.setPosition(inputStart);
        edit.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        edit.insertText(value);
        setTextCursor(edit);
    }
};

TerminalWidget::TerminalWidget(QWidget *parent) : QWidget(parent), proc(new QProcess(this)) {
    auto *layout = new QVBoxLayout(this);
    auto *bar = new QHBoxLayout;
    shell = new QComboBox;
    shell->addItems({"PowerShell", "Command Prompt", "WSL (if installed)"});
    auto *restart = new QPushButton("Restart shell");
    auto *stop = new QPushButton("Stop process");
    auto *clear = new QPushButton("Clear");
    status = new QLabel("Shell stopped");
    bar->addWidget(new QLabel("Environment"));
    bar->addWidget(shell);
    bar->addWidget(restart);
    bar->addWidget(stop);
    bar->addWidget(clear);
    bar->addWidget(status, 1);
    layout->addLayout(bar);

    console = new TerminalConsole;
    console->setObjectName("terminalConsole");
    console->setPlaceholderText("Type directly here and press Enter to run a command.");
    layout->addWidget(console, 1);

    connect(restart, &QPushButton::clicked, this, &TerminalWidget::startShell);
    connect(stop, &QPushButton::clicked, this, [this] {
        if (proc->state() == QProcess::NotRunning) return;
        status->setText("Stopping process…");
        proc->terminate();
        if (!proc->waitForFinished(1200)) proc->kill();
    });
    connect(clear, &QPushButton::clicked, this, [this] {
        console->clear();
        console->resetInputRegion();
    });
    connect(shell, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { startShell(); });
    connect(proc, &QProcess::started, this, [this] { status->setText("Running"); });
    connect(proc, &QProcess::readyReadStandardOutput, this, &TerminalWidget::readOutput);
    connect(proc, &QProcess::readyReadStandardError, this, &TerminalWidget::readOutput);
    connect(proc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &TerminalWidget::processEnded);
    connect(proc, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        status->setText("Shell unavailable");
        if (error == QProcess::FailedToStart) {
            console->writeOutput("\n[EVILWARE] Could not start the selected shell. Check that it is installed and available on PATH.\n");
        } else {
            console->writeOutput("\n[EVILWARE] The shell process reported an error.\n");
        }
    });

    console->submit = [this](const QString &command) { sendCommand(command); };
    startShell();
}

void TerminalWidget::setShellIndex(int index) {
    if (index >= 0 && index < shell->count() && shell->currentIndex() != index)
        shell->setCurrentIndex(index);
}

void TerminalWidget::startShell() {
    if (proc->state() != QProcess::NotRunning) {
        proc->terminate();
        if (!proc->waitForFinished(1000)) proc->kill();
    }

    QString program;
    QStringList arguments;
#ifdef Q_OS_WIN
    if (shell->currentIndex() == 0) {
        program = "powershell.exe";
        arguments = {"-NoLogo", "-NoProfile", "-NoExit", "-Command", "-"};
    } else if (shell->currentIndex() == 1) {
        program = "cmd.exe";
        arguments = {"/Q", "/K"};
    } else {
        program = "wsl.exe";
        arguments = {"--exec", "bash", "-l"};
    }
#else
    program = "bash";
    arguments = {"-i"};
#endif
    proc->setWorkingDirectory(QDir::homePath());
    console->clear();
    console->resetInputRegion();
    console->writeOutput("EVILWARE terminal\nStarting " + program + "…\n");
    status->setText("Starting shell…");
    proc->start(program, arguments);
    console->setFocus();
}

void TerminalWidget::sendCommand(const QString &command) {
    if (command.trimmed().isEmpty()) return;
    if (proc->state() != QProcess::Running) {
        console->writeOutput("[Shell is not running. Restart it and try again.]\n");
        return;
    }
    proc->write((command + "\n").toLocal8Bit());
    status->setText("Command sent");
}

void TerminalWidget::readOutput() {
    const QByteArray standardOutput = proc->readAllStandardOutput();
    const QByteArray standardError = proc->readAllStandardError();
    if (!standardOutput.isEmpty()) console->writeOutput(QString::fromLocal8Bit(standardOutput));
    if (!standardError.isEmpty()) console->writeOutput(QString::fromLocal8Bit(standardError));
    status->setText("Running");
}

void TerminalWidget::processEnded(int exitCode, QProcess::ExitStatus exitStatus) {
    status->setText(QString("Exited (%1)").arg(exitCode));
    console->writeOutput(QString("\n[Shell exited: %1]\n").arg(exitStatus == QProcess::NormalExit ? exitCode : -1));
}
