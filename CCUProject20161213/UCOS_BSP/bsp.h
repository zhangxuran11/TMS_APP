/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                         STM3240G-EVAL
*                                        Evaluation Board
*
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <stdarg.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_def.h>
#include  <lib_ascii.h>

#include <string.h>
#include <stdlib.h>

#include "stm32f4xx.h"
#include "SysTick.h"
#include "sys.h"
#include "os.h"
#include  <app_cfg.h>
#include  <includes.h>
#include "stdio.h"
#include "Uart.h"
#include "delay.h"
#include "i2c_gpio.h"
#include <string.h>
#include "bsp_tim_pwm.h"
#include "malloc.h"
#include "bsp_os.h"
#include "bsp_iwdg.h"
#include "Software_Timer.h"
#include "adc_temper.h"
#include "RunLed.h"
#include "Buzzer.h"
#include "can.h"
#include "config.h"
#include "RunLed.h"
#include "MTD_CCU_CAN3_MCP2515.h"
#include "CCUCAN3Master.h"
#include "MTD_CCU_CanAddres.h"
#include "can_stm32.h"
#include "DebugUart1.h"
#include "CCU_MasterSlave_Led.h"
#include "canfestival.h"
#include "CCUCAN1Master.h"
#include "CCUCAN2Master.h"

extern u8 ChooseBoard;	

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                          (void);

CPU_INT32U  BSP_CPU_ClkFreq                   (void);

void        BSP_Tick_Init                     (void);

/*	延时的定义		*/
void BSP_DelayMs(uint32_t _ulDelayTime);
void BSP_DelayUs(uint32_t _ulDelayTime);
void BSP_Idle(void);

void Check_CPU_Id(void);					//检查cpu的id
void DispTaskInfo(void);					//打印堆栈以及cpu使用情况
void MainCheckMacId(void);				//当前设备自己发送检测id
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */

