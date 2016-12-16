#ifndef __MTD_MFD_CURRENTLOOP_H__
#define __MTD_MFD_CURRENTLOOP_H__

#include "stm32f4xx.h"

//#define MTD_MFD_1_0_ENABLE
#define MTD_MFD_2_0_ENABLE


/*V1.0 板卡引脚定义*/
#ifdef MTD_MFD_1_0_ENABLE	

/*定义输入GPIO端口*/
#define  MTDMFD_CLRX_RCC      	 RCC_AHB1Periph_GPIOC
#define  MTDMFD_CLRX_GPIO 			 GPIOC
#define  MTDMFD_CLRX_GPIO_PIN 	 GPIO_Pin_11
#define  MTDMFD_CLRX_READ()		 	 GPIO_ReadInputDataBit(MTDMFD_CLRX_GPIO,MTDMFD_CLRX_GPIO_PIN)

#define  MTDMFD_CLTX_RCC      	 RCC_AHB1Periph_GPIOC
#define  MTDMFD_CLTX_GPIO 			 GPIOC
#define  MTDMFD_CLTX_GPIO_PIN 	 GPIO_Pin_10
#define  MTDMFD_CLTX_WRITE(x)		 GPIO_WriteBit(MTDMFD_CLTX_GPIO,MTDMFD_CLTX_GPIO_PIN,x)

void MTDMFD_CL_Gpio_Init(void);
#endif 

/*V2.0 板卡引脚定义*/
#ifdef MTD_MFD_2_0_ENABLE

extern u8 USART4_CL_RxBuffer[100]; 	//接收缓冲,最大100个字节
//extern u8 USART4_CL_TxBuffer[100]; 	//接收缓冲,最大100个字节
extern u8 USART4_CL_RxLength;   			//接收到的数据长度

#define  MTDMFD_UARTx_RX_RCC      	 RCC_AHB1Periph_GPIOC
#define  MTDMFD_UARTx_RX_GPIO_PORT 	 GPIOC
#define  MTDMFD_UARTx_RX_GPIO_PIN 	 GPIO_Pin_11
#define  MTDMFD_UARTx_RX_SOURCE			 GPIO_PinSource11
#define  MTDMFD_UARTx_RX_AF					 GPIO_AF_UART4

#define  MTDMFD_UARTx_TX_RCC      	 RCC_AHB1Periph_GPIOC
#define  MTDMFD_UARTx_TX_GPIO_PORT 	 GPIOC
#define  MTDMFD_UARTx_TX_GPIO_PIN 	 GPIO_Pin_10
#define  MTDMFD_UARTx_TX_SOURCE			 GPIO_PinSource10
#define  MTDMFD_UARTx_TX_AF					 GPIO_AF_UART4

#define  MTDMFD_USARTx                UART4
#define  MTDMFD_USARTx_CLK            RCC_APB1Periph_UART4
#define  MTDMFD_USARTx_CLK_INIT       RCC_APB1PeriphClockCmd
#define  MTDMFD_USARTx_IRQn           UART4_IRQn
#define  MTDMFD_USARTx_IRQHandler     UART4_IRQHandler

void MTDMFD_CL_UART_Gpio_Init(u32 BaudRate);
void MTDMFD_CL_Send_Data(u8 *buf,u8 len);

#endif

#endif //__MTD_MFD_CURRENTLOOP_H__

