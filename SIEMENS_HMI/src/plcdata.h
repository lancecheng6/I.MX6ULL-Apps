#ifndef PLCDATA_H
#define PLCDATA_H

#include <cstdint>
#include <cstring>
#include <QObject>

typedef uint8_t byte;

struct PlcData
{
    bool FeedPumpState = false;
    bool AddPumpState = false;
    bool AirPumpState = false;
    bool CirclePumpState = false;
    bool OutValveState = false;
    bool SystemState = false;

    float PressureIn = 0;
    float PressureOut = 0;
    float TempIn = 0;
    float TempOut = 0;
    float PressureBoiler = 0;
    float LevelBoiler = 0;
    float TempBoiler = 0;
    float LevelTank = 0;

    static float getBeFloat(const byte *b)
    {
        uint32_t u = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
                     ((uint32_t)b[2] << 8) | b[3];
        float f;
        memcpy(&f, &u, 4);
        return f;
    }

    void parse(const byte *buf)
    {
        FeedPumpState = (buf[0] >> 0) & 1;
        AddPumpState  = (buf[0] >> 1) & 1;
        AirPumpState  = (buf[0] >> 2) & 1;
        CirclePumpState = (buf[0] >> 3) & 1;
        OutValveState = (buf[0] >> 4) & 1;
        SystemState   = (buf[2] >> 0) & 1;

        PressureIn     = getBeFloat(buf + 4);
        PressureOut    = getBeFloat(buf + 8);
        TempIn         = getBeFloat(buf + 12);
        TempOut        = getBeFloat(buf + 16);
        PressureBoiler = getBeFloat(buf + 20);
        LevelBoiler    = getBeFloat(buf + 24);
        TempBoiler     = getBeFloat(buf + 28);
        LevelTank      = getBeFloat(buf + 32);
    }
};

#endif
