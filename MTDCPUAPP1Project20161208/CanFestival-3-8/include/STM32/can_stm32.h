/**
  ******************************************************************************
  * @file    can_stm32.h
  * @author  Ganhua R&D Driver Software Team
  * @version V1.0.0
  * @date    26/04/2015
  * @brief   This file is can_stm32 file.
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_STM32_H__
#define __CAN_STM32_H__

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "can.h"
#include "stm32f4xx.h"
#include "os.h"
/* Exported types ------------------------------------------------------------*/

#define 	RX_BUF_LEN		200//100	
#define 	TX_BUF_LEN		200//100	

/*can1信号量定义*/
extern OS_SEM  	can1slaverecv_sem;		
extern OS_SEM 	can1slavetran_sem;

/*can2 信号量定义*/
extern OS_SEM  	can2slaverecv_sem;		
extern OS_SEM 	can2slavetran_sem;

/*can3 信号量*/
extern OS_SEM   can3recv_sem;		
extern OS_SEM 	can3tran_sem;
extern OS_SEM		can3RxProc_sem;
 
 /*全局变量标志来替代信号量，表示数据的接受，发送标志*/
extern u32  g_can1RxCount;
extern u32 	g_can1TxCount;

extern u32 	g_can2RxCount;
extern u32 	g_can2TxCount;

extern u32 	g_can3RxCount;
extern u32 	g_can3TxCount;

/*can1 缓冲区*/
extern Message    can1slaverx_msg_buf[RX_BUF_LEN];
extern uint32_t		can1slaverx_save, can1slaverx_read;
extern CanTxMsg 	can1slavetx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can1slavetx_save, can1slavetx_read;

/*can2 缓冲区*/
extern Message    can2slaverx_msg_buf[RX_BUF_LEN];
extern uint32_t	  can2slaverx_save, can2slaverx_read;
extern CanTxMsg 	can2slavetx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can2slavetx_save, can2slavetx_read;

/*can3 缓冲区*/
extern Message   	can3rx_msg_buf[RX_BUF_LEN];
extern uint32_t		can3rx_save, can3rx_read;
extern CanTxMsg 	can3tx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can3tx_save, can3tx_read;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// Number of receive MB
#define NB_MB						8
#define NB_RX_MB				4
// Number of transmit MB
#define NB_TX_MB			(NB_MB - NB_RX_MB)

#if (NB_TX_MB < 1)
#error define less RX MBs, you must have at least 1 TX MB!
#elif (NB_RX_MB < 1)
#error define at least 1 RX MBs!
#endif

#define START_TX_MB			NB_RX_MB
#define TX_INT_MSK			((0xFF << (NB_MB - NB_TX_MB)) & 0xFF)
#define RX_INT_MSK			(0xFF >> (NB_MB - NB_RX_MB))


/* Exported functions ------------------------------------------------------- */
unsigned char canInit(CAN_TypeDef* CANx,unsigned int bitrate);
unsigned char canSend(CAN_PORT notused, Message *m);
unsigned char canReceive(Message *m);

#endif
