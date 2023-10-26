#include <rtdevice.h>


#include "mod_input.h"
#include "lv_port_indev.h"
#include "touch.h"
#include "drv_gpio.h"

static struct rt_touch_data *read_data;

static rt_err_t TouchIrq_callback(rt_device_t dev, rt_size_t size)
{
	M_InputOpr *p_TouchPad = (M_InputOpr *)dev->user_data;
	M_TouchPadPara *para = (M_TouchPadPara *)p_TouchPad->user_data;

	rt_device_control(dev, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);	
    if (rt_pin_read(GET_PIN(H, 7)))
    {
		if(para->bIsIrqOccur == RT_TRUE)
		{
			para->bIsIrqOccur = RT_FALSE;
			//rt_kprintf("callback Relese\n");
		} 
		rt_device_control(p_TouchPad->InputDev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);		
		return 0;
    }
	rt_sem_release(&p_TouchPad->IrqSem);
	return 0;
}


void ts_entry(void *parameter)
{
	M_InputOpr* pTsDev = (M_InputOpr *)parameter;
	M_TouchPadPara *para = (M_TouchPadPara *)pTsDev->user_data;
	
    while (1)
    {
		rt_sem_take(&pTsDev->IrqSem, RT_WAITING_FOREVER);
        if (rt_device_read(pTsDev->InputDev, 0, (void *)read_data, 1) == 1)
        {
			if (read_data->event == RT_TOUCH_EVENT_DOWN)
			{
				para->last_x = read_data->x_coordinate;
				para->last_y = read_data->y_coordinate;
				para->bIsIrqOccur = RT_TRUE;
				//rt_kprintf(" Press, last_x = %d, last_y = %d\n", 480 - para->last_x, 272 - para->last_y);
			}     	
        }
		rt_device_control(pTsDev->InputDev, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);
    }
}


void HW_TsReadCount(M_InputOpr *m_InputDev)
{
	return;
}



int HW_TsInputRegister(M_listObject *list, void *obj)
{
	int ret;
	rt_thread_t  ts_thread = RT_NULL;
	lv_indev_drv_t *p_indevDrv = (lv_indev_drv_t *)obj;
		
	M_InputOpr *p_TouchPad = (M_InputOpr *)rt_malloc(sizeof(M_InputOpr));
	if(RT_NULL == p_TouchPad)
	{
	    rt_kprintf("m_Encoder malloc Fail\n");
		return -1;
	}
	/********************************************
	*0. 查找并打开(只读方式)脉冲编码器设备 
	*********************************************/
	p_TouchPad->InputDev = rt_device_find(TS_DEV_NAME);
	if(p_TouchPad->InputDev == RT_NULL)
	{
		rt_kprintf("touch Register failed! can't find %s device!\n", TS_DEV_NAME);
		return -1;
	}
	/*****************************************
	*1. private数据赋值
	******************************************/ 	
	p_TouchPad->user_data = (M_TouchPadPara *)rt_calloc(1, sizeof(M_TouchPadPara));
	/*********************
	*2. public数据赋值
	*********************/	
	p_TouchPad->id = TS_DEV_ID;
	p_TouchPad->event_proc = HW_TsReadCount;
	/*****************************************
	*3. device private数据赋值
	******************************************/ 
	p_TouchPad->InputDev->user_data = (M_InputOpr *)p_TouchPad;
	read_data = (struct rt_touch_data *)rt_calloc(1, sizeof(struct rt_touch_data));
	/*****************************************
	*4. 创建信号量及监听线程
	******************************************/
	ret = rt_sem_init(&p_TouchPad->IrqSem, "tsSem", 0, RT_IPC_FLAG_FIFO);
	if (ret != RT_EOK)
	{
			rt_kprintf("init tsSem failed!\n");
			return -1;
	}
	ts_thread = rt_thread_create("ts_thread", ts_entry, (void *)p_TouchPad, 1024, 25, 5);
	if (ts_thread != RT_NULL)
			rt_thread_startup(ts_thread);

	/*********************
	*6. lvgl 输入驱动private数据赋值
	*********************/		
	p_indevDrv->user_data = (M_InputOpr *)p_TouchPad;
	M_listNode *node = list_rpush(list, list_node_new((void *)p_TouchPad));
	if(node == NULL)
		return -1;
	//rt_kprintf("init ts success!\n");
	/*****************************************
	*5. 挂接目标接收中断函数
	******************************************/
    rt_device_set_rx_indicate(p_TouchPad->InputDev, TouchIrq_callback);	
	if(rt_device_open(p_TouchPad->InputDev, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
	{
		rt_kprintf("open device failed!");
		return -1;
	}
	return 0;
}


