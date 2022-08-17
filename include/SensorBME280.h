
#ifndef _SENSOR_BME280_H_
#define _SENSOR_BME280_H_
#include "Config.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME280_I2C_ADDRESS  0x76

class SensorBME280
{
private:
    Adafruit_BME280 bme280;
    float Temperature;
    float Humidity;
    float Pressure;
public:
    bool init();
    void getData();
    float getHumidity();
    float getTemperature();
    float getPressure();
    SensorBME280(/* args */);
    ~SensorBME280();
};

#endif