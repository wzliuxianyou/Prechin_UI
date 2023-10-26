/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-4-06     wzlxy         The first version for STM32F4xx
 */

#include <board.h>


#if defined(PKG_USING_GT9147)
#include "gt9147.h"

#define GT9147_RST_PIN    GET_PIN(I, 8)
#define GT9147_IRQ_PIN    GET_PIN(H, 7)

int rt_hw_gt9147_port(void)
{
    struct rt_touch_config config;
    rt_uint8_t rst;
	
    rst = GT9147_RST_PIN;
    config.dev_name = "i2c1";
    config.irq_pin.pin  = GT9147_IRQ_PIN;
    config.irq_pin.mode = PIN_MODE_INPUT_PULLUP;
    config.user_data = &rst;
    rt_hw_gt9147_init("touchPad", &config);
    return 0;
}
INIT_ENV_EXPORT(rt_hw_gt9147_port);
#endif


