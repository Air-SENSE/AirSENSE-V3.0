/*
 * lv_gui.c
 *
 *  Created on: Nov 20, 2020
 *      Author: nhat
 */

#include "lv_gui.h"
#include "lvgl.h"
#include "stdio.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
enum {ID_GOOD_FACE, ID_NORMAL_FACE, ID_NOT_GOOD_FACE, ID_BAD_FACE, ID_VERY_BAD_FACE, ID_EXTREMELY_BAD_FACE};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void show_face(uint8_t _face_id);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * tv;
static lv_obj_t * t1;
// static lv_obj_t * t2;
// static lv_obj_t * t3;
// static lv_obj_t * kb;
static lv_obj_t * middle;
static lv_obj_t * top;

static lv_obj_t * lb_clock;
static lv_obj_t * lb_pm2p5;
static lv_obj_t * lb_temp;
static lv_obj_t * lb_humi;
static lv_obj_t * lb_press;
static lv_obj_t * img_face;
static lv_obj_t * img_sdcard;
static lv_obj_t * img_wifi;
static lv_obj_t * gau_aqi;
static lv_style_t my_style;

LV_IMG_DECLARE(goodface);
LV_IMG_DECLARE(normalface);
LV_IMG_DECLARE(notgoodface);
LV_IMG_DECLARE(badface);
LV_IMG_DECLARE(verybadface);
LV_IMG_DECLARE(extremelybadface);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_gui_update_time(uint8_t _hour, uint8_t _min)
{
    char time[10];
    sprintf(time,"%02d:%02d", _hour, _min);
    lv_label_set_text(lb_clock, &time[0]);
}
void lv_gui_task()
{
    //set light theme
    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_secondary(),
                LV_THEME_MATERIAL_FLAG_LIGHT,
                lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());

    //tab 
	// static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,1}};
	// tv = lv_tileview_create(lv_scr_act(), NULL);
	// lv_tileview_set_valid_positions(tv, valid_pos, 4);
	// lv_tileview_set_edge_flash(tv, true);

	t1 = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(t1, LV_HOR_RES, LV_VER_RES);
	lv_tileview_add_element(tv, t1);

    /*Tile1: just a label*/
    // lv_obj_t * label = lv_label_create(t1, NULL);
    // lv_label_set_text(label, "Scroll down");
    // lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

 
    /**
     * @brief Font style
     * 
     */
    // my_style = lv_fo
    // lv_style_set_text_font(&my_style, LV_STATE_DEFAULT, &lv_font_montserrat_28);  
    // lv_style_init(&my_style);
    // lv_style_set_size(&my_style, LV_STATE_DEFAULT, 28);
//    lv_style_set_text_font(&my_style, LV_STATE_DEFAULT, &lv_font_montserrat_28);

    //create top
    top = lv_obj_create(t1, NULL);
    lv_obj_set_size(top, 320, 30);    
    lv_obj_align(top, t1, LV_ALIGN_IN_TOP_MID, 0, 0);

    //create middle
    middle = lv_obj_create(t1, NULL);
    lv_obj_set_size(middle, 320, 212);
    lv_obj_align(middle, t1, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

   

    /**
     * @brief Create clock
     * 
     */
    lb_clock = lv_label_create(top, NULL);
    lv_label_set_text(lb_clock, "10:11");
    // lv_obj_set_style_local_text_font(lb_clock, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER );
    // lv_style_t * style =;lv_sty
    // lv_style_init()
    lv_gui_update_time(1,0);
    lv_obj_align(lb_clock, top, LV_ALIGN_IN_TOP_RIGHT, -10,7);

    // /**
    //  * @brief airsense logo
    //  * 
    //  */
    // lv_obj_t * img_airsense = lv_img_create(top, NULL);
    // LV_IMG_DECLARE(airsense);
    // lv_img_set_src(img_airsense, &airsense);
    // lv_obj_set_size(img_airsense, airsense.header.w, airsense.header.h);
    // lv_obj_align(img_airsense, top, LV_ALIGN_IN_TOP_LEFT, 5, 5);

    // /**
    //  * @brief sparc logo
    //  * 
    //  */
    // lv_obj_t * img_sparc = lv_img_create(top, NULL);
    // LV_IMG_DECLARE(sparc);
    // lv_img_set_src(img_sparc, &sparc);
    // lv_obj_set_size(img_sparc, sparc.header.w, sparc.header.h);
    // lv_obj_align(img_sparc, top, LV_ALIGN_IN_TOP_LEFT, 88, 7);

    // /**
    //  * @brief face
    //  * 
    //  */
    // img_face = lv_img_create(middle, NULL);
    // lv_img_set_src(img_face, &goodface);
    // lv_obj_set_size(img_face,goodface.header.w, goodface.header.h);
    // lv_obj_align(img_face, middle, LV_ALIGN_CENTER,-83,-22);

    // /**
    //  * @brief temperature
    //  * 
    //  */
    // lv_obj_t * img_temperature = lv_img_create(middle, NULL);
    // LV_IMG_DECLARE(temp);
    // lv_img_set_src(img_temperature, &temp);
    // lv_obj_set_size(img_temperature,temp.header.w, temp.header.h);
    // lv_obj_align(img_temperature, middle, LV_ALIGN_IN_BOTTOM_LEFT,10,-15);
    
    // /**
    //  * @brief humidity
    //  * 
    //  */
    // lv_obj_t * img_humidity = lv_img_create(middle, NULL);
    // LV_IMG_DECLARE(humidity);
    // lv_img_set_src(img_humidity, &humidity);
    // lv_obj_set_size(img_humidity,humidity.header.w, humidity.header.h);
    // lv_obj_align(img_humidity, middle, LV_ALIGN_IN_BOTTOM_MID,-30,-15);
    
    // /**
    //  * @brief pressure
    //  * 
    //  */
    // lv_obj_t * img_pressure = lv_img_create(middle, NULL);
    // LV_IMG_DECLARE(press);
    // lv_img_set_src(img_pressure, &press);
    // lv_obj_set_size(img_pressure,press.header.w, press.header.h);
    // lv_obj_align(img_pressure, middle, LV_ALIGN_IN_BOTTOM_RIGHT,-70,-15);
    
    // /**
    //  * @brief pm2.5
    //  * 
    //  */
    // lv_obj_t * img_pm2p5 = lv_img_create(middle, NULL);
    // LV_IMG_DECLARE(pm2p5);
    // lv_img_set_src(img_pm2p5, &pm2p5);
    // lv_obj_set_size(img_pm2p5,pm2p5.header.w, pm2p5.header.h);
    // lv_obj_align(img_pm2p5, middle, LV_ALIGN_IN_TOP_RIGHT,-110,24);
    
    // /**
    //  * @brief SD card
    //  * 
    //  */
    // img_sdcard = lv_img_create(top, NULL);
    // LV_IMG_DECLARE(sdcard);
    // lv_img_set_src(img_sdcard, &sdcard);
    // lv_obj_set_size(img_sdcard,sdcard.header.w, sdcard.header.h);
    // lv_obj_align(img_sdcard, top, LV_ALIGN_IN_TOP_RIGHT,-85,5);

    // /**
    //  * @brief WiFi
    //  * 
    //  */
    // img_wifi= lv_img_create(top, NULL);
    // LV_IMG_DECLARE(wifi);
    // lv_img_set_src(img_wifi, &wifi);
    // lv_obj_set_size(img_wifi,wifi.header.w, wifi.header.h);
    // lv_obj_align(img_wifi, top, LV_ALIGN_IN_TOP_RIGHT,-56,5);

    // /**
    //  * @brief gauge
    //  * 
    //  */
    // gau_aqi= lv_img_create(middle, NULL);
    // LV_IMG_DECLARE(gauge);
    // lv_img_set_src(gau_aqi, &gauge);
    // lv_obj_set_size(gau_aqi,gauge.header.w, gauge.header.h);
    // lv_obj_align(gau_aqi, middle, LV_ALIGN_IN_BOTTOM_RIGHT,-20,-65);
    
    //create a gauge
    // lv_obj_t * gauge1 = lv_gauge_create(lv_scr_act(), NULL);
    // lv_obj_set_size(gauge1, 150, 150);
    // lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, 0);
    // lv_gauge_
    // lv_gauge_format_cb_t
    // lv_gauge_set_value(gauge1, 0, 30);

    /**
     * @brief label for value
     * 
     */
    lb_pm2p5 = lv_label_create(middle, NULL);
    lv_obj_align(lb_pm2p5, middle, LV_ALIGN_IN_TOP_RIGHT, -72, 36);
    lv_label_set_text(lb_pm2p5, "??? ug/m3");

    lb_temp = lv_label_create(middle, NULL);
    lv_obj_align(lb_temp, middle, LV_ALIGN_IN_BOTTOM_LEFT, 30, -15);
    lv_label_set_text(lb_temp, "??? *C");

    lb_humi = lv_label_create(middle, NULL);
    lv_obj_align(lb_humi, middle, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
    lv_label_set_text(lb_humi, "??? %");

    lb_press = lv_label_create(middle, NULL);
    lv_obj_align(lb_press, middle, LV_ALIGN_IN_BOTTOM_RIGHT, -35, -15);
    lv_label_set_text(lb_press, "??? atm");

    show_face(ID_EXTREMELY_BAD_FACE);

    // lv_obj_t * lmeter;
    // lmeter = lv_linemeter_create(middle, NULL);
    // lv_linemeter_set_range(lmeter, 0, 100);                   /*Set the range*/
    // lv_linemeter_set_value(lmeter, 80);                       /*Set the current value*/
    // lv_linemeter_set_scale(lmeter, 240, 21);                  /*Set the angle and number of lines*/
    // lv_obj_set_size(lmeter, 120, 120);
    // lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);
  
    /*Tile2: a list*/
    // lv_obj_t * list = lv_list_create(tv, NULL);
    // lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    // lv_obj_set_pos(list, 0, LV_VER_RES);
    // lv_list_set_scroll_propagation(list, true);
    // lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
    // lv_tileview_add_element(tv, list);

    // lv_list_add_btn(list, NULL, "One");
    // lv_list_add_btn(list, NULL, "Two");
    // lv_list_add_btn(list, NULL, "Three");
    // lv_list_add_btn(list, NULL, "Four");
    // lv_list_add_btn(list, NULL, "Five");
    // lv_list_add_btn(list, NULL, "Six");
    // lv_list_add_btn(list, NULL, "Seven");
    // lv_list_add_btn(list, NULL, "Eight");
}
void lv_gui_full_screen()
{
    middle = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(middle, 320, 240);
    lv_obj_t * img_face = lv_img_create(middle, NULL);
    LV_IMG_DECLARE(screen);
    lv_img_set_src(img_face, &screen);
    lv_obj_set_size(img_face,screen.header.w, screen.header.h);
    lv_obj_align(img_face, middle, LV_ALIGN_CENTER,0,0);
}
void lv_gui_update_value(float _temp, float _humi, float _pres, float _pm2p5)
{
    char data[10];
    sprintf(data, "%.1f ug/m3", _pm2p5);
    lv_label_set_text(lb_pm2p5, &data[0]);
    
    sprintf(data, "%.1f *C", _temp);
    lv_label_set_text(lb_temp, &data[0]);

    sprintf(data, "%.1f %%", _humi);
    lv_label_set_text(lb_humi, &data[0]);

    sprintf(data, "%.1f atm", _pres);
    lv_label_set_text(lb_press, &data[0]);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
void show_face(uint8_t _face_id)
{
    if(_face_id == ID_GOOD_FACE)
    {
        lv_img_set_src(img_face, &goodface);
        lv_obj_set_size(img_face,goodface.header.w, goodface.header.h);
    }
    else if(_face_id == ID_BAD_FACE)
    {
        lv_img_set_src(img_face, &badface);
        lv_obj_set_size(img_face,badface.header.w, badface.header.h);
    }
    else if(_face_id == ID_NORMAL_FACE)
    {
        lv_img_set_src(img_face, &normalface);
        lv_obj_set_size(img_face,normalface.header.w, normalface.header.h);
    }
    else if(_face_id == ID_VERY_BAD_FACE)
    {
        lv_img_set_src(img_face, &verybadface);
        lv_obj_set_size(img_face,verybadface.header.w, verybadface.header.h);
    }
    else if(_face_id == ID_EXTREMELY_BAD_FACE)
    {
        lv_img_set_src(img_face, &extremelybadface);
        lv_obj_set_size(img_face,extremelybadface.header.w, extremelybadface.header.h);
    }
    else if(_face_id == ID_NOT_GOOD_FACE)
    {
        lv_img_set_src(img_face, &notgoodface);
        lv_obj_set_size(img_face,notgoodface.header.w, notgoodface.header.h);
    }
    else
    {

    }
}
