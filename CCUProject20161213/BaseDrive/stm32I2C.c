#include "stm32I2C.h"
#include "stm32f4xx.h"
#include "os.h"
#include "Uart.h"

/*
STM32 I2C接口设计上的一些缺陷和如何避开这些缺陷的推荐程序模型：
（1）把I2C的中断优先级提升到最高
（2）把发送多于2个字节的发送与接收封装成利用DMA收发的函数，而把对某I2C设备接收和发送一个字节的函数单独封装为一个POLLING （轮询）函数。
（3）在寻址某一I2C DEVICE时要先CHECK I2C BUS 是否BUSY，如果忙，则等待指定时间，如果还是忙就说明I2C BUS 挂了
（原因99.9%是由于我们的I2C通信时序并不十分尊守I2C规约，或者我们所封装的I2C通信模块没有加上防守代码（出错恢复代码）），
这时要调用一个专门的用于通知 I2C BUS上的所有device，让他们结束当前内部的工作，重新准备好（下雨了，收衣服啦）。如下面的我的I2C模块的FUN 切片：

*/

__IO uint32_t I2C_Timeout = I2C_TIMEOUT;

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
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
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

I2C_Status STM32_I2C_WriteByte(uint8_t* pBuffer, uint8_t WriteAddr)
{
		
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
	/*调度器上锁*/
		I2C_Timeout = I2C_TIMEOUT;
		while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY))		//W H H
		{
			if((I2C_Timeout--) ==0)
			{
				#ifdef DEBUG_PRINTF
					debugprintf("Wirte I2C Bus Busy!\r\n");
				#endif
				return I2C_FAIL;
		  }
		}	

		I2C_AcknowledgeConfig(RTC_I2C, ENABLE);		/*允许1字节1应答模式*/		//W H H
	
		I2C_GenerateSTART(RTC_I2C,ENABLE);				//产生起始条件
	
		I2C_Timeout = I2C_TIMEOUT;
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT))	//等待ACK/*EV5,主模式*/
		{
			if((I2C_Timeout--) ==0)
			{
				#ifdef DEBUG_PRINTF
					debugprintf("Write I2C master mode error!\r\n");
				#endif				
				return I2C_FAIL;
		  }
		}
			
		I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7,I2C_Direction_Transmitter);	//向设备发送设备地址
		
		I2C_Timeout = I2C_TIMEOUT;
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//等待ACK			//W H H 如果没有i2c设备响应，程序会卡死到这里
		{
			if((I2C_Timeout--) ==0)
			{
				#ifdef DEBUG_PRINTF
					debugprintf("Write I2C master transmit mode error!\r\n");
				#endif					
				return I2C_FAIL;
		  }		
		}
		
		I2C_SendData(RTC_I2C, WriteAddr);//发送写入的寄存器地址
		
		I2C_Timeout = I2C_TIMEOUT;
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))	//等待ACK	/*数据已发送*/
		{
			if((I2C_Timeout--) ==0)
			{
				#ifdef DEBUG_PRINTF
					debugprintf("Write I2C Master Transmit Error!\r\n");
				#endif					
				return I2C_FAIL;
		  }				
		}
	
		I2C_SendData(RTC_I2C, *pBuffer);	//发送数据
		
		I2C_Timeout = I2C_TIMEOUT;
		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))	//发送完成
		{
			if((I2C_Timeout--) ==0)
			{
				return I2C_FAIL;
		  }				
		}
	
		I2C_GenerateSTOP(RTC_I2C, ENABLE);	//产生结束信号
		
		//	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/

		return I2C_OK;
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
	
  	I2C_Cmd(RTC_I2C, ENABLE);			/*W H H  这里不一样  注意*/ /* Clear EV6 by setting again the PE bit */
	
  	I2C_SendData(RTC_I2C, ReadAddr); 	//发送读取的寄存器地址
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	/*数据已发送*/
	
  	I2C_GenerateSTART(RTC_I2C, ENABLE);
	
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT));
		
  	I2C_Send7bitAddress(RTC_I2C, I2C1_SLAVE_ADDRESS7, I2C_Direction_Receiver);
		
  	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
    I2C_AcknowledgeConfig(RTC_I2C, DISABLE);	// 最后一位后要关闭应答的
		
    I2C_GenerateSTOP(RTC_I2C, ENABLE);			//发送停止位
		
    while(!(I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))); 
		
    temp = I2C_ReceiveData(RTC_I2C);
		
  	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);	//再次允许应答模式
		
//	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/
		
    return temp;
}

/*
	向i2c中写入数据
*/
I2C_Status STM32_I2C_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
//	CPU_SR_ALLOC();			//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断

	I2C_Timeout = I2C_TIMEOUT;
	/*等待IIC总线空闲*/
	while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY))
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Writing to the I2C bus is busy!\r\n");
			#endif			
			return I2C_FAIL;
		}		
	}		

	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);		//W H H
	
	/* 发送起始位 */
  I2C_GenerateSTART(RTC_I2C, ENABLE);

	I2C_Timeout = I2C_TIMEOUT;	
  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT))		/*EV5,主模式*/
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Write I2C master mode error!\r\n");
			#endif			
			return I2C_FAIL;
		}				
	}

	/*发送器件地址(写)*/
  I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Transmitter);
 
	I2C_Timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))	 /*!< Test on EV6 and clear it */
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Write I2C master transmit mode error!\r\n");
			#endif				
			return I2C_FAIL;
		}			
	}

	I2C_SendData(RTC_I2C, WriteAddr);		//发送写地址

	I2C_Timeout = I2C_TIMEOUT;  
	while (!I2C_CheckEvent(RTC_I2C, /*I2C_EVENT_MASTER_BYTE_TRANSMITTED*/I2C_EVENT_MASTER_BYTE_TRANSMITTING))	/*W H H 修改为EV8模式，数据已发送*/
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Write I2C send address error!\r\n");
			#endif				
			return I2C_FAIL;
		}	
	}
	
	while(NumByteToWrite--)
	{
		/* 写一个字节*/
	  I2C_SendData(RTC_I2C, *pBuffer); 
		
		I2C_Timeout = I2C_TIMEOUT;
	  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  //故这种模式只能写一字节  
		{
			if((I2C_Timeout--) ==0)
			{
				#ifdef DEBUG_PRINTF
					debugprintf("Write I2C data error!\r\n");
				#endif					
				return I2C_FAIL;
			}				
		}
		
		pBuffer++;
	}
	/* 停止位*/
	I2C_GenerateSTOP(RTC_I2C, ENABLE);

//	OS_CRITICAL_EXIT();	//退出临界区
	return I2C_OK;
}


/*
	读取i2c数据
*/
I2C_Status STM32_I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead)
{
//	CPU_SR_ALLOC();			//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断

	I2C_Timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BUSY))		/*等待IIC总线空闲*/
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Read I2C Bus Busy!\r\n");
			#endif					
			return I2C_FAIL;
		}	
	}		
	
  I2C_GenerateSTART(RTC_I2C, ENABLE);					/* 发送起始位 */
  
	I2C_Timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT))		/*EV5,选择主模式*/
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Read I2C master mode error!\r\n");
			#endif				
			return I2C_FAIL;
		}		
	}
		
	/*发送i2c器件地址(写)*/
  I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Transmitter);
  
	I2C_Timeout = I2C_TIMEOUT;
  while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))	/*EV6*/
	{
		if((I2C_Timeout--) ==0)
		{
			return I2C_FAIL;
		}			
	}
	
	I2C_SendData(RTC_I2C, ReadAddr);	//发送读的地址
	
	/*
		W H H 自己的配置
	*/
	//	I2C_Timeout = I2C_TIMEOUT;
	//  while (!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))		/*数据已发送*/
	//  {
	//		if((I2C_Timeout--) ==0)
	//		{
	//			#ifdef DEBUG_PRINTF
	//				debugprintf("读I2C主发送模式错误!\r\n");
	//			#endif				
	//			return I2C_FAIL;
	//		}		
	//	}
	
	
	/*
		疑问？？  官方的配置
	*/
	 /*!< Test on EV8 and clear it */
	I2C_Timeout = I2C_TIMEOUT;
  while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_BTF) == RESET)
  {
    if((I2C_Timeout--) == 0) return I2C_FAIL;
  }
	
	I2C_GenerateSTART(RTC_I2C, ENABLE);  	/*起始位*/
	
	I2C_Timeout = I2C_TIMEOUT;
	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT))		/*EV5*/
	{
		if((I2C_Timeout--) ==0)
		{
			#ifdef DEBUG_PRINTF
				debugprintf("Read I2C master selection error!\r\n");
			#endif				
			return I2C_FAIL;
		}			
	}

	I2C_Send7bitAddress(RTC_I2C,I2C1_SLAVE_ADDRESS7, I2C_Direction_Receiver);	/*发送器件地址(读)*/
	
	//	/*
	//		W H H 自己的配置,如果使用官方配置，这句话一定不能使用。
	//	*/	
	//	I2C_Timeout = I2C_TIMEOUT;
	//	while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))	/*EV6*/
	//	{
	//		if((I2C_Timeout--) ==0)
	//		{
	//			#ifdef DEBUG_PRINTF
	//				debugprintf("读I2C接受错误!\r\n");
	//			#endif				
	//			return I2C_FAIL;
	//		}				
	//	}

	/* If number of data to be read is 1, then DMA couldn't be used */
  /* One Byte Master Reception procedure (POLLING) ---------------------------*/
	while(NumByteToRead)
	{
		/*
			W H H 自己的配置
		*/
	//		if(1==NumByteToRead)
	//		{
	//			I2C_AcknowledgeConfig(RTC_I2C, DISABLE);	//最后一位后要关闭应答的
	//			
	//    	I2C_GenerateSTOP(RTC_I2C, ENABLE);			//发送停止位
	//		}
	//		
	//		I2C_Timeout = I2C_TIMEOUT;
	//		while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) /*W H H 不一样的地方 *//* EV7 */
	//		{
	//			if((I2C_Timeout--) ==0)
	//			{
	//				#ifdef DEBUG_PRINTF
	//					debugprintf("读I2C数据错误!\r\n");
	//				#endif					
	//				return I2C_FAIL;
	//			}			
	//		}
	//		
	//		*pBuffer = I2C_ReceiveData(RTC_I2C);
	//		pBuffer++;
	//		NumByteToRead--;
		
		
	/*
		 官方的配置
	*/
		/*官网给的读取一个字节的函数*/
		if ((uint16_t)(NumByteToRead) < 2)
		{
			I2C_Timeout = I2C_TIMEOUT;
			while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_ADDR) == RESET)
			{
				if((I2C_Timeout--) == 0)
				{
					return I2C_FAIL;
				}
			}

			/*!< Disable Acknowledgement */
			I2C_AcknowledgeConfig(RTC_I2C, DISABLE);   
			
			/* Clear ADDR register by reading SR1 then SR2 register (SR1 has already been read) */
			(void)RTC_I2C->SR2;
			
			/*!< Send STOP Condition */
			I2C_GenerateSTOP(RTC_I2C, ENABLE);
			
			/* Wait for the byte to be received */
			I2C_Timeout = I2C_TIMEOUT;
			while(I2C_GetFlagStatus(RTC_I2C, I2C_FLAG_RXNE) == RESET)
			{
				if((I2C_Timeout--) == 0) 
				{
					return I2C_FAIL;
				}
			}
			
			/*!< Read the byte received from the EEPROM */
			*pBuffer = I2C_ReceiveData(RTC_I2C);
			pBuffer++;
			/*!< Decrement the read bytes counter */
			(NumByteToRead)--;        
			
			/* Wait to make sure that STOP control bit has been cleared */
			I2C_Timeout = I2C_TIMEOUT;
			while(RTC_I2C->CR1 & I2C_CR1_STOP)
			{
				if((I2C_Timeout--) == 0) 
				{
					return I2C_FAIL;
				}
			}  
			
			/*!< Re-Enable Acknowledgement to be ready for another reception */
			I2C_AcknowledgeConfig(RTC_I2C, ENABLE); 				
		}
		
		else
		{
			I2C_Timeout = I2C_TIMEOUT;
			while(!I2C_CheckEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)) /*W H H 不一样的地方 *//* EV7 */
			{
				if((I2C_Timeout--) ==0)
				{
					#ifdef DEBUG_PRINTF
						debugprintf("Read I2C data error!\r\n");
					#endif					
					return I2C_FAIL;
				}			
			}
			
			*pBuffer = I2C_ReceiveData(RTC_I2C);
			pBuffer++;
			NumByteToRead--;
		}
	}
 
	I2C_AcknowledgeConfig(RTC_I2C, ENABLE);  //再次允许应答模式
//	OS_CRITICAL_EXIT();	//退出临界区
	return I2C_OK;
}
