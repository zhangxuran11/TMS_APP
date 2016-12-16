#include "CCU_LED_LedControl.h"

CCULED_LedGpio_Info CCULED_Led_info[CCULED_LED_NUMBER]={
    {LED1_RCC,LED1_GPIO,LED1_GPIO_PIN},
    {LED2_RCC,LED2_GPIO,LED2_GPIO_PIN},
    {LED3_RCC,LED3_GPIO,LED3_GPIO_PIN},
    {LED4_RCC,LED4_GPIO,LED4_GPIO_PIN},
		 
    {LED5_RCC,LED5_GPIO,LED5_GPIO_PIN},
    {LED6_RCC,LED6_GPIO,LED6_GPIO_PIN},
    {LED7_RCC,LED7_GPIO,LED7_GPIO_PIN},
    {LED8_RCC,LED8_GPIO,LED8_GPIO_PIN},
	
		{LED9_RCC,LED9_GPIO,LED9_GPIO_PIN},
    {LED10_RCC,LED10_GPIO,LED10_GPIO_PIN},
    {LED11_RCC,LED11_GPIO,LED11_GPIO_PIN},
    {LED12_RCC,LED12_GPIO,LED12_GPIO_PIN},
		
    {LED13_RCC,LED13_GPIO,LED13_GPIO_PIN},
    {LED14_RCC,LED14_GPIO,LED14_GPIO_PIN},
    {LED15_RCC,LED15_GPIO,LED15_GPIO_PIN},
    {LED16_RCC,LED16_GPIO,LED16_GPIO_PIN},
					
		{LED17_RCC,LED17_GPIO,LED17_GPIO_PIN},
    {LED18_RCC,LED18_GPIO,LED18_GPIO_PIN},
    {LED19_RCC,LED19_GPIO,LED19_GPIO_PIN},
    {LED20_RCC,LED20_GPIO,LED20_GPIO_PIN},
		
    {LED21_RCC,LED21_GPIO,LED21_GPIO_PIN},
    {LED22_RCC,LED22_GPIO,LED22_GPIO_PIN},
    {LED23_RCC,LED23_GPIO,LED23_GPIO_PIN},
    {LED24_RCC,LED24_GPIO,LED24_GPIO_PIN},
		
    {LED25_RCC,LED25_GPIO,LED25_GPIO_PIN},
    {LED26_RCC,LED26_GPIO,LED26_GPIO_PIN},
	
};


/******************************************************************************************
*函数名称：void CCULED_LEDGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：LED的GPIO初始化
*******************************************************************************************/
void 	CCULED_LEDGpio_Init(void)
{
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for(i=0;i<CCULED_LED_NUMBER;i++)                                    
    {
        RCC_AHB1PeriphClockCmd( CCULED_Led_info[i].rcc , ENABLE); 
        GPIO_InitStructure.GPIO_Pin = CCULED_Led_info[i].pin;
        GPIO_Init(CCULED_Led_info[i].gpio, &GPIO_InitStructure);
			
        GPIO_WriteBit(CCULED_Led_info[i].gpio,CCULED_Led_info[i].pin,Bit_RESET);	//LED All Off
    }
}

/********************************************************************************************
*函数名称：void CCULED_LED_On(uint8_t i)
*入口参数：无
*出口参数：无
*功能说明：点亮指定LED灯
*******************************************************************************************/
void  CCULED_LED_On(uint8_t i)
{
    GPIO_SetBits(CCULED_Led_info[i].gpio,CCULED_Led_info[i].pin);          //点亮指定LED灯  		
}

/********************************************************************************************
*函数名称：void CCULED_LED_Off(uint8_t i)
*入口参数：无
*出口参数：无
*功能说明：熄灭指定LED灯
*******************************************************************************************/
void  CCULED_LED_Off(uint8_t i)
{
		GPIO_ResetBits(CCULED_Led_info[i].gpio,CCULED_Led_info[i].pin);    	   //关闭指定LED灯          
}

/********************************************************************************************
*函数名称：void CCULED_LED_Turn(uint16_t GPIO_Pin)
*入口参数：无
*出口参数：无
*功能说明：翻转指定LED灯状态
*******************************************************************************************/
void  CCULED_LED_Turn(uint8_t i)
{
		GPIO_WriteBit(CCULED_Led_info[i].gpio, CCULED_Led_info[i].pin,(BitAction)(1-(GPIO_ReadOutputDataBit(CCULED_Led_info[i].gpio, CCULED_Led_info[i].pin))));                 
}


