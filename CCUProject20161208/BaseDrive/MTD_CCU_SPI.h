#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h" 	
#define  MTDCCU_SPI1_CS_GPIO   				GPIOA
#define  MTDCCU_SPI1_CS_CPIO_PIN			GPIO_Pin_4

/* 片选口线置低选中  */
#define MTDCCU_SPI1_CS_LOW()      		MTDCCU_SPI1_CS_GPIO->BSRRH = MTDCCU_SPI1_CS_CPIO_PIN

/* 片选口线置高不选中 */
#define MTDCCU_SPI1_CS_HIGH()      		MTDCCU_SPI1_CS_GPIO->BSRRL = MTDCCU_SPI1_CS_CPIO_PIN


#define  MTDCCU_SPI1 					SPI1
#define  MTDCCU_SPI1_CLK			RCC_APB2Periph_SPI1

void MTDCCU_SPI1_Init(void);			 //初始化SPI1口
void MTDCCU_SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 	 MTDCCU_SPI1_SendByte(u8 TxData);		//SPI1总线发送一个字节
u8 	 MTDCCU_SPI1_ReadByte(void);    		//SPI1总线读取一个字节的数据
#endif

