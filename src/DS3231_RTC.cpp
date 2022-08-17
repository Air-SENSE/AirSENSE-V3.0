#include "DS3231_RTC.h"

DS3231_RTC::DS3231_RTC(/* args */)
{
}

DS3231_RTC::~DS3231_RTC()
{
}

void DS3231_RTC::updateTime()
{
    DateTime _date_time = this->ds3231.now();
    this->date_time.second = _date_time.second();
    this->date_time.minute = _date_time.minute();
    this->date_time.hour   = _date_time.hour();
    this->date_time.month   = _date_time.month();
    this->date_time.date    = _date_time.day();
    this->date_time.year    = _date_time.year();
    this->date_time.unix_time   = _date_time.unixtime();
}
void DS3231_RTC::setTime(uint32_t _uinx_time){
    this->ds3231.adjust(DateTime (_uinx_time));
}
bool DS3231_RTC::init(){
    if(this->ds3231.begin())
    {
        // setAlarm();
        this->ds3231.disable32K();
        return true;
    }
    return false;
}
void DS3231_RTC::setAlarm()
{
    DateTime dt;
    // dt = DateTime(2020, 10,10, 10, 10, 30);
    dt = DateTime(1602839550);
    this->ds3231.setAlarm2(dt, DS3231_A2_PerMinute);
    this->ds3231.setAlarm1(dt, DS3231_A1_Second   );
}
void DS3231_RTC::clearAlarm1() 
{
    this->ds3231.clearAlarm(1);
}

void DS3231_RTC::clearAlarm2() 
{
    this->ds3231.clearAlarm(2);
}

bool DS3231_RTC::isAlarm1Fired()
{
    return this->ds3231.alarmFired(1);
}

bool DS3231_RTC::isAlarm2Fired()
{
    return this->ds3231.alarmFired(2);
}

TimeNow DS3231_RTC::getTime()
{
    return this->date_time;
}