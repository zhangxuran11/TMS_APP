#ifndef __DEBUGUART1_H__
#define __DEBUGUART1_H__

#include "stm32f4xx.h"

/* 定义调试打印语句，用于排错 */		//W H H  两种的定义都可以
#define printf_info	printf
#define printf_ok(...)	
//#define debugprintf printf
//#define debugprintf(...) printf (__VA_ARGS__)

/* Definition for DMAx resources ********************************************/
#define USARTx_DR_ADDRESS                ((uint32_t)USART1 + 0x04) 

#define USARTx_DMA                       DMA2
#define USARTx_DMAx_CLK                  RCC_AHB1Periph_DMA2
 
#define USARTx_TX_DMA_CHANNEL            DMA_Channel_4
#define USARTx_TX_DMA_STREAM             DMA2_Stream7
#define USARTx_TX_DMA_FLAG_FEIF          DMA_FLAG_FEIF7
#define USARTx_TX_DMA_FLAG_DMEIF         DMA_FLAG_DMEIF7
#define USARTx_TX_DMA_FLAG_TEIF          DMA_FLAG_TEIF7
#define USARTx_TX_DMA_FLAG_HTIF          DMA_FLAG_HTIF7
#define USARTx_TX_DMA_FLAG_TCIF          DMA_FLAG_TCIF7
		  
#define USARTx_RX_DMA_CHANNEL            DMA_Channel_4
#define USARTx_RX_DMA_STREAM             DMA2_Stream2
#define USARTx_RX_DMA_FLAG_FEIF          DMA_FLAG_FEIF2
#define USARTx_RX_DMA_FLAG_DMEIF         DMA_FLAG_DMEIF2
#define USARTx_RX_DMA_FLAG_TEIF          DMA_FLAG_TEIF2
#define USARTx_RX_DMA_FLAG_HTIF          DMA_FLAG_HTIF2
#define USARTx_RX_DMA_FLAG_TCIF          DMA_FLAG_TCIF2

#define USARTx_DMA_TX_IRQn               DMA2_Stream7_IRQn
#define USARTx_DMA_RX_IRQn               DMA2_Stream2_IRQn
#define USARTx_DMA_TX_IRQHandler         DMA2_Stream7_IRQHandler
#define USARTx_DMA_RX_IRQHandler         DMA2_Stream2_IRQHandler

#define UART1BUFFERSIZE                       20

extern uint8_t Uart1_Tx_Buff[UART1BUFFERSIZE];
extern uint8_t Uart1_Rx_Buff[UART1BUFFERSIZE];

void User_Uart1SendChar(unsigned char ch);
void User_Uart1SendString(unsigned char *s);
void Uart1_init(void);
void Uart1_IRQ(void);
void Uart1_NVIC_Config(void);
uint16_t USART1_DMA_SendBuffer(const uint8_t* buffer, uint16_t length);

void Debug_Printf(const char* format,...);

#endif		//__DEBUGUART1_H__
