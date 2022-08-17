/**
 * @file SensorPMS7003.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#ifndef _SENSOR_PMS7003_H_
#define _SENSOR_PMS7003_H_
#include "PMS.h"
#include "Arduino.h"
#include "Config.h"

#define PMS7003_BAUD_RATE 9600

class SensorPMS7003
{
private:
    PMS::DATA data;
public:
    // PMS pms7003(Serial1);
    void init();
    void getData();
    void sleep();
    void wakeUp();
    void activeMode();
    void passiveMode();
    bool isConnected();
    void readData();
    uint16_t getPM1();
    uint16_t getPM2p5();
    uint16_t getPM10();
    SensorPMS7003();
    ~SensorPMS7003();
};

#endif