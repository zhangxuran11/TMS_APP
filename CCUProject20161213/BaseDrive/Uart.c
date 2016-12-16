
#include "stm32f4xx.h"
#include "stdio.h"
#include "Uart.h"
#include <stdarg.h>

void uart3_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 开启GPIO_D的时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* 开启串口3的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);


	USART_InitStructure.USART_BaudRate   = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_Odd;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART3, &USART_InitStructure);

	/* 使能串口3 */
	USART_Cmd(USART3, ENABLE);

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */   

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	/*uart3中断优先级的配置*/
	Uart3_NVIC_Config();
}

/*********************************************************************************************************
** Function name:       User_Uart3SendChar
** Descriptions:        从串口发送数据
** input parameters:    ch: 发送的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart3SendChar(unsigned char ch)
{
     USART_SendData (USART3,ch);                                        /* 发送字符                     */
     while( USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET );         /* 等待发送完成                 */
     USART_ClearFlag(USART3,USART_FLAG_TC);                             /* 清除发送完成标识             */
}

/*********************************************************************************************************
** Function name:       User_Uart3SendString
** Descriptions:        向串口发送字符串
** input parameters:    s:   要发送的字符串指针
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void User_Uart3SendString(unsigned char *s)
{
    while (*s != '\0') {
           User_Uart3SendChar(*s++);
    }
}

void Uart3_IRQ(void)
{
	unsigned char ch;				
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		ch = (USART_ReceiveData(USART3));
    ch=ch;           
	} 
}

///*
//*********************************************************************************************************
//*	函 数 名: fputc
//*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//int fputc(int ch, FILE *f)
//{
//	/* Place your implementation of fputc here */
//	/* e.g. write a character to the USART */
//	USART_SendData(USART3, (uint8_t) ch);

//	/* Loop until the end of transmission */
//	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
//	{}

//	return ch;
//}

///*
//*********************************************************************************************************
//*	函 数 名: fgetc
//*	功能说明: 重定义getc函数，这样可以使用scanff函数从串口1输入数据
//*	形    参: 无
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//int fgetc(FILE *f)
//{
//	/* 等待串口3输入数据 */
//	while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);

//	return (int)USART_ReceiveData(USART3);
//}

/*uart3 的中断嵌套向量控制的配置*/
void Uart3_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void _send(const char *str, unsigned int size)
{
    int pos = 0;

    while(size)
    {
        if (str[pos] == '\0')
            break;
        
				/* 等待缓冲区空 */
        while(!(USART3->SR & 0x80));
        /* 发送数据 */
        USART3->DR = str[pos];

        pos ++;
        size --;
    }
}

void debug(const char* fmt,...)
{
    va_list ap;
    char string[129];

    string[128]='\0';
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    _send(string,64);
}



////加入以下代码,支持printf函数,而不需要选择use MicroLIB	  //W H H
//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 
//}; 

//FILE __stdout;      
//FILE __stdin;

//void _ttywrch(int ch) 
//{
////	USART_SendData(USART3, (uint8_t) ch);
//	/*W H H ,八通线测试用的的uart1*/
//	USART_SendData(USART1, (uint8_t) ch);
//	
//}

//int ferror(FILE *f)
//{ 
//	// Your implementation of ferror
//	return EOF;
//}

////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit( int x) 
//{ 
//	x = x; 
//} 

//#endif
