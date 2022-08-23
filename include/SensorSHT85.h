
#ifndef _SENSOR_SHT85_H_
#define _SENSOR_SHT85_H_
#include "Config.h"
#include <Adafruit_Sensor.h>
#include "SHTSensor.h"


class SensorSHT85
{
private:
    SHTSensor sht;
    float Temperature;
    float Humidity;
public:
    bool init();
    void getData();
    float getHumidity();
    float getTemperature();
    SensorSHT85(/* args */);
    ~SensorSHT85();
};

#endif