#include "CrystalTest.h"

void System_Clk_Output_Init(void)				//PC9
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//使能GPIOC的时钟

	  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_MCO);//打开GPIOC_Pin_9的MCO功能
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //选择管脚号
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz; //设置管脚的速度
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF; //设置管脚位复用功能
    GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//设置管脚位推完输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉电阻	
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化管脚

    RCC_MCO2Config(RCC_MCO2Source_SYSCLK,RCC_MCO2Div_2);//设置GPIOC_Pin_9输出系统时钟频率的二分频
}        

void HSE_Clk_Output_Init(void)				//PA8
{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOA的时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 	 //选择管脚号
    GPIO_InitStructure.GPIO_Speed =GPIO_Speed_100MHz;// 设置管脚的速度为100M
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;  //设置管脚位复用功能
    GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//设置管脚位推完输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉电阻
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化管脚

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_MCO);//打开GPIOA_Pin_9的MCO功能
	
    RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO2Div_1);//设置GPIOA_Pin_8输出外部高速晶振HSE的频率

}        
