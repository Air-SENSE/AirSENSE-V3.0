/**
 * @file UserInterfaceManager.h
 * @author Le Duy Nhat
 * @date 2020-08-26
 * @brief 
 */
#pragma once
#include "Config.h"
#include "UserInput.h"
#include "LedSignal.h"
#include "ILI9341Display.h"

class UserInterfaceManager
{
private:
    /* data */
public:
    ILI9341Display display;
    UserInput user_input;
    LedSignal led_signal;
    void init();
    void show(SCREEN_ID _screen);
    UserInterfaceManager(/* args */);
    ~UserInterfaceManager();
};


