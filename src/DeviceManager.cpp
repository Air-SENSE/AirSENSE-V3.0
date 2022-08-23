#include "DeviceManager.h"
#include "esp_task_wdt.h"
#include "WiFi.h"
#include "Queue.h"

#define _DB_LOG_
#define WDT_TIMEOUT 35

static unsigned long last_switch_state = 0;
DeviceManager * DeviceManager::instance = nullptr;

// this is for checking error data when there is duplicated data or zero data
static uint8_t err_data_count = 0;
static DataCore pre_data_core;

DeviceManager::~DeviceManager()
{

}

void DeviceManager::switchState(uint8_t _next_state)
{
    this->state.previous_state = this->state.currrent_state;
    this->state.currrent_state = _next_state;
}
void DeviceManager::initHardware()
{
    Serial.begin(SERIAL_DEBUG_BAUDRATE);
    Serial1.begin(PMS7003_BAUD_RATE, SERIAL_8N1, PIN_RX_PMS7003, PIN_TX_PMS7003);

    pinMode(PIN_NUM_MOSI, INPUT_PULLUP);
    SPI.begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD);
    Wire.begin(PIN_SDA_GPIO, PIN_SCL_GPIO, I2C_CLOCK_SPEED);

    WiFi.macAddress(this->MACAddress); 
    sprintf(this->mac_address, "%02X%02X%02X%02X%02X%02X", MACAddress[0], MACAddress[1], MACAddress[2],
                                                        MACAddress[3], MACAddress[4], MACAddress[5]);
    // // this->checkConnectedDevices();
    Serial.println(mac_address);
    // delay(1000);
    Serial.println("InitHardware successfully!!!");
}

void DeviceManager::checkConnectedDevices()
{
    if(this->checkI2CDevices(BME280_I2C_ADDRESS))
    {
        this->connected_devices_status.bme280 = DEVICE_CONNECTED;
        #ifdef _DB_LOG_
            Serial.println("BME280 is connected");
        #endif
    }
    else 
    {
        this->connected_devices_status.bme280 = DEVICE_DISCONNECTED;
        #ifdef _DB_LOG_
            log_e("BME280 no found!");
        #endif
    }
        
    if(this->checkI2CDevices(DS3231_ADDRESS))
    {
        this->connected_devices_status.ds3231 = DEVICE_CONNECTED;
        #ifdef _DB_LOG_
            Serial.println("DS3231 is connected!");
        #endif
    }
    else 
    {
        this->connected_devices_status.ds3231 = DEVICE_DISCONNECTED;
        #ifdef _DB_LOG_
            ESP_LOGE(DEVICETAG, "DS3231 no found!");
        #endif
    }

    if(g_store.init())
    {
        this->connected_devices_status.sd_card = DEVICE_CONNECTED;
        #ifdef _DB_LOG_
            Serial.println( "SD card is connected");
        #endif
        g_store.end();
    }    
    else
    {
        this->connected_devices_status.sd_card = DEVICE_DISCONNECTED;
        #ifdef _DB_LOG_
            ESP_LOGE(DEVICETAG, "SD card no found!");
        #endif
    }
        

    if(g_pms7003.isConnected())
    {
        this->connected_devices_status.pms7003 = DEVICE_CONNECTED;
        #ifdef _DB_LOG_
            Serial.println("PMS7003 is connected");
        #endif
    }
    else
    {
        this->connected_devices_status.pms7003 = DEVICE_DISCONNECTED;
        #ifdef _DB_LOG_
            ESP_LOGE(DEVICETAG, "PMS7003 no found!");
        #endif
    }
   if(g_sht85.init())
    {
        this->connected_devices_status.sht85 = DEVICE_CONNECTED;
        #ifdef _DB_LOG_
            Serial.println("SHT85 is connected");
        #endif
    }
    else
    {
        this->connected_devices_status.sht85 = DEVICE_DISCONNECTED;
        #ifdef _DB_LOG_
            ESP_LOGE(DEVICETAG, "SHT85 no found!");
        #endif
    }
}

bool DeviceManager::checkI2CDevices(uint8_t _i2c_address)
{
    uint8_t error;
    Wire.beginTransmission(_i2c_address);
    error = Wire.endTransmission();
    #ifdef _DB_LOG_
        // Serial.println(error);
    #endif
    if(error == 0)
        return true;
    return false;
}
void DeviceManager::initConnectedDevices()
{
    if(this->connected_devices_status.bme280)    
        if(g_bme280.init())
        {
            #ifdef _DB_LOG_
                Serial.println("BME280 initialized completely!");
            #endif
        }
    if(this->connected_devices_status.ds3231)
        if(g_ds3231.init())
        {
            #ifdef _DB_LOG_
                Serial.println("DS3231 initialized completely!");
            #endif
        }
    if(this->connected_devices_status.pms7003)
    {
        g_pms7003.init();
        Serial.println("PMS7003 initialized completely!");
    }
        
    if(this->connected_devices_status.sht85)
    {
        g_sht85.init();
        Serial.println("SHT85 initialized completely!");
    }
        
    if(this->connected_devices_status.sd_card)
        if(g_store.init())
        {
            #ifdef _DB_LOG_
                Serial.println("SD initialized completely");
            #endif
        }
    #ifdef _DB_LOG_
        Serial.println( "Init devices successfully!!");
    #endif
    // reset state to get data mode
    state.status = DEVICE_STATUS_WORKING;
    state.entry = 0;
    state.currrent_state = DEVICE_STATE_INIT;
    this->switchState(DEVICE_STATE_GET_DATA);
}
void DeviceManager::updateDataCore()
{
    // turn current value to the previous one
    pre_data_core =  this->data_core;

    // now update noew data
    updateTimeFromRTC();
    this->data_core.UNIXTime = this->time_now.unix_time - 3600*7;
    if(this->connected_devices_status.bme280)
    {
        this->g_bme280.getData();
        this->data_core.pressure = g_bme280.getPressure();
    }
    if(this->connected_devices_status.sht85)
    {
        this->g_sht85.getData();
        this->data_core.humidity = g_sht85.getHumidity();
        this->data_core.temperature = g_sht85.getTemperature();
    }

    if(this->connected_devices_status.pms7003)
    {
        this->g_pms7003.readData();
        if(g_pms7003.getPM10() != 0xFFFF)
        {
            this->data_core.pm1_0 = (float)g_pms7003.getPM1();
            this->data_core.pm2_5 = (float)g_pms7003.getPM2p5();
            this->data_core.pm10_0 = (float)g_pms7003.getPM10(); 
        }
    }    
    this->state.entry ++;

    // this is for error data check
    if((pre_data_core.pm10_0 == this->data_core.pm10_0  && 
        pre_data_core.pm2_5 == this->data_core.pm2_5))
        err_data_count ++;
    else
    {
        err_data_count = 0;
    }

}
bool DeviceManager::isSDcardConnected()
{
    if(g_store.isExist()) 
    {
        this->connected_devices_status.sd_card = DEVICE_CONNECTED;
        return true;
    }
    else
    {
        this->connected_devices_status.sd_card = DEVICE_DISCONNECTED;
        return false;
    }
}
void DeviceManager::updateTimeFromRTC()
{
    if(this->connected_devices_status.ds3231)
    {
        this->g_ds3231.updateTime();
        this->time_now = this->g_ds3231.getTime();
        // this->data_core.UNIXTime = this->time_now.unix_time - 3600*7;
    }
    else
    {
        this->time_now.hour = 0;
        this->time_now.minute = 0;
    }
    
}
void DeviceManager::setTimeToRTC(uint32_t _unix_time)
{
    if(this->connected_devices_status.ds3231 && _unix_time > 1605666264)
    {
        this->g_ds3231.setTime(_unix_time);
    }
}
void DeviceManager::setAlarm()
{
    this->g_ds3231.setAlarm();
}
void DeviceManager::sleep()
{
    if(this->connected_devices_status.pms7003)
    {
        this->g_pms7003.sleep();
    }
    //other sensors
    if(this->connected_devices_status.bme280)
    {

    }
    this->state.status = DEVICE_STATUS_SLEEPING;   
}
void DeviceManager::wake()
{
    if(this->connected_devices_status.pms7003)
    {
        this->g_pms7003.wakeUp();
    }
    this->state.status = DEVICE_STATUS_WORKING;
    //other sensors

}
bool DeviceManager::checkAlarm1()
{
    return this->g_ds3231.isAlarm1Fired();
}
bool DeviceManager::checkAlarm2()
{
    return this->g_ds3231.isAlarm2Fired();
}
void DeviceManager::clearAlarm1()
{
    this->g_ds3231.clearAlarm1();
}
void DeviceManager::clearAlarm2()
{
    this->g_ds3231.clearAlarm2();
}
void DeviceManager::logDataCoreToSDCard()
{
    if(this->connected_devices_status.sd_card)
    {
        char file_name[36];
        sprintf(file_name, "/%s %4d-%02d-%02d.txt", this->mac_address,
                this->time_now.year, this->time_now.month, this->time_now.date);
        // sprintf(file_name, "/%02X%02X%02X%02X%02X%02X %4d-%02d-%02d.txt", this->MACAddress[0], this->MACAddress[1],this->MACAddress[2],
        //         this->MACAddress[3], this->MACAddress[4], this->MACAddress[5],
        //         this->time_now.year, this->time_now.month, this->time_now.date);
        g_store.logData(file_name, this->data_core, this->mac_address, this->time_now);
        // g_store.logData("/hello.txt", data_core);        
    }
    this->state.entry = 0;
}
void DeviceManager::setPeriodTimeSample()
{

}
void DeviceManager::calibrate()
{

}
void DeviceManager::printData()
{
    // Serial.print("\nCO: ");
    // Serial.print(data_core.CO);
    Serial.print("\nPM1: ");
    Serial.print(data_core.pm1_0);
    Serial.print("\nPM2.5: ");
    Serial.print(data_core.pm2_5);
    Serial.print("\nPM10: ");
    Serial.print(data_core.pm10_0);
    Serial.print("\nTemperature: ");
    Serial.print(data_core.temperature);
    Serial.print("\nHumidity: ");
    Serial.print(data_core.humidity);
    Serial.print("\nPressure: ");
    Serial.print(data_core.pressure);
    Serial.print("\nUNIX time: ");
    Serial.println(data_core.UNIXTime);
    Serial.println(time_now.hour);
    Serial.println(time_now.date);
    Serial.println(time_now.month);
}
void DeviceManager::waitPeriod(uint16_t _time_stamp)
{
    if(millis() > last_switch_state + _time_stamp)
    {
        if(this->state.previous_state == DEVICE_STATE_WAKE)
        {
            switchState(DEVICE_STATE_GET_DATA);
        }
        else if(this->state.previous_state == DEVICE_STATE_SLEEP)
        {
            switchState(DEVICE_STATE_WAKE);
        }
        last_switch_state = millis();
    }
}
DataCore DeviceManager::getDataCore()
{
    return this->data_core;
}
TimeNow DeviceManager::getTime()
{
    return this->time_now;
}
DeviceManagerState DeviceManager::getState()
{
    return this->state;
}
ConnectedDevicesStatus DeviceManager::getConnectionStatus()
{
    return this->connected_devices_status;
}
char* DeviceManager::getMAC()
{
    return this->mac_address;
}
void DeviceManager::initWDT()
{
    // Set watchdog timer 
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
}
void DeviceManager::resetWDT()
{
    // Reset watchdog timer
    esp_task_wdt_reset();
}

bool DeviceManager::dataError()
{
    // if this count exceeds ten, this device should be restart
    if(err_data_count == 10)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}