#ifndef __MTD_CPU_KEY_H__
#define __MTD_CPU_KEY_H__

#include "stm32f4xx.h"

#define MTDCPU_KEY_NUMBER         4    //按键总数
/************按键接口******************/
#define MTDCPU_KEY_S1_RCC         RCC_AHB1Periph_GPIOF
#define MTDCPU_KEY_S1_GPIO        GPIOF
#define MTDCPU_KEY_S1_GPIO_PIN    GPIO_Pin_6
#define MTDCPU_KEY_S1_UPDOWM()    GPIO_ReadInputDataBit(MTDCPU_KEY_S1_GPIO,MTDCPU_KEY_S1_GPIO_PIN)

#define MTDCPU_KEY_S2_RCC         RCC_AHB1Periph_GPIOF
#define MTDCPU_KEY_S2_GPIO        GPIOF
#define MTDCPU_KEY_S2_GPIO_PIN    GPIO_Pin_7
#define MTDCPU_KEY_S2_UPDOWM()    GPIO_ReadInputDataBit(MTDCPU_KEY_S2_GPIO,MTDCPU_KEY_S2_GPIO_PIN)

#define MTDCPU_KEY_S3_RCC         RCC_AHB1Periph_GPIOF
#define MTDCPU_KEY_S3_GPIO        GPIOF
#define MTDCPU_KEY_S3_GPIO_PIN    GPIO_Pin_8
#define MTDCPU_KEY_S3_UPDOWM()    GPIO_ReadInputDataBit(MTDCPU_KEY_S3_GPIO,MTDCPU_KEY_S3_GPIO_PIN)

#define MTDCPU_KEY_S4_RCC         RCC_AHB1Periph_GPIOF
#define MTDCPU_KEY_S4_GPIO        GPIOF
#define MTDCPU_KEY_S4_GPIO_PIN    GPIO_Pin_9
#define MTDCPU_KEY_S4_UPDOWM()    GPIO_ReadInputDataBit(MTDCPU_KEY_S4_GPIO,MTDCPU_KEY_S4_GPIO_PIN)

#define MTDCPU_UP 			1	
#define MTDCPU_DOWN			2
#define MTDCPU_SET			3
#define MTDCPU_OK   		4

typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}MTDCPU_KeyGpio_Info;

void 		MTDCPU_KEYGpio_Init(void);
uint8_t	MTDCPU_KEY_Scan(uint8_t mode);  		//按键扫描函数	

#endif //__MTD_CPU_KEY_H__
