/**
 * @file Ledsignal.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#ifndef _LED_SIGNAL_H_
#define _LED_SIGNAL_H_

// #define _LED_NEO_RGB_
#define _THREE_LED_

#include "Config.h"

enum {LED_OFF, LED_ON};
enum {COL_BLACK, COL_RED, COL_GREEN, COL_BLUE};
struct LED
{
    uint8_t pin;
    uint8_t state;
};
class LedSignal
{
private:
    /* data */
    #ifdef _THREE_LED_
        LED red_led;
        LED blue_led;
        LED green_led;
    #endif
    #ifdef _LED_NEO_RGB_
        
    #endif
    uint8_t current_color;
public:
    void init();
    void setLED();
    void resetLED();
    void setRedLED();
    void setBlueLED();
    void setGreenLED();
    void resetRedLED();
    void resetBlueLED();
    void resetGreenLED();
    void blinkLedBlue(uint8_t _times, uint16_t _period);
    LED getLED(uint8_t _led_num);
    LedSignal(uint8_t _pin_red_led, uint8_t _pin_green_led, uint8_t _pin_blue_led );
    LedSignal();
    ~LedSignal();
};

#endif