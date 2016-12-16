#include "Buzzer.h"

void Buzzer_Func_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(BUZZER_RCC, ENABLE);//使能GPIOA时钟
  
  //初始化蜂鸣器对应引脚GPIOA8
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);//初始化GPIOA
	
//  GPIO_ResetBits(BUZZER_GPIO,BUZZER_GPIO_PIN);  //蜂鸣器对应引脚GPIOA8拉低， 
		Buzzer_Func_Off();
}

void Buzzer_Func_On(void)   //蜂鸣器高电平导通
{
	BUZZER_ONOFF(1);
}


void Buzzer_Func_Off(void)   //蜂鸣器高电平导通
{
	BUZZER_ONOFF(0);
}

