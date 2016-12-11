#include "MTD_CPU_CanAddres.h"

void MTDCPU_CanAdd_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 开启GPIO 的时钟  */
	RCC_AHB1PeriphClockCmd(MTDCPU_CANADD0_RCC ,ENABLE);		//因为所有的引脚都使用的CPIOF
			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	//GPIO_PuPd_UP;				//W H H 

	GPIO_InitStructure.GPIO_Pin= MTDCPU_CANADD0_GPIO_PIN | MTDCPU_CANADD1_GPIO_PIN | MTDCPU_CANADD2_GPIO_PIN | MTDCPU_CANADD3_GPIO_PIN 
					| MTDCPU_CANADD4_GPIO_PIN |MTDCPU_CANADD5_GPIO_PIN | MTDCPU_CANADD6_GPIO_PIN | MTDCPU_CANADD7_GPIO_PIN ;					// 或者为0x303f
	
	GPIO_Init(MTDCPU_CANADD0_GPIO, &GPIO_InitStructure);		//都是用GPIOF
}


uint8_t MTDCPU_CanAddVal(void)
{
	uint8_t  vale=0;
	vale = (MTDCPU_CANADD4_READ() <<7) |(MTDCPU_CANADD5_READ() <<6) |(MTDCPU_CANADD6_READ() <<5) | (MTDCPU_CANADD7_READ() <<4)
				|(MTDCPU_CANADD0_READ() <<3) | (MTDCPU_CANADD1_READ() <<2)| (MTDCPU_CANADD2_READ() <<1)|(MTDCPU_CANADD3_READ() <<0)	  ;
	
	return ~vale;		//将读到的数据按位取反
	/*
	或者
	{
		uint16_t ReadValue;
		ReadValue = GPIO_ReadInputData(GPIOF);
		vale = (uint8_t)( (ReadValue >> 6 ) | (ReadValue & 0x3f) ) & 0xff ;
	}
	return vale;
	*/

}


