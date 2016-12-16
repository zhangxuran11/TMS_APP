/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN
AT91 Port: Peter CHRISTEN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "stm32f4xx.h"


#define	FALSE					0
#define	TRUE					1

// Needed defines by Atmel lib
//#define AT91C_MASTER_CLOCK      168000000UL    //时钟为120M

/* 设置波特率*/
#define CAN_BAUDRATE 125
#define CAN_BAUD_1M    3
#define CAN_BAUD_500K  6
#define CAN_BAUD_250K  12
#define CAN_BAUD_125K  24
#define CAN_BAUD_DEFAULT  CAN_BAUD_1M

/***********CAN1*/
  //#define CAN1                        0
  #define CAN1_CLK                    RCC_APB1Periph_CAN1
  #define CAN1_RX_PIN                 GPIO_Pin_11
  #define CAN1_TX_PIN                 GPIO_Pin_12
  #define CAN1_GPIO_PORT              GPIOA
  #define CAN1_GPIO_CLK               RCC_AHB1Periph_GPIOA
  #define CAN1_AF_PORT                GPIO_AF_CAN1
  #define CAN1_RX_SOURCE              GPIO_PinSource11
  #define CAN1_TX_SOURCE              GPIO_PinSource12      

/*_CAN2*/
  //#define CAN2                       1
  #define CAN2_CLK                    RCC_APB1Periph_CAN2
  #define CAN2_RX_PIN                 GPIO_Pin_5       //RX	12
  #define CAN2_TX_PIN                 GPIO_Pin_13				//TX  
  #define CAN2_GPIO_PORT              GPIOB
  #define CAN2_GPIO_CLK               RCC_AHB1Periph_GPIOB
  #define CAN2_AF_PORT                GPIO_AF_CAN2
  #define CAN2_RX_SOURCE              GPIO_PinSource5
  #define CAN2_TX_SOURCE              GPIO_PinSource13    


//// Needed defines by Canfestival lib
//#define MAX_CAN_BUS_ID 2
/* Needed defines by Canfestival lib */
#define MAX_CAN_BUS_ID 1		//W H H
/* New define, if SDO_MAX_LENGTH_TRANSFERT is exceeded allocate data buffer dynamically */
#define SDO_DYNAMIC_BUFFER_ALLOCATION
#define SDO_DYNAMIC_BUFFER_ALLOCATION_SIZE 	(1024)
#define SDO_MAX_LENGTH_TRANSFER 						256			//W H H
#define SDO_MAX_SIMULTANEOUS_TRANSFERS 			4				//W H H
#define NMT_MAX_NODE_ID 	128
#define SDO_TIMEOUT_MS 		3000U
#define MAX_NB_TIMER 			32												//W H H

#define SDO_BLOCK_SIZE 		16										  	//W H H 	SDO 中发现 SDO_BLOCK_SIZE 常量未定义.

// CANOPEN_BIG_ENDIAN is not defined
#define CANOPEN_LITTLE_ENDIAN 1	             		  //整个Cortex-M3系列为小端存储

#define US_TO_TIMEVAL_FACTOR 8

#define REPEAT_SDO_MAX_SIMULTANEOUS_TRANSFERS_TIMES(repeat)\
repeat
#define REPEAT_NMT_MAX_NODE_ID_TIMES(repeat)\
repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat repeat

#define EMCY_MAX_ERRORS 	8
//#define LSS_TIMEOUT_MS 1000											//W H H  增加这两个层管理定义，导致canfestival 会死机
//#define LSS_FS_TIMEOUT_MS 100		

#define REPEAT_EMCY_MAX_ERRORS_TIMES(repeat)\
repeat repeat repeat repeat repeat repeat repeat repeat


#endif /* _CONFIG_H_ */

