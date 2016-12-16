#include "MTD_MFD_CurrentLoop.h"
#include "Uart.h"
#include "sys.h"
#include "bsp_os.h"
#include "main.h"
#include "BCUComPro.h"

u8 USART4_CL_RxBuffer[100]; 	//接收缓冲,最大100个字节
//u8 USART4_CL_TxBuffer[100]; 	//接收缓冲,最大100个字节
u8 USART4_CL_RxLength;   			//接收到的数据长度

#ifdef MTD_MFD_1_0_ENABLE	
void MTDMFD_CL_Gpio_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* 开启GPIO 的时钟  */
		RCC_AHB1PeriphClockCmd(MTDMFD_CLTX_RCC ,ENABLE);		//因为所有的引脚都使用的CPIOF
	
		/*TX*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = MTDMFD_CLTX_GPIO_PIN;
		
		GPIO_Init(MTDMFD_CLTX_GPIO, &GPIO_InitStructure);
	
		/*RX*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 	
		GPIO_InitStructure.GPIO_Pin = MTDMFD_CLRX_GPIO_PIN;
	
		GPIO_Init(MTDMFD_CLRX_GPIO, &GPIO_InitStructure);
}
#endif


/*版本 V2.0*/
#ifdef MTD_MFD_2_0_ENABLE

void MTDMFD_CL_UART_Gpio_Init(u32 BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	/* 开启GPIO_C的时钟 */
	RCC_AHB1PeriphClockCmd(MTDMFD_UARTx_RX_RCC | MTDMFD_UARTx_TX_RCC, ENABLE);
	/* 开启串口4的时钟 */
	RCC_APB1PeriphClockCmd(MTDMFD_USARTx_CLK, ENABLE);

	/*PC10,PC11 引脚复用到UART*/
	GPIO_PinAFConfig(MTDMFD_UARTx_TX_GPIO_PORT, MTDMFD_UARTx_TX_SOURCE, MTDMFD_UARTx_TX_AF);
	GPIO_PinAFConfig(MTDMFD_UARTx_RX_GPIO_PORT, MTDMFD_UARTx_RX_SOURCE, MTDMFD_UARTx_RX_AF);	
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin   = MTDMFD_UARTx_TX_GPIO_PIN;
	GPIO_Init(MTDMFD_UARTx_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = MTDMFD_UARTx_RX_GPIO_PIN;
	GPIO_Init(MTDMFD_UARTx_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate   = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;	//手册697页，如果使用奇偶校验，数据位为9位
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_Even;		//偶数校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(MTDMFD_USARTx, &USART_InitStructure);

	/* 使能串口4 */
	USART_Cmd(MTDMFD_USARTx, ENABLE);

	USART_ITConfig(MTDMFD_USARTx, USART_IT_RXNE, ENABLE);		//使能接受中断
	USART_ITConfig(MTDMFD_USARTx, USART_IT_PE, 	 ENABLE); 	//开启PE错误接收中断Bit 8PEIE: PE interrupt enable，奇偶校验错误中断位
	USART_ITConfig(MTDMFD_USARTx, USART_IT_ERR,  ENABLE);	  //CR2 开启ERR中断
	//USART_ITConfig(MTDMFD_USARTx, USART_IT_FE, 	 ENABLE);		//帧错误中断位
	
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(MTDMFD_USARTx, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */  
	
	/*uart4中断优先级的配置*/
  NVIC_InitStructure.NVIC_IRQChannel = MTDMFD_USARTx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;		//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;					//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

void MTDMFD_CL_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	//USART_ClearFlag(MTDMFD_USARTx, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */  
	
	for(t=0;t<len;t++)
	{  
		 while(RESET == USART_GetFlagStatus(MTDMFD_USARTx, USART_FLAG_TXE));				//等待发送缓冲区为空
			USART_SendData (MTDMFD_USARTx,buf[t]);                                    /* 发送字符                     */
		 while( USART_GetFlagStatus(MTDMFD_USARTx,USART_FLAG_TC)==RESET );         	/* 等待发送完成                 */
		 //USART_ClearFlag(MTDMFD_USARTx,USART_FLAG_TC);                           	/* 清除发送完成标识             */
	}
	
	USART4_CL_RxLength =0; //发送一帧数据，等待接受的时候，将数据长度清零
}
	
void MTDMFD_USARTx_IRQHandler(void)
{
	//OS_ERR err;
	static unsigned char StartFlag = 0;	
	//static u8 RecordDataLen=0;
	uint8_t RxChar;
	
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	

	if(USART_GetITStatus(MTDMFD_USARTx,USART_IT_PE)!= RESET || USART_GetITStatus(MTDMFD_USARTx,USART_IT_FE)!= RESET)		
	{
		g_Uart4FrameErrorFlag=1;		//将奇偶错误 帧错误 标志设置1
		
		USART_ClearITPendingBit(MTDMFD_USARTx,USART_IT_FE);			//清除帧错误标志
		USART_ClearITPendingBit(MTDMFD_USARTx,USART_IT_PE);			//清除奇偶错误标志
		
		printf("Uart4 奇偶校验错误!\r\n");		
	}
	
	 /* USART in Receiver mode */
  if (USART_GetITStatus(MTDMFD_USARTx, USART_IT_RXNE) != RESET)
  {
		if(USART4_CL_RxLength >=100 )
		{
			USART4_CL_RxLength =0;//暂时清0，循环接受
		}

		/* Read one byte from the receive data register */
		RxChar = USART_ReceiveData(MTDMFD_USARTx);	//读取接收到的数据
		USART4_CL_RxBuffer[USART4_CL_RxLength++] = RxChar;
		
		if(USART4_CL_RxBuffer[0] ==  BCU_NACK)		//如果接受的数据为NACK标志。因为在发送一帧数据的时候则将数据长度清零，接受的时候一定从USART4_CL_RxBuffer[0]开始接受
		{
			g_Uart4RxStartFlag = 1;  //	接受数据开始标志	
			g_Uart4NackFlag = 1;		//设置NACK标志
		}
		
		else		// 接受到正确数据
		{
			//if(RxChar == 0x02)  //表示接受的为开始数据
			if((USART4_CL_RxBuffer[0] ==  0x02 ) && (USART4_CL_RxLength ==1 )) //表示第一字节 为0x02 才为开始		
			{
				StartFlag = 1;
				
				g_Uart4RxStartFlag = 1;  //	接受数据开始标志	
				
				//RecordDataLen = USART4_CL_RxLength-1;		//记录0x02数据所在 数组下标 的位置
					
				//			/*发送接受开始事件标志*/
				//			OSFlagPost((OS_FLAG_GRP*)&BCUEventFlags,
				//				 (OS_FLAGS	  )RxStartFlag,				//发送奇接受开始 事件标志
				//				 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
				//				 (OS_ERR*	    )&err);
			}
			if((StartFlag == 1) && (USART4_CL_RxBuffer[/*RecordDataLen+1*/1] == BCU_SD_COMMAND) && (USART4_CL_RxLength ==54))		//BCU SD响应数据长度为54，表示数据接受完成
			{										
				g_Uart4RxDataFlag = 1;
				StartFlag = 0;
				
				//g_BCURxSdOrTdFlag = BCU_RxSD;		// 表示接受到SD消息
				
				//			/*发送接受结束事件标志*/
				//			OSFlagPost((OS_FLAG_GRP*)&BCUEventFlags,
				//				 (OS_FLAGS	  )RxEndFlag,										//发送奇接受结束 事件标志
				//				 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
				//				 (OS_ERR*	    )&err);			
			}
			else if((StartFlag == 1) && (USART4_CL_RxBuffer[/*RecordDataLen+1*/1] == BCU_TD_COMMAND) && (USART4_CL_RxLength ==56))		//Td 数据 为56字节
			{
				g_Uart4RxDataFlag = 1;
				StartFlag = 0;
				
				//g_BCURxSdOrTdFlag = BCU_RxTD;		// 表示接受到TD消息
				
				//			/*发送接受结束事件标志*/
				//			OSFlagPost((OS_FLAG_GRP*)&BCUEventFlags,
				//				 (OS_FLAGS	  )RxEndFlag,										//发送奇接受结束 事件标志
				//				 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
				//				 (OS_ERR*	    )&err);			
			}			
		}
		
		USART_ClearITPendingBit(MTDMFD_USARTx,USART_IT_RXNE);				//清除接受中断标志	
	}

	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题[牛人说要这样]
	if(USART_GetFlagStatus(MTDMFD_USARTx,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(MTDMFD_USARTx,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(MTDMFD_USARTx);   					  //读DR
		printf("Uart4 溢出！\r\n");
	}	
	
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    		 //退出中断
	#endif	
}

#endif

