#ifndef CHATGPTCLIENT_H
#define CHATGPTCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QList>
#include <QPair>

class ChatGPTClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatGPTClient(QObject *parent = nullptr);
    void setApiKey(const QString &key);
    Q_INVOKABLE void sendMessage(const QString &text);

signals:
    void replyReceived(const QString &reply);
    void replyError(const QString &error);

private slots:
    void onFinished(QNetworkReply *reply);

private:
    QString m_apiKey;
    QNetworkAccessManager *m_net;
    QList<QPair<QString, QString> > m_history;
};

#endif // CHATGPTCLIENT_H
