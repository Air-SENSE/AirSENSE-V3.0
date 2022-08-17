/**
 * @file DS3231_RTC.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#pragma once
#include "RTClib.h"
#include "Config.h"

#define DS3231_I2C_ADDRESS 0x68

class DS3231_RTC
{
private:
    /* data */
    RTC_DS3231 ds3231;
    // DateTime dt;
    TimeNow date_time;
    // DateTime date_time;
public:
    bool init();
    void updateTime();
    void setTime(uint32_t _unix_time);
    void setAlarm();
    void clearAlarm1();
    void clearAlarm2();
    bool isAlarm1Fired();
    bool isAlarm2Fired();
    TimeNow getTime();
    DS3231_RTC(/* args */);
    ~DS3231_RTC();
};
