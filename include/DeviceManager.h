/**
 * @file DeveceManager.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief This file includ
 */
#pragma once

#include "SensorBME280.h"
#include "SensorPMS7003.h"
#include "DS3231_RTC.h"
#include "UserInput.h"
#include "LedSignal.h"
#include "Store.h"

enum{DEVICE_DISCONNECTED, DEVICE_CONNECTED};
enum{DEVICE_STATUS_WORKING, DEVICE_STATUS_SLEEPING};
enum{DEVICE_STATE_SLEEP, 
    DEVICE_STATE_WAKE, 
    DEVICE_STATE_GET_DATA, 
    DEVICE_STATE_LOG_DATA, 
    DEVICE_STATE_WAIT,
    DEVICE_STATE_INIT
    };

// enum{SD_CARD, BME280, DS3231, PMS7003};
struct DeviceManagerState
{
    uint8_t currrent_state;
    uint8_t previous_state;
    uint8_t entry;
    uint8_t status;
};
struct ConnectedDevicesStatus
{
    uint8_t pms7003;
    uint8_t bme280;
    uint8_t ds3231;
    // uint8_t lcd1602;
    uint8_t ili9341;
    uint8_t sds011;
    uint8_t sd_card;
    // uint8_t sht85;
};
class DeviceManager
{
private:
    DeviceManager()
    {
        //init default data;
        this->data_core.UNIXTime = 0;  
        this->data_core.pm2_5 = 0;
        this->data_core.pm10_0 = 0;
        this->data_core.pm1_0 = 0;
        this->data_core.temperature = 0;
        this->data_core.humidity = 0;
        this->data_core.pressure = 0;
        // this->data_core.SO2 = 0;
        // this->data_core.NO2 = 0;
        // this->data_core.CO2 = 0;
        // this->data_core.CO = 0;
        // this->data_core.O3 = 0;
        //init defalt connected devices
        this->connected_devices_status = {0, 0, 0, 0, 0, 0};
    }
    static DeviceManager* instance;
    
    TimeNow time_now;
    DataCore data_core;
    uint8_t MACAddress[6];
    char mac_address[13];
    ConnectedDevicesStatus connected_devices_status;
    DeviceManagerState state;
public:
    SensorBME280 g_bme280;
    SensorPMS7003 g_pms7003;
    DS3231_RTC g_ds3231;
    Store g_store;

    // DeviceManager(/* args */);
    ~DeviceManager();
    void initHardware();
    void checkConnectedDevices();
    void initConnectedDevices();
    bool checkI2CDevices(uint8_t _i2c_address);
    void checkSPI();
    bool isSDcardConnected();
    void updateDataCore();
    void updateStatus();
    void updateTimeFromRTC();
    void setTimeToRTC(uint32_t _unix_time);
    void setAlarm();
    void sleep();
    void wake();
    bool checkAlarm1();
    bool checkAlarm2();
    void clearAlarm1();
    void clearAlarm2();
    bool dataError();
    void logDataCoreToSDCard();
    void setPeriodTimeSample();
    void calibrate();
    void printData();
    void initWDT();
    void resetWDT();
    void switchState(uint8_t _next_state);
    void waitPeriod(uint16_t _time_stamp);
    TimeNow getTime();
    DataCore getDataCore();
    DeviceManagerState getState();
    ConnectedDevicesStatus getConnectionStatus();
    char* getMAC();
    static DeviceManager* getInstance()
    {
        if(nullptr == instance)
        {
            instance = new DeviceManager();
        }
        return instance;
    }
};