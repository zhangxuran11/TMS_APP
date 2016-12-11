/*
*********************************************************************************************************
*	                                  
*	模块名称 : 看门狗程序头文件
*	文件名称 : bsp_iwdg.h
*	版    本 : V1.0
*	说    明 : IWDG例程。
*	修改记录 :
*		版本号  日期         作者    说明
*		v1.0    2012-12-12 stm32f4  ST固件库V1.0.2版本。
*
*	Copyright (C), 2013-2014
*   QQ超级群：216681322
*   BLOG: http://blog.sina.com.cn/u/2565749395
*********************************************************************************************************
*/

#ifndef _BSP_IWDG_H
#define _BSP_IWDG_H


#include "stm32f4xx.h"
/** 描述    : 独立看门狗初始化*/
void bsp_InitIwdg(uint32_t _ulIWDGTime);


/** 描述    : 喂独立看门狗*/
void IWDG_Feed(void);

#endif
