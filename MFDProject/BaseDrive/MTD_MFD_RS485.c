#include "MTD_MFD_RS485.h"
#include "delay.h"
#include "sys.h"
#include "os.h"
#include "main.h"
#include "CommunPro.h"
#include "Uart.h"
#include "bsp.h"

u8 USART2_RS485_RxBuffer[100]; 	//接收缓冲,最大100个字节
//u8 USART2_RS485_TxBuffer[100]; 	//接收缓冲,最大100个字节
u8 USART2_RS485_RxLength = 0;   //接收到的数据长度


u8 USART3_RS485_RxBuffer[100]; 	//接收缓冲,最大100个字节
//u8 USART3_RS485_TxBuffer[100]; 	//接收缓冲,最大100个字节
u8 USART3_RS485_RxLength = 0;   //接收到的数据长度

/*
初始化
*/
void MTDMFD_USART2_RS485_Init(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 开启GPIO_A的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* 开启串口2的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
	/*串口2引脚复用映射*/
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3复用为USART2
	
	/*USART2*/    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					 //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				 //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 					 //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 //速度100MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
	GPIO_Init(GPIOA,&GPIO_InitStructure); 								 //初始化PA2，PA3
	
	//PA4推挽输出，RS485模式控制  
		/* 开启GPIO_A的时钟 */
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;						//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_Pin = MTDMFD_USART2_DR_GPIO_PIN; //GPIOA4
	GPIO_Init(MTDMFD_USART2_DR_GPIO,&GPIO_InitStructure); 	//初始化PA4	
	
	/*USART2 配置*/
	USART_InitStructure.USART_BaudRate = BaudRate;								//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;		//字长为9位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Odd;					//奇校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART2, &USART_InitStructure);										  //初始化串口2
	
	/* 使能串口2 */
	USART_Cmd(USART2, ENABLE);  
	
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC); 	 				/* 清发送完成标志，Transmission Complete flag */   
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		//开启UART2接受中断
	USART_ITConfig(USART2, USART_IT_PE, 	ENABLE);    //开启PE错误接收中断Bit 8PEIE: PE interrupt enable
	USART_ITConfig(USART2, USART_IT_ERR, 	ENABLE);		//CR2 开启ERR中断
	//USART_ITConfig(USART2, USART_IT_FE, 	ENABLE);

	/*uart2中断优先级的配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;					//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	USART2_RS485_RX_EN();					 //默认为接收模式	
}

/*
	RS485发送len个字节.
	buf:发送区首地址
	len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
*/
void MTDMFD_USART2_RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	USART2_RS485_TX_EN();				//设置为发送模式
	for(t=0;t<len;t++)
	{
		while(RESET == USART_GetFlagStatus(USART2, USART_FLAG_TXE));	//等待发送缓冲区为空
		USART_SendData(USART2,buf[t]); //发送数据
		while(RESET == USART_GetFlagStatus(USART2,USART_FLAG_TC)); 	//等待发送结束
	}
	
	USART2_RS485_RxLength = 0;			//接受数据的长度为0
	USART2_RS485_RX_EN();					 //默认为接收模式	
}


/*
	RS485查询接收到的数据
	buf:接收缓存首地址
	len:读到的数据长度
*/
void MTDMFD_USART2_RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 RxLen = USART2_RS485_RxLength ;
	u8 i=0;
	*len =0; //默认为0
	
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if((RxLen==USART2_RS485_RxLength)&&RxLen)//接收到了数据,且接收完成了
	{
		for(i=0;i<RxLen;i++)
		{
			buf[i]=USART2_RS485_RxBuffer[i];	
		}		
		*len=USART2_RS485_RxLength;	//记录本次数据长度
		//USART2_RS485_RxLength=0;		//清零
	}
}	

/*
	中断接受数据
*/
void USART2_IRQHandler(void)
{
	//OS_ERR err;
	static unsigned char StartFlag = 0;	
	uint8_t RxChar;

	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	
	
	if(USART_GetITStatus(USART2,USART_IT_PE)!= RESET || USART_GetITStatus(USART2,USART_IT_FE)!= RESET)		//这段代码将会将程序卡死到 B . 汇编代码部分
	{
		g_Uart2FrameErrorFlag=1;		//将奇偶错误 帧错误 标志设置1
		
		//		/*发送奇偶校验错误，帧错误 事件标志*/
		//		OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
		//			 (OS_FLAGS	  )RxParityFrameError,				//发送奇偶校验错误，帧错误 事件标志
		//			 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
		//			 (OS_ERR*	    )&err);
					
		USART_ClearITPendingBit(USART2,USART_IT_FE);			//清除帧错误标志
		USART_ClearITPendingBit(USART2,USART_IT_PE);			//清除奇偶错误标志
		printf("Uart2 奇偶校验错误!\r\n");		
	}
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)		//接收中断
	{
		if(USART2_RS485_RxLength >=100 )
		{
			USART2_RS485_RxLength =0;//暂时清0，循环接受
		}
		
		/* Read one byte from the receive data register */
		//USART2_RS485_RxBuffer[USART2_RS485_RxLength++] = USART_ReceiveData(USART2);	//读取接收到的数据
		RxChar = USART_ReceiveData(USART2);	//读取接收到的数据
		USART2_RS485_RxBuffer[USART2_RS485_RxLength++] = RxChar;
		
		switch(ChooseBoard)
		{	
			case MC1_MFD1_NODEID:			//空调接受中断
			case T_MFD1_NODEID:	
			case M_MFD1_NODEID:
			case T1_MFD1_NODEID:
			case T2_MFD1_NODEID:
			case MC2_MFD1_NODEID:	
				//if(RxChar == 0x02)  //表示接受的为开始数据
				if((USART2_RS485_RxBuffer[0] ==  0x02 ) && (USART2_RS485_RxLength ==1 )) //表示第一字节 为0x02 才为开始
				{
					StartFlag = 1;
					g_Uart2RxStartFlag = 1;
					
					//					/*发送接受开始事件标志*/
					//					OSFlagPost((OS_FLAG_GRP*)&ACCEventFlags,
					//						 (OS_FLAGS	  )RxStartFlag,				//发送奇接受开始 事件标志
					//						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
					//						 (OS_ERR*	    )&err);
				}
				if((StartFlag == 1) && (USART2_RS485_RxLength == 14))		//ACC响应数据长度为14，表示数据接受完成
				{
					g_Uart2RxDataFlag = 1;
					StartFlag = 0;
					
					//					/*发送接受结束事件标志*/
					//					OSFlagPost((OS_FLAG_GRP*)&ACCEventFlags,
					//						 (OS_FLAGS	  )RxEndFlag,										//发送奇接受结束 事件标志
					//						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
					//						 (OS_ERR*	    )&err);			
				}
			break;
				
			case	MC1_MFD2_NODEID:		//PIS接受中断
			case	MC2_MFD2_NODEID:
				if((USART2_RS485_RxBuffer[0] ==  0x02 ) && (USART2_RS485_RxLength ==1 )) //表示第一字节 为0x02 才为开始
				{
					StartFlag = 1;
					g_Uart2RxStartFlag = 1;				
				}
				if((StartFlag == 1) && (USART2_RS485_RxLength == 29))	//PIS响应数据长度为29，表示数据接受完成
				{
					g_Uart2RxDataFlag = 1;
					StartFlag = 0;					
				}
				break;

			default:
				printf("uart2 Rx ChooseBoard Error !\r\n");
				break;				
		}
		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);				//清除接受中断标志
	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题[牛人说要这样]
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART2,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(USART2);   					  //读DR
		printf("Uart2 溢出！\r\n");	
	}
	
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    		 //退出中断
	#endif	
}



/*
	串口3的初始化程序
*/

void MTDMFD_USART3_RS485_Init(u32 BaudRate,u16 VerifyMode)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;
	
	//PE14推挽输出，RS485模式控制  
	/* 开启GPIO_E的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;						//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					//推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						//上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//速度100MHz
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //GPIOE14
	GPIO_Init(GPIOE,&GPIO_InitStructure); 	//初始化PE14	
	
	
	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
	/*串口3引脚复用映射*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
				
	/*USART3*/    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					 //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				 //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 					 //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 //速度100MHz
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*USART3 配置*/
	USART_InitStructure.USART_BaudRate = BaudRate;								//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;		//字长为9位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				//一个停止位
	USART_InitStructure.USART_Parity = VerifyMode;		//USART_Parity_Odd;					//奇校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure);										  //初始化串口3
	
	/* 使能串口3 */
	USART_Cmd(USART3, ENABLE);  
	
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART3, USART_FLAG_TC); 	 				/* 清发送完成标志，Transmission Complete flag */   
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		//开启UART3接受中断
	USART_ITConfig(USART3, USART_IT_PE, 	ENABLE);    //开启PE错误接收中断Bit 8PEIE: PE interrupt enable
	USART_ITConfig(USART3, USART_IT_ERR, 	ENABLE);		//CR2 开启ERR中断
	//USART_ITConfig(USART3, USART_IT_FE, 	ENABLE);

	/*uart3中断优先级的配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;					//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	USART3_RS485_RX_EN();					 //默认为接收模式	
}


/*
	RS485发送len个字节.
	buf:发送区首地址
	len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
*/
void MTDMFD_USART3_RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	USART3_RS485_TX_EN();						//设置为发送模式
	for(t=0;t<len;t++)
	{
		while(RESET == USART_GetFlagStatus(USART3, USART_FLAG_TXE));	//等待发送缓冲区为空
		USART_SendData(USART3,buf[t]); 	//发送数据	
		while(RESET ==  USART_GetFlagStatus(USART3,USART_FLAG_TC)); 	//等待发送结束
	}
	USART3_RS485_RxLength = 0;			//接受数据的长度为0
	USART3_RS485_RX_EN();					 //默认为接收模式	
	//debugprintf("send u3!\r\n");
}


void MTDMFD_USART3_RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8  RxLen = 0 ;
	u8 i=0;
	*len =0; //默认为0
	
	RxLen = USART3_RS485_RxLength;
	//printf("rxlen=%d\r\n",RxLen);
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if((RxLen==USART3_RS485_RxLength)&&RxLen)//接收到了数据,且接收完成了
	{
		for(i=0;i<RxLen;i++)
		{
			buf[i]=USART3_RS485_RxBuffer[i];	
			//printf("u3r[%d]=0x%x\r\n",i,buf[i]);
		}		
		*len=USART3_RS485_RxLength;	//记录本次数据长度
	//	USART3_RS485_RxLength=0;		//清零
	}
}

/*
	中断接受数据
*/
void USART3_IRQHandler(void)
{
	//OS_ERR err;
	static unsigned char StartFlag = 0;	
	uint8_t RxChar;
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	

	if(USART_GetITStatus(USART3,USART_IT_PE)!= RESET || USART_GetITStatus(USART3,USART_IT_FE)!= RESET)		//这段代码将会将程序卡死到 B . 汇编代码部分
	{
		g_Uart3FrameErrorFlag=1;		//将奇偶错误 帧错误 标志设置1
		
		//		/*发送奇偶校验错误，帧错误 事件标志*/
		//		OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
		//			 (OS_FLAGS	  )RxParityFrameError,				//发送奇偶校验错误，帧错误 事件标志
		//			 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
		//			 (OS_ERR*	    )&err);
		
		USART_ClearITPendingBit(USART3,USART_IT_FE);			//清除帧错误标志
		USART_ClearITPendingBit(USART3,USART_IT_PE);			//清除奇偶错误标志
		
		printf("Uart3 奇偶校验错误!\r\n");		
	}
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)		//接收中断
	{
		if(USART3_RS485_RxLength >=100 )
		{
			USART3_RS485_RxLength =0;//暂时清0，循环接受
		}

		/* Read one byte from the receive data register */
		//USART3_RS485_RxBuffer[USART3_RS485_RxLength++] = USART_ReceiveData(USART3);	//读取接收到的数据
		RxChar = USART_ReceiveData(USART3);	//读取接收到的数据
		USART3_RS485_RxBuffer[USART3_RS485_RxLength++] = RxChar;

		/*发送任务级别信号量*/
		//	OSTaskSemPost(&USART3Rx_TCB,OS_OPT_POST_NONE,&err);	
		//printf("U3[0]=0x%x\r\n",USART3_RS485_RxBuffer[0]);
		//printf("u3len=%d\r\n",USART3_RS485_RxLength);
		
		switch(ChooseBoard)
		{	
			case MC1_MFD1_NODEID:			//门接受中断
			case T_MFD1_NODEID:	
			case M_MFD1_NODEID:
			case T1_MFD1_NODEID:
			case T2_MFD1_NODEID:
			case MC2_MFD1_NODEID:	
				//if(RxChar == 0x02)  //表示接受的为开始数据
				if((USART3_RS485_RxBuffer[0] ==  0x02 ) && (USART3_RS485_RxLength ==1 )) //表示第一字节 为0x02 才为开始
				{
					StartFlag = 1;
					
					g_Uart3RxStartFlag =1;	
					
					//printf("U3 Rx start!\r\n");

					//					/*发送接受开始事件标志*/
					//					OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
					//						 (OS_FLAGS	  )RxStartFlag,				//发送奇接受开始 事件标志
					//						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
					//						 (OS_ERR*	    )&err);
				}
				if((StartFlag == 1) && (USART3_RS485_RxLength ==8))		//门响应数据长度为8，表示数据接受完成
				{
					g_Uart3RxDataFlag = 1;
					StartFlag = 0;
					
					//					printf("U3 Rx end!\r\n");
					//					{
					//						u8 i=0;
					//						for(i=0;i<8;i++)
					//						{
					//							printf("u3[%d]=0x%x\r\n",i,USART3_RS485_RxBuffer[i]);
					//						}
					//					}
					
					//					/*发送接受结束事件标志*/
					//					OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
					//						 (OS_FLAGS	  )RxEndFlag,				//发送奇接受结束 事件标志
					//						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
					//						 (OS_ERR*	    )&err);			
				}
			break;
				
			case	MC1_MFD2_NODEID:		//ATC接受中断
			case	MC2_MFD2_NODEID:
					//判断接受开始标志
					if((USART3_RS485_RxBuffer[0] == ATC_DLE) &&(USART3_RS485_RxLength ==1 ))
					{
						StartFlag =1;
						
						g_Uart3RxStartFlag =1;	
					}
					
					/*判断接受结束标志*/
					if((StartFlag == 1) && (USART3_RS485_RxLength ==54))		//ATC 接受SD为54字节
					{
						g_Uart3RxDataFlag = 1;
						
						StartFlag = 0;
					}
			
				break;
			
			case  MC1_MFD3_NODEID:		//ACP接受中断
			case 	MC2_MFD3_NODEID:
					//判断接受开始标志
					if((USART3_RS485_RxBuffer[0] == ACP_DEVICEADDRES) &&(USART3_RS485_RxBuffer[1] == ACP_COMMAND) &&(USART3_RS485_RxLength ==2 ))
					{
						StartFlag =1;
						g_Uart3RxStartFlag =1;	
					}
					
					/*判断接受结束标志*/
					if((StartFlag == 1) && (USART3_RS485_RxLength ==16))		//ACP 接受SD为16字节
					{
						g_Uart3RxDataFlag = 1;
						StartFlag = 0;
					}				
				break;
				
			default:
				printf("uart3 Rx ChooseBoard Error !\r\n");
				break;
		}
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);				//清除接受中断标志
	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题[牛人说要这样]
	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART3,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(USART3);   					  //读DR
		printf("Uart3 溢出！\r\n");
	}
	
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    		 //退出中断
	#endif	
}
