#ifndef  __CCU_LED_LEDCONTROL_H__
#define  __CCU_LED_LEDCONTROL_H__

#include "stm32f4xx.h"
#define CCULED_LED_NUMBER         26    //LED总数

//LED灯电路图上有下拉电阻，接到三极管上，故需要用高电平点亮。
/************LED灯接口******************/
#define LED1_RCC           RCC_AHB1Periph_GPIOC
#define LED1_GPIO          GPIOC
#define LED1_GPIO_PIN      GPIO_Pin_12
#define LED1_ONOFF(x)      GPIO_WriteBit(LED1_GPIO,LED1_GPIO_PIN,(BitAction)x)		

#define LED2_RCC           RCC_AHB1Periph_GPIOD
#define LED2_GPIO          GPIOD
#define LED2_GPIO_PIN      GPIO_Pin_13
#define LED2_ONOFF(x)      GPIO_WriteBit(LED2_GPIO,LED2_GPIO_PIN,(BitAction)x)

#define LED3_RCC           RCC_AHB1Periph_GPIOC
#define LED3_GPIO          GPIOC
#define LED3_GPIO_PIN      GPIO_Pin_11
#define LED3_ONOFF(x)      GPIO_WriteBit(LED3_GPIO,LED3_GPIO_PIN,(BitAction)x)

#define LED4_RCC           RCC_AHB1Periph_GPIOD
#define LED4_GPIO          GPIOD
#define LED4_GPIO_PIN      GPIO_Pin_12
#define LED4_ONOFF(x)      GPIO_WriteBit(LED4_GPIO,LED4_GPIO_PIN,(BitAction)x)

#define LED5_RCC           RCC_AHB1Periph_GPIOC
#define LED5_GPIO          GPIOC
#define LED5_GPIO_PIN      GPIO_Pin_10
#define LED5_ONOFF(x)      GPIO_WriteBit(LED5_GPIO,LED5_GPIO_PIN,(BitAction)x)		

#define LED6_RCC           RCC_AHB1Periph_GPIOD
#define LED6_GPIO          GPIOD
#define LED6_GPIO_PIN      GPIO_Pin_11
#define LED6_ONOFF(x)      GPIO_WriteBit(LED6_GPIO,LED6_GPIO_PIN,(BitAction)x)

#define LED7_RCC           RCC_AHB1Periph_GPIOA
#define LED7_GPIO          GPIOA
#define LED7_GPIO_PIN      GPIO_Pin_12
#define LED7_ONOFF(x)      GPIO_WriteBit(LED7_GPIO,LED7_GPIO_PIN,(BitAction)x)

#define LED8_RCC           RCC_AHB1Periph_GPIOD
#define LED8_GPIO          GPIOD
#define LED8_GPIO_PIN      GPIO_Pin_10
#define LED8_ONOFF(x)      GPIO_WriteBit(LED8_GPIO,LED8_GPIO_PIN,(BitAction)x)

#define LED9_RCC           RCC_AHB1Periph_GPIOA
#define LED9_GPIO          GPIOA
#define LED9_GPIO_PIN      GPIO_Pin_11
#define LED9_ONOFF(x)      GPIO_WriteBit(LED9_GPIO,LED9_GPIO_PIN,(BitAction)x)		

#define LED10_RCC           RCC_AHB1Periph_GPIOD
#define LED10_GPIO          GPIOD
#define LED10_GPIO_PIN      GPIO_Pin_9
#define LED10_ONOFF(x)      GPIO_WriteBit(LED10_GPIO,LED10_GPIO_PIN,(BitAction)x)

#define LED11_RCC           RCC_AHB1Periph_GPIOA
#define LED11_GPIO          GPIOA
#define LED11_GPIO_PIN      GPIO_Pin_8
#define LED11_ONOFF(x)      GPIO_WriteBit(LED11_GPIO,LED11_GPIO_PIN,(BitAction)x)

#define LED12_RCC           RCC_AHB1Periph_GPIOD
#define LED12_GPIO          GPIOD
#define LED12_GPIO_PIN      GPIO_Pin_8
#define LED12_ONOFF(x)      GPIO_WriteBit(LED12_GPIO,LED12_GPIO_PIN,(BitAction)x)

#define LED13_RCC           RCC_AHB1Periph_GPIOC
#define LED13_GPIO          GPIOC
#define LED13_GPIO_PIN      GPIO_Pin_9
#define LED13_ONOFF(x)      GPIO_WriteBit(LED13_GPIO,LED13_GPIO_PIN,(BitAction)x)		

#define LED14_RCC           RCC_AHB1Periph_GPIOB
#define LED14_GPIO          GPIOB
#define LED14_GPIO_PIN      GPIO_Pin_15
#define LED14_ONOFF(x)      GPIO_WriteBit(LED14_GPIO,LED14_GPIO_PIN,(BitAction)x)

#define LED15_RCC           RCC_AHB1Periph_GPIOC
#define LED15_GPIO          GPIOC
#define LED15_GPIO_PIN      GPIO_Pin_8
#define LED15_ONOFF(x)      GPIO_WriteBit(LED15_GPIO,LED15_GPIO_PIN,(BitAction)x)

#define LED16_RCC           RCC_AHB1Periph_GPIOB
#define LED16_GPIO          GPIOB
#define LED16_GPIO_PIN      GPIO_Pin_14
#define LED16_ONOFF(x)      GPIO_WriteBit(LED16_GPIO,LED16_GPIO_PIN,(BitAction)x)

#define LED17_RCC           RCC_AHB1Periph_GPIOC
#define LED17_GPIO          GPIOC
#define LED17_GPIO_PIN      GPIO_Pin_7
#define LED17_ONOFF(x)      GPIO_WriteBit(LED17_GPIO,LED17_GPIO_PIN,(BitAction)x)

#define LED18_RCC           RCC_AHB1Periph_GPIOB
#define LED18_GPIO          GPIOB
#define LED18_GPIO_PIN      GPIO_Pin_12
#define LED18_ONOFF(x)      GPIO_WriteBit(LED18_GPIO,LED18_GPIO_PIN,(BitAction)x)

#define LED19_RCC           RCC_AHB1Periph_GPIOC
#define LED19_GPIO          GPIOC
#define LED19_GPIO_PIN      GPIO_Pin_6
#define LED19_ONOFF(x)      GPIO_WriteBit(LED19_GPIO,LED19_GPIO_PIN,(BitAction)x)

#define LED20_RCC           RCC_AHB1Periph_GPIOB
#define LED20_GPIO          GPIOB
#define LED20_GPIO_PIN      GPIO_Pin_11
#define LED20_ONOFF(x)      GPIO_WriteBit(LED20_GPIO,LED20_GPIO_PIN,(BitAction)x)		

#define LED21_RCC           RCC_AHB1Periph_GPIOG
#define LED21_GPIO          GPIOG
#define LED21_GPIO_PIN      GPIO_Pin_8
#define LED21_ONOFF(x)      GPIO_WriteBit(LED21_GPIO,LED21_GPIO_PIN,(BitAction)x)

#define LED22_RCC           RCC_AHB1Periph_GPIOB
#define LED22_GPIO          GPIOB
#define LED22_GPIO_PIN      GPIO_Pin_10
#define LED22_ONOFF(x)      GPIO_WriteBit(LED22_GPIO,LED22_GPIO_PIN,(BitAction)x)

#define LED23_RCC           RCC_AHB1Periph_GPIOG
#define LED23_GPIO          GPIOG
#define LED23_GPIO_PIN      GPIO_Pin_7
#define LED23_ONOFF(x)      GPIO_WriteBit(LED23_GPIO,LED23_GPIO_PIN,(BitAction)x)

#define LED24_RCC           RCC_AHB1Periph_GPIOE
#define LED24_GPIO          GPIOE
#define LED24_GPIO_PIN      GPIO_Pin_15
#define LED24_ONOFF(x)      GPIO_WriteBit(LED24_GPIO,LED24_GPIO_PIN,(BitAction)x)		

#define LED25_RCC           RCC_AHB1Periph_GPIOG
#define LED25_GPIO          GPIOG
#define LED25_GPIO_PIN      GPIO_Pin_6
#define LED25_ONOFF(x)      GPIO_WriteBit(LED25_GPIO,LED25_GPIO_PIN,(BitAction)x)

#define LED26_RCC           RCC_AHB1Periph_GPIOE
#define LED26_GPIO          GPIOE
#define LED26_GPIO_PIN      GPIO_Pin_14
#define LED26_ONOFF(x)      GPIO_WriteBit(LED26_GPIO,LED26_GPIO_PIN,(BitAction)x)

typedef struct{
  uint32_t     rcc;
  GPIO_TypeDef *gpio;
  uint16_t     pin;
}CCULED_LedGpio_Info;

void 	CCULED_LEDGpio_Init(void);
void  CCULED_LED_On(uint8_t i);
void  CCULED_LED_Off(uint8_t i);
void  CCULED_LED_Turn(uint8_t i);

#endif  //__CCU_LED_LEDCONTROL_H__

