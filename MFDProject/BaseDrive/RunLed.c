#include "RunLed.h"


void RunLedGpio_Init(void)
{
//    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;
     
		RCC_AHB1PeriphClockCmd( RUNLED_RCC , ENABLE); 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_InitStructure.GPIO_Pin = RUNLED_GPIO_PIN;
		GPIO_Init(RUNLED_GPIO, &GPIO_InitStructure);
			
		RUNLED_ONOFF(0);				//中继器
//		RUNLED_ONOFF(1);			//  W H H ccu为高电平熄灭，低电平点亮

}


void  RunLed_On (void)
{	/*ccu 为低电平点亮*/
//    GPIO_ResetBits(RUNLED_GPIO,RUNLED_GPIO_PIN);                                     //点亮指定LED灯       

		/*中继器为高电平点亮*/
	    GPIO_SetBits(RUNLED_GPIO,RUNLED_GPIO_PIN);                 //熄灭指定LED灯  
}

void  RunLed_Off (void )
{
		/*ccu为高电平熄灭*/
//    GPIO_SetBits(RUNLED_GPIO,RUNLED_GPIO_PIN);                 //熄灭指定LED灯       

			/*中继器为低电平点亮*/
	    GPIO_ResetBits(RUNLED_GPIO,RUNLED_GPIO_PIN);                 //熄灭指定LED灯  
}


void  RunLed_Turn (void)
{
   
	GPIO_WriteBit(RUNLED_GPIO, RUNLED_GPIO_PIN,(BitAction)(1-(GPIO_ReadOutputDataBit(RUNLED_GPIO, RUNLED_GPIO_PIN))));                                            //组合并写入                  
}

