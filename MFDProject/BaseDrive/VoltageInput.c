#include "VoltageInput.h"
#include "stm32f4xx.h"
#include "uart.h"
#include "bsp_os.h"

Voltage_Gpio_Info Vol_110VGpio_info[VOL110V_NUMBER]={
    {VOLTAGE_110V_DI1_RCC,VOLTAGE_110V_DI1_GPIO,VOLTAGE_110V_DI1_GPIO_PIN},
		{VOLTAGE_110V_DI2_RCC,VOLTAGE_110V_DI2_GPIO,VOLTAGE_110V_DI2_GPIO_PIN},
    {VOLTAGE_110V_DI3_RCC,VOLTAGE_110V_DI3_GPIO,VOLTAGE_110V_DI3_GPIO_PIN},
    {VOLTAGE_110V_DI4_RCC,VOLTAGE_110V_DI4_GPIO,VOLTAGE_110V_DI4_GPIO_PIN},		
    {VOLTAGE_110V_DI5_RCC,VOLTAGE_110V_DI5_GPIO,VOLTAGE_110V_DI5_GPIO_PIN},		
    {VOLTAGE_110V_DI6_RCC,VOLTAGE_110V_DI6_GPIO,VOLTAGE_110V_DI6_GPIO_PIN},		
    {VOLTAGE_110V_DI7_RCC,VOLTAGE_110V_DI7_GPIO,VOLTAGE_110V_DI7_GPIO_PIN},		
    {VOLTAGE_110V_DI8_RCC,VOLTAGE_110V_DI8_GPIO,VOLTAGE_110V_DI8_GPIO_PIN},		
    {VOLTAGE_110V_DI9_RCC,VOLTAGE_110V_DI9_GPIO,VOLTAGE_110V_DI9_GPIO_PIN},		
    {VOLTAGE_110V_DI10_RCC,VOLTAGE_110V_DI10_GPIO,VOLTAGE_110V_DI10_GPIO_PIN},		
    {VOLTAGE_110V_DI11_RCC,VOLTAGE_110V_DI11_GPIO,VOLTAGE_110V_DI11_GPIO_PIN},
    {VOLTAGE_110V_DI12_RCC,VOLTAGE_110V_DI12_GPIO,VOLTAGE_110V_DI12_GPIO_PIN},		
};

Voltage_Gpio_Info Vol_24VGpio_info[VOL24V_NUMBER]=
{
    {VOLTAGE_24V_DI1_RCC,VOLTAGE_24V_DI1_GPIO,VOLTAGE_24V_DI1_GPIO_PIN},
		{VOLTAGE_24V_DI2_RCC,VOLTAGE_24V_DI2_GPIO,VOLTAGE_24V_DI2_GPIO_PIN},
    {VOLTAGE_24V_DI3_RCC,VOLTAGE_24V_DI3_GPIO,VOLTAGE_24V_DI3_GPIO_PIN},
    {VOLTAGE_24V_DI4_RCC,VOLTAGE_24V_DI4_GPIO,VOLTAGE_24V_DI4_GPIO_PIN},	
};


/******************************************************************************************
*函数名称：void Vol_110VGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：KEY初始化
*******************************************************************************************/
void Vol_110VGpio_Init(void)
{
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    for(i=0;i<VOL110V_NUMBER;i++)
    {
        RCC_AHB1PeriphClockCmd( Vol_110VGpio_info[i].rcc , ENABLE); 
        GPIO_InitStructure.GPIO_Pin = Vol_110VGpio_info[i].pin;
        GPIO_Init(Vol_110VGpio_info[i].gpio, &GPIO_InitStructure);
    }
}


/******************************************************************************************
*函数名称：void Vol_24VGpio_Init(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：KEY初始化
*******************************************************************************************/
void Vol_24VGpio_Init(void)
{
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for(i=0;i<VOL24V_NUMBER;i++)
    {
        RCC_AHB1PeriphClockCmd( Vol_24VGpio_info[i].rcc , ENABLE); 
        GPIO_InitStructure.GPIO_Pin = Vol_24VGpio_info[i].pin;
        GPIO_Init(Vol_24VGpio_info[i].gpio, &GPIO_InitStructure);
    }
}

void Vol_110VRead_Scan(u8* DIBuf)
{	 
	u8 i,read;
	u8 V110Buf1[VOL110V_NUMBER],V110Buf2[VOL110V_NUMBER];
	for(i=0 ;i<VOL110V_NUMBER;i++)
	{	
		read=GPIO_ReadInputDataBit(Vol_110VGpio_info[i].gpio,Vol_110VGpio_info[i].pin);
		//DIBuf[i+1]= read;
		V110Buf1[i] = read;
	}
	BSP_OS_TimeDlyMs(10);   //延时10ms滤波
	
	for(i=0 ;i<VOL110V_NUMBER;i++)
	{	
		read=GPIO_ReadInputDataBit(Vol_110VGpio_info[i].gpio,Vol_110VGpio_info[i].pin);
		//DIBuf[i+1]= read;
		V110Buf2[i] = read;
	}		
	
	for(i=0 ;i<VOL110V_NUMBER;i++)
	{	
		if((V110Buf1[i] ==1)&&(V110Buf2[i] ==1))
		{
			DIBuf[i+1]= 1;
		}
		else
		{
			DIBuf[i+1]= 0;	
		}
	}			
}


void Vol_24VRead_Scan(u8* DIBuf)
{	 
	u8 i,read;
	u8 V24Buf1[VOL24V_NUMBER],V24Buf2[VOL24V_NUMBER];
	for(i=0 ;i<VOL24V_NUMBER;i++)
	{	
		read=GPIO_ReadInputDataBit(Vol_24VGpio_info[i].gpio,Vol_24VGpio_info[i].pin);
		//DIBuf[i+1]= read;
		V24Buf1[i]=read;
	}

	BSP_OS_TimeDlyMs(3);   //延时10ms滤波
	
	for(i=0 ;i<VOL24V_NUMBER;i++)
	{	
		read=GPIO_ReadInputDataBit(Vol_24VGpio_info[i].gpio,Vol_24VGpio_info[i].pin);
		//DIBuf[i+1]= read;
		V24Buf2[i]=read;
	}	

	for(i=0 ;i<VOL24V_NUMBER;i++)
	{	
		if((V24Buf1[i] ==1)&&(V24Buf2[i] ==1))
		{
			DIBuf[i+1]= 1;
		}
		else
		{
			DIBuf[i+1]= 0;	
		}
	}		
}

