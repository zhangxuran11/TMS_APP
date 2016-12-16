#include "MTD_MFD_CanAddres.h"

void MTDMFD_CanAdd_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 开启GPIO 的时钟  */
	RCC_AHB1PeriphClockCmd(MTDMFD_CANADD0_RCC | MTDMFD_CANADD3_RCC | MTDMFD_CANADD4_RCC ,ENABLE);		//因为所有的引脚都使用的CPIOF
			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	//GPIO_PuPd_UP;				//W H H 

	GPIO_InitStructure.GPIO_Pin = MTDMFD_CANADD0_GPIO_PIN | MTDMFD_CANADD1_GPIO_PIN | MTDMFD_CANADD2_GPIO_PIN | MTDMFD_CANADD7_GPIO_PIN  ;			
	GPIO_Init(MTDMFD_CANADD0_GPIO, &GPIO_InitStructure);		//都是用GPIOC
	
	GPIO_InitStructure.GPIO_Pin = MTDMFD_CANADD3_GPIO_PIN;
	GPIO_Init(MTDMFD_CANADD3_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MTDMFD_CANADD4_GPIO_PIN | MTDMFD_CANADD5_GPIO_PIN | MTDMFD_CANADD6_GPIO_PIN ;
	GPIO_Init(MTDMFD_CANADD4_GPIO, &GPIO_InitStructure);	
}


/*
WHH 2016-11-17
1.MFD旋钮有红色 和 黄色之分。
    红色的旋钮 与 黄色旋钮的取值正好相反。（注意 ！！！）
	红色 return  vale；
	黄色 return ~vale;	
*/
uint8_t MTDMFD_CanAddVal(void)
{
	uint8_t  vale=0;
	vale = (MTDMFD_CANADD4_READ() <<7) |(MTDMFD_CANADD5_READ() <<6) |(MTDMFD_CANADD6_READ() <<5) | (MTDMFD_CANADD7_READ() <<4)
				|(MTDMFD_CANADD0_READ() <<3) | (MTDMFD_CANADD1_READ() <<2)| (MTDMFD_CANADD2_READ() <<1)|(MTDMFD_CANADD3_READ() <<0)	  ;
	
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


