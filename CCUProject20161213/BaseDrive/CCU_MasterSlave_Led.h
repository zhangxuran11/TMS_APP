#ifndef __CCU_MASTERSLAVE_LED_H__
#define __CCU_MASTERSLAVE_LED_H__
#include "stm32f4xx.h"


/************LEDµÆ½Ó¿Ú******************/

#define MASTER_LED3  3
#define SLAVE_LED4	 4

#define MASTER_SLAVE_LED_ON 	0
#define MASTER_SLAVE_LED_OFF	1

/*ccu °å×Ó*/
#define MASTER_LED3_RCC           RCC_AHB1Periph_GPIOC
#define MASTER_LED3_GPIO          GPIOC
#define MASTER_LED3_GPIO_PIN      GPIO_Pin_6
#define MASTER_LED3_ONOFF(x)      GPIO_WriteBit(MASTER_LED3_GPIO,MASTER_LED3_GPIO_PIN,(BitAction)x)

#define SLAVE_LED4_RCC            RCC_AHB1Periph_GPIOC
#define SLAVE_LED4_GPIO          	GPIOC
#define SLAVE_LED4_GPIO_PIN      	GPIO_Pin_7
#define SLAVE_LED4_ONOFF(x)      	GPIO_WriteBit(SLAVE_LED4_GPIO,SLAVE_LED4_GPIO_PIN,(BitAction)x)

void 	CCUMasterSlaveGpio_Init(void);
void  CCUMasterSlave_On (uint8_t LedNum);
void  CCUMasterSlave_Off (uint8_t LedNum);
void  CCUMasterSlave_Turn (uint8_t LedNum);

#endif //__CCU_MASTERSLAVE_LED_H__

