#include "PiAgentclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QProcessEnvironment>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

PiAgentclient::PiAgentclient(QObject *parent)
    : QObject(parent), m_proc(new QProcess(this)), m_running(false),
      m_throttle(new QTimer(this))
{
    m_proc->setProcessChannelMode(QProcess::MergedChannels);
    m_throttle->setInterval(100);
    m_throttle->setSingleShot(true);
    connect(m_throttle, &QTimer::timeout, this, [this]() {
        emit partialUpdate(m_pendingText);
    });
    connect(m_proc, &QProcess::readyReadStandardOutput,
            this, &PiAgentclient::onReadyRead);
    connect(m_proc, &QProcess::errorOccurred,
            this, &PiAgentclient::onProcessError);
    connect(m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus) {
        m_running = false;
        m_throttle->stop();
        QFile dbg("/tmp/piagent_dbg.txt");
        if (dbg.open(QIODevice::Append)) {
            QTextStream(&dbg) << "EXIT code=" << code << "\n";
        }
        emit replyError(QString("pi exited with code %1").arg(code));
    });
}

PiAgentclient::~PiAgentclient()
{
    if (m_proc->state() != QProcess::NotRunning) {
        m_proc->terminate();
        if (!m_proc->waitForFinished(2000))
            m_proc->kill();
    }
}

void PiAgentclient::startAgent()
{
    if (m_running)
        return;
    m_turnText.clear();
    m_buffer.clear();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path = env.value("PATH");
    if (!path.contains("/opt/node/bin"))
        env.insert("PATH", "/opt/node/bin:" + path);
    env.insert("HOME", "/root");
    m_proc->setProcessEnvironment(env);

    QFile dbg("/tmp/piagent_dbg.txt");
    if (dbg.open(QIODevice::Append)) {
        QTextStream(&dbg) << "START env.PATH=" << env.value("PATH")
                          << " HOME=" << env.value("HOME") << "\n";
    }

    m_proc->start("/bin/sh", QStringList()
                  << "-c"
                  << "/opt/node/bin/node /opt/node/lib/node_modules/@earendil-works/pi-coding-agent/dist/cli.js --mode rpc --provider opencode-zen --model deepseek-v4-flash");
    m_running = true;
    emit agentReady();
}

void PiAgentclient::sendMessage(const QString &text)
{
    if (!m_running || m_proc->state() == QProcess::NotRunning)
        startAgent();

    QJsonObject obj;
    obj.insert("id", QString("q%1").arg(QDateTime::currentMSecsSinceEpoch()));
    obj.insert("type", QStringLiteral("prompt"));
    obj.insert("message", text);
    m_turnText.clear();
    m_proc->write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n");
}

void PiAgentclient::stopAgent()
{
    if (!m_running)
        return;
    m_proc->terminate();
    if (!m_proc->waitForFinished(2000))
        m_proc->kill();
    m_running = false;
    emit agentStopped();
}

void PiAgentclient::onReadyRead()
{
    m_buffer += m_proc->readAllStandardOutput();
    int idx;
    while ((idx = m_buffer.indexOf('\n')) >= 0) {
        const QByteArray line = m_buffer.left(idx);
        m_buffer.remove(0, idx + 1);
        if (!line.trimmed().isEmpty())
            handleLine(line.trimmed());
    }
}

void PiAgentclient::handleLine(const QByteArray &line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        QFile log("/tmp/piagent.log");
        if (log.open(QIODevice::Append)) {
            QTextStream(&log) << "PI_RAW: " << line << "\n";
        }
        return;
    }
    const QJsonObject obj = doc.object();
    const QString type = obj.value("type").toString();

    if (type == "message_update") {
        const QJsonObject ev = obj.value("assistantMessageEvent").toObject();
        const QString evType = ev.value("type").toString();
        if (evType == "text_delta" || evType == "text_start") {
            m_turnText += ev.value("delta").toString();
            m_pendingText = m_turnText;
            if (!m_throttle->isActive())
                m_throttle->start();
        }
    } else if (type == "agent_end") {
        m_throttle->stop();
        emit replyReceived(m_turnText);
        m_turnText.clear();
    } else if (type == "response") {
        const QString cmd = obj.value("command").toString();
        const bool ok = obj.value("success").toBool();
        if (cmd == "prompt" && !ok)
            emit replyError("Prompt rejected by pi agent");
    } else if (type == "error") {
        emit replyError(obj.value("message").toString());
    }
}

void PiAgentclient::onProcessError(QProcess::ProcessError error)
{
    QFile dbg("/tmp/piagent_dbg.txt");
    if (dbg.open(QIODevice::Append)) {
        QTextStream(&dbg) << "ERROR " << error << " "
                          << m_proc->errorString() << "\n";
    }
    if (error == QProcess::FailedToStart) {
        m_running = false;
        emit replyError("Failed to start pi: " + m_proc->errorString());
    }
}
