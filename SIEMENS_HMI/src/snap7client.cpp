#include "snap7client.h"
#include <unistd.h>

Snap7Client::Snap7Client() : m_cli(Cli_Create()), m_connected(false) {}

Snap7Client::~Snap7Client()
{
    if (m_connected) Cli_Disconnect(m_cli);
    Cli_Destroy(&m_cli);
}

bool Snap7Client::connect(const QString &ip, int rack, int slot)
{
    if (!m_cli) return false;
    if (m_connected) Cli_Disconnect(m_cli);
    int res = Cli_ConnectTo(m_cli, ip.toUtf8().constData(), rack, slot);
    m_connected = (res == 0);
    return m_connected;
}

void Snap7Client::disconnect()
{
    if (m_connected) {
        Cli_Disconnect(m_cli);
        m_connected = false;
    }
}

bool Snap7Client::readDB1(PlcData &data)
{
    if (!m_connected) return false;
    byte buf[36] = {0};
    int res = Cli_DBRead(m_cli, 1, 0, 36, buf);
    if (res != 0) return false;
    data.parse(buf);
    return true;
}

bool Snap7Client::pulseBit(int dbNumber, int byteIndex, int bitIndex)
{
    if (!m_connected) return false;
    byte one = 1, zero = 0;
    int addr = (byteIndex << 3) | bitIndex;
    int r1 = Cli_WriteArea(m_cli, S7AreaDB, dbNumber, addr, 1, S7WLBit, &one);
    usleep(80000);
    int r2 = Cli_WriteArea(m_cli, S7AreaDB, dbNumber, addr, 1, S7WLBit, &zero);
    return r1 == 0 && r2 == 0;
}

bool Snap7Client::writeBit(int dbNumber, int byteIndex, int bitIndex, bool value)
{
    if (!m_connected) return false;
    byte v = value ? 1 : 0;
    int addr = (byteIndex << 3) | bitIndex;
    return Cli_WriteArea(m_cli, S7AreaDB, dbNumber, addr, 1, S7WLBit, &v) == 0;
}

bool Snap7Client::writeQOutput(int byteIndex, int bitIndex, bool value)
{
    FILE *f = fopen("/tmp/scada_debug.log", "a");
    if (f) { fprintf(f, "writeQOutput byte=%d bit=%d val=%d connected=%d\n", byteIndex, bitIndex, value, m_connected); fclose(f); }
    if (!m_connected) return false;
    byte v = value ? 1 : 0;
    int addr = (byteIndex << 3) | bitIndex;
    bool ok = Cli_WriteArea(m_cli, S7AreaPA, 0, addr, 1, S7WLBit, &v) == 0;
    if (f) { f = fopen("/tmp/scada_debug.log", "a"); fprintf(f, "  Q write result=%d\n", ok); fclose(f); }
    return ok;
}
