#include "WiFiConnectionManager.h"
#include "HTTPUpdate.h"
#include "ESP32Ping.h"
#include "WiFiManager.h"
#include "esp_task_wdt.h"

const char* serverUrl = "http://airsense.vn:3030/firmware_v3.bin";
static unsigned long last_check_wifi_status = 0;
static unsigned long last_reconnect_wifi   = 0;
static unsigned long time_to_reconnect_wifi = TIME_TO_RECONNECT_WIFI_DEFAULT;

#define _DB_LOG_
WiFiConnectionManager::WiFiConnectionManager()
{

}
WiFiConnectionManager::~WiFiConnectionManager()
{

}
void WiFiConnectionManager::init()
{
    #ifdef _DB_LOG_
        Serial.println("WiFi init station mode");       
        // Serial.println(WiFi.BSSIDstr());
        // Serial.println(WiFi.psk());
    #endif
    WiFi.disconnect();
    WiFi.mode(WIFI_MODE_STA);
    this->state.current_state = WIFI_STATE_INIT;
    this->state.status = WIFI_STATUS_NO_WIFI_CONNECTED;
    this->state.entry = 0;
    this->switchState(WIFI_STATE_CONNECT_OLD_CONFIG);
}
void WiFiConnectionManager::switchState(uint8_t _next_state)
{
    this->state.previous_state = this->state.current_state;
    this->state.current_state = _next_state;
}
void WiFiConnectionManager::setMode(wifi_mode_t _mode)
{
    WiFi.mode(_mode);
}
void WiFiConnectionManager::connect(uint32_t connectTimeout)
{
    WiFi.begin();
    uint8_t status = WiFi.status();
    auto startTime = millis();
    while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeout) {
        delay(10);
        status = WiFi.status();
    }

}
void WiFiConnectionManager::reconnect()
{ 
    WiFi.reconnect();
}
bool WiFiConnectionManager::isConnected()
{
    if(WiFi.isConnected())
    {
    #ifdef _DB_LOG_
        // Serial.println("WiFi is connected to:");       
        // Serial.println(WiFi.SSID());
        // Serial.println(WiFi.psk());
    #endif    
        return true;
    }
    else
    {
    #ifdef _DB_LOG_
        // Serial.println("No WiFi connection!");
    #endif
        return false;
    }
}
bool WiFiConnectionManager::isIntenetConnected()
{
    return Ping.ping("google.com", 2);
}

void WiFiConnectionManager::smartConfig()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    unsigned long start_time = millis();
    while(millis() < (start_time + 20000))
    {
        if(WiFi.smartConfigDone())
        {
            #ifdef _DB_LOG_
                Serial.println("Smart config successed");
            #endif
            uint8_t i = 0;
            while(WiFi.status() != WL_CONNECTED && i != 50)
            {
                i++;
                delay(100);
            }
            if(WiFi.status() == WL_CONNECTED)
            {
                this->state.status = WIFI_STATUS_WIFI_CONNECTED_BUT_NO_INTERNET;
            }
            else
            {
                this->state.status = WIFI_STATUS_NO_WIFI_CONNECTED;
            }
            
            // wifi_mul.addAP(&WiFi.SSID()[0], &WiFi.psk()[0]);
            //logWiFi info to SD
            // DeviceManager *device_manager;
            // device_manager = DeviceManager::getInstance();
            // if(device_manager->g_store.isExist())
            // {
            //     device_manager->g_store.logWiFiInfo(WiFi.SSID(), WiFi.psk(), WIFI_INFO_FILE);
            // }
            return;
        }
        delay(500);
        esp_task_wdt_reset();
    }
    #ifdef _DB_LOG_
        Serial.println("Smart config failed");
    #endif
}

bool WiFiConnectionManager::isSmartConfigDone()
{
    return WiFi.smartConfigDone();
}

void WiFiConnectionManager::getWiFiListFromStore()
{
    // DeviceManager *device_manager;
    // device_manager = DeviceManager::getInstance();
    // #ifdef _DB_LOG_
    //     Serial.println("WiFi from SD card:");
    // #endif
    // device_manager->g_store.readWiFiInfo(&this->wifi_mul);
}
void WiFiConnectionManager::startAP()
{
   //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //reset settings - for testing
    // wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    wifiManager.setConfigPortalTimeout(120);
    //set menu
    std::vector<const char *> menu = {"wifi","wifinoscan","restart","exit"};
    wifiManager.setMenu(menu);
    // set dark theme
    wifiManager.setClass("invert");
    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    //WiFi.mode(WIFI_STA);
    
    if (!wifiManager.startConfigPortal("AirSENSE V3", "sparclab")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
    //   ESP.reset();
    //   delay(5000);
    }
    else
    {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
    }
    
}
uint32_t WiFiConnectionManager::updateUnixTime()
{
    if(this->isConnected())
    {
        configTime(GMT_OFFSET_SECOND, DAY_LIGHR_OFFSET_SECOND, NTP_SERVER);
        struct tm timeinfo;
        if(!getLocalTime(&timeinfo))
        {
            #ifdef _DB_LOG_
                Serial.println("Failed to obtain time");
            #endif
        }
        else
        {
            time_t unixtime;
            unixtime = mktime(&timeinfo);
            // if(unixtime > 1605666264)
            {
                #ifdef _DB_LOG_
                    Serial.print("Get time success, Unixtime: ");
                    Serial.println(unixtime);
                #endif
                return unixtime + GMT_OFFSET_SECOND;
            }   
        }
    }
    else
    {
        #ifdef _DB_LOG_
            Serial.println("No Internet so can not get time online");
        #endif
    }
    return 0;
}
void WiFiConnectionManager::connectBestWiFi(uint32_t connectTimeout) 
{
    this->wifi_mul.run(connectTimeout);
}
WiFiMulti *WiFiConnectionManager::getWiFiMul()
{
    return &wifi_mul;
}
WiFiConnectionState WiFiConnectionManager::getState()
{
    return this->state;
}
void WiFiConnectionManager::updateOTA()
{
    WiFiClient wificlient;
    t_httpUpdate_return ret = httpUpdate.update(wificlient, serverUrl, FIRMWARE_VERSION);
    switch (ret) 
    {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(),
                            httpUpdate.getLastErrorString().c_str());
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            ESP.restart();
            break;
        default: 
            break;
    } 
}
void WiFiConnectionManager::waitAndCheckEvent()
{
    /**
     * if Internet is connected before, we will lengthen the time period
     * every check to minimize resource consumed.
     */
    uint32_t time_to_check = 60000;
    if(this->state.status != WIFI_STATUS_INTERNET_CONNECTED) 
    // || this->state.previous_state == WIFI_STATE_SMARTCONFIG)
    {
        time_to_check = 10000;
    }

    //wait to check Internet and WiFi
    if(millis() > last_check_wifi_status + time_to_check)
    {
        #ifdef _DB_LOG_
            Serial.print("WiFi status: ");       
        #endif   
        if(this->isIntenetConnected())
        {
            #ifdef _DB_LOG_
                Serial.println("Internet connected");       
            #endif               
            this->state.status = WIFI_STATUS_INTERNET_CONNECTED;
            this->state.entry = 0;
        }
        else if(WiFi.status() == WL_CONNECTED)
        {
            #ifdef _DB_LOG_
                Serial.println("WiFi but no Internet");       
            #endif   
            this->state.status = WIFI_STATUS_WIFI_CONNECTED_BUT_NO_INTERNET;
            this->switchState(WIFI_STATE_RECONNECT);
        }
        else
        {
            #ifdef _DB_LOG_
                Serial.println("WiFi disconnected");       
            #endif   
            this->state.status = WIFI_STATUS_NO_WIFI_CONNECTED;
            this->switchState(WIFI_STATE_RECONNECT);
        }
        //handle event after check status
        last_check_wifi_status = millis();
        if(this->state.status == WIFI_STATUS_INTERNET_CONNECTED)
        {
            if(this->state.previous_state == WIFI_STATE_CONNECT_OLD_CONFIG)
            {
                this->switchState(WIFI_STATE_STORE_WIFI_INFO);
            }
            else if(this->state.previous_state == WIFI_STATE_SMARTCONFIG)
            {
                this->switchState(WIFI_STATE_STORE_WIFI_INFO);
            }
            else if(this->state.previous_state == WIFI_STATE_CONNECT_BEST_WIFI)
            {
                // this->switchState(WIFI_SATE)
            }
        }
        else if(this->state.entry == 5)
        {
            //this WiFi is no longer usable so we remove it and try another
            this->removeCurrentWiFi();
            if(!wifi_mul.remainWiFi())
            {
                // if there is no WiFi info, so get info from SD card and check again
                this->switchState(WIFI_STATE_GET_WIFI_INFO_FROM_SD);
            }
            else
            {
                this->state.entry = 0;
                this->switchState(WIFI_STATE_CONNECT_BEST_WIFI);
            } 
        }
        else if(this->state.previous_state == WIFI_STATE_RECONNECT)
        {
            this->switchState(WIFI_STATE_RECONNECT);
        }
        else
        {
            /* code */
        }
    }    
}
void WiFiConnectionManager::reconnectSequence()
{
    if(millis() > (last_reconnect_wifi + time_to_reconnect_wifi))
    {
        
        this->reconnect();
        this->state.entry ++;
        if(time_to_reconnect_wifi < TIME_TO_RECONNECT_WIFI_MAX)
        {
          time_to_reconnect_wifi *= 2;
        }
        else 
        {
          time_to_reconnect_wifi = TIME_TO_RECONNECT_WIFI_MAX;
        }  
        last_reconnect_wifi = millis();
        #ifdef _DB_LOG_
            Serial.printf("WiFi entry = %d. ", this->state.entry);
            Serial.println("WiFi reconnecting ...");     
            
        #endif   
    }
    else
    {

    }    
    this->switchState(WIFI_STATE_CHECK_AND_WAIT);   
}
void WiFiConnectionManager::removeCurrentWiFi()
{
    #ifdef _DB_LOG_
        Serial.println("WiFi remove current WiFi info");       
    #endif 
    wifi_mul.removeWiFiNoInternet(WiFi.SSID());
}