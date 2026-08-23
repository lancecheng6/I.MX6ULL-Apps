#ifndef SNAP7CLIENT_H
#define SNAP7CLIENT_H

#include <QString>
#include "snap7.h"
#include "plcdata.h"

class Snap7Client
{
public:
    Snap7Client();
    ~Snap7Client();

    bool connect(const QString &ip, int rack = 0, int slot = 0);
    void disconnect();
    bool isConnected() const { return m_connected; }

    bool readDB1(PlcData &data);
    bool pulseBit(int dbNumber, int byteIndex, int bitIndex);
    bool writeBit(int dbNumber, int byteIndex, int bitIndex, bool value);
    bool writeQOutput(int byteIndex, int bitIndex, bool value);

private:
    S7Object m_cli = 0;
    bool m_connected = false;
};

#endif
