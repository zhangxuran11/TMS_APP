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
#include "stdbool.h"
/* Exported types ------------------------------------------------------------*/
//typedef struct{
//	CanRxMsg m;
//}CANOpen_Message;			//W H H 

/* Exported constants --------------------------------------------------------*/


#define 	RX_BUF_LEN		500		//1024
#define 	TX_BUF_LEN		500		//1024

/*can1 信号量*/
extern OS_SEM   can1recv_sem;		
extern OS_SEM 	can1tran_sem;

/*can2 信号量*/
extern OS_SEM   can2recv_sem;		
extern OS_SEM 	can2tran_sem;

/*can3 信号量*/
extern OS_SEM   can3recv_sem;		
extern OS_SEM 	can3tran_sem;


/*全局变量标志来替代信号量，表示数据的接受，发送标志*/
extern u32  g_can1RxCount;
extern u32 	g_can1TxCount;

extern u32 	g_can2RxCount;
extern u32 	g_can2TxCount;

extern u32 	g_can3RxCount;
extern u32 	g_can3TxCount;


/*can1 缓冲区*/
extern Message    can1rx_msg_buf[RX_BUF_LEN];
extern uint32_t		can1rx_save, can1rx_read;
extern CanTxMsg 	can1tx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can1tx_save, can1tx_read;

/*can2 缓冲区*/
extern Message    can2rx_msg_buf[RX_BUF_LEN];
extern uint32_t	  can2rx_save, can2rx_read;
extern CanTxMsg 	can2tx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can2tx_save, can2tx_read;

/*can3 缓冲区*/
extern Message   	can3rx_msg_buf[RX_BUF_LEN];
extern uint32_t		can3rx_save, can3rx_read;
extern CanTxMsg 	can3tx_msg_buf[TX_BUF_LEN];
extern uint32_t 	can3tx_save, can3tx_read;


/* Exported macro ------------------------------------------------------------*/
// Number of receive MB
#define NB_MB					8
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
