/**
 * @file DeveceManager.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief This file contains configuration parameters including hardware pin definition, 
 * calibaraions of sensors and server things. Those can be adjusted by either user interface 
 * or server mqtt.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Arduino.h>

#define TIME_INTERVAL_SYSTEM 1500
#define DISPLAY_SET_ROTATION 1

//#define MQTT_PUSH
//#define THINGSPEAK_PUSH

#define SERIAL_DEBUG_BAUDRATE 115200
#define PIN_TX_PMS7003  17
#define PIN_RX_PMS7003  16
// #define PORT_UART_PMS7003 Serial2

#define PIN_TX_SDS011 9
#define PIN_RX_SDS011 10
// #define PORT_UART_SDS011 Serial2

#define PIN_NUM_MISO 21
#define PIN_NUM_MOSI 19
#define PIN_NUM_CLK  18
// #define PIN_NUM_CS   17

#define PIN_CS_SD_CARD 5

#define PIN_TFT_RST     15
#define PIN_TFT_LED     14
#define PIN_TFT_DC      12
#define PIN_TFT_CS      13

#define PIN_SDA_GPIO 26
#define PIN_SCL_GPIO 27
#define I2C_CLOCK_SPEED 100000

#define PIN_LED_RED     25
#define PIN_LED_GREEN     33
#define PIN_LED_BLUE     32

#define PIN_NEO_PIXEL   23

#define PIN_BUTTON_1    35
#define PIN_BUTTON_2    34

#define PIN_OUTA        4
#define PIN_OUTB        2
#define PIN_SWITCH      15
// #define PMS7003_RX_DATA_RECIEVED_LENGTH 32
// #define PMS7003_TX_DATA_TRANSMIT_LENGTH 7

/**
 * @brief Devive parametter
 * 
 */
#define RESET_PERIOD (2592000000) //30 days
#define SAMPLING_TIME (30000) // 10 seconds
#define DEVICE_NAME "Alphasense V1"
#define FIRMWARE_VERSION "1.0.0"

/**
 * @brief Serial debug enable 
 * 
 */
// #define SERIAL_DEBUG_BAUDRATE (115200)
#define _SERIAL_DEBUG_ALL_
#define _SERIAL_DEBUG_DEVICE_MANAGER_
#define _SERIAL_DEBUG_ILI9341_DISPLAY_
#define _SERIAL_DEBUG_MQTT_
// #define _SERIAL_DEBUG_QUEUE_
#define _SERIAL_DEBUG_STORE_
#define _SERIAL_DEBUG_UI_MANAGER_
#define _SERIAL_DEBUG_WIFI_MANAGER_
#define _SERIAL_DEBUG_MAIN_
struct DataCore{
   // uint8_t MACAddress[6];
   unsigned long UNIXTime;
   float pm2_5;
   float pm10_0;
   float pm1_0;
   float temperature;
   float humidity;
   float pressure;
   // float SO2;
   // float NO2;
   float CO2;
   // float CO;
   // float O3;
};

struct TimeNow{
    u_char second;
    u_char minute;
    u_char hour;
    u_char date;
    u_char month;
    u_int year;
    uint32_t unix_time;
};

// extern DataCore data_core;

#endif