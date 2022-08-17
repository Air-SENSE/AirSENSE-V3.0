/**
 * @file Ledsignal.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#ifndef _USER_INPUT_H_
#define _USER_INPUT_H_
#include "Config.h"
// #include "AiEsp32RotaryEncoder.h"

enum{BUTTON_RELEASED, BUTTON_PRESSED, BUTTON_HELD};
struct Button
{
    uint8_t pin;
    uint8_t currentState;
    uint8_t previousState;
};

class UserInput
{
private:
    /* data */
    Button button1;
    Button button2;
public:
    void init();
    void buttonScan();
    Button getButtonStatus(uint8_t _button_number);
    UserInput(/* args */);
    ~UserInput();
};


#endif