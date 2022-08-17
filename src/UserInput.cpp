#include "UserInput.h"
// static AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(PIN_OUTA, PIN_OUTB, PIN_SWITCH);
static unsigned long lastPress;

UserInput::UserInput(/* args */)
{
    this->button1.pin = PIN_BUTTON_1;
    this->button2.pin = PIN_BUTTON_2;
}

UserInput::~UserInput()
{
}
// static void IRAM_ATTR ISRButton1()
// {

// }
// static void IRAM_ATTR ISRButton2()
// {
    
// }
void UserInput::init()
{
    pinMode(button1.pin, INPUT);
    button1.currentState = BUTTON_RELEASED;
    // pinMode(PIN_BUTTON_2, INPUT);
    // pinMode(PIN_OUTA, INPUT);
    // pinMode(PIN_OUTB, INPUT);
    // pinMode(PIN_SWITCH, INPUT);

    // attachInterrupt(PIN_BUTTON_1, ISRButton1, FALLING);
    // attachInterrupt(PIN_BUTTON_2, ISRButton2, FALLING);
}
void switchState(Button *_button, uint8_t _current_state)
{
    _button->previousState = _button->currentState;
    _button->currentState = _current_state;
}
void UserInput::buttonScan()
{
    if (millis() - lastPress > 2000 && digitalRead(PIN_BUTTON_1) == 0) 
    { 
        switchState(&this->button1, BUTTON_HELD);
    } 
    else if (digitalRead(PIN_BUTTON_1) == 1) 
    {
        switchState(&this->button1, BUTTON_RELEASED);
        lastPress = millis();
    }
}


Button UserInput::getButtonStatus(uint8_t _button_number)
{
    if(_button_number == 1)
    {
        return this->button1;
    }
    else
    {
        return this->button2;
    }
}
