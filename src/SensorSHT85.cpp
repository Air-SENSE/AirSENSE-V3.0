#include "SensorSHT85.h"

SensorSHT85::SensorSHT85(/* args */)
{
}

SensorSHT85::~SensorSHT85()
{
}

bool SensorSHT85::init()
{
    return sht.init();
}

float SensorSHT85::getHumidity()
{
    return Humidity;
}
float SensorSHT85::getTemperature()
{
    return Temperature;
}
void SensorSHT85::getData(){
    this->Humidity      = this->sht.getHumidity();
    this->Temperature   = this->sht.getTemperature();
}