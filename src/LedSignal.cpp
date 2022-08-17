#include "LedSignal.h"
#include "Adafruit_NeoPixel.h"

#ifdef _THREE_LED_
    Adafruit_NeoPixel pixels(1, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);
#endif

LedSignal::LedSignal(uint8_t _pin_red_led, uint8_t _pin_green_led, uint8_t _pin_blue_led )
{
    #ifdef _THREE_LED_
        this->red_led.pin = _pin_red_led;
        this->blue_led.pin = _pin_blue_led;
        this->green_led.pin = _pin_green_led;
    #endif
}
LedSignal::LedSignal()
{
    #ifdef _THREE_LED_
        this->red_led.pin = PIN_LED_RED;
        this->blue_led.pin = PIN_LED_BLUE;
        this->green_led.pin = PIN_LED_GREEN;
    #endif
}
LedSignal::~LedSignal()
{
}
void LedSignal::init()
{
    #ifdef _LED_NEO_RGB_
        pixels.begin();
        pixels.setBrightness(100);
    #endif

    #ifdef _THREE_LED_
        pinMode(this->red_led.pin, OUTPUT);
        pinMode(this->blue_led.pin, OUTPUT);
        pinMode(this->green_led.pin, OUTPUT);
        pinMode(PIN_NEO_PIXEL, OUTPUT);
        digitalWrite(PIN_NEO_PIXEL, HIGH);
    #endif
    resetLED();
}
void LedSignal::setLED()
{
    #ifdef _THREE_LED_
        setBlueLED();
        setRedLED();
        setGreenLED();
    #endif
}
void LedSignal::resetLED()
{
    #ifdef _LED_NEO_RGB_
        pixels.clear();
        pixels.show();
    #endif
    #ifdef _THREE_LED_
        resetBlueLED();
        resetGreenLED();
        resetRedLED();
    #endif
    this->current_color = COL_BLACK;
}
void LedSignal::setRedLED()
{
    #ifdef _LED_NEO_RGB_
        pixels.setPixelColor(0, pixels.Color(100,0,0));
        pixels.show();
    #endif
    #ifdef _THREE_LED_
        digitalWrite(this->red_led.pin, LOW);
        this->red_led.state = LED_ON;
    #endif
    this->current_color = COL_RED;
}
void LedSignal::setGreenLED()
{
    #ifdef _LED_NEO_RGB_
        pixels.setPixelColor(0, pixels.Color(0,100,0));
        pixels.show();
    #endif
    #ifdef _THREE_LED_
        digitalWrite(this->green_led.pin, LOW);
        this->green_led.state = LED_ON;
    #endif
    this->current_color = COL_GREEN;
}
void LedSignal::setBlueLED()
{
    #ifdef _LED_NEO_RGB_
        pixels.setPixelColor(0, pixels.Color(0,0,100));
        pixels.show();
    #endif
    #ifdef _THREE_LED_
        digitalWrite(this->blue_led.pin, LOW);
        this->blue_led.state = LED_ON;
    #endif
    this->current_color = COL_BLUE;
}
void LedSignal::resetRedLED()
{
    #ifdef _THREE_LED_
        digitalWrite(this->red_led.pin, HIGH);
        this->red_led.state = LED_OFF;
    #endif
}
void LedSignal::resetGreenLED()
{
    #ifdef _THREE_LED_
        digitalWrite(this->green_led.pin, HIGH);
        this->green_led.state = LED_OFF;
    #endif
}
void LedSignal::resetBlueLED()
{
    #ifdef _THREE_LED_
        digitalWrite(this->blue_led.pin, HIGH);
        this->blue_led.state = LED_OFF;
    #endif
}
void LedSignal::blinkLedBlue(uint8_t _times, uint16_t _period)
{
    #ifdef _THREE_LED_
        while(_times)
        {
            setBlueLED();
            delay(_period);
            resetBlueLED();
            delay(_period);
            _times--;
        }
    #endif

    #ifdef _LED_NEO_RGB_
        uint8_t previous_color = current_color;
        while (_times)
        {
            setBlueLED();
            delay(_period);
            resetLED();
            delay(_period);
            _times--;
        }
        switch (previous_color)
        {
        case COL_BLACK:
            resetLED();
            break;
        case COL_RED:
            setRedLED();
            break;
        case COL_GREEN:
            setGreenLED();
            break;
        case COL_BLUE:
            setBlueLED();
            break;
        default:
            break;
        }
    #endif
}
