#ifndef  __MTD_MFD_CANLED_H__
#define  __MTD_MFD_CANLED_H__

#include "stm32f4xx.h"
#define MTDMFD_CANLED_NUMBER        2   //LED总数

#define CAN2_TX_LED 0
#define CAN2_RX_LED 1

//LED灯电路图上有下拉电阻，接到三极管上，故需要用高电平点亮。
/************LED灯接口******************/

#define CAN2_TX_RCC           RCC_AHB1Periph_GPIOE
#define CAN2_TX_GPIO          GPIOE
#define CAN2_TX_GPIO_PIN      GPIO_Pin_0
#define CAN2_TX_ONOFF(x)      GPIO_WriteBit(CAN2_TX_GPIO,CAN2_TX_GPIO_PIN,(BitAction)x)

#define CAN2_RX_RCC           RCC_AHB1Periph_GPIOE
#define CAN2_RX_GPIO          GPIOE
#define CAN2_RX_GPIO_PIN      GPIO_Pin_1
#define CAN2_RX_ONOFF(x)      GPIO_WriteBit(CAN2_RX_GPIO,CAN2_RX_GPIO_PIN,(BitAction)x)


typedef struct{
  uint32_t     rcc;
  GPIO_TypeDef *gpio;
  uint16_t     pin;
}MTD_MFD_CANLED_LedGpio_Info;

void 	MTD_MFD_CANLED_Gpio_Init(void);
void  MTD_MFD_CANLED_On(uint8_t i);
void  MTD_MFD_CANLED_Off(uint8_t i);
void  MTD_MFD_CANLED_Turn(uint8_t i);


#endif //__MTD_MFD_CANLED_H__
