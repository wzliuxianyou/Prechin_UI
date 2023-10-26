/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-4-06     wzlxy         The first version for STM32F4xx
 */



#ifndef __SPI_PORT_H__
#define __SPI_PORT_H__

/*****************************
* use for XT2046 touch Chip
******************************/
#ifdef BSP_USING_SOFT_SPI1

#define BSP_S_SPI1_SCK_PIN    GET_PIN(H, 6)
#define BSP_S_SPI1_MOSI_PIN   GET_PIN(I, 3)
#define BSP_S_SPI1_MISO_PIN   GET_PIN(G, 3)

#endif


#endif

