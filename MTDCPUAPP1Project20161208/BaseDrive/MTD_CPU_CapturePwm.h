#ifndef  __MTD_CPU_CAPTUREPWM_H__
#define  __MTD_CPU_CAPTUREPWM_H__

#include "stm32f4xx.h"

#define MTDCPU_PWM1IN_RCC					RCC_AHB1Periph_GPIOA
#define MTDCPU_PWM1IN_GPIO        GPIOA
#define MTDCPU_PWM1IN_GPIO_PIN    GPIO_Pin_1

#define MTDCPU_PWM2IN_RCC					RCC_AHB1Periph_GPIOA
#define MTDCPU_PWM2IN_GPIO        GPIOA
#define MTDCPU_PWM2IN_GPIO_PIN    GPIO_Pin_0

void 	MTDCPU_PWM1IN_TIM2_CH2_Init(u32 arr,u16 psc);
void  MTDCPU_PWM2IN_TIM5_CH1_Init(u32 arr,u16 psc);
void PWM_Test_GPIO(void);

#endif  //__MTD_CPU_CAPTUREPWM_H__
