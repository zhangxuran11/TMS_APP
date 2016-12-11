#ifndef  __MTD_CPU_CANLED_H__
#define  __MTD_CPU_CANLED_H__

#include "stm32f4xx.h"

//#define MTDCPU_VERSION_1		//表示1代硬件板卡

#define MTDCPU_CANLED_NUMBER        4    //LED总数

#define CAN1_TX_LED 0
#define CAN1_RX_LED 1
#define CAN2_TX_LED 2
#define CAN2_RX_LED 3

//LED灯电路图上有下拉电阻，接到三极管上，故需要用高电平点亮。
/************LED灯接口******************/
#define CAN1_TX_RCC           RCC_AHB1Periph_GPIOG
#define CAN1_TX_GPIO          GPIOG
#define CAN1_TX_GPIO_PIN      GPIO_Pin_15
#define CAN1_TX_ONOFF(x)      GPIO_WriteBit(CAN1_TX_GPIO,CAN1_TX_GPIO_PIN,(BitAction)x)		

#define CAN1_RX_RCC           RCC_AHB1Periph_GPIOG
#define CAN1_RX_GPIO          GPIOG
#define CAN1_RX_GPIO_PIN      GPIO_Pin_8
#define CAN1_RX_ONOFF(x)      GPIO_WriteBit(CAN1_RX_GPIO,CAN1_RX_GPIO_PIN,(BitAction)x)

#ifdef   MTDCPU_VERSION_1     //表示1代板卡
#define CAN2_TX_RCC           RCC_AHB1Periph_GPIOB
#define CAN2_TX_GPIO          GPIOB
#define CAN2_TX_GPIO_PIN      GPIO_Pin_15
#define CAN2_TX_ONOFF(x)      GPIO_WriteBit(CAN2_TX_GPIO,CAN2_TX_GPIO_PIN,(BitAction)x)

#define CAN2_RX_RCC           RCC_AHB1Periph_GPIOB
#define CAN2_RX_GPIO          GPIOB
#define CAN2_RX_GPIO_PIN      GPIO_Pin_14
#define CAN2_RX_ONOFF(x)      GPIO_WriteBit(CAN2_RX_GPIO,CAN2_RX_GPIO_PIN,(BitAction)x)

#else                        //表示二代板卡
#define CAN2_TX_RCC           RCC_AHB1Periph_GPIOB
#define CAN2_TX_GPIO          GPIOB
#define CAN2_TX_GPIO_PIN      GPIO_Pin_14
#define CAN2_TX_ONOFF(x)      GPIO_WriteBit(CAN2_TX_GPIO,CAN2_TX_GPIO_PIN,(BitAction)x)

#define CAN2_RX_RCC           RCC_AHB1Periph_GPIOB
#define CAN2_RX_GPIO          GPIOB
#define CAN2_RX_GPIO_PIN      GPIO_Pin_15
#define CAN2_RX_ONOFF(x)      GPIO_WriteBit(CAN2_RX_GPIO,CAN2_RX_GPIO_PIN,(BitAction)x)
#endif

typedef struct{
  uint32_t     rcc;
  GPIO_TypeDef *gpio;
  uint16_t     pin;
}MTD_CPU_CANLED_LedGpio_Info;

void 	MTD_CPU_CANLED_Gpio_Init(void);
void  MTD_CPU_CANLED_On(uint8_t i);
void  MTD_CPU_CANLED_Off(uint8_t i);
void  MTD_CPU_CANLED_Turn(uint8_t i);


#endif //__MTD_CPU_CANLED_H__
