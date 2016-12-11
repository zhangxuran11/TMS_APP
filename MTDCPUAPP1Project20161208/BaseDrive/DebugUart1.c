#include "DebugUart1.h"
#include "stdio.h"
#include <stdarg.h>

uint8_t Uart1_Tx_Buff[UART1BUFFERSIZE];
uint8_t Uart1_Rx_Buff[UART1BUFFERSIZE];

uint8_t UART1_Rx_Cnt=0;
static uint8_t print_buffer[UART1BUFFERSIZE];//打印缓存
static volatile uint32_t USART1_ReadIndex = 0;//读索引
static volatile uint8_t sendDoneFlag = 0;//发送完成标志
static volatile uint8_t recvDoneFlag = 0;//接收完成标志

void Uart1_init(void)			/* 串口1 TX = PA9   RX = PA10 */
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
	/* 第1步： 配置GPIO */
	/* 打开 GPIO 时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* 打开 UART 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* 使能  DMA clock */
	RCC_AHB1PeriphClockCmd(USARTx_DMAx_CLK, ENABLE);
	
	/* 将 PA9 映射为 USART1_TX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/* 将 PA10 映射为 USART1_RX */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* 配置 USART Tx 为复用功能 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 配置 USART Rx 为复用功能 */
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
/* USARTx configured as follows:
		- BaudRate = 5250000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 8
			 is: (USART APB Clock / 8) 
			 Example: 
				- (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
				- (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
		   - Maximum BaudRate that can be achieved when using the Oversampling by 16
			 is: (USART APB Clock / 16) 
			 Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
			 Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
		- Word Length = 8 Bits
		- one Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/ 
	/* 第2步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = 115200;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	/* When using Parity the word length must be configured to 9 bits */
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);


	/* Configure DMA controller to manage USART TX and RX DMA request ----------*/ 
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize = UART1BUFFERSIZE ;											/* 配置DMA大小 */
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;									/* 在这个程序里面使能或者禁止都可以的 */
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ; 	/* 设置阀值 */
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;						/* 设置内存为单字节突发模式 */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					/* 设置内存数据的位宽是字节 */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;									/* 使能地址自增 */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;														/* 设置DMA是正常模式 */
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR)) ;	/* 设置外设地址 */
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			/* 设置外设为单字节突发模式 */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* 设置外设数据的位宽是字节 */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				/* 禁止外设地址自增 */
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;											/* 设置优先级 */
  /* Configure TX DMA */
  DMA_InitStructure.DMA_Channel = USARTx_TX_DMA_CHANNEL ;									 /* 配置发送通道 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ;								 /* 设置从内存到外设 */
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)Uart1_Tx_Buff ;				 /* 设置内存地址 */
  DMA_Init(USARTx_TX_DMA_STREAM,&DMA_InitStructure);
	
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = USARTx_RX_DMA_CHANNEL ;									 /* 配置接收通道 */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;								 /* 设置从外设到内存 */
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)Uart1_Rx_Buff ; 				 /* 设置内存地址 */
  DMA_Init(USARTx_RX_DMA_STREAM,&DMA_InitStructure);


	
	//USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);		/*使能UART1空中断*/
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
	USART_Cmd(USART1, ENABLE);		/* 使能串口 */
	
	
	/* 使能 USART DMA RX 请求 */
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	/* 使能 USART DMA TX 请求 */
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	/* 使能发送传输完成中断 */
	DMA_ITConfig(USARTx_TX_DMA_STREAM, DMA_IT_TC, ENABLE);  
    
  /* 使能接收传输完成中断 */
	DMA_ITConfig(USARTx_RX_DMA_STREAM, DMA_IT_TC, ENABLE); 		
	
//	/* 使能 DMA USART TX Stream */
//	DMA_Cmd(USARTx_TX_DMA_STREAM, ENABLE); 		//这里还不能使能，发送函数中 重新写入了 发送缓冲区内存地址
	
	/* 使能 DMA USART RX Stream */
//	DMA_Cmd(USARTx_RX_DMA_STREAM, ENABLE); 

	

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */

	/*uart1中断优先级的配置*/
	Uart1_NVIC_Config();
}

/*********************************************************************************************************
** Function name:       User_Uart3SendChar
** Descriptions:        从串口发送数据
** input parameters:    ch: 发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart1SendChar(unsigned char ch)
{
     USART_SendData (USART1,ch);                                        /* 发送字符                     */
     while( USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET );         /* 等待发送完成                 */
     USART_ClearFlag(USART1,USART_FLAG_TC);                             /* 清除发送完成标识             */
}

/*********************************************************************************************************
** Function name:       User_Uart3SendString
** Descriptions:        向串口发送字符串
** input parameters:    s:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart1SendString(unsigned char *s)
{
    while (*s != '\0') {
           User_Uart1SendChar(*s++);
    }
}

void Uart1_IRQ(void)
{
	unsigned char ch;				
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		ch = (USART_ReceiveData(USART1));
    ch=ch;           
	} 
}


/**
* 串口发送函数
* @param buffer	发送内容
* @param length	发送长度
* @return
*/
uint16_t USART1_DMA_SendBuffer(const uint8_t* buffer, uint16_t length)
{
	if( (buffer==0) || (length==0) )
	{
		return 0;
	}
	sendDoneFlag = 0;
	DMA_Cmd(USARTx_TX_DMA_STREAM, DISABLE);
	USARTx_TX_DMA_STREAM->M0AR = (uint32_t)buffer;				/* 设置内存地址 */
	DMA_SetCurrDataCounter(USARTx_TX_DMA_STREAM, length);
	DMA_Cmd(USARTx_TX_DMA_STREAM, ENABLE);
	return length;
}

/*
自定义打印函数
*/
void Debug_Printf(const char* format,...)
{
  	uint32_t length;
		va_list args;
		va_start(args, format);
		length = vsnprintf((char*)print_buffer,sizeof(print_buffer), (char*)format, args);//格式化内容
		while(!sendDoneFlag);//等待发送完成
		USART1_DMA_SendBuffer(print_buffer,length);//发送
		va_end(args);
}

/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART1, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用scanff函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);
}


/*uart1 的中断嵌套向量控制的配置*/
void Uart1_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
			/* NVIC configuration */
			/* Configure the Priority Group to 2 bits */
		//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

		/* 使能 DMA Stream 中断通道 */
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_DMA_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	
	/* 使能 DMA Stream 中断通道 */
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_DMA_RX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}


#include "iap.h"
void USART1_IRQHandler(void)
{	
unsigned char ch;	

#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
  CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
	CPU_CRITICAL_EXIT();	//开中断
#endif	
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)		//接收中断
	{
		/* Read one byte from the receive data register */
		ch = (USART_ReceiveData(USART1));		//读取接收到的数据
		IAP_UART_Proceed_Cell(ch);
		if(UART1_Rx_Cnt < UART1BUFFERSIZE)
		{
			Uart1_Rx_Buff[UART1_Rx_Cnt] = ch;
			UART1_Rx_Cnt++;
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);				//清除接受中断标志
	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题[牛人说要这样]
 if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
 {
		USART_ClearFlag(USART1,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(USART1);    //读DR
 }

	/*DMA 使用的IDLE 空中断*/ 
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		//		USART_ClearITPendingBit(USART1, USART_IT_IDLE);//空闲
		USART_GetITStatus(USART1, USART_IT_IDLE);	//只能通过:读取USART_SR->读取USART_DR来清除USART1_IT_IDLE标志
		USART_ReceiveData(USART1);//空读清状态 注意：这句必须要，否则不能够清除中断标志位。我也不知道为啥！
		recvDoneFlag = 1;
	}
	
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntExit();    	//退出中断
#endif	
}

/*
*
*********************************************************************************************************
*	函 数 名: USARTx_DMA_TX_IRQHandler
*	功能说明: 串口1在DMA方式下的发送中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void USARTx_DMA_TX_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
  CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
	CPU_CRITICAL_EXIT();	//开中断
#endif	
	/*
	  函数DMA_GetITStatus在使用上存在问题，产生标志了，检测无法通过。但是使用函数
	  DMA_GetFlagStatus却可以，这里记录一下这个问题，方便后面解决。
	*/
//	if (DMA_GetFlagStatus(USARTx_TX_DMA_STREAM,USARTx_TX_DMA_FLAG_TCIF)==SET)
//// 	if (DMA_GetITStatus(USARTx_TX_DMA_STREAM,USARTx_TX_DMA_FLAG_TCIF) == SET)
// 	{
// 		/* 清除DMA传输完成标志 */
// 		DMA_ClearITPendingBit(USARTx_TX_DMA_STREAM, USARTx_TX_DMA_FLAG_TCIF);			
// 	}
	
//	/* 这里采用寄存器直接操作 */
//	if(DMA2->HISR & USARTx_TX_DMA_FLAG_TCIF)
// 	{
//		DMA2->HIFCR = USARTx_TX_DMA_FLAG_TCIF;
//		sendDoneFlag = 1;
//	}
	
	/*库函数版本*/
	 /* Test on DMA Stream Transfer Complete interrupt */
	if(DMA_GetITStatus(USARTx_TX_DMA_STREAM, DMA_IT_TCIF7))
	{
		 /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(USARTx_TX_DMA_STREAM, DMA_IT_TCIF7);
		sendDoneFlag = 1;
	}
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntExit();    	//退出中断
#endif	
}

/*
*********************************************************************************************************
*	函 数 名: USARTx_DMA_RX_IRQHandler
*	功能说明: 串口1在DMA方式下的接收中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void USARTx_DMA_RX_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
  CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
	CPU_CRITICAL_EXIT();	//开中断
#endif		
	
	/* 这里采用寄存器直接操作 */
	if(DMA2->LISR & USARTx_RX_DMA_FLAG_TCIF)
 	{
		recvDoneFlag = 1;		//接受完成中断标志
		DMA2->LIFCR = USARTx_RX_DMA_FLAG_TCIF;
		printf("uart1!\2\3\4\r\n");
 	}
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntExit();    	//退出中断
#endif		
}




//static void uart1_send(const char *str, unsigned int size)
//{
//    int pos = 0;

//    while(size)
//    {
//        if (str[pos] == '\0')
//            break;
//        
//				/* 等待缓冲区空 */
//        while(!(USART1->SR & 0x80));
//        /* 发送数据 */
//        USART1->DR = str[pos];

//        pos ++;
//        size --;
//    }
//}




//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  //W H H
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;      
FILE __stdin;

void _ttywrch(int ch) 
{
//	USART_SendData(USART1, (uint8_t) ch);
	/*W H H ,八通线测试用的的uart1*/
	USART_SendData(USART1, (uint8_t) ch);
	
}

int ferror(FILE *f)
{ 
	// Your implementation of ferror
	return EOF;
}

//定义_sys_exit()以避免使用半主机模式    
void _sys_exit( int x) 
{ 
	x = x; 
} 

#endif
