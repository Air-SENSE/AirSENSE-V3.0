#include "ILI9341Display.h"

// #define _LVGV_IN_USE_
#ifdef _LVGV_IN_USE_
#include "lv_gui.h"
#include "lvgl.h"
#else
// #include "Graphic/Free_Fonts.h"
// #include "Graphic/Logo_bitmaps.h"
#include "JPEGDecoder.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true

#endif
#define _DB_LOG_

TFT_eSPI tft; //= TFT_eSPI(); /* TFT instance */
#ifdef _LVGV_IN_USE_
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{

    Serial.printf("%s@%d->%s\r\n", file, line, dsc);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

/* Reading input device (simulated encoder here) */
bool read_encoder(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    static int32_t last_diff = 0;
    int32_t diff = 0;                   /* Dummy - no movement */
    int btn_state = LV_INDEV_STATE_REL; /* Dummy - no press */

    data->enc_diff = diff - last_diff;
    ;
    data->state = btn_state;

    last_diff = diff;

    return false;
}
#endif
//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos)
{

    //jpegInfo(); // Print information from the JPEG file (could comment this line out)

    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width;
    uint32_t max_y = JpegDec.height;

    bool swapBytes = tft.getSwapBytes();
    tft.setSwapBytes(true);

    // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
    uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

    // save the current image block size
    uint32_t win_w = mcu_w;
    uint32_t win_h = mcu_h;

    // record the current time so we can measure how long it takes to draw an image
    //   uint32_t drawTime = millis();

    // save the coordinate of the right and bottom edges to assist image cropping
    // to the screen size
    max_x += xpos;
    max_y += ypos;

    // Fetch data from the file, decode and display
    while (JpegDec.read())
    {                          // While there is more data in the file
        pImg = JpegDec.pImage; // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)

        // Calculate coordinates of top left corner of current MCU
        int mcu_x = JpegDec.MCUx * mcu_w + xpos;
        int mcu_y = JpegDec.MCUy * mcu_h + ypos;

        // check if the image block size needs to be changed for the right edge
        if (mcu_x + mcu_w <= max_x)
            win_w = mcu_w;
        else
            win_w = min_w;

        // check if the image block size needs to be changed for the bottom edge
        if (mcu_y + mcu_h <= max_y)
            win_h = mcu_h;
        else
            win_h = min_h;

        // copy pixels into a contiguous block
        if (win_w != mcu_w)
        {
            uint16_t *cImg;
            int p = 0;
            cImg = pImg + win_w;
            for (int h = 1; h < win_h; h++)
            {
                p += mcu_w;
                for (int w = 0; w < win_w; w++)
                {
                    *cImg = *(pImg + w + p);
                    cImg++;
                }
            }
        }

        // calculate how many pixels must be drawn
        // uint32_t mcu_pixels = win_w * win_h;

        // draw image MCU block only if it will fit on the screen
        if ((mcu_x + win_w) <= tft.width() && (mcu_y + win_h) <= tft.height())
            tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
        else if ((mcu_y + win_h) >= tft.height())
            JpegDec.abort(); // Image has run off bottom of screen so abort decoding
    }

    tft.setSwapBytes(swapBytes);

    //   showTime(millis() - drawTime); // These lines are for sketch testing only
}

//####################################################################################################
// Print image information to the serial port (optional)
//####################################################################################################
// JpegDec.decodeFile(...) or JpegDec.decodeArray(...) must be called before this info is available!
void jpegInfo()
{

    // Print information extracted from the JPEG file
    Serial.println("JPEG image info");
    Serial.println("===============");
    Serial.print("Width      :");
    Serial.println(JpegDec.width);
    Serial.print("Height     :");
    Serial.println(JpegDec.height);
    Serial.print("Components :");
    Serial.println(JpegDec.comps);
    Serial.print("MCU / row  :");
    Serial.println(JpegDec.MCUSPerRow);
    Serial.print("MCU / col  :");
    Serial.println(JpegDec.MCUSPerCol);
    Serial.print("Scan type  :");
    Serial.println(JpegDec.scanType);
    Serial.print("MCU width  :");
    Serial.println(JpegDec.MCUWidth);
    Serial.print("MCU height :");
    Serial.println(JpegDec.MCUHeight);
    Serial.println("===============");
    Serial.println("");
}
//####################################################################################################
// Draw a JPEG on the TFT pulled from SD Card
//####################################################################################################
// xpos, ypos is top left corner of plotted image
void drawSdJpeg(const char *filename, int xpos, int ypos)
{

    // Open the named file (the Jpeg decoder library will close it)
    File jpegFile = SPIFFS.open(filename, FILE_READ); // or, file handle reference for SD library

    if (!jpegFile)
    {
        Serial.print("ERROR: File \"");
        Serial.print(filename);
        Serial.println("\" not found!");
        return;
    }

    //   Serial.println("===========================");
    //   Serial.print("Drawing file: "); Serial.println(filename);
    //   Serial.println("===========================");

    // Use one of the following methods to initialise the decoder:
    boolean decoded = JpegDec.decodeSdFile(jpegFile); // Pass the SD file handle to the decoder,
    //boolean decoded = JpegDec.decodeSdFile(filename);  // or pass the filename (String or character array)

    if (decoded)
    {
        // print information about the image to the serial port
        jpegInfo();
        // render the image onto the screen at given coordinates
        jpegRender(xpos, ypos);
    }
    else
    {
        Serial.println("Jpeg file format not supported!");
    }
}

ILI9341Display::ILI9341Display()
{
}

ILI9341Display::~ILI9341Display()
{
}

void ILI9341Display::init()
{
    status.current_screen = PROGRESS_SCREEN;
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
#ifndef _LVGV_IN_USE_
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    tft.init();
    // tft.fillScreen(TFT_BLACK);
    tft.setRotation(3);
#else
    lv_init();
#if USE_LV_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    tft.begin();        /* TFT init */
    tft.setRotation(1); /* Landscape orientation */
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = read_encoder;
    lv_indev_drv_register(&indev_drv);
#endif

#ifdef _DB_LOG_
    Serial.println("Init TFT display");
#endif

    // lv_gui_task();
}

void ILI9341Display::progressScreen(bool _sd, bool _pms7003, bool _bme280, bool _ds3231, bool _sds011, bool _htu21d)
{
#ifdef _DB_LOG_
    Serial.println("Progess screen");
#endif

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0, 2);
    tft.setTextColor(TFT_WHITE);
    tft.print("Device ");
    tft.println(DEVICE_NAME);
    tft.print("Firmware version ");
    tft.println(FIRMWARE_VERSION);
    tft.println("Checking device, please wait...");
    if (_bme280)
    {
        tft.setTextColor(TFT_WHITE);
        tft.println("BME280 is connected");
    }
    else
    {
        tft.setTextColor(TFT_RED);
        tft.println("BME280 is disconnected");
    }
    if (_ds3231)
    {
        tft.setTextColor(TFT_WHITE);
        tft.println("DS3231 is connected");
    }
    else
    {
        tft.setTextColor(TFT_RED);
        tft.println("Ds3231 is disconnected");
    }
    if (_pms7003)
    {
        tft.setTextColor(TFT_WHITE);
        tft.println("PMS7003 is connected");
    }
    else
    {
        tft.setTextColor(TFT_RED);
        tft.println("PMS7003 is disconnected");
    }
    if (_sd)
    {
        tft.setTextColor(TFT_WHITE);
        tft.println("SD card is connected");
    }
    else
    {
        tft.setTextColor(TFT_RED);
        tft.println("SD card is disconnected");
    }
}
void ILI9341Display::mainScreen()
{
#ifdef _DB_LOG_
    Serial.println("Main screen");
#endif
#ifdef _LVGV_IN_USE_
    lv_gui_task();
#else
    // tft.fillScreen(TFT_BLACK);
    // tft.setFreeFont(FMBO9);
    // tft.setTextColor(TFT_GREEN, TFT_BLACK);
    // tft.drawString("AirSENSE V3" , 5, 5, 4);
    // tft.setTextColor(TFT_RED, TFT_BLACK);
    // tft.drawString("AQI:" , 200, 90, 4);
    // tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
    // tft.drawString("PM2.5: " , 5, 90, 4);
    // tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    // tft.drawString("Temp : " , 5, 130, 4);
    // tft.setTextColor(TFT_BLUE, TFT_BLACK);
    // tft.drawString("Hum  : " , 5, 170, 4);
    // tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    // tft.drawString("Press: " , 5, 210, 4);
    // tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    // tft.drawString("SPARC Lab" , 40, 30, 2);
    // showWiFiWaitSymbol();
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE);
    drawSdJpeg("/airsense.jpg", 10, 1);
    drawSdJpeg("/sparc.jpg", 95, 3);
    // drawSdJpeg("/sd.jpg", 225,1);
    // drawSdJpeg("/greenwifi.jpg", 250,1);
    tft.drawLine(5, 28, 315, 28, TFT_BLACK);

    drawSdJpeg("/pm2p5.jpg", 170, 50);
    drawSdJpeg("/press.jpg", 220, 200);
    drawSdJpeg("/humidity.jpg", 115, 200);
    drawSdJpeg("/temp.jpg", 10, 200);

    drawSdJpeg("/goodface.jpg", 10, 50);
    drawSdJpeg("/goodaqimeter.jpg", 160, 100);

    // tft.setCursor(10, 230, 2);
    // tft.setTextColor(TFT_BLACK);
    // tft.printf("ID: %s", );
#endif
}
void ILI9341Display::guiHandler()
{
#ifdef _LVGV_IN_USE_
    lv_task_handler();
    delay(10);
#endif
}
void ILI9341Display::showScreen()
{
    switch (this->status.current_screen)
    {
    case PROGRESS_SCREEN:
        // progressScreen();
        break;
    case MENU_SCREEN:
        break;
    case MAIN_SCREEN:
        mainScreen();
        break;
    default:
        break;
    }
}
void ILI9341Display::updateData(float _temp, float _humi, float _pres, float _pm2p5, char *_mac)
{
#ifdef _DB_LOG_
    Serial.println("Update data to display");
#endif
#ifdef _LVGV_IN_USE_
    lv_gui_update_value(_temp, _humi, _pres, _pm2p5);
#else
    //     tft.setCursor(0, 70, 2);
    //     tft.setTextColor(TFT_WHITE);
    //     tft.print("ID: ");
    //     tft.print(_mac);

    //     int xpos= 90;
    //     int ypos= 90;
    //     //Delete old PM2.5 value
    //     tft.fillRect(xpos, ypos, 60, 20, TFT_BLACK);

    //     if (_pm2p5 <= 50){
    //         tft.setTextColor(TFT_GREEN, TFT_BLACK);
    //         if(_pm2p5<10){
    //         xpos += tft.drawChar(' ', xpos, ypos , 4);
    //         xpos += tft.drawChar(' ', xpos, ypos , 4);
    //         xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //         }
    //         else{
    //             xpos += tft.drawChar(' ', xpos, ypos , 4);
    //             xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //         }

    //     }
    //     else if(_pm2p5 <= 100){
    //         tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    //         if(_pm2p5<100){
    //             xpos += tft.drawChar(' ', xpos, ypos , 4);
    //             xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //         }
    //         else
    //             xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //     }
    //     else if(_pm2p5 <= 150){
    //         tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    //         xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //     }
    //     else if(_pm2p5 <= 200){
    //         tft.setTextColor(TFT_RED, TFT_BLACK);
    //         xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //     }
    //     else if(_pm2p5 <= 300){
    //         tft.setTextColor(TFT_VIOLET, TFT_BLACK);
    //         xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //     }
    //     else{
    //         tft.setTextColor(TFT_BROWN, TFT_BLACK);
    //         xpos += tft.drawNumber(_pm2p5, xpos, ypos, 4);
    //     }
    //     xpos += tft.drawChar(' ', xpos, ypos-5 , 2);
    //     xpos += tft.drawChar('u', xpos, ypos+5 , 2);
    //     xpos += tft.drawChar('g', xpos, ypos+5, 2);
    //     xpos += tft.drawChar('/', xpos, ypos+5, 2);
    //     xpos += tft.drawChar('m', xpos, ypos+5, 2);
    //     xpos += tft.drawChar('3', xpos, ypos, 2);

    //     // Temperature, Humidity and Pressure quantity display
    //     xpos= 85;
    //     ypos= 130;
    //     tft.setTextColor(TFT_WHITE, TFT_BLACK );
    //     xpos += tft.drawNumber(_temp, xpos, ypos, 4);
    //     // tft.setCursor(xpos+7, ypos);
    //     // tft.printf("%.1f", temp);
    //     xpos += 4;
    //     xpos += tft.drawChar(' ', xpos, ypos-5 , 2);
    //     xpos += tft.drawChar('o', xpos, ypos , 2);
    //     xpos += tft.drawChar('C', xpos, ypos+5, 2);

    //     xpos= 85;
    //     ypos= 170;
    //     tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //     xpos += tft.drawNumber(_humi, xpos , ypos, 4);
    //     xpos += tft.drawChar(' ', xpos, ypos , 2);
    //     xpos += tft.drawChar('%', xpos, ypos+5  , 2);

    //     xpos= 85;
    //     ypos= 210;
    //     tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //     xpos += tft.drawNumber(_pres, xpos , ypos, 4);
    //     xpos += tft.drawChar(' ', xpos, ypos , 2);
    //     xpos += tft.drawChar('P', xpos, ypos+5, 2);
    //     xpos += tft.drawChar('a', xpos, ypos+5, 2);

    //     // AQI display
    //     xpos= 255;
    //     ypos= 90;
    //     tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //     xpos += tft.drawNumber(100, xpos, ypos, 4);

    // // AQI emotion face
    //     tft.setTextDatum(TL_DATUM);
    //     if(_pm2p5 <=50.0){
    //         //tft.drawBitmap(210, 120, Green_Face, Green_face_Width, Green_face_Height , TFT_GREEN);
    //         tft.fillCircle(240, 160, 40, TFT_GREEN);
    //         tft.fillCircle(255, 145, 5, TFT_BLACK);
    //         tft.fillCircle(225, 145, 5, TFT_BLACK);
    //         tft.fillCircle(240, 170, 18, TFT_WHITE);
    //         tft.fillRect(220, 150, 50, 25, TFT_GREEN);
    //     }
    //     else if(_pm2p5<=100.0){
    //         tft.fillCircle(240, 160, 40, TFT_YELLOW);
    //         tft.drawLine(245, 145, 260,145, TFT_BLACK);
    //         tft.drawLine(215, 145, 230,145, TFT_BLACK);
    //         tft.fillCircle(240, 170, 18, TFT_WHITE);
    //         tft.fillRect(220, 150, 50, 25, TFT_YELLOW);

    //     }
    //         //tft.drawBitmap(220, 120, Yellow_Face, Yellow_Face_Width, Yellow_Face_Height, TFT_YELLOW);
    //     else if(_pm2p5<=150.0){
    //         tft.fillCircle(240, 160, 40, TFT_ORANGE);
    //         tft.drawLine(245, 145, 260,145, TFT_BLACK);
    //         tft.drawLine(215, 145, 230,145, TFT_BLACK);
    //         tft.drawLine(230, 170, 250, 170, TFT_BLACK);
    //     }

    //     //  tft.drawBitmap(220, 120, Orange_Face, Orange_Face_Width, Orange_Face_Height, TFT_ORANGE );
    //     else if(_pm2p5<=200.0){
    //         tft.fillCircle(240, 160, 40, TFT_ORANGE);
    //         tft.fillCircle(255, 145, 5, TFT_BLACK);
    //         tft.fillCircle(225, 145, 5, TFT_BLACK);
    //         tft.drawLine(220, 155, 240, 155, TFT_BLACK);
    //     }
    //     // tft.drawBitmap(220, 120, Red_Face, Red_Face_Width, Red_Face_Height, TFT_RED);
    //     else if(_pm2p5 <=300.0){
    //         tft.fillCircle(240, 160, 40, TFT_PURPLE);
    //         tft.fillCircle(255, 145, 5, TFT_BLACK);
    //         tft.fillCircle(225, 145, 5, TFT_BLACK);
    //         tft.fillCircle(240, 170, 20, TFT_WHITE);
    //     }
    //         //tft.drawBitmap(220, 120, Purple_Face, Purple_Face_Width, Purple_Face_Height, TFT_PURPLE);
    //     else{
    //         tft.fillCircle(240, 160, 40, TFT_BLUE);
    //         tft.drawLine(245, 145, 260,145, TFT_BLACK);
    //         tft.drawLine(215, 145, 230,145, TFT_BLACK);
    //         tft.fillCircle(240, 170, 20, TFT_WHITE);
    //     }

    //PM2.5
    tft.setCursor(215, 60, 4);
    tft.setTextColor(TFT_BLACK);
    tft.fillRect(215, 60, 100, 20, TFT_WHITE);
    tft.setCursor(215, 60, 4);
    tft.printf("%.1f", _pm2p5);
    tft.setCursor(275, 65, 2);
    tft.print(" ug/m");
    tft.setCursor(tft.getCursorX(), tft.getCursorY() - 5, 2);
    tft.print("3");

    if (_pm2p5 <= 50)
    {
        drawSdJpeg("/goodface.jpg", 10, 50);
        drawSdJpeg("/goodaqimeter.jpg", 160, 100);
    }
    else if (_pm2p5 <= 100)
    {
        drawSdJpeg("/normalface.jpg", 10, 50);
        drawSdJpeg("/normalaqimeter.jpg", 160, 100);
    }
    else if (_pm2p5 <= 150)
    {
        drawSdJpeg("/notgoodface.jpg", 10, 50);
        drawSdJpeg("/notgoodaqimeter.jpg", 160, 100);
    }
    else if (_pm2p5 <= 200)
    {
        drawSdJpeg("/badface.jpg", 10, 50);
        drawSdJpeg("/badaqimeter.jpg", 160, 100);
    }
    else if (_pm2p5 <= 300)
    {
        drawSdJpeg("/verybadface.jpg", 10, 50);
        drawSdJpeg("/verybadaqimeter.jpg", 160, 100);
    }
    else
    {
        drawSdJpeg("/extremlybadface.jpg", 10, 50);
        drawSdJpeg("/extremlybadaqimeter.jpg", 160, 100);
    }

    //Temperature
    tft.fillRect(30, 200, 50, 20, TFT_WHITE);
    tft.setCursor(30, 200, 4);
    tft.printf("%.1f", _temp);
    tft.setCursor(80, 201, 2);
    tft.print("o");
    tft.setCursor(tft.getCursorX(), tft.getCursorY() + 5, 2);
    tft.print("C");

    //Humidity
    tft.fillRect(145, 200, 50, 20, TFT_WHITE);
    tft.setCursor(145, 200, 4);
    tft.printf("%.1f", _humi);
    tft.setCursor(195, 206, 2);
    tft.print("%");

    //Pressure
    tft.fillRect(242, 200, 50, 20, TFT_WHITE);
    tft.setCursor(242, 200, 4);
    tft.printf("%.1f", _pres / 101325.0);
    tft.setCursor(280, 206, 2);
    tft.print("atm");

    tft.setCursor(190, 225, 2);
    tft.setTextColor(TFT_BLACK);
    tft.printf("ID: %s", _mac);

#endif
}

void ILI9341Display::updateClock(uint8_t _hour, uint8_t _min)
{
#ifdef _DB_LOG_
    Serial.println("Update clock to display");
#endif
#ifdef _LVGV_IN_USE_
    lv_gui_update_time(_hour, _min);
#else
    // uint8_t xpos = 255;
    // uint8_t ypos = 23; // Top left corner ot clock text, about half way down
    // tft.setCursor(xpos, ypos, 4);
    // tft.fillRect(xpos, 0, 60, 30, TFT_BLACK);
    // tft.setCursor(xpos, ypos, 4);
    // if(_hour < 10)
    //     tft.print(0);
    // tft.print(_hour);
    // tft.print(":");

    // if(_min < 10)
    //     tft.print(0);
    // tft.print(_min);

    tft.fillRect(280, 8, 40, 20, TFT_WHITE);
    tft.setCursor(280, 8, 2);
    tft.setTextColor(TFT_BLACK);

    if (_hour < 10)
        tft.print(0);
    tft.print(_hour);
    tft.print(":");

    if (_min < 10)
        tft.print(0);
    tft.print(_min);
#endif
}

#ifdef _LVGV_IN_USE_
void ILI9341Display::showSDsymbol()
{
    lv_gui_show_sdcard_symbol();
}
void ILI9341Display::showNonSDsymbol()
{
    lv_gui_show_sdcard_symbol();
}
void ILI9341Display::showWiFiSymbol()
{
    lv_gui_show_wifi_symbol();
}
void ILI9341Display::showWiFiWaitSymbol()
{
    lv_gui_show_wifi_symbol();
}
void ILI9341Display::showNonWiFiSymbol()
{
    lv_gui_show_wifi_symbol();
}
void ILI9341Display::showWiFiConfigSymbol()
{
    lv_gui_show_wifi_symbol();
}
void ILI9341Display::showWiFiButNoInternetSymbol()
{
    lv_gui_show_wifi_symbol();
}

#else
void ILI9341Display::showSDsymbol()
{
    // tft.fillRect(230,0, SD_Width, SD_Height, TFT_BLACK);
    // tft.drawXBitmap(230, 0, SD_logo, SD_Width, SD_Height, TFT_GREEN);
    drawSdJpeg("/sd.jpg", 225, 1);
}
void ILI9341Display::showNonSDsymbol()
{
    // tft.fillRect(230, 0, noSD_Width, noSD_Height, TFT_BLACK);
    // tft.drawXBitmap(230, 0, noSD_logo, noSD_Width, noSD_Height, TFT_RED);
    drawSdJpeg("/nosd.jpg", 225, 1);
}
void ILI9341Display::showWiFiSymbol()
{
    // tft.fillRect(205, 5, Wifi_Height, Wifi_Width, TFT_BLACK);
    // tft.drawXBitmap(205, 5, logo_Wifi, Wifi_Width, Wifi_Height, TFT_GREEN);
    drawSdJpeg("/greenwifi.jpg", 250, 1);
}
void ILI9341Display::showWiFiWaitSymbol()
{
    // tft.fillRect(205, 5, Wifi_Height, Wifi_Width, TFT_BLACK);
    // tft.drawXBitmap(205, 5, logo_Wifi, Wifi_Width, Wifi_Height, TFT_WHITE);
    drawSdJpeg("/nowifi.jpg", 250, 1);
}
void ILI9341Display::showNonWiFiSymbol()
{
    // tft.fillRect(205, 5, noWifi_Width, noWifi_Width, TFT_BLACK);
    // tft.drawXBitmap(205, 5, logo_noWifi, noWifi_Width, noWifi_Height, TFT_RED);
    drawSdJpeg("/redwifi.jpg", 250, 1);
}
void ILI9341Display::showWiFiConfigSymbol()
{
    // tft.fillRect(205, 5, noWifi_Width, noWifi_Width, TFT_BLACK);
    // tft.drawXBitmap(205, 5, logo_noWifi, noWifi_Width, noWifi_Height, TFT_YELLOW);
    drawSdJpeg("/orangewifi.jpg", 250, 1);
}
void ILI9341Display::showWiFiButNoInternetSymbol()
{
    // tft.fillRect(205, 5, noWifi_Width, noWifi_Width, TFT_BLACK);
    // tft.drawXBitmap(205, 5, logo_noWifi, noWifi_Width, noWifi_Height, TFT_GREEN);
    drawSdJpeg("/wifinointernet.jpg", 250, 1);
}
#endif
ILI9341Status ILI9341Display::getStatus()
{
    return this->status;
}
void ILI9341Display::setDisplayScreen(SCREEN_ID _screen)
{
    this->status.previous_screen = status.current_screen;
    this->status.current_screen = _screen;
}
