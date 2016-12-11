#include "stm32I2C.h"
#include "stm32f4xx.h"

void STM32_I2C_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	I2C_InitTypeDef  	I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RTC_I2C_RCC, ENABLE);			//使能I2C时钟线
	RCC_AHB1PeriphClockCmd(RTC_I2C_GPIO_RCC, ENABLE);	//使能I2C_SCL_GPIO_CLK and I2C_SDA_GPIO_CLK GPIO时钟线
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);	//使能系统时钟
	
	RCC_APB1PeriphResetCmd(RTC_I2C_RCC, ENABLE);	//复位 I2C IP 
		/* Release reset signal of I2C IP */
	RCC_APB1PeriphResetCmd(RTC_I2C_RCC, DISABLE);
	
	/*!< GPIO configuration */
	/* Connect PXx to I2C_SCL*/
	GPIO_PinAFConfig(RTC_I2C_PROT, RTC_I2C_SCL_GPIOSource, RTC_I2C_SCL_AF);
	/* Connect PXx to I2C_SDA*/
	GPIO_PinAFConfig(RTC_I2C_PROT, RTC_I2C_SDA_GPIOSource, RTC_I2C_SDA_AF);  
	
	/*!< Configure sEE_I2C pins: SCL */   
	GPIO_InitStructure.GPIO_Pin = RTC_I2C_SCL_GPIO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(RTC_I2C_PROT, &GPIO_InitStructure);
	
	/*!< Configure sEE_I2C pins: SDA */
	GPIO_InitStructure.GPIO_Pin = RTC_I2C_SDA_GPIO;
	GPIO_Init(RTC_I2C_PROT, &GPIO_InitStructure);
	
	
		/*!< I2C configuration */
	/* I2C configuration */
//	I2C_DeInit(RTC_I2C);	//复位使能i2c，关闭i2c复位
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;		//ack enable
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
	
	/* I2C Peripheral Enable */
	I2C_Cmd(RTC_I2C, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(RTC_I2C, &I2C_InitStructure);
    
	/* 允许1字节1应答模式*/
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);		//W H H
}

void STM32_I2C_WriteByte(uint8_t* pBuffer, uint8_t WriteAddr)
{
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
	/*调度器上锁*/
	
		while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY));		//W H H

		I2C_AcknowledgeConfig(RTC_I2C, ENABLE);		/*允许1字节1应答模式*/		//W H H
	
		I2C_GenerateSTART(RTC_I2C,ENABLE);		//产生起始条件
	
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));	//等待ACK/*EV5,主模式*/
		
		I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7,I2C_Direction_Transmitter);	//向设备发送设备地址
		
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//等待ACK
		
		I2C_SendData(RTC_I2C, WriteAddr);//发送写入的寄存器地址
	
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//等待ACK	/*数据已发送*/
	
		I2C_SendData(RTC_I2C, *pBuffer);	//发送数据
		
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));//发送完成
	
		I2C_GenerateSTOP(RTC_I2C, ENABLE);	//产生结束信号
		
//	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/

}

uint8_t STM32_I2C_ReadByte(uint8_t ReadAddr)
{  
		unsigned char temp; 	
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
	/*调度器上锁*/
	
		while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY));	 /*等待IIC总线空闲*/
	
		/*允许1字节1应答模式*/
		I2C_AcknowledgeConfig(RTC_I2C, ENABLE);	  //W H H
	
  	I2C_GenerateSTART(RTC_I2C, ENABLE);	/* 发送起始位 */
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));  /*EV5,主模式*/
	
  	I2C_Send7bitAddress(RTC_I2C, I2C1_SLAVE_ADDRESS7, I2C_Direction_Transmitter);  /*发送器件地址(写)*/
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));/* Test on EV6 and clear it */
	
  	I2C_Cmd(RTC_I2C, ENABLE);			/*W H H  这里不一样  注意*/
	
  	I2C_SendData(RTC_I2C, ReadAddr); 	//发送读取的寄存器地址
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	/*数据已发送*/
	
  	I2C_GenerateSTART(RTC_I2C, ENABLE);
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));
		
  	I2C_Send7bitAddress(RTC_I2C, I2C1_SLAVE_ADDRESS7, I2C_Direction_Receiver);
		
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) ;
		
    I2C_AcknowledgeConfig(RTC_I2C, DISABLE);	// 最后一位后要关闭应答的
		
    I2C_GenerateSTOP(RTC_I2C, ENABLE);			//发送停止位
		
    while(!(I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))); 
		
    temp = I2C_ReceiveData(RTC_I2C);
		
  	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);	//再次允许应答模式
		
//	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/
		
    return temp;
}

void STM32_I2C_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断

	/*等待IIC总线空闲*/
	while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY));		

	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);		//W H H
	
	/* 发送起始位 */
  I2C_GenerateSTART(RTC_I2C, ENABLE);
	
  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

	/*发送器件地址(写)*/
  I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Transmitter);
 
	while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(RTC_I2C, WriteAddr);	//发送写地址
  
	while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*数据已发送*/

	while(NumByteToWrite--)
	{
		/* 写一个字节*/
	  I2C_SendData(RTC_I2C, *pBuffer); 
		
	  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
		pBuffer++;
	}
	/* 停止位*/
	I2C_GenerateSTOP(RTC_I2C, ENABLE);

//	OS_CRITICAL_EXIT();	//退出临界区
}


void STM32_I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToWrite)
{
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断

	/*等待IIC总线空闲*/
	while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY));		
	
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);/*允许1字节1应答模式*/	//W H H 
	
  I2C_GenerateSTART(RTC_I2C, ENABLE);/* 发送起始位 */
  
  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

	/*发送器件地址(写)*/
  I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Transmitter);
  
  while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_Cmd(RTC_I2C, ENABLE);	//i2c使能
	
	I2C_SendData(RTC_I2C, ReadAddr);	//发送读的地址
	
  while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));/*数据已发送*/
    
	/*起始位*/
	I2C_GenerateSTART(RTC_I2C, ENABLE);
	
	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Receiver);	/*发送器件地址(读)24C01*/
	
	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(NumByteToWrite)
	{
		if(1==NumByteToWrite)
		{
			I2C_AcknowledgeConfig(RTC_I2C, DISABLE);	//最后一位后要关闭应答的
			
    	I2C_GenerateSTOP(RTC_I2C, ENABLE);			//发送停止位
		}

		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)); /*W H H 不一样的地方 */
		
		*pBuffer = I2C_ReceiveData(RTC_I2C);
		pBuffer++;
		NumByteToWrite--;
	}
 
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);  //再次允许应答模式
//	OS_CRITICAL_EXIT();	//退出临界区
}






