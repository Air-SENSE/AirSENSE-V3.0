#include "SensorPMS7003.h"
PMS pms7003(Serial1);
// #define _DB_LOG_

SensorPMS7003::SensorPMS7003()
{
}

SensorPMS7003::~SensorPMS7003()
{
}

void SensorPMS7003::init()
{
    pms7003.passiveMode();
    pms7003.wakeUp();
}
void SensorPMS7003::getData()
{
    // pms7003.read(this->data);
    // Serial.println(data.PM_AE_UG_2_5);
    // // pms7003.requestRead();
    // readData();
}
void SensorPMS7003::sleep()
{
    pms7003.sleep();
}
void SensorPMS7003::wakeUp()
{
    pms7003.wakeUp();
}
void SensorPMS7003::activeMode()
{
    pms7003.activeMode();
}
void SensorPMS7003::passiveMode()
{
    pms7003.passiveMode();
}
bool SensorPMS7003::isConnected()
{
    //send a message
    pms7003.activeMode();
    delay(10);
    pms7003.sleep();
    //wait a moment for reciever
    delay(10);
    //if buffer is not empty, there is a connection.
    if(Serial1.available())
    {
        pms7003.wakeUp();
        return true;
    } 
    return false;
}
uint16_t SensorPMS7003::getPM1()
{
    return this->data.PM_AE_UG_1_0;
}
uint16_t SensorPMS7003::getPM2p5()
{
    return this->data.PM_AE_UG_2_5;
}
uint16_t SensorPMS7003::getPM10()
{
    return this->data.PM_AE_UG_10_0;
}
void SensorPMS7003::readData()
{
    // Clear buffer (removes potentially old data) before read. Some data could have been also sent before switching to passive mode.
    while (Serial1.available()) { Serial1.read(); }
    #ifdef _DB_LOG_
        Serial.println("Send read request...");
    #endif

    pms7003.requestRead();

    #ifdef _DB_LOG_
        Serial.println("Reading data...");
    #endif
    if (pms7003.readUntil(this->data))
    {
    #ifdef _DB_LOG_
        Serial.println();

        Serial.print("PM 1.0 (ug/m3): ");
        Serial.println(data.PM_AE_UG_1_0);

        Serial.print("PM 2.5 (ug/m3): ");
        Serial.println(data.PM_AE_UG_2_5);

        Serial.print("PM 10.0 (ug/m3): ");
        Serial.println(data.PM_AE_UG_10_0);

        Serial.println();
    #endif
    }
    else
    {
    #ifdef _DB_LOG_
        Serial.println("No data.");
    #endif
    }
}