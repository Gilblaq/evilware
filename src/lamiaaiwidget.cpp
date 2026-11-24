#include "lamiaaiwidget.h"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QMessageBox>
#include <QDateTime>
#ifdef Q_OS_WIN
#include <windows.h>
#include <wincrypt.h>
#endif

namespace {
QByteArray protectKey(const QByteArray &plain) {
#ifdef Q_OS_WIN
    if (plain.isEmpty()) return {};
    DATA_BLOB input{};
    input.cbData = static_cast<DWORD>(plain.size());
    input.pbData = reinterpret_cast<BYTE *>(const_cast<char *>(plain.constData()));
    DATA_BLOB output{};
    if (!CryptProtectData(&input, L"EVILWARE LAMIA AI API key", nullptr, nullptr, nullptr, 0, &output))
        return {};
    QByteArray encrypted(reinterpret_cast<const char *>(output.pbData), static_cast<qsizetype>(output.cbData));
    LocalFree(output.pbData);
    return encrypted;
#else
    Q_UNUSED(plain);
    return {};
#endif
}
QByteArray unprotectKey(const QByteArray &encrypted) {
#ifdef Q_OS_WIN
    if (encrypted.isEmpty()) return {};
    DATA_BLOB input{};
    input.cbData = static_cast<DWORD>(encrypted.size());
    input.pbData = reinterpret_cast<BYTE *>(const_cast<char *>(encrypted.constData()));
    DATA_BLOB output{};
    if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, 0, &output))
        return {};
    QByteArray plain(reinterpret_cast<const char *>(output.pbData), static_cast<qsizetype>(output.cbData));
    SecureZeroMemory(output.pbData, output.cbData);
    LocalFree(output.pbData);
    return plain;
#else
    Q_UNUSED(encrypted);
    return {};
#endif
}
}

LamiaAIWidget::LamiaAIWidget(QWidget *parent) : QWidget(parent), network(new QNetworkAccessManager(this)) {
    setObjectName("lamiaAIWorkspace");
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 20, 24, 20);
    root->setSpacing(12);

    auto *title = new QLabel("LAMIA AI");
    title->setObjectName("pageTitle");
    root->addWidget(title);
    auto *subtitle = new QLabel("Your built-in development and security assistant. Connect a local Ollama model or an OpenAI-compatible API to activate chat.");
    subtitle->setObjectName("mutedText");
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto *splitter = new QSplitter(Qt::Horizontal);
    splitter->setChildrenCollapsible(false);
    auto *configuration = new QGroupBox("PROVIDER CONFIGURATION");
    configuration->setMinimumWidth(285);
    configuration->setMaximumWidth(390);
    auto *form = new QFormLayout(configuration);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    provider = new QComboBox;
    provider->setObjectName("lamiaProvider");
    provider->addItems({"Ollama (Local)", "OpenAI-compatible API"});
    endpoint = new QLineEdit;
    endpoint->setObjectName("lamiaEndpoint");
    model = new QComboBox;
    model->setObjectName("lamiaModel");
    model->setEditable(true);
    apiKey = new QLineEdit;
    apiKey->setObjectName("lamiaApiKey");
    apiKey->setEchoMode(QLineEdit::Password);
    apiKey->setPlaceholderText("API key (hosted providers)");
    form->addRow("Provider", provider);
    form->addRow("Base URL", endpoint);
    form->addRow("Model", model);
    form->addRow("API key", apiKey);

    auto *save = new QPushButton("Save configuration");
    save->setObjectName("lamiaSaveConfig");
    auto *test = new QPushButton("Test connection");
    test->setObjectName("lamiaTestConnection");
    auto *providerActions = new QHBoxLayout;
    providerActions->addWidget(save);
    providerActions->addWidget(test);
    form->addRow(providerActions);

    status = new QLabel("Not configured — choose a provider to begin.");
    status->setObjectName("lamiaStatus");
    status->setWordWrap(true);
    form->addRow(status);
    auto *privacy = new QLabel("API keys are protected with Windows DPAPI before being saved. Keys are never written to the application log or repository.");
    privacy->setObjectName("mutedText");
    privacy->setWordWrap(true);
    form->addRow(privacy);
    splitter->addWidget(configuration);

    auto *chatPanel = new QWidget;
    auto *chatLayout = new QVBoxLayout(chatPanel);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    auto *chatHeader = new QHBoxLayout;
    auto *chatName = new QLabel("Lamia");
    chatName->setStyleSheet("font-size:16px;font-weight:700;");
    auto *clear = new QPushButton("Clear conversation");
    chatHeader->addWidget(chatName);
    chatHeader->addStretch();
    chatHeader->addWidget(clear);
    chatLayout->addLayout(chatHeader);

    conversation = new QPlainTextEdit;
    conversation->setObjectName("lamiaChat");
    conversation->setReadOnly(true);
    conversation->setPlaceholderText("LAMIA AI responses will appear here.");
    conversation->appendPlainText("LAMIA AI  /  THE UNHOLY CORE 3\n\nWelcome. I can help with code, debugging, project planning, documentation, terminal explanations and defensive security analysis.\n\nConnect Ollama or an OpenAI-compatible provider on the left to start a real conversation.");
    chatLayout->addWidget(conversation, 1);

    prompt = new QPlainTextEdit;
    prompt->setObjectName("lamiaPrompt");
    prompt->setMaximumHeight(110);
    prompt->setPlaceholderText("Ask Lamia about code, projects, debugging or security…");
    chatLayout->addWidget(prompt);
    auto *sendRow = new QHBoxLayout;
    auto *sendHint = new QLabel("Enter a prompt, then send. Commands are never executed automatically.");
    sendHint->setObjectName("mutedText");
    sendButton = new QPushButton("Send to Lamia");
    sendButton->setObjectName("lamiaSend");
    sendButton->setProperty("primaryButton", true);
    sendButton->setEnabled(false);
    sendRow->addWidget(sendHint, 1);
    sendRow->addWidget(sendButton);
    chatLayout->addLayout(sendRow);
    splitter->addWidget(chatPanel);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({330, 760});
    root->addWidget(splitter, 1);

    QSettings settings("EVILWARE", "TheUnholyCore3");
    provider->setCurrentIndex(settings.value("ai/provider", 0).toInt());
    endpoint->setText(settings.value("ai/endpoint", "http://127.0.0.1:11434").toString());
    const QString savedModel = settings.value("ai/model", "llama3.2").toString();
    model->addItem(savedModel);
    model->setCurrentText(savedModel);
    const QByteArray protectedValue = QByteArray::fromBase64(settings.value("ai/apiKeyProtected").toByteArray());
    const QByteArray key = unprotectKey(protectedValue);
    if (!key.isEmpty()) apiKey->setText(QString::fromUtf8(key));
    apiKey->setPlaceholderText("API key (optional for local compatible endpoints)");
    sendButton->setEnabled(false);
    if (provider->currentIndex() == 1 && endpoint->text().isEmpty())
        endpoint->setText("https://api.openai.com");
    status->setText("Ready to connect. Save your provider settings, then test the connection.");

    connect(provider, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        connected = false;
        sendButton->setEnabled(false);
        if (index == 0) {
            if (endpoint->text().isEmpty() || endpoint->text().startsWith("https://api.openai.com"))
                endpoint->setText("http://127.0.0.1:11434");
            if (model->currentText().isEmpty() || model->currentText() == "gpt-4o-mini")
                model->setCurrentText("llama3.2");
            apiKey->setEnabled(false);
            apiKey->setPlaceholderText("Not required for local Ollama");
        } else {
            if (endpoint->text().startsWith("http://127.0.0.1:11434"))
                endpoint->setText("https://api.openai.com");
            if (model->currentText().isEmpty() || model->currentText() == "llama3.2")
                model->setCurrentText("gpt-4o-mini");
            apiKey->setEnabled(true);
            apiKey->setPlaceholderText("API key (required by most hosted providers)");
        }
    });
    apiKey->setEnabled(provider->currentIndex() == 1);
    auto resetConnection = [this] { connected = false; sendButton->setEnabled(false); };
    connect(endpoint, &QLineEdit::textChanged, this, resetConnection);
    connect(model, &QComboBox::currentTextChanged, this, resetConnection);
    connect(apiKey, &QLineEdit::textChanged, this, resetConnection);
    connect(save, &QPushButton::clicked, this, [this] {
        if (saveConfiguration())
            status->setText("Configuration saved. Test the connection to activate Lamia.");
    });
    connect(test, &QPushButton::clicked, this, &LamiaAIWidget::testConnection);
    connect(sendButton, &QPushButton::clicked, this, &LamiaAIWidget::sendMessage);
    connect(clear, &QPushButton::clicked, this, [this] {
        history = QJsonArray{};
        conversation->clear();
        appendMessage("LAMIA", "Conversation cleared. What would you like to work on?");
    });
}

QString LamiaAIWidget::selectedProvider() const {
    return provider->currentIndex() == 0 ? "ollama" : "openai";
}

QString LamiaAIWidget::normalizedEndpoint(bool chatRequest) const {
    QString base = endpoint->text().trimmed();
    while (base.endsWith('/')) base.chop(1);
    if (selectedProvider() == "ollama")
        return base + (chatRequest ? "/api/chat" : "/api/tags");
    if (chatRequest) {
        if (base.endsWith("/chat/completions")) return base;
        if (base.endsWith("/v1")) return base + "/chat/completions";
        return base + "/v1/chat/completions";
    }
    if (base.endsWith("/models")) return base;
    if (base.endsWith("/v1")) return base + "/models";
    return base + "/v1/models";
}

QByteArray LamiaAIWidget::storedApiKey() const {
    return apiKey->text().trimmed().toUtf8();
}

bool LamiaAIWidget::saveConfiguration() {
    const QUrl url(endpoint->text().trimmed());
    if (!url.isValid() || (url.scheme() != "http" && url.scheme() != "https") || url.host().isEmpty()) {
        QMessageBox::warning(this, "LAMIA AI", "Enter a valid HTTP or HTTPS provider URL.");
        return false;
    }
    QSettings settings("EVILWARE", "TheUnholyCore3");
    settings.setValue("ai/provider", provider->currentIndex());
    settings.setValue("ai/endpoint", endpoint->text().trimmed());
    settings.setValue("ai/model", model->currentText().trimmed());
    if (!apiKey->text().trimmed().isEmpty()) {
        const QByteArray encrypted = protectKey(storedApiKey());
        if (encrypted.isEmpty()) {
            QMessageBox::warning(this, "LAMIA AI", "Windows could not protect this API key. It was not saved. You can still use it for this session.");
            return false;
        }
        settings.setValue("ai/apiKeyProtected", encrypted.toBase64());
    }
    settings.sync();
    return settings.status() == QSettings::NoError;
}

void LamiaAIWidget::testConnection() {
    if (busy) return;
    if (!saveConfiguration()) return;
    QNetworkRequest request(QUrl(normalizedEndpoint(false)));
    request.setTransferTimeout(20000);
    if (selectedProvider() == "openai" && !apiKey->text().trimmed().isEmpty())
        request.setRawHeader("Authorization", "Bearer " + storedApiKey());
    status->setText("Testing provider connection…");
    auto *reply = network->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const QByteArray body = reply->readAll();
        const auto error = reply->error();
        const QString errorText = reply->errorString();
        reply->deleteLater();
        if (error != QNetworkReply::NoError) {
            status->setText("Connection failed: " + errorText);
            appendMessage("SYSTEM", "Provider connection failed. Check the URL, provider service, model host and API key.");
            return;
        }
        QJsonParseError parseError{};
        const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            status->setText("Connected, but the provider returned an unexpected response.");
            return;
        }
        model->clear();
        if (selectedProvider() == "ollama") {
            const QJsonArray models = doc.object().value("models").toArray();
            for (const auto &item : models) {
                const QString name = item.toObject().value("name").toString();
                if (!name.isEmpty()) model->addItem(name);
            }
        } else {
            const QJsonArray models = doc.object().value("data").toArray();
            for (const auto &item : models) {
                const QString name = item.toObject().value("id").toString();
                if (!name.isEmpty()) model->addItem(name);
            }
        }
        if (model->count() == 0) model->addItem(selectedProvider() == "ollama" ? "llama3.2" : "gpt-4o-mini");
        connected = true;
        status->setText("Connected successfully. Choose an available model and start chatting.");
        sendButton->setEnabled(true);
    });
}

void LamiaAIWidget::appendMessage(const QString &speaker, const QString &message) {
    conversation->appendPlainText("\n" + speaker + "\n" + message + "\n");
}

void LamiaAIWidget::sendMessage() {
    if (busy) return;
    if (!connected) {
        status->setText("Connect and test your selected AI provider before sending a message.");
        return;
    }
    const QString userText = prompt->toPlainText().trimmed();
    if (userText.isEmpty()) return;
    if (endpoint->text().trimmed().isEmpty() || model->currentText().trimmed().isEmpty()) {
        status->setText("Configure a provider URL and model first.");
        return;
    }
    if (!saveConfiguration()) return;

    prompt->clear();
    appendMessage("YOU", userText);
    QJsonArray messages;
    messages.append(QJsonObject{{"role", "system"}, {"content",
        "You are Lamia AI, the assistant inside EVILWARE. Help with programming, debugging, project planning, documentation, terminal explanations, databases and defensive cybersecurity. Be accurate and admit uncertainty. Never claim to execute commands or change files. Explain risks and ask for confirmation before recommending destructive operations."}});
    for (const auto &entry : history) messages.append(entry);
    messages.append(QJsonObject{{"role", "user"}, {"content", userText}});
    QJsonObject payload{
        {"model", model->currentText().trimmed()},
        {"messages", messages},
        {"stream", false}
    };
    QNetworkRequest request(QUrl(normalizedEndpoint(true)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(90000);
    if (selectedProvider() == "openai")
        request.setRawHeader("Authorization", "Bearer " + storedApiKey());
    busy = true;
    sendButton->setEnabled(false);
    status->setText("Lamia is thinking…");
    auto *reply = network->post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, userText] {
        const QByteArray body = reply->readAll();
        const auto error = reply->error();
        const QString errorText = reply->errorString();
        reply->deleteLater();
        busy = false;
        sendButton->setEnabled(true);
        if (error != QNetworkReply::NoError) {
            connected = false;
            sendButton->setEnabled(false);
            status->setText("Request failed: " + errorText);
            appendMessage("SYSTEM", "The provider request failed. Verify your endpoint, model name, network connection and API key.");
            return;
        }
        QJsonParseError parseError{};
        const QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            status->setText("The provider returned invalid JSON.");
            appendMessage("SYSTEM", "Could not read the provider response.");
            return;
        }
        QString answer;
        if (selectedProvider() == "ollama")
            answer = doc.object().value("message").toObject().value("content").toString();
        else {
            const QJsonArray choices = doc.object().value("choices").toArray();
            if (!choices.isEmpty())
                answer = choices.first().toObject().value("message").toObject().value("content").toString();
        }
        if (answer.trimmed().isEmpty()) {
            status->setText("The provider returned no assistant message.");
            appendMessage("SYSTEM", "No assistant text was returned. Check that the selected model supports chat completions.");
            return;
        }
        history.append(QJsonObject{{"role", "user"}, {"content", userText}});
        history.append(QJsonObject{{"role", "assistant"}, {"content", answer}});
        while (history.size() > 24) history.removeAt(0);
        appendMessage("LAMIA", answer);
        status->setText("Connected · " + model->currentText());
    });
}
