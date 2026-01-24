///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// LVGL Example: 2.8inch_ESP32-S3_E32C28P_E32N28P [Cubic Bezier animation]   // 
// LVGL V9.3                                                                 //
// Youtube:https://www.youtube.com/@pangcrd                                  //
// Github: https://github.com/pangcrd                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/integration/framework/arduino.html*/
#include <Arduino.h>
#include <lvgl.h>
#include "touch.h"
#include "ui.h"

#if LV_USE_TFT_ESPI
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#endif

/*Set to your screen resolution and rotation*/
#define TFT_ROTATION  LV_DISPLAY_ROTATION_270

#define TFT_HOR_RES   240  
#define TFT_VER_RES   320

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE/4];

/* LVGL calls it when a rendered image needs to copied to the display*/
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushPixels( (uint16_t*) px_map, w * h );
    tft.endWrite();

    lv_display_flush_ready( disp );
}
/*Read the touchpad*/
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
  if (touch_touched()) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;
        
        // Debug touch coordinates (uncomment if needed)
        // Serial.print("Touch: x=");
        // Serial.print(touch_last_x);
        // Serial.print(", y=");
        // Serial.println(touch_last_y);
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void){return millis();}

void setup()
{
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.begin( 115200 );
    Serial.println( LVGL_Arduino );

    // Initialize touch before initializing LVGL.
    // Map rotation from LVGL to FT6336
    /** LV_DISPLAY_ROTATION_90 → Touch ROTATION_INVERTED
        LV_DISPLAY_ROTATION_180  → Touch ROTATION_NORMAL
        LV_DISPLAY_ROTATION_0 → Touch ROTATION_NORMAL
        LV_DISPLAY_ROTATION_270 Touch ROTATION_INVERTED */
    uint8_t touch_rotation = (TFT_ROTATION == LV_DISPLAY_ROTATION_90 || 
                          TFT_ROTATION == LV_DISPLAY_ROTATION_270) 
                          ? ROTATION_INVERTED : ROTATION_NORMAL;
    
    touch_init(TFT_HOR_RES, TFT_VER_RES, touch_rotation);
    Serial.print("Touch initialized with rotation: ");
    Serial.println(touch_rotation);

    lv_init();
    lv_tick_set_cb(my_tick);

    lv_display_t * disp;
    /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);

    /*Initialize the (dummy) input device driver*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, my_touchpad_read);
    lv_example_anim_3();
    Serial.println( "Setup done" );
}
void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}