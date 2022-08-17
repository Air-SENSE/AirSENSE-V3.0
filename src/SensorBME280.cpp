#include "SensorBME280.h"

SensorBME280::SensorBME280(/* args */)
{
}

SensorBME280::~SensorBME280()
{
}

bool SensorBME280::init()
{
    // return bme280.begin(BME280_I2C_ADDRESS, &Config::getInstance()->i2c_config);
    return bme280.begin(BME280_I2C_ADDRESS, &Wire);
}

float SensorBME280::getHumidity()
{
    return Humidity;
}
float SensorBME280::getTemperature()
{
    return Temperature;
}
float SensorBME280::getPressure()
{
    return Pressure;
}
void SensorBME280::getData(){
    this->Humidity      = this->bme280.readHumidity();
    this->Temperature   = this->bme280.readTemperature();
    this->Pressure      = this->bme280.readPressure();
}