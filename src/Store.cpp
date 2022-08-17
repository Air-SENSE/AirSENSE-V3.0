#include "Store.h"
// #include "SPIFFS.h"

#define _DB_LOG_
Store::Store(/* args */)
{
}

Store::~Store()
{
}

bool Store::init(){
    if(!SD.begin(PIN_CS_SD_CARD, SPI))
    // if(SD.cardType() == CARD_NONE)
    {
        // while (!SD.begin(PIN_CS_SD_CARD))
        // {
        //     /* code */
        // }
        
        return false;
    }
    #ifdef _DB_LOG_
        //  File file = SD.open("/datalog.txt", FILE_APPEND);
        // if(file)
        // {
        //     file.print();
        // }
        // else
        // {
        //     Serial.print("canot open file to write");
        // }
        // file.close();
        uint8_t cardType = SD.cardType();
        Serial.print("SD Card Type: ");
        if (cardType == CARD_MMC) {
            Serial.println("MMC");
        } else if (cardType == CARD_SD) {
            Serial.println("SDSC");
        } else if (cardType == CARD_SDHC) {
            Serial.println("SDHC");
        } else {
            Serial.println("UNKNOWN");
        }
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Card Size: %lluMB\n", cardSize);
    #endif
    
    if(isExist())
    {
        return true;
    }
    else
    {
        File file = SD.open(WIFI_INFO_FILE, FILE_WRITE);
        if(file)
        {
            // wifi_config_t current_conf;
            // esp_wifi_get_config(WIFI_IF_STA, &current_conf);
            // char _ssid[32];
            // char _pass_word[64];
            // memcpy(&_ssid[0], current_conf.sta.ssid, sizeof(current_conf.sta.ssid));
            // memcpy(&_pass_word[0], current_conf.sta.password, sizeof(current_conf.sta.password));

            // // String _ssid = (char*)current_conf.sta.ssid;
            // // String _pass = (char*)current_conf.sta.password;
            // #ifdef _DB_LOG_
            //     Serial.print(_ssid);
            //     Serial.print(_pass_word);
            // #endif
            // file.println(_ssid);
            // file.println(_pass_word);   
        }
        file.close();
    } 
    return true;
}

bool Store::isExist()
{
    return SD.exists(WIFI_INFO_FILE);
}
bool Store::isFileEmpty(const char* _file_name)
{
    File file = SD.open(_file_name, FILE_READ);
    if(file)
    {
        if(!file.size())
        {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}
bool Store::logData(const char* _file_name, DataCore _data_core, char* _mac, TimeNow _date_time)
{
    bool ret = true;
    bool add_label = false;
    // String fn = String(_file_name) + _retry?String("sub"):String("");
    // File file = SD.open(fn, FILE_APPEND);
    if(!SD.exists(_file_name)){
        add_label = true;
    }
    File file = SD.open(_file_name, FILE_APPEND);
    // File file = SD.open("/heo.txt", FILE_APPEND);
    if(file)
    {
        if(add_label)
        {
            file.println("Device Id, Date time, Unix Time, Temperature, Humidity, Pressure, PM1, PM2.5, PM10");
        }
        char log_data[130];
        sprintf(log_data, "%s, %4d-%02d-%02d %02d:%02d:%02d, %ld, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", 
                _mac, _date_time.year, _date_time.month, _date_time.date, _date_time.hour, _date_time.minute, _date_time.second,
                _data_core.UNIXTime,
                _data_core.temperature, _data_core.humidity, _data_core.pressure,
                _data_core.pm1_0, _data_core.pm2_5, _data_core.pm10_0);
        file.print(log_data);
        #ifdef _SERIAL_DEBUG_STORE_
            Serial.print("->Data to SD: ");
            Serial.print(log_data);
        #endif
        ret = true;
    }
    else
    {
        file.close();
        ESP_LOGE(SDTAG, "CANNOT open file %s", _file_name);
        #ifdef _SERIAL_DEBUG_STORE_
            Serial.println("->Try to reconnect SD");
        #endif
        delay(10);
        this->end();
        delay(10);
        this->init();
        ret = false;
    }
    file.close();
    return ret;
}
void Store::logTemperatureMessage(String _mes, const char* _file_name)
{
    File file = SD.open(_file_name, FILE_APPEND);
    if(file)
    {
        file.println(_mes);
    }
    file.close();
}
void Store::readData(const char* _file_name){
    // sizeof(DataCore);
}
String Store::readLine(const char* _file_name, uint16_t line)
{
    //read ssid and password in ISC.txt file
    File file = SD.open(_file_name, FILE_READ);
    String data;
    if(file.available())
    {
        file.seek(0);
        char cr;
        for(unsigned int i = 0; i < (line -1);)
        {
            cr = file.read();
            if(cr == '\n')
            {
                i++;
            }
        }
        data = file.readStringUntil('\n');
        data[data.length()-1] = '\0';
    }
    file.close();
    #ifdef _DB_LOG_
        // Serial.println("hoho");
        // Serial.print(data);
    #endif
    return data;
}
void Store::readWiFiInfo(WiFiMulti *_wifi_mul)
{
    if(this->isExist())
    {
        //read ssid and password in WIFI.txt file
        File file = SD.open(WIFI_INFO_FILE, FILE_READ);
        String _ssid = "";
        String _pwd = "";
        if(file)
        {
            file.seek(0);
            while(file.available())
            {
                _ssid = file.readStringUntil('\n');
                _ssid[_ssid.length() - 1] = '\0';
                _pwd = file.readStringUntil('\n');
                _pwd[_pwd.length() - 1] = '\0';
                #ifdef _DB_LOG_
                    Serial.println(_ssid);
                    Serial.println(_pwd);
                #endif
                if(!_wifi_mul->addAP(&_ssid[0], &_pwd[0]))
                {
                    break;
                }
            }
        }
        file.close();
        //delete duplicated WiFi info by delete WIFI_INFO_FILE and rewrite
        SD.remove(WIFI_INFO_FILE);
        
        file = SD.open(WIFI_INFO_FILE, FILE_WRITE);
        if(file)
        {
            file.print(_wifi_mul->APdataString());
            
        }
        file.close();
    }
}
void Store::logWiFiInfo(String _ssid, String _pass_wd ,const char *_file_name)
{
    if(this->isExist())
    {
        File file = SD.open(_file_name, FILE_APPEND);
        if(file)
        {
            Serial.println("\nhelo");
            file.println(_ssid);
            file.println(_pass_wd);
        }
        file.close();
    }
}
void Store::end()
{
    SD.end();
}