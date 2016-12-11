#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h" 	
#define  MTDCPU_SPI1_CS_GPIO   				GPIOA
#define  MTDCPU_SPI1_CS_CPIO_PIN			GPIO_Pin_4

/* 片选口线置低选中  */
#define MTDCPU_SPI1_CS_LOW()      		MTDCPU_SPI1_CS_GPIO->BSRRH = MTDCPU_SPI1_CS_CPIO_PIN

/* 片选口线置高不选中 */
#define MTDCPU_SPI1_CS_HIGH()      		MTDCPU_SPI1_CS_GPIO->BSRRL = MTDCPU_SPI1_CS_CPIO_PIN


#define  MTDCPU_SPI1 				SPI1
#define  MTDCPU_SPI1_CLK			RCC_APB2Periph_SPI1

void MTDCPU_SPI1_Init(void);			 //初始化SPI1口
void MTDCPU_SPI1_SetSpeed(u8 SpeedSet); //设置SPI1速度   
u8 	 MTDCPU_SPI1_SendByte(u8 TxData);		//SPI1总线发送一个字节
u8 	 MTDCPU_SPI1_ReadByte(void);    		//SPI1总线读取一个字节的数据
#endif

