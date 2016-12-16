#ifndef  __BUZZER_H__
#define  __BUZZER_H__

#include "stm32f4xx.h"
#define BUZZER_RCC          	RCC_AHB1Periph_GPIOA    
#define BUZZER_GPIO          	GPIOA
#define BUZZER_GPIO_PIN       GPIO_Pin_8
#define BUZZER_ONOFF(x)       GPIO_WriteBit(BUZZER_GPIO,BUZZER_GPIO_PIN,(BitAction)x)

void Buzzer_Func_On(void) ;
void Buzzer_Func_Off(void);
void Buzzer_Func_Init(void);

#endif  //__BUZZER_H__

