#include "CCU_MasterSlave_Led.h"

void 	CCUMasterSlaveGpio_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
     
		RCC_AHB1PeriphClockCmd( MASTER_LED3_RCC | SLAVE_LED4_RCC , ENABLE); 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_InitStructure.GPIO_Pin = MASTER_LED3_GPIO_PIN;
		GPIO_Init(MASTER_LED3_GPIO, &GPIO_InitStructure);
			
		GPIO_InitStructure.GPIO_Pin = SLAVE_LED4_GPIO_PIN;
		GPIO_Init(SLAVE_LED4_GPIO,&GPIO_InitStructure);
		
		MASTER_LED3_ONOFF(1);			//  W H H ccu为高电平熄灭，低电平点亮
		SLAVE_LED4_ONOFF(1);
}

void  CCUMasterSlave_On (uint8_t LedNum)
{
	/*ccu 为低电平点亮*/
	if(LedNum == MASTER_LED3)
	{
		GPIO_ResetBits(MASTER_LED3_GPIO,MASTER_LED3_GPIO_PIN);     //点亮指定LED灯    
	}
	else if(LedNum == SLAVE_LED4)
	{
		GPIO_ResetBits(SLAVE_LED4_GPIO,SLAVE_LED4_GPIO_PIN);     //点亮指定LED灯    
	}
}

void  CCUMasterSlave_Off (uint8_t LedNum)
{
	/*ccu为高电平熄灭*/
	if(LedNum == MASTER_LED3)
	{
		GPIO_SetBits(MASTER_LED3_GPIO,MASTER_LED3_GPIO_PIN);     //点亮指定LED灯    
	}
	else if(LedNum == SLAVE_LED4)
	{
		GPIO_SetBits(SLAVE_LED4_GPIO,SLAVE_LED4_GPIO_PIN);     //点亮指定LED灯    
	}
}

void  CCUMasterSlave_Turn (uint8_t LedNum)
{
	/*ccu为高电平熄灭*/
	if(LedNum == MASTER_LED3)
	{
		GPIO_WriteBit(MASTER_LED3_GPIO, MASTER_LED3_GPIO_PIN,(BitAction)(1-(GPIO_ReadOutputDataBit(MASTER_LED3_GPIO, MASTER_LED3_GPIO_PIN))));              
	}
	else if(LedNum == SLAVE_LED4)
	{
		GPIO_WriteBit(SLAVE_LED4_GPIO, SLAVE_LED4_GPIO_PIN,(BitAction)(1-(GPIO_ReadOutputDataBit(SLAVE_LED4_GPIO, SLAVE_LED4_GPIO_PIN))));                 
	}
}
