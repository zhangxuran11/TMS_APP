#ifndef __MTD_MFD_RS485_H__
#define __MTD_MFD_RS485_H__

#include "stm32f4xx.h"	 

extern u8 USART2_RS485_RxBuffer[100]; 	//接收缓冲,最大100个字节
//extern u8 USART2_RS485_TxBuffer[100]; 	//接收缓冲,最大100个字节
extern u8 USART2_RS485_RxLength;   			//接收到的数据长度

extern u8 USART3_RS485_RxBuffer[100]; 	//接收缓冲,最大100个字节
//extern u8 USART3_RS485_TxBuffer[100]; 	//接收缓冲,最大100个字节
extern u8 USART3_RS485_RxLength;   			//接收到的数据长度

	//RS485模式控制.0,接收;1,发送.
	
	/*定义输入GPIO端口*/
	/*USART2*/
#define  MTDMFD_USART2_RX_RCC      	 RCC_AHB1Periph_GPIOA
#define  MTDMFD_USART2_RX_GPIO 			 GPIOA
#define  MTDMFD_USART2_RX_GPIO_PIN 	 GPIO_Pin_3

#define  MTDMFD_USART2_TX_RCC      	 RCC_AHB1Periph_GPIOA
#define  MTDMFD_USART2_TX_GPIO 			 GPIOA
#define  MTDMFD_USART2_TX_GPIO_PIN 	 GPIO_Pin_2

#define  MTDMFD_USART2_DR_RCC      	 RCC_AHB1Periph_GPIOA
#define  MTDMFD_USART2_DR_GPIO 			 GPIOA
#define  MTDMFD_USART2_DR_GPIO_PIN 	 GPIO_Pin_4
#define  USART2_RS485_RX_EN()				 GPIO_ResetBits(MTDMFD_USART2_DR_GPIO, MTDMFD_USART2_DR_GPIO_PIN)  //低电平，接受使能
#define  USART2_RS485_TX_EN()				 GPIO_SetBits(MTDMFD_USART2_DR_GPIO, MTDMFD_USART2_DR_GPIO_PIN)    //高电平，发送使能


/*USART3*/
#define  MTDMFD_USART3_RX_RCC      	 RCC_AHB1Periph_GPIOB
#define  MTDMFD_USART3_RX_GPIO 			 GPIOB
#define  MTDMFD_USART3_RX_GPIO_PIN 	 GPIO_Pin_11

#define  MTDMFD_USART3_TX_RCC      	 RCC_AHB1Periph_GPIOB
#define  MTDMFD_USART3_TX_GPIO 			 GPIOB
#define  MTDMFD_USART3_TX_GPIO_PIN 	 GPIO_Pin_10

#define  MTDMFD_USART3_DR_RCC      	 RCC_AHB1Periph_GPIOE
#define  MTDMFD_USART3_DR_GPIO 			 GPIOE
#define  MTDMFD_USART3_DR_GPIO_PIN 	 GPIO_Pin_14
#define  USART3_RS485_RX_EN()				 GPIO_ResetBits(MTDMFD_USART3_DR_GPIO, MTDMFD_USART3_DR_GPIO_PIN)  //低电平，接受使能
#define  USART3_RS485_TX_EN()				 GPIO_SetBits(MTDMFD_USART3_DR_GPIO, MTDMFD_USART3_DR_GPIO_PIN)    //高电平，发送使能


void MTDMFD_USART2_RS485_Init(u32 BaudRate);
void MTDMFD_USART2_RS485_Send_Data(u8 *buf,u8 len);
void MTDMFD_USART2_RS485_Receive_Data(u8 *buf,u8 *len);		

/*
USART_Parity_Odd  奇校验
USART_Parity_Even	偶校验
*/
void MTDMFD_USART3_RS485_Init(u32 BaudRate,u16 VerifyMode);
void MTDMFD_USART3_RS485_Send_Data(u8 *buf,u8 len);
void MTDMFD_USART3_RS485_Receive_Data(u8 *buf,u8 *len);		

#endif //__MTD_MFD_RS485_H__
