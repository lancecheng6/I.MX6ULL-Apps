#include "DeepSeekclient.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QSslError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ChatGPTClient::ChatGPTClient(QObject *parent)
    : QObject(parent)
    , m_net(new QNetworkAccessManager(this))
{
}

void ChatGPTClient::setApiKey(const QString &key)
{
    m_apiKey = key;
}

void ChatGPTClient::sendMessage(const QString &text)
{
    if (m_apiKey.isEmpty()) {
        emit replyError("API key not set. Edit .env file.");
        return;
    }

    m_history.append(qMakePair(QStringLiteral("user"), text));

    QJsonArray messages;
    messages.append(QJsonObject{{"role", "system"}, {"content", "You are a helpful assistant."}});
    for (const auto &m : m_history)
        messages.append(QJsonObject{{"role", m.first}, {"content", m.second}});

    QJsonObject payload;
    payload.insert("model", "deepseek-v4-flash");
    payload.insert("messages", messages);

    QNetworkRequest request(QUrl("https://opencode.ai/zen/go/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_apiKey.toUtf8());
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();
    sslConf.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(sslConf);

    QNetworkReply *reply = m_net->post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::sslErrors, reply, [reply](const QList<QSslError> &) { reply->ignoreSslErrors(); });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onFinished(reply); });
}

void ChatGPTClient::onFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit replyError(reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    QJsonParseError parseErr;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError) {
        emit replyError("JSON parse error");
        return;
    }

    const QJsonArray choices = doc.object().value("choices").toArray();
    if (choices.isEmpty()) {
        emit replyError("No choices in response");
        return;
    }

    const QString content = choices.first().toObject().value("message").toObject().value("content").toString();
    m_history.append(qMakePair(QStringLiteral("assistant"), content));
    emit replyReceived(content);
}
