/**
 * @file main.c
 * @author Le Duy Nhat
 * @brief This main stuff made off 
 * @date 23/10/2020
 */
#include <Arduino.h>
#include "DeviceManager.h"
#ifdef MQTT_PUSH
#include "MQTTConnection.h"
#endif
#ifdef THINGSPEAK_PUSH
#include "ThingspeakConnection.h"
#endif
#include "WiFiConnectionManager.h"
#include "UserInterfaceManager.h"
#include "Queue.h"

//////////////////////////////////////////////////////////DEFINE////////////////////////////////////////////////////////////////
#define _DB_LOG_

//////////////////////////////////////////////////////GLOBAL VARABLE/////////////////////////////////////////////////////////////
#ifdef MQTT_PUSH
MQTTConnection mqtt = MQTTConnection();
#endif
DeviceManager *device_manager =  DeviceManager::getInstance();
Queue *queue = Queue::getInstance();
#ifdef THINGSPEAK_PUSH
THINGSPEAKConnection speak = THINGSPEAKConnection();
#endif
WiFiConnectionManager wifi_manager;
UserInterfaceManager user_interface;

//Data sum and count data collected
DataCore data_core_sum;
DataCore data_to_publish;
/**
 * If there is no Internet connection when first start device, so time to reconnect will be double 
 * every time WiFi can not be connected and stop at maximun 32 minutes. This value will be reset 
 * to defaut evey time the devices is connected. This will save enegy in case there is no WiFi around 
 * in addition to performing display.
*/
// unsigned long time_to_reconnect_wifi = TIME_TO_RECONNECT_WIFI_DEFAULT;

//static tick
static unsigned long last_check_update_ota = 0;
static unsigned long last_update_screen = 0;

/**
 * function
 */

////////////////////////////FSM//////////////////////////////////////////////////////////////////////////////////
/**********************************************
 * @brief Device State machine
 **********************************************/
void DeviceUpdateDataCore()
{
    device_manager->updateDataCore();
    device_manager->printData();
    #ifdef _DB_LOG_
      Serial.println("Devices [2] update DataCore");
    #endif

    //switch state
    if(device_manager->getState().entry == 3)
    {
      device_manager->switchState(DEVICE_STATE_LOG_DATA); //log data to SD card
    }
    else
    {
      device_manager->switchState(DEVICE_STATE_SLEEP); // go to sleep
    }
}
void DeviceSleep()
{
  device_manager->sleep();
  #ifdef _DB_LOG_
    Serial.println("Devices [0] sleep");
  #endif
  device_manager->switchState(DEVICE_STATE_WAIT);
}
void DeviceWakeUp()
{
  device_manager->wake();
  #ifdef _DB_LOG_
    Serial.println("Devices [1] wake");
  #endif
  device_manager->switchState(DEVICE_STATE_WAIT);
}
void DeviceWaitPeriod()
{   
  device_manager->waitPeriod(SAMPLING_TIME);//config->getTimeStamp());
}
void DeviceLogData()
{
  #ifdef _DB_LOG_
    Serial.println("Devices [3] log data");
  #endif
  device_manager->logDataCoreToSDCard();
  queue->enQueue(device_manager->getDataCore());
  device_manager->switchState(DEVICE_STATE_SLEEP);
}
///function pointer////////////
void (*device_function_ptr[])(void) = {&DeviceSleep, &DeviceWakeUp, &DeviceUpdateDataCore, &DeviceLogData, &DeviceWaitPeriod};
void checkDeviceState()
{
    device_function_ptr[device_manager->getState().currrent_state]();
}

/*******************************************
 * @brief WiFi state machine
 *******************************************/
void connectOldWiFi()
{
  #ifdef _DB_LOG_
    Serial.println("WiFi [0] Connect old WiFi");       
  #endif
  wifi_manager.connect();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void storeWiFiInfoToSD()
{
  #ifdef _DB_LOG_
    Serial.println("WiFi [1] log info to WIFI.txt");
    Serial.println(WiFi.SSID());
    Serial.println(WiFi.psk());
  #endif
  // update time to RTC
  device_manager->setTimeToRTC(wifi_manager.updateUnixTime());
  device_manager->updateTimeFromRTC();

  // store WiFi info
  device_manager->g_store.logWiFiInfo(WiFi.SSID(), WiFi.psk(), WIFI_INFO_FILE);
  // wifi_manager.getWiFiMul()->addAP(&WiFi.SSID()[0], &WiFi.psk()[0]);
  wifi_manager.switchState(WIFI_STATE_GET_WIFI_INFO_FROM_SD);
}
void wifiCheckAndWait()
{
  wifi_manager.waitAndCheckEvent();
}
void wifiSmartConfig()
{
  /**
   * This function is now AutoConnect from WiFiManager
   */
  #ifdef _DB_LOG_
      // Serial.println("WiFi [4] smartconfig start...");
    Serial.println("WiFi [4] AP starting ...");
  #endif
  // wifi_manager.smartConfig();
  wifi_manager.startAP();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiReconnect()
{
  wifi_manager.reconnectSequence();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiConnectBestWifi()
{
  #ifdef _DB_LOG_
    Serial.println("WiFi [3] Connect best WiFi");       
  #endif   
  wifi_manager.connectBestWiFi();
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void wifiGetWifiInfoFromSd()
{
  #ifdef _DB_LOG_
    Serial.println("WiFi [2] get WiFi info from SD");       
  #endif   
  device_manager->g_store.readWiFiInfo(wifi_manager.getWiFiMul());
  wifi_manager.switchState(WIFI_STATE_CHECK_AND_WAIT);
}
void (*wifi_function_ptr[])(void) = {&connectOldWiFi, &storeWiFiInfoToSD, &wifiGetWifiInfoFromSd,
                                      &wifiConnectBestWifi, &wifiSmartConfig, &wifiReconnect, &wifiCheckAndWait};
void checkWiFiState()
{
  wifi_function_ptr[wifi_manager.getState().current_state]();
}


#ifdef MQTT_PUSH
/**********************************************
 * @brief mqtt state machine
 **********************************************/

void mqttCheckAndWait()
{
  mqtt.mqttCheckAndWait(wifi_manager.getState().status == WIFI_STATUS_INTERNET_CONNECTED, !queue->isEmpty());
}
void mqttCreateMessage()
{
  #ifdef _DB_LOG_
    Serial.println("MQTT [1] create message");       
  #endif   
  data_to_publish = queue->deQueue();
  // mqtt.encryptDataToJSON(data_to_publish, device_manager->getMAC());
  mqtt.encryptDataToJSON(data_to_publish, "DC4F227E6793");
  
  mqtt.encryptStatusToJSon( device_manager->getTime().unix_time, device_manager->getConnectionStatus().sd_card,
                                                              device_manager->getConnectionStatus().pms7003,
                                                              device_manager->getConnectionStatus().bme280,
                                                              device_manager->getConnectionStatus().ds3231);
  mqtt.switchState(MQTT_STATE_PUBLISH_MESSAGE);
}
void mqttPublishMessage()
{
  #ifdef _DB_LOG_
    Serial.print("MQTT [2] publish message");       
  #endif  
  if(mqtt.publish(true))
  {
    #ifdef _DB_LOG_
      Serial.println(" successful");       
    #endif  
    mqtt.switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
  }
  else
  {
    #ifdef _DB_LOG_
      Serial.println(" failed");       
    #endif  
    mqtt.publishFailed();
    mqtt.switchState(MQTT_STATE_PUSH_BACK_DATA_QUEUE);
  }
  
}
void mqttPushBackDataQueue()
{
  #ifdef _DB_LOG_
      Serial.println("MQTT [3] push back data core");       
  #endif  
  queue->enQueue(data_to_publish);
  mqtt.switchState(MQTT_STATE_CHECK_QUEUE_AND_WAIT);
}

void (*mqtt_function_ptr[])(void) = {&mqttCheckAndWait, &mqttCreateMessage, &mqttPublishMessage, &mqttPushBackDataQueue};

void checkmqttstate()
{
  mqtt_function_ptr[mqtt.getState().current_state]();
  // printf("hello");
}
#endif
/**
 * @brief error check
 */
/**
 * @brief ui state machine
 */
//////////////////////////////////////////////////////////////SET UP////////////////////////////////////////////////////////////////
void setup() {   
  //initialize hardware
  device_manager->initHardware();
  //set watchdog timer

  //Show up firmware version
  #ifdef _SERIAL_DEBUG_MAIN_
    Serial.print("Device name: ");
    Serial.println(DEVICE_NAME);
    Serial.print("Firmware version: ");            
    Serial.println(FIRMWARE_VERSION);            
  #endif

  //Start WiFi mode by Station
  wifi_manager.init();
  wifi_manager.connect();
  
  //Check connected device_manager
  device_manager->checkConnectedDevices();

  device_manager->g_store.readWiFiInfo(wifi_manager.getWiFiMul());

  delay(100);
  // initialize connected devices
  device_manager->initConnectedDevices();
  
  //initialize user interface
  user_interface.init();
  user_interface.display.progressScreen(device_manager->getConnectionStatus().sd_card,
                                        device_manager->getConnectionStatus().pms7003, 
                                        device_manager->getConnectionStatus().bme280,
                                        device_manager->getConnectionStatus().ds3231);
  delay(3000);

  //Show main screen including data that just collected and clock.
  user_interface.display.mainScreen();

  device_manager->setTimeToRTC(wifi_manager.updateUnixTime());
  device_manager->updateDataCore();

  user_interface.display.updateData(device_manager->getDataCore().temperature, device_manager->getDataCore().humidity, device_manager->getDataCore().pressure, device_manager->getDataCore().pm2_5, device_manager->getMAC());
  user_interface.display.updateClock(device_manager->getTime().hour, device_manager->getTime().minute);
  #ifdef MQTT_PUSH
  //mqtt thing
  mqtt.init();
  mqtt.encryptStatusToJSon(device_manager->getTime().unix_time, device_manager->getConnectionStatus().sd_card,
                                                                device_manager->getConnectionStatus().pms7003,
                                                                device_manager->getConnectionStatus().bme280,
                                                                device_manager->getConnectionStatus().ds3231);
  mqtt.connect();
  //if device can't reach mqtt station, show green non wifi symbol
  // device_manager->initWDT();
  // xTaskCreate(checkDeviceState, "Devices", 2048, NULL, 1, NULL);
  #endif
  #ifdef THINGSPEAK_PUSH
  speak.init();
  #endif
}
void loop() 
{
  /**
   * This is the state machine include 2 state wait and get data.
   * In state one, device collect data then let sensors sleep to save energy.
   * Then data will be display and increase data count by one.
   * If data count equal to three (three data has been collected) the device will log data to SD card 
   * and queue it for publish to mqtt server.
   * In state zero, device wake the sensors up for state one to get data.
   * Interval between two state is store in config class
   */
  // user_interface.display.guiHandler();
  checkDeviceState();
  checkWiFiState();
  #ifdef MQTT_PUSH
  checkmqttstate();
  #endif
  if(millis() > last_update_screen + TIME_INTERVAL_SYSTEM)
  {
    last_update_screen = millis();
    user_interface.display.updateData(device_manager->getDataCore().temperature, device_manager->getDataCore().humidity, device_manager->getDataCore().pressure, device_manager->getDataCore().pm2_5, device_manager->getMAC());
    user_interface.display.updateClock(device_manager->getTime().hour, device_manager->getTime().minute);
    if(device_manager->getConnectionStatus().sd_card)
    {
      user_interface.display.showSDsymbol();
    }
    else
    {
      user_interface.display.showNonSDsymbol();
    }
    if(wifi_manager.getState().status == WIFI_STATUS_INTERNET_CONNECTED)
    {
      user_interface.display.showWiFiSymbol();
      #ifdef THINGSPEAK_PUSH
      if(speak.publish(int(device_manager->getDataCore().temperature), int(device_manager->getDataCore().humidity), int(device_manager->getDataCore().pm2_5)))
      {        
        #ifdef _DB_LOG_
          Serial.print("THINGSPEAK [WRITE] Write to Filed Done");       
        #endif 
      }
      else
      {
        #ifdef _DB_LOG_
          Serial.print("THINGSPEAK [WRITE] Write Error");       
        #endif 
      }
      #endif
    }
    else if(wifi_manager.getState().status ==  WIFI_STATUS_WIFI_CONNECTED_BUT_NO_INTERNET)
    {
      user_interface.display.showWiFiButNoInternetSymbol();
    }
    else
    {
      user_interface.display.showNonWiFiSymbol();
    }
  }
  // user_interface.display.guiHandler();

  //check for update firmware
  if(device_manager->getTime().hour == 23 && (millis() > (last_check_update_ota + 300000)))
  // if((millis() > (last_check_update_ota + 120000)))
  {
    wifi_manager.updateOTA();
    last_check_update_ota = millis();
  }  
  /**
   * This makes the device restart every 30 days
   * and if there is error from data, we should do restart 
   * to reinitilize sensor thing
   */
  if((millis() > RESET_PERIOD) || (device_manager->dataError() && device_manager->getConnectionStatus().pms7003))
  {
    #ifdef _DB_LOG_
      Serial.println("Resetting device");
    #endif
    ESP.restart();
  }     
    /**
   * Smartconfig thing
   */
  user_interface.user_input.buttonScan();
  if(user_interface.user_input.getButtonStatus(1).currentState == BUTTON_HELD)
  {
    wifi_manager.switchState(WIFI_STATE_SMARTCONFIG);
    user_interface.display.showWiFiConfigSymbol();
  }
  /**
   * Watchdog timer things
   */
  // device_manager->resetWDT();
}
