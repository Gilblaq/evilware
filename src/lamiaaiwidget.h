#pragma once
#include <QWidget>
#include <QJsonArray>
class QComboBox;
class QLabel;
class QLineEdit;
class QNetworkAccessManager;
class QPlainTextEdit;
class QPushButton;
class QNetworkReply;

class LamiaAIWidget : public QWidget {
    Q_OBJECT
public:
    explicit LamiaAIWidget(QWidget *parent = nullptr);
private:
    QComboBox *provider = nullptr;
    QComboBox *model = nullptr;
    QLineEdit *endpoint = nullptr;
    QLineEdit *apiKey = nullptr;
    QPlainTextEdit *conversation = nullptr;
    QPlainTextEdit *prompt = nullptr;
    QLabel *status = nullptr;
    QPushButton *sendButton = nullptr;
    QNetworkAccessManager *network = nullptr;
    QJsonArray history;
    bool busy = false;
    bool connected = false;

    QString selectedProvider() const;
    QString normalizedEndpoint(bool chatRequest) const;
    QByteArray storedApiKey() const;
    bool saveConfiguration();
    void testConnection();
    void sendMessage();
    void appendMessage(const QString &speaker, const QString &message);
};
