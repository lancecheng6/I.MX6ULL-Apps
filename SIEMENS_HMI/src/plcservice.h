#ifndef PLCSERVICE_H
#define PLCSERVICE_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include "snap7client.h"
#include "plcdata.h"

class PlcServiceWorker : public QObject
{
    Q_OBJECT
public:
    PlcServiceWorker(Snap7Client *client);
public slots:
    void doRead();
signals:
    void dataReady(float pressureIn, float pressureOut, float tempIn, float tempOut,
                   float pressureBoiler, float levelBoiler, float tempBoiler, float levelTank,
                   bool feedPump, bool addPump, bool airPump, bool circlePump,
                   bool outValve, bool systemState);
    void errorOccurred(const QString &msg);
private:
    Snap7Client *m_client;
};

class PlcService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool feedPump READ feedPump NOTIFY feedPumpChanged)
    Q_PROPERTY(bool addPump READ addPump NOTIFY addPumpChanged)
    Q_PROPERTY(bool airPump READ airPump NOTIFY airPumpChanged)
    Q_PROPERTY(bool circlePump READ circlePump NOTIFY circlePumpChanged)
    Q_PROPERTY(bool outValve READ outValve NOTIFY outValveChanged)
    Q_PROPERTY(bool systemState READ systemState NOTIFY systemStateChanged)
    Q_PROPERTY(float pressureIn READ pressureIn NOTIFY dataChanged)
    Q_PROPERTY(float pressureOut READ pressureOut NOTIFY dataChanged)
    Q_PROPERTY(float tempIn READ tempIn NOTIFY dataChanged)
    Q_PROPERTY(float tempOut READ tempOut NOTIFY dataChanged)
    Q_PROPERTY(float pressureBoiler READ pressureBoiler NOTIFY dataChanged)
    Q_PROPERTY(float levelBoiler READ levelBoiler NOTIFY dataChanged)
    Q_PROPERTY(float tempBoiler READ tempBoiler NOTIFY dataChanged)
    Q_PROPERTY(float levelTank READ levelTank NOTIFY dataChanged)

public:
    explicit PlcService(QObject *parent = nullptr);
    ~PlcService();

    bool isConnected() const { return m_connected; }
    bool feedPump() const { return m_data.FeedPumpState; }
    bool addPump() const { return m_data.AddPumpState; }
    bool airPump() const { return m_data.AirPumpState; }
    bool circlePump() const { return m_data.CirclePumpState; }
    bool outValve() const { return m_data.OutValveState; }
    bool systemState() const { return m_data.SystemState; }
    float pressureIn() const { return m_data.PressureIn; }
    float pressureOut() const { return m_data.PressureOut; }
    float tempIn() const { return m_data.TempIn; }
    float tempOut() const { return m_data.TempOut; }
    float pressureBoiler() const { return m_data.PressureBoiler; }
    float levelBoiler() const { return m_data.LevelBoiler; }
    float tempBoiler() const { return m_data.TempBoiler; }
    float levelTank() const { return m_data.LevelTank; }

    Q_INVOKABLE void connectToPlc(const QString &ip);
    Q_INVOKABLE void disconnectPlc();
    Q_INVOKABLE void feedPumpControl(bool start);
    Q_INVOKABLE void addPumpControl(bool start);
    Q_INVOKABLE void airPumpControl(bool start);
    Q_INVOKABLE void circlePumpControl(bool start);
    Q_INVOKABLE void outValveControl(bool open);
    Q_INVOKABLE QString plcIp();

signals:
    void connectedChanged();
    void dataChanged();
    void feedPumpChanged();
    void addPumpChanged();
    void airPumpChanged();
    void circlePumpChanged();
    void outValveChanged();
    void systemStateChanged();
    void errorOccurred(const QString &msg);

private slots:
    void onDataReady(float, float, float, float, float, float, float, float,
                     bool, bool, bool, bool, bool, bool);
    void onWorkerError(const QString &msg);

private:
    Snap7Client *m_client = nullptr;
    PlcData m_data;
    bool m_connected = false;
    QTimer *m_timer = nullptr;
    QThread *m_thread = nullptr;
    PlcServiceWorker *m_worker = nullptr;
    QString m_ip;
};

#endif
