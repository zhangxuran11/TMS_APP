#include "MTD_CPU_Key.h"
#include "delay.h"

MTDCPU_KeyGpio_Info MTDCPU_Key_info[MTDCPU_KEY_NUMBER] = {
    {MTDCPU_KEY_S1_RCC,MTDCPU_KEY_S1_GPIO,MTDCPU_KEY_S1_GPIO_PIN},
    {MTDCPU_KEY_S2_RCC,MTDCPU_KEY_S2_GPIO,MTDCPU_KEY_S2_GPIO_PIN},
    {MTDCPU_KEY_S3_RCC,MTDCPU_KEY_S3_GPIO,MTDCPU_KEY_S3_GPIO_PIN},
    {MTDCPU_KEY_S4_RCC,MTDCPU_KEY_S4_GPIO,MTDCPU_KEY_S4_GPIO_PIN},
};

void 	MTDCPU_KEYGpio_Init(void)
{
	uint8_t i;
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	for(i=0;i<MTDCPU_KEY_NUMBER;i++)
	{
		RCC_AHB1PeriphClockCmd( MTDCPU_Key_info[i].rcc , ENABLE); 
		GPIO_InitStructure.GPIO_Pin = MTDCPU_Key_info[i].pin;
		GPIO_Init(MTDCPU_Key_info[i].gpio, &GPIO_InitStructure);
	}
}


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，K1按下
//2，K2按下
//3，K3按下 
//4，k4按下 

//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3>KEY4!!
uint8_t		MTDCPU_KEY_Scan(uint8_t mode)  		//按键扫描函数	
{
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(MTDCPU_KEY_S1_UPDOWM()==0||MTDCPU_KEY_S2_UPDOWM()==0||MTDCPU_KEY_S3_UPDOWM()==0||MTDCPU_KEY_S4_UPDOWM()==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(MTDCPU_KEY_S1_UPDOWM() == 0)					return 1;
		else if(MTDCPU_KEY_S2_UPDOWM() == 0)		return 2;
		else if(MTDCPU_KEY_S3_UPDOWM() == 0)		return 3;
		else if(MTDCPU_KEY_S4_UPDOWM() == 0)		return 4;
	}
	else if(MTDCPU_KEY_S1_UPDOWM()==1&&MTDCPU_KEY_S2_UPDOWM()==1&&MTDCPU_KEY_S3_UPDOWM()==1&&MTDCPU_KEY_S4_UPDOWM()==1)
		key_up=1; 	    
 	return 0;// 无按键按下
}


