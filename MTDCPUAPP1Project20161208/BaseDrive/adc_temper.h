#include "stm32f4xx.h"
#include "uart.h"
#include "delay.h"

#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C)


void ADC1_CH16_Config(void);
short Get_Temprate(void);
u32 Get_Adc_Average(u8 number);

void ADC_Temper_Test(void);
