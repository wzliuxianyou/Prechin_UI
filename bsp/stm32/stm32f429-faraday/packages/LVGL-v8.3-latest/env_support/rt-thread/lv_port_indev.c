/*********************
 *      INCLUDES
 *********************/

#include <board.h>
#include "lv_port_indev.h"
#include "lvgl.h"
#include "touch.h"

#include "application.h"


static lv_indev_drv_t ts_indev_drv;

#if 0
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    data->point.x = last_x;
    data->point.y = last_y;
}
static bool touchpad_is_pressed(void){
    if(true == g_Coord.isPressed){
        return true;
    }
    return false;
}
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    (*x) = (lv_coord_t)(g_Coord.xCoord);
    (*y) = (lv_coord_t)(g_Coord.yCoord);
}
#endif

static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	M_InputOpr* pDev = (M_InputOpr* )indev_drv->user_data;
	M_TouchPadPara *para = (M_TouchPadPara *)pDev->user_data;

	data->point.x = 480 - para->last_x;
	data->point.y = 272 - para->last_y;
	data->state = para->bIsIrqOccur? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/*Initialize your touchpad*/
static void touchpad_init(lv_indev_drv_t *indev_drv)
{
	MainCore.new_object(&g_tLaserManager, CLASS_INPUT, DEV_TYPE_TS, (void *)indev_drv);
}

void lv_port_indev_init(void)
{
    lv_indev_drv_init(&ts_indev_drv);
	touchpad_init(&ts_indev_drv);
    ts_indev_drv.type = LV_INDEV_TYPE_POINTER;
    ts_indev_drv.read_cb = touchpad_read;
    lv_indev_drv_register(&ts_indev_drv);
}







