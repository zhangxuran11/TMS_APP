#include "MTD_CPU_CANLED.h"

MTD_CPU_CANLED_LedGpio_Info MTD_CPU_CANLED_info[MTDCPU_CANLED_NUMBER]={
    {CAN1_TX_RCC,CAN1_TX_GPIO,CAN1_TX_GPIO_PIN},
    {CAN1_RX_RCC,CAN1_RX_GPIO,CAN1_RX_GPIO_PIN},
    {CAN2_TX_RCC,CAN2_TX_GPIO,CAN2_TX_GPIO_PIN},
    {CAN2_RX_RCC,CAN2_RX_GPIO,CAN2_RX_GPIO_PIN},
	
};



/******************************************************************************************
*函数名称：void MTD_CPU_CANLED_LEDGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：LED的GPIO初始化
*******************************************************************************************/
void 	MTD_CPU_CANLED_Gpio_Init(void)
{
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for(i=0;i<MTDCPU_CANLED_NUMBER;i++)                                    
    {
        RCC_AHB1PeriphClockCmd( MTD_CPU_CANLED_info[i].rcc , ENABLE); 
        GPIO_InitStructure.GPIO_Pin = MTD_CPU_CANLED_info[i].pin;
        GPIO_Init(MTD_CPU_CANLED_info[i].gpio, &GPIO_InitStructure);
			
        GPIO_WriteBit(MTD_CPU_CANLED_info[i].gpio,MTD_CPU_CANLED_info[i].pin,Bit_RESET);	//LED All Off
    }
}

/********************************************************************************************
*函数名称：void MTD_CPU_CANLED_LED_On(uint8_t i)
*入口参数：无
*出口参数：无
*功能说明：点亮指定LED灯
*******************************************************************************************/
void  MTD_CPU_CANLED_On(uint8_t i)
{
  //GPIO_SetBits(MTD_CPU_CANLED_info[i].gpio,MTD_CPU_CANLED_info[i].pin);          //点亮指定LED灯  
	GPIO_ResetBits(MTD_CPU_CANLED_info[i].gpio,MTD_CPU_CANLED_info[i].pin);    	   //点亮指定LED灯 	 2代板卡低电平点亮	
}

/********************************************************************************************
*函数名称：void MTD_CPU_CANLED_LED_Off(uint8_t i)
*入口参数：无
*出口参数：无
*功能说明：熄灭指定LED灯
*******************************************************************************************/
void  MTD_CPU_CANLED_Off(uint8_t i)
{
		//GPIO_ResetBits(MTD_CPU_CANLED_info[i].gpio,MTD_CPU_CANLED_info[i].pin);    	   //关闭指定LED灯 
		GPIO_SetBits(MTD_CPU_CANLED_info[i].gpio,MTD_CPU_CANLED_info[i].pin);          //关闭指定LED灯  	 2代板卡高电平熄灭	
}

/********************************************************************************************
*函数名称：void MTD_CPU_CANLED_LED_Turn(uint16_t GPIO_Pin)
*入口参数：无
*出口参数：无
*功能说明：翻转指定LED灯状态
*******************************************************************************************/
void  MTD_CPU_CANLED_Turn(uint8_t i)
{
		GPIO_WriteBit(MTD_CPU_CANLED_info[i].gpio, MTD_CPU_CANLED_info[i].pin,(BitAction)(1-(GPIO_ReadOutputDataBit(MTD_CPU_CANLED_info[i].gpio, MTD_CPU_CANLED_info[i].pin))));                 
}

