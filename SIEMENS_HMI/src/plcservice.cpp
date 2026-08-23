#include "plcservice.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>

static QString defaultIp()
{
    QString path = QCoreApplication::applicationDirPath() + "/plcinfo.ini";
    QSettings s(path, QSettings::IniFormat);
    return s.value("PLC/IPAddress", "192.168.1.200").toString();
}

PlcServiceWorker::PlcServiceWorker(Snap7Client *client) : m_client(client) {}

void PlcServiceWorker::doRead()
{
    if (!m_client->isConnected()) {
        emit errorOccurred("not connected");
        return;
    }
    PlcData data;
    if (m_client->readDB1(data)) {
        emit dataReady(data.PressureIn, data.PressureOut,
                       data.TempIn, data.TempOut,
                       data.PressureBoiler, data.LevelBoiler,
                       data.TempBoiler, data.LevelTank,
                       data.FeedPumpState, data.AddPumpState,
                       data.AirPumpState, data.CirclePumpState,
                       data.OutValveState, data.SystemState);
    } else {
        emit errorOccurred("read failed");
    }
}

PlcService::PlcService(QObject *parent) : QObject(parent)
{
    m_client = new Snap7Client();
    m_timer = new QTimer(this);
    m_timer->setInterval(200);

    m_worker = new PlcServiceWorker(m_client);
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);
    m_thread->start();

    connect(m_timer, &QTimer::timeout, m_worker, &PlcServiceWorker::doRead);
    connect(m_worker, &PlcServiceWorker::dataReady, this, &PlcService::onDataReady);
    connect(m_worker, &PlcServiceWorker::errorOccurred, this, &PlcService::onWorkerError);
}

PlcService::~PlcService()
{
    m_timer->stop();
    m_thread->quit();
    m_thread->wait();
    delete m_worker;
    delete m_client;
}

void PlcService::connectToPlc(const QString &ip)
{
    m_ip = ip.isEmpty() ? defaultIp() : ip;
    if (m_client->connect(m_ip)) {
        m_connected = true;
        emit connectedChanged();
        m_timer->start();
    } else {
        emit errorOccurred("connect failed to " + m_ip);
    }
}

void PlcService::disconnectPlc()
{
    m_timer->stop();
    m_client->disconnect();
    m_connected = false;
    emit connectedChanged();
    // Do not clear m_data here; sensor values persist after disconnect
}

void PlcService::feedPumpControl(bool start)
{
    if (m_client->isConnected()) {
        m_client->pulseBit(1, 100, start ? 0 : 1);
        m_client->writeQOutput(0, 0, start);
    }
}

void PlcService::addPumpControl(bool start)
{
    if (m_client->isConnected()) {
        m_client->pulseBit(1, 100, start ? 2 : 3);
        m_client->writeQOutput(0, 1, start);
    }
}

void PlcService::airPumpControl(bool start)
{
    if (m_client->isConnected()) {
        m_client->pulseBit(1, 100, start ? 4 : 5);
        m_client->writeQOutput(0, 2, start);
    }
}

void PlcService::circlePumpControl(bool start)
{
    if (m_client->isConnected()) {
        m_client->pulseBit(1, 100, start ? 6 : 7);
        m_client->writeQOutput(0, 3, start);
    }
}

void PlcService::outValveControl(bool open)
{
    if (m_client->isConnected()) {
        m_client->pulseBit(1, 101, open ? 0 : 1);
        m_client->writeQOutput(0, 4, open);
    }
}

QString PlcService::plcIp()
{
    return m_ip.isEmpty() ? defaultIp() : m_ip;
}

void PlcService::onDataReady(float pIn, float pOut, float tIn, float tOut,
                              float pBoiler, float lBoiler, float tBoiler, float lTank,
                              bool fPump, bool aPump, bool airPump, bool cPump,
                              bool oValve, bool sysState)
{
    m_data.PressureIn = pIn;
    m_data.PressureOut = pOut;
    m_data.TempIn = tIn;
    m_data.TempOut = tOut;
    m_data.PressureBoiler = pBoiler;
    m_data.LevelBoiler = lBoiler;
    m_data.TempBoiler = tBoiler;
    m_data.LevelTank = lTank;

    bool changed = (m_data.FeedPumpState != fPump || m_data.AddPumpState != aPump ||
                    m_data.AirPumpState != airPump || m_data.CirclePumpState != cPump ||
                    m_data.OutValveState != oValve || m_data.SystemState != sysState);

    m_data.FeedPumpState = fPump;
    m_data.AddPumpState = aPump;
    m_data.AirPumpState = airPump;
    m_data.CirclePumpState = cPump;
    m_data.OutValveState = oValve;
    m_data.SystemState = sysState;

    if (changed) {
        emit feedPumpChanged();
        emit addPumpChanged();
        emit airPumpChanged();
        emit circlePumpChanged();
        emit outValveChanged();
        emit systemStateChanged();
    }
    emit dataChanged();
}

void PlcService::onWorkerError(const QString &msg)
{
    emit errorOccurred(msg);
}
