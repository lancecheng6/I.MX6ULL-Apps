#ifndef PIAGENTCLIENT_H
#define PIAGENTCLIENT_H

#include <QObject>
#include <QProcess>
#include <QByteArray>
#include <QTimer>

class PiAgentclient : public QObject
{
    Q_OBJECT
public:
    explicit PiAgentclient(QObject *parent = nullptr);
    ~PiAgentclient();

    Q_INVOKABLE void startAgent();
    Q_INVOKABLE void sendMessage(const QString &text);
    Q_INVOKABLE void stopAgent();

signals:
    void replyReceived(const QString &reply);
    void replyError(const QString &error);
    void partialUpdate(const QString &text);
    void agentReady();
    void agentStopped();

private slots:
    void onReadyRead();
    void onProcessError(QProcess::ProcessError error);

private:
    void handleLine(const QByteArray &line);

    QProcess *m_proc;
    QByteArray m_buffer;
    QString m_turnText;
    QString m_pendingText;
    QTimer *m_throttle;
    bool m_running;
};

#endif // PIAGENTCLIENT_H
