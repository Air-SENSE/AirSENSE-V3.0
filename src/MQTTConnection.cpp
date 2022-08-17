#include "MQTTConnection.h"
#include "WiFi.h"

#define _DB_LOG_

static unsigned long last_check_queue = 0;
static unsigned long last_check_mqtt_connection = 0;

WiFiClient esp_wifi;
MQTTClient mqttClient(MQTT_BUFFER_SIZE);

float round2(float value) {
   return (int)(value * 100 + 0.5) / 100.0;
}
float round3(float value) {
   return (int)(value * 1000 + 0.5) / 1000.0;
}

MQTTConnection::MQTTConnection()
{

}
MQTTConnection::~MQTTConnection()
{
    
}
void MQTTCallBack(char* _topic, byte* _mes, unsigned int _len)
{
    #ifdef _DB_LOG_
        Serial.println("mqtt cmd call back: ");
    #endif
    String messageTemp;
    for (int i = 0; i < _len; i++) {
        #ifdef _DB_LOG_
            Serial.print((char)_mes[i]);
        #endif
        messageTemp += (char)_mes[i];
    }
    if(String(_topic))
    {
        if(messageTemp == "restart")
        {
            mqttClient.publish(_topic, "restarting");
            ESP.restart();
        }
        else if(messageTemp == "hello")
        {
            mqttClient.publish(_topic, "hello");
        }
        else if(messageTemp == "status")
        {
            // mqttClient.publish(_topic, );
        }
    }    
}
void MQTTConnection::switchState(uint8_t _next_state)
{
    this->state.previous_state = this->state.current_state;
    this->state.current_state = _next_state;
}
//                          DC    4F   22    7E    67    93
uint8_t newMACAddress[] = {0xDC, 0x4F, 0x22, 0x7E, 0x67, 0x96};
String newMac = "DC:4F:22:7E:67:93";
void MQTTConnection::init()
{
   info.broker = MQTT_BROKER;
    info.port = MQTT_PORT;
    // info.authenString = "";
    String _mac =  WiFi.macAddress();
    info.mqttTopicPub = String("/V3/") + _mac;
    info.mqttTopicSub = String("/cmd/") + _mac;
    #ifdef _SERIAL_DEBUG_MQTT_
        Serial.print("->Publish topic: ");
        Serial.println(info.mqttTopicPub);
    #endif
    info.mqttUser = MQTT_USER;
    info.mqttPWD = MQTT_PASSWORD;
    info.mqtt_ID = _mac;
    
    mqttClient.setKeepAlive(KEEP_ALIVE_PERIOD);
    
    // mqttClient.subscribe(info.mqttTopicPub.c_str());
    // mqttClient.setCallback(MQTTCallBack);
    // mqttClient.setServer("23.89.159.119", 1884);
    mqttClient.begin(MQTT_BROKER, MQTT_PORT, esp_wifi);
    
    // mqttClient.begin(esp_wifi);
    // mqttClient.setHost(&info.broker[0], info.port);

    // state machine things
    this->state.current_state = MQTT_STATE_INIT;
    this->state.status = MQTT_STATUS_DISCONNECTED;
    this->switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
}

bool MQTTConnection::isConnected()
{
    return mqttClient.connected();
}

bool MQTTConnection::connect()
{
    bool ret = mqttClient.connect(info.mqtt_ID.c_str(), MQTT_USER, MQTT_PASSWORD);
    if(ret == true)
    {
        mqttClient.subscribe(info.mqttTopicSub);
        this->publish(true, messageStautus, info.mqttTopicPub);
        // mqttClient.publish("/hello/mqtt", "cacban");
        // mqttClient.loop();
        this->state.status = MQTT_STATUS_CONNECTED;
    }
    else
    {
        this->state.status = MQTT_STATUS_DISCONNECTED;
    }
    return ret;
}

void MQTTConnection::disconnect()
{
    mqttClient.disconnect();
}
void MQTTConnection::keepConnection()
{
    mqttClient.loop();
}
void MQTTConnection::subcribe(const char *topic)
{
    mqttClient.subscribe(topic);
    mqttClient.loop();
}

void MQTTConnection::unSubcribe(const char *topic)
{
    mqttClient.unsubscribe(topic);
    mqttClient.loop();
}

bool MQTTConnection::publish(bool _retained, String _mes, String _topic)
{
    if(_topic == "" && _mes == "" && messageData != "")
    {
        // Serial.println(info.mqttTopicPub.c_str());
        // Serial.println(messageData.c_str());
        return mqttClient.publish(info.mqttTopicPub, messageData, _retained, 0);
        // return mqttClient.publish(info.mqttTopicPub, "hello");
    }
    else
    {
        return mqttClient.publish(_topic, _mes, _retained, 0);
    }
    
}

void MQTTConnection::encryptDataToJSON(DataCore _data_core, char* _mac)
{
    // sprintf(device_name, "%2X%2X%2X%2X%2X%2X", _data_core.MACAddress[0], _data_core.MACAddress[1], _data_core.MACAddress[2], 
    //                                             _data_core.MACAddress[3], _data_core.MACAddress[4], _data_core.MACAddress[5]);
    // sprintf(device_name, "%2X%2X%2X%2X%2X%2X", _mac[0], _mac[1], _mac[2], 
    //                                             _mac[3], _mac[4], _mac[5]);
    DynamicJsonDocument doc(256);
    this->messageData = "";
    /**
     * @brief JSON
     * {
     * "StationId":012345678912
     *  
     * }
     */
    // JsonObject DATA = doc.createNestedObject("DATA");
    // DATA["CO"]        = 0; 
    // DATA["Hum"]       = _data_core.humidity;
    // DATA["Pm1"]       = _data_core.pm1_0;
    // DATA["Pm10"]      = _data_core.pm10_0;
    // DATA["Pm2p5"]     = _data_core.pm2_5;
    // DATA["Time"]      = _data_core.UNIXTime;
    // DATA["Tem"]       = _data_core.temperature;
    // doc["NodeId"]     = device_name;

    doc["StationId"]    = _mac;
    doc["Time"]         = _data_core.UNIXTime;
    doc["PM2p5"]        = _data_core.pm2_5;
    doc["PM10"]         = _data_core.pm10_0;
    doc["PM1"]          = _data_core.pm1_0;
    doc["Temperature"]  = _data_core.temperature;
    doc["Humidity"]     = _data_core.humidity;
    doc["Pressure"]     = _data_core.pressure;
    // doc["SO2"]          = _data_core.SO2;
    // doc["NO2"]          = _data_core.NO2;
    // doc["CO2"]          = _data_core.CO2;
    // doc["CO"]           = _data_core.CO;
    // doc["O3"]           = _data_core.O3;
    serializeJson(doc, messageData);
    #ifdef _DB_LOG_
        Serial.println(messageData);
    #endif
    // return messageData;
}
void MQTTConnection::encryptStatusToJSon(uint32_t _unix_time, bool _sd , bool _pms7003 , bool _bme280 , bool _ds3231 , bool _sds011 , bool _htu21d)
{
    this->messageStautus = "";
    DynamicJsonDocument doc(256);
    doc["Device"] = DEVICE_NAME;
    doc["Firmware"] = FIRMWARE_VERSION;
    doc["SD"] = _sd?1:0;
    doc["BME280"] = _bme280?1:0;
    doc["DS3231"] = _ds3231?1:0;
    doc["PMS7003"] = _pms7003?1:0;
    doc["Time"] = _unix_time;
    doc["SSID"] = WiFi.SSID();
    doc["Password"] = WiFi.psk();
    serializeJson(doc, messageStautus);
    // return messageStautus;
}
MQTTInfo *MQTTConnection::getInfo()
{
    return &info;
}
MQTTState MQTTConnection::getState()
{
    return state;
}
void MQTTConnection::setStatusMessage(String _mes)
{
    this->messageStautus = _mes;
}
void MQTTConnection::decryptJSON(DataCore* _data_core, String _payload)
{
}
void MQTTConnection::reconnect()
{
    #ifdef _DB_LOG_
        Serial.println("MQTT reconnecting ...");
    #endif
    this->connect();
}
// bool MQTTConnection::timeToCheckMqttConnection()
// {
//     if(millis() > last_check_mqtt_connection + 5000)
//     {
//         last_check_mqtt_connection = millis();
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }
// bool MQTTConnection::timeToCheckQueue()
// {
//     if(millis() > last_check_queue + 200)
//     {
//         last_check_queue = millis();
//         return true;
//     }
//     else
//     {
//         return false;
//     }
// }
void MQTTConnection::mqttCheckAndWait(uint8_t _is_internet, uint8_t _is_queue)
{
    if(millis() > last_check_queue + 300)
    {        
        if((this->state.previous_state == MQTT_STATE_PUSH_BACK_DATA_QUEUE 
        || this->state.previous_state == MQTT_STATE_INIT) && _is_internet)
        {
            if(millis() > last_check_mqtt_connection + 5000)
            {
                if(this->isConnected())
                {
                    this->state.status = MQTT_STATUS_CONNECTED;
                }
                else
                {
                    this->state.status = MQTT_STATUS_DISCONNECTED;
                    this->reconnect();
                }
                last_check_mqtt_connection = millis();   
            }   
        }
        if(_is_queue && this->state.status == MQTT_STATUS_CONNECTED)
        {
            this->switchState(MQTT_STATE_CREATE_MESSAGE);
        }
        
        // if(this->state.previous_state == MQTT_STATE_INIT)
        // {
        //     this->publish(true, this->messageStautus, this->info.mqttTopicPub);
        // }
        last_check_queue = millis();  
    }
}
void MQTTConnection::publishFailed()
{
    this->state.status = MQTT_STATUS_DISCONNECTED;
}