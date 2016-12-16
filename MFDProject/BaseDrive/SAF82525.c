#include "SAF82525.h"
#include "delay.h"
#include "os.h"
#include "Uart.h"
#include "malloc.h"
#include "string.h"
#include "DebugUart1.h"
#include "VVVFComPro.h"

#define Bank1_SRAM1_ADDR  ((uint32_t)0x60000000)  
#define SAF82525_BASE_ADDR 	Bank1_SRAM1_ADDR
#define SAF82525_FIFO_LEN		32		//芯片的缓冲区大小为32字节

__IO uint8_t HDLCARxBuf[200];
__IO uint8_t HDLCBRxBuf[200];
__IO uint16_t HDLCARxLen=0;
__IO uint16_t HDLCBRxLen=0;

u8  g_HDLCARxStartFlag=0;		//A通道接受开始标志
u8  g_HDLCARxEndFlag=0;			//A通道接受结束标志

u8  g_HDLCBRxStartFlag=0;		//B通道接受开始标志
u8  g_HDLCBRxEndFlag=0;			//B通道接受结束标志



/*定义中断寄存器 状态 全局变量*/
uint8_t  g_ucBISTAStatus,g_ucBEXIRStatus,g_ucBRSTAStatus,	 g_ucAISTAStatus,g_ucAEXIRStatus,g_ucARSTAStatus;

void SAF82525_INT_GPIO_Init(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(SAF82525_INT_RCC, ENABLE); 		//使能IO时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //设置上接
  GPIO_InitStructure.GPIO_Pin = SAF82525_INT_GPIO_PIN;  //IO口为6
  GPIO_Init(SAF82525_INT_GPIO, &GPIO_InitStructure);    //如上配置gpio

  SYSCFG_EXTILineConfig(SAF82525_INT_EXIT_PORT, SAF82525_INT_EXIT_PIN);//实始化中断线11

  EXTI_InitStructure.EXTI_Line = EXTI_Line6;                   //配置中断线为中断线11
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //配置中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //配置为下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //配置中断线使能
  EXTI_Init(&EXTI_InitStructure);                              

  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;             
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


//硬件中断函数的服务程序
void EXTI9_5_IRQHandler(void)		
{
	//static u8 i=0;
	//static u8 HDLCAComandVale = 0;
	//static u8 HDLCBComandVale = 0;
	//u8 StartVale=0;
	
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     		//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	

	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{
		/*读取状态寄存器的值*/
		/*先读取B寄存*/
		g_ucBISTAStatus = SAF82525_Read_Register(BISTA);
		g_ucBEXIRStatus = SAF82525_Read_Register(BEXIR);
		g_ucBRSTAStatus = SAF82525_Read_Register(BRSTA);
		
		/*在读取A寄存器*/
		g_ucAISTAStatus = SAF82525_Read_Register(AISTA);
		g_ucAEXIRStatus = SAF82525_Read_Register(AEXIR);	
		g_ucARSTAStatus = SAF82525_Read_Register(ARSTA);
			
		if(((g_ucBISTAStatus & RME) == RME)||((g_ucBISTAStatus & RPF) == RPF) || ((g_ucBISTAStatus & EXB) == EXB))
		{
			BChanleRxData(HDLCBRxBuf,&HDLCBRxLen);	//B接受数据
					
					//			if((HDLCBRxBuf[0] == 0x7E) && (HDLCBRxLen<=32))		//第一次接受HDLC时计算下就可以
					//			{
					//				g_HDLCBRxStartFlag =1;	//接受数据开始
									
					//				i=0;		//计算出有多少个0x7E
					//				while(1)
					//				{
					//					StartVale = *(HDLCBRxBuf+i);
					//					if(StartVale == VVVF_PAD)
					//					{
					//						i++;
					//					}
					//					else
					//					{
					//						break; //跳出while循环
					//					}
					//				}
					//				
					//				HDLCBComandVale = HDLCBRxBuf[i+4];		//得到B命令字节 
					//			}
					
					//			switch(HDLCBComandVale)		//分析B得到的命令
					//			{
					//				case VVVF_SD_COMMAND:
					//					if(g_HDLCBRxStartFlag==1 && HDLCBRxLen >= 51)
					//					{
					//						g_HDLCBRxEndFlag =1; //接受结束
					//						
					//						g_HDLCBRxStartFlag = 0;//清除开始标志
					//					}
					//				break;
					//				
					//				case VVVF_TD_COMMAND:
					//					if(g_HDLCBRxStartFlag==1 && HDLCBRxLen >= 120)
					//					{
					//						g_HDLCBRxEndFlag =1; //接受结束
					//						
					//						g_HDLCBRxStartFlag = 0;//清除开始标志
					//					}					
					//					break;
					//				
					//				default:
					//					printf("HDLC B Comdand err!\r\n");
					//			}
		}
		
		if(((g_ucBISTAStatus &ICA) ==ICA)||((g_ucBISTAStatus & EXA )==EXA) )
		{
			AChanleRxData(HDLCARxBuf,&HDLCARxLen);	//A接受数据
						
					//			if((HDLCARxBuf[0] == 0x7E) && (HDLCARxLen<=32))	//第一次接受HDLC时计算下就可以
					//			{
					//				g_HDLCARxStartFlag =1;	//接受数据开始
									
					//				i=0;		//计算出有多少个0x7E
					//				while(1)
					//				{
					//					StartVale = *(HDLCBRxBuf+i);
					//					if(StartVale == VVVF_PAD)
					//					{
					//						i++;
					//					}
					//					else
					//					{
					//						break; //跳出while循环
					//					}
					//				}
									
					//				HDLCAComandVale = HDLCARxBuf[i+4];		//得到A命令字节 				
					//			}
					
					//			switch(HDLCAComandVale)		//分析A得到的命令
					//			{
					//				case VVVF_SD_COMMAND:
					//					if(g_HDLCARxStartFlag==1 && HDLCARxLen >= 51)
					//					{
					//						g_HDLCARxEndFlag =1; //接受结束
					//						
					//						g_HDLCARxStartFlag = 0;//清除开始标志
					//					}
					//				break;
					//				
					//				case VVVF_TD_COMMAND:
					//					if(g_HDLCARxStartFlag==1 && HDLCARxLen >= 120)
					//					{
					//						g_HDLCARxEndFlag =1; //接受结束
					//						
					//						g_HDLCARxStartFlag = 0;//清除开始标志
					//					}					
					//					break;
					//				
					//				default:
					//					printf("HDLC A Comdand err!\r\n");
					//			}			
		}
				
		/* 对于沿触发中断方式的CPU,当还有中断为处理完时,以下指令可再次产生中断 手册40页 */	
		SAF82525_Write_Register(BMASK,0xFF);
		SAF82525_Write_Register(BMASK,(RSC|TIN|XPR/*|ICA|EXA|EXB*/));	//使能RPF, RME, XPR中断
		SAF82525_Write_Register(AMASK,0xFF);
		SAF82525_Write_Register(AMASK,(RSC|TIN|XPR/*|ICA|EXB|EXA*/));	//使能RPF, RME, XPR中断
		
		EXTI_ClearITPendingBit(EXTI_Line6);		//清除中断标志 中断线6
	}
	
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    		//退出中断
	#endif
}

void SAF82525_Rest_CPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
		/* 开启GPIO 的时钟  */
	RCC_AHB1PeriphClockCmd(SAF82525_REST_RCC ,ENABLE);		//GPIOF
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;				//GPIO_PuPd_NOPULL;	修改为上拉电阻
	GPIO_InitStructure.GPIO_Pin = SAF82525_REST_GPIO_PIN;
	
	GPIO_Init(SAF82525_REST_GPIO,&GPIO_InitStructure);
	
	SAF82525_REST_ON();		//上电之后复位
}


void SAF82525_GPIO_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  ReadTim;
	FSMC_NORSRAMTimingInitTypeDef  WriteTim;
  GPIO_InitTypeDef GPIO_InitStructure; 
	
	  /* Enable GPIOs clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF ,ENABLE);

  /* Enable FSMC clock */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE); 
	
	/*-- GPIOs Configuration -----------------------------------------------------*/
/*
 +-------------------+--------------------+------------------+------------------+
 |   	 								PD14 <-> FSMC_D0  | PF0 <-> FSMC_A0  |  |
 | PD7  <-> FSMC_NE1	PD15 <-> FSMC_D1	| PF1 <-> FSMC_A1  |  |
 | PD4  <-> FSMC_NOE |PD0  <-> FSMC_D2	| PF2 <-> FSMC_A2  |  |
 | PD5  <-> FSMC_NWE |PD1  <-> FSMC_D3  | PF3 <-> FSMC_A3  |  |
 |									 |PE7  <-> FSMC_D4  | PF4 <-> FSMC_A4  |  |
 |									 |PE8  <-> FSMC_D5  | PF5 <-> FSMC_A5  |  |
 |									 |PE9  <-> FSMC_D6  | PF12 <-> FSMC_A6 |  |
 |									 |PE10 <-> FSMC_D7 	| 										|------------------+
	
	PE6  <-> SAF_INT
	PF6  <-> SAF_RES
*/

  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  | 
                                GPIO_Pin_7  | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_Init(GPIOD, &GPIO_InitStructure);


  /* GPIOE configuration */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 ;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  | 
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_12 ;      

  GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	
/*-- FSMC Configuration ------------------------------------------------------*/
  ReadTim.FSMC_AddressSetupTime = 3;						//地址建立时间（ADDSET）为2个HCLK 1/168M=6ns*3=18ns	
  ReadTim.FSMC_AddressHoldTime = 3;							//地址保持时间（ADDHLD）模式A未用到
  ReadTim.FSMC_DataSetupTime = 12;						  //数据保存时间为18个HCLK	=6*6=36ns
  ReadTim.FSMC_BusTurnAroundDuration = 0;
  ReadTim.FSMC_CLKDivision = 0;
  ReadTim.FSMC_DataLatency = 0;
  ReadTim.FSMC_AccessMode = FSMC_AccessMode_A;
	
	WriteTim.FSMC_AddressSetupTime = 3;						//地址建立时间（ADDSET）为4个HCLK 1/168M=6ns*3=18ns	
  WriteTim.FSMC_AddressHoldTime = 3;						//地址保持时间（ADDHLD）模式A未用到
  WriteTim.FSMC_DataSetupTime = 11;							//数据保存时间为18个HCLK	=6*3=18ns
  WriteTim.FSMC_BusTurnAroundDuration = 0;				
  WriteTim.FSMC_CLKDivision = 0;
  WriteTim.FSMC_DataLatency = 0;
  WriteTim.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;  
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;				//W H H   修改为扩展模式，选择模式A.
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &ReadTim;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &WriteTim;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 

  /*!< Enable FSMC Bank1_SRAM1 Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 

}

/*
	读指定寄存器的值
	返回读取到的数据
*/
uint8_t SAF82525_Read_Register(const uint8_t ReadAddr)
{
	//	/*__IO*/ uint8_t ReadData;		//W H H 一定要使volatile
	//	ReadData =  *(__IO uint8_t *)(SAF82525_BASE_ADDR | ReadAddr);
	//	return ReadData;
	return  *(__IO uint8_t *)(SAF82525_BASE_ADDR | ReadAddr);
}

/*
	写指定寄存器的值
*/
void SAF82525_Write_Register(const uint8_t WriteAddr, uint8_t WriteData)
{
	*(uint8_t *) (SAF82525_BASE_ADDR | WriteAddr) = WriteData;
}


/*
	修改寄存器的某位
*/
void SAF82525_ModifyBit_Register(uint8_t Addr,uint8_t Mask,uint8_t Data)
{
	uint8_t CurrentRegVale,ModifyRegVale,temp;
	CurrentRegVale = SAF82525_Read_Register(Addr);
	
	if((CurrentRegVale&Mask)==Mask)
	{
		temp = 1;
	}

	else 
		temp = 0;
	
	if(Data)
	{
		if(temp == 0)
		{
			ModifyRegVale = CurrentRegVale | Mask;		//将某位修改为1
			SAF82525_Write_Register(Addr,ModifyRegVale);
		}
	}
	else
	{
		if(temp ==1)
		{
			ModifyRegVale = CurrentRegVale &(!Mask);	//将某位修改为0
			SAF82525_Write_Register(Addr,ModifyRegVale);
		}
	}
}



/*
初始化
//W H H  ,注意，要想读取的数据正确，HDLC芯片不能有命令在执行，必须等到命令执行完成再去读取寄存器。
*/
void SAF82525_Init(uint8_t HDLC_BautRate)
{
	//	uint8_t a,b;
	SAF82525_INT_GPIO_Init();
	SAF82525_Rest_CPIO_Init();
	SAF82525_GPIO_Init();
	
	//step1: SAF82525芯片复位
	SAF82525_REST_ON();		//开启复位
	delay_ms(50);
	SAF82525_REST_OFF();  //关闭复位

	
	/*通道A配置*/
	//step2: SAF82525 芯片REG设置							// W H H 帧间填充FLAG序列 1(有疑问) ？？？？？
	SAF82525_Write_Register(ACCR1,/*0x5F*/0x5F);	//下电模式 0| NRZI数据编码 1 0|推免上拉输出 1|  帧间填充FLAG序列 1(有疑问),或者连续IDLE序列输出（T×D引脚保持在“1”状态）0|选择时钟模式7	(重点，漏极开路输出，和时钟7)
	SAF82525_Write_Register(AMODE,/*0x98*/MDS1|TMD|RAC/*|TLP(自环模式)*/);	//(MDS1 MDS0)transparent mode | (TMD)internal mode | (RAC)HDLC receiver active
	SAF82525_Write_Register(ACCR2,/*0x28*/BDF | TIO);	//(BDF ) BR9 - BR0 [CCR2 BRG] |( TIO) Tx CLKA, Tx CLKB pins are outputs
	
	/*手册57页 the DPLL is supplied with a reference clock from BRG which is 16 times the
		data clock rate (clock mode 2, 3, 6, 7). */
	SAF82525_Write_Register(ABGR,HDLC_BautRate);	//k = (0x11 + 1)*2 =36 <== >设置波特率为19200 bps		11059200 / 19200 =576 /16 =36
	
	/*中断状态位，为0表示相应位的中断使能*/
	SAF82525_Write_Register(AMASK,/*~(RME|RPF|XPR|EXA)*/(RSC|TIN|XPR/*|ICA|EXB|EXA*/));	//使能RPF, RME, XPR中断

	/*复位接受，发送状态*/	
	SetACMDRegister(RHR|XRES);
	SAF82525_Write_Register(ACCR1,PU|/*0x5F*/0x5F);	//PU :power up (active)

		/*
		当读取ISTA寄存器，屏蔽中断不会被显示。
		相反，保存的中断 各位将会被复位。
		Masked interrupts are not indicated when reading
		ISTA. Instead, they remain internally stored and will be indicated after the respective MASK bit
		is reset
		*/
		//	b=SAF82525_Read_Register(AISTA);		//不能读取ISTA寄存器，这样会复位MASK寄存器值为0x00；
		//	debugprintf("AMASK:%x\r\n",b);
	
	//	/*复位接受，发送状态*/	
	//	SetACMDRegister(RHR|XRES);

	//	#ifdef DEBUG_PRINTF
	//		a=CheckASTAR();			//W H H  ,注意，要想读取的数据正确，HDLC芯片不能有命令在执行，必须等到命令执行完成再去读取寄存器。
	//		debugprintf("A state=0x%x\r\n",a);
	//		
	//			/*测试使用，可以读取数据 正确*/
	//		a=SAF82525_Read_Register(ACCR1);
	//		b=SAF82525_Read_Register(ASTAR);
	//		debugprintf("ACCR1:%x,ASTAR:%x\r\n",a,b);

	//		a=SAF82525_Read_Register(AMODE);
	//		b=SAF82525_Read_Register(ACCR2);
	//		debugprintf("AMODE:%x,ACCR2:%x\r\n",a,b);
	//	#endif



	
	/*通道B配置*/
	//step2: SAF82525 芯片REG设置
	SAF82525_Write_Register(BCCR1,/*0x5F*/0x5F);	//下电模式| NRZI数据编码 |推免上拉输出|  帧间填充FLAG序列 |选择时钟模式7  (重点，漏极开路输出，和时钟7)
	SAF82525_Write_Register(BMODE,/*0x98*/MDS1|TMD|RAC/*|TLP*/);	//(MDS1 MDS0)transparent mode | (TMD)internal mode | (RAC)HDLC receiver active
	SAF82525_Write_Register(BCCR2,/*0x28*/BDF | TIO);	//(BDF ) BR9 - BR0 [CCR2 BRG] |( TIO) Tx CLKA, Tx CLKB pins are outputs

	/*手册57页 the DPLL is supplied with a reference clock from BRG which is 16 times the
		data clock rate (clock mode 2, 3, 6, 7). */
	SAF82525_Write_Register(BBGR,HDLC_BautRate);	//k = (0x11 + 1)*2 =36 <== >设置波特率为19200 bps		11059200 / 19200 =576 /16 =36
																								//k= (0x02+1)*2 = 6 <== >设置波特率为125000 bps		12000000 / 125000 =96 /16 = 6
	/*中断状态位，为0表示相应位的中断使能*/
	SAF82525_Write_Register(BMASK,/*~(RME|RPF|XPR|EXB)*/(RSC|TIN|XPR/*|ICA|EXA|EXB*/));	//使能RPF, RME, XPR,EXB中断

	/*复位接受，发送状态*/	
	SetBCMDRegister(RHR|XRES);
	SAF82525_Write_Register(BCCR1,PU|/*0x5F*/0x5F);	//PU :power up (active)
		
	//	/*复位接受，发送状态*/	
	//SetBCMDRegister(RHR|XRES);

	//	#ifdef DEBUG_PRINTF
	//		b=CheckBSTAR();				//当有命令在执行的时候，读取寄存器的值 会不准。当命令执行完成的时候读取数据就准确了。
	//		debugprintf("B state=0x%x\r\n",b);
	//		
	//		/*测试使用，可以读取数据 正确*/
	//		a=SAF82525_Read_Register(BCCR1);
	//		b=SAF82525_Read_Register(BSTAR);
	//		debugprintf("BCCR1:%x,BSTAR:%x\r\n",a,b);

	//		a=SAF82525_Read_Register(BMODE);
	//		b=SAF82525_Read_Register(BCCR2);
	//		debugprintf("BMODE:%x,BCCR2:%x\r\n",a,b);
	//	#endif
}

/*
	检测SAB82525状态函数   
*/
uint8_t CheckASTAR(void)		
{
	uint8_t ucStatus;
	while(1)
	{
		ucStatus = SAF82525_Read_Register(ASTAR);
		if((ucStatus&CEC)==0x00) 	/* CMDR可写 */
		{
			break;
		}
	}
	return ucStatus;
}

/*
	检测SAB82525状态函数   
*/
uint8_t CheckBSTAR(void)		
{
	uint8_t ucStatus;
	while(1)
	{
		ucStatus = SAF82525_Read_Register(BSTAR);
		if((ucStatus&CEC) == 0x00) 	/* CMDR可写 */
		{
			break;
		}
	}
	return ucStatus;
}


/*
	手册87页

CEC :
//  0 : no command is currently executed the CMDR Reg can be written to.
//  1 : a command(written previously to CMDR) is currently executed.no
//      further command can be temporarily written via CMDR Register.

	The maximum time between writing to the CMDR register and the execution of the
	command is 2.5 clock cycles. Therefore, if the CPU operates with a very high clock in
	comparison with the HSCX’s clock, it's recommended that the CEC bit of the STAR
	register is checked before writing to the CMDR register to avoid any loss of commands.
	
	写入到CMDR寄存器 和 命令执行最大的时间是2.5个时钟周期，如果cpu的操作时钟比 HSCX的时钟非常高时，
	建议在写入CMDR寄存器之前，检测STAR寄存器的CEC位(0,表示CMDR可以写入)，以避免命令丢失。
*/
/*
	设置通道A的命令寄存器
*/
void SetACMDRegister(uint8_t CMD_Data)
{
	uint8_t ucStatus;
	while(1)					//WAIT_UNTIL_NO_CMD_EXECUTE,等待直到没有cmd命令在执行
	{
		ucStatus = SAF82525_Read_Register(ASTAR);
		if((ucStatus & CEC)==0x00) 	/* CMDR可写 */
		{
			SAF82525_Write_Register(ACMDR,CMD_Data);
			break;
		}
	}
}

/*
	设置通道B的命令寄存器
*/
void SetBCMDRegister(uint8_t CMD_Data)
{
	uint8_t ucStatus;
	while(1)					//WAIT_UNTIL_NO_CMD_EXECUTE,等待直到没有cmd命令在执行
	{
		ucStatus = SAF82525_Read_Register(BSTAR);
		if((ucStatus & CEC)==0x00) 	/* CMDR可写 */
		{
			SAF82525_Write_Register(BCMDR,CMD_Data);
			break;
		}
	}
}

/*
	A通道发送缓冲区写使能
*/
__inline void WaitXFIFOWriteEnable(uint8_t ChanleRegister)
{
	uint8_t ucStatus;
	while(1)
	{
		ucStatus = SAF82525_Read_Register(ChanleRegister);
		if((ucStatus & XFW_CEC) == XFW)	// 首先等待发送缓冲区写使能  WAIT_UNTIL_XFIFO_WRITE_ENABLE 
		{
			break;
		}
	}
}


/*
	//A通道发送数据
*/
void AChanleTxData(uint8_t *buf,int len)
{
	int RecordLen;	//记录数据长度
	u8  RecordWriteCount =0 ;//记录写入32字节的次数
	u8  i=0;						//用于for
	
	RecordLen = len;		//数据长度记录

	while(RecordLen > SAF82525_FIFO_LEN)		//发送长度大于32字节部分
	{
		// 首先等待发送缓冲区写使能  WAIT_UNTIL_XFIFO_WRITE_ENABLE 
		WaitXFIFOWriteEnable(ASTAR);		//while  whh
		
		for(i=0;i<SAF82525_FIFO_LEN;i++)
		{
			SAF82525_Write_Register(AFIFO,buf[i+RecordWriteCount*SAF82525_FIFO_LEN]);
		}
		
		SetACMDRegister(XTF);					//while  whh
		
		RecordLen -= SAF82525_FIFO_LEN;
		RecordWriteCount++;
	}
	

	//发送长度剩余部分 或者 小于等于32字节
	{
			// 首先等待发送缓冲区写使能  WAIT_UNTIL_XFIFO_WRITE_ENABLE 
		WaitXFIFOWriteEnable(ASTAR);		//while  whh

		for(i=0;i<RecordLen;i++)
		{
			SAF82525_Write_Register(AFIFO,buf[i+RecordWriteCount*SAF82525_FIFO_LEN]);
		}
				
		SetACMDRegister(XTF | XME);		//while  whh
	}
			
}

/*
//B通道发送数据
*/
void BChanleTxData(uint8_t *buf,int len)
{
	int RecordLen;	//记录数据长度
	u8  RecordWriteCount =0 ;//记录写入32字节的次数
	u8  i=0;						//用于for
	
	RecordLen = len;

	while(RecordLen > SAF82525_FIFO_LEN)		//发送长度大于32字节部分
	{
		// 首先等待发送缓冲区写使能  WAIT_UNTIL_XFIFO_WRITE_ENABLE 
		WaitXFIFOWriteEnable(BSTAR);		//while  whh
		
		for(i=0;i<SAF82525_FIFO_LEN;i++)
		{
			SAF82525_Write_Register(BFIFO,buf[i+RecordWriteCount*SAF82525_FIFO_LEN]);
		}
		
		SetBCMDRegister(XTF);					//while  whh
		
		RecordLen -= SAF82525_FIFO_LEN;
		RecordWriteCount++;
	}
	

	//发送长度剩余部分 或者 小于等于32字节
	{
			// 首先等待发送缓冲区写使能  WAIT_UNTIL_XFIFO_WRITE_ENABLE 
		WaitXFIFOWriteEnable(BSTAR);		//while  whh
		
		for(i=0;i<RecordLen;i++)
		{
			SAF82525_Write_Register(BFIFO,buf[i+RecordWriteCount*SAF82525_FIFO_LEN]);
		}
		
		SetBCMDRegister(XTF | XME);			//while  whh
	}
}


/*
//A通道接受数据

手册83页
The ICA, EXA, and EXB bit are present in channel B only and point to the ISTA (CHA),
EXIR (CHA), and EXIR (CHB) registers.
After the HSCX has requested an interrupt by turning its INT pin to low, the CPU must
first read the ISTA register of channel B and check the state of these bits in order to
determine which interrupt source(s) of which channel(s) has caused the interrupt. More
than one interrupt source may be indicated by a single interrupt request

当中断来的时候，先检测B通道的ISTA的ICA，EXA，EXB为，为了明确中断源，中断通道。

手册76页
RME 中断之后，最后一个字节为RSTA 接受状态值。 RBCH，RBCL寄存器中存储数据长度，
*/
void AChanleRxData(__IO uint8_t *buf,__IO uint16_t *len)
{

	/*
	说明
		1. 可能产生RPF中断.
		2. 考虑包超过32B FIFO大小的情况.
		3．RME:Receive Message End. XPR：Transmit Pool Ready.
		4. 因为包大小超过256B，所以使用RBCL,RBCH。
		5. 检测RME中断（收），XPR中断（发）.
	*/
	//	uint8_t  m_ucISTAStatus,m_ucEXIRStatus,m_ucRSTAStatus;
	uint8_t m_ucRBCL,m_ucRBCH;
	uint8_t  i=0;
	uint16_t RxLen,RecordTempLen;
	static uint8_t FrameSectionRxCount=0;
	
	//	m_ucISTAStatus = SAF82525_Read_Register(AISTA);
	//	m_ucEXIRStatus = SAF82525_Read_Register(AEXIR);
	//	m_ucRSTAStatus = SAF82525_Read_Register(ARSTA);
	
	#ifdef DEBUG_PRINTF
	//	debugprintf("AAAm_ISTA=0x%x,m_EXIR=0x%x,m_RSTA=0x%x\r\n",g_ucAISTAStatus,g_ucAEXIRStatus,g_ucARSTAStatus);
	//	debugprintf("Acount=%d\r\n",FrameSectionRxCount);
	#endif 
	
	/*处理A通道EXIR指示中断*/
	/*在状态A寄存器中不显示 ICA，EXA，EXB*/
	//if((g_ucAISTAStatus & EXA) == EXA)
	//{
		if((g_ucAEXIRStatus & RFO) == RFO)	//表示接收帧溢出
		{
			#ifdef DEBUG_PRINTF
	//				debugprintf("A_RFO!\r\n");
	//				debugprintf("A_溢出!\r\n");
	//				debugprintf("A_溢出!\r\n");
	//				debugprintf("A_溢出!\r\n");
	//				debugprintf("A_溢出!\r\n");
	//				debugprintf("A_溢出!\r\n");
	//				debugprintf("A_溢出!\r\n");
			#endif
					
			/* 接收帧溢出--RECEIVE FRAME OVERFLOW */
			/* 复位接收FIFO及接收器RECEIVER     */
			SetACMDRegister(RHR);
		}
	//}
	

	/* 处理A通道RPF中断  */
	if((g_ucAISTAStatus & RPF) == RPF)
	{
		#ifdef DEBUG_PRINTF
	//		debugprintf("A_RPF!\r\n");
		#endif 

		 /* 收到>32字节的帧--RECEIVE POOL FULL   */
		for(i=0;i<SAF82525_FIFO_LEN;i++)
		{
			buf[i+FrameSectionRxCount*SAF82525_FIFO_LEN]=SAF82525_Read_Register(AFIFO);
		}
		SetACMDRegister(RMC);//发送接受完成数据，让HSCX可以接受数据
	
		//SAF82525_ModifyBit_Register(AISTA,RPF,0);//清除中断位，读取中断寄存器的时候，中断就被清除了。
		FrameSectionRxCount++;		
	}

	
  /* 处理A通道RME中断  */
	if((g_ucAISTAStatus & RME) == RME)
	{
		#ifdef DEBUG_PRINTF
	//		debugprintf("A_RME!\r\n");
		#endif 
		
		 /* 收到<32字节的帧--RECEIVE MESSAGE END */
		 /* 收到有效的帧--VALID FRAME RECEIVED,且CRC正确,没有RDO--RECEIVE DATA OVERFLOW  */
		if( ((g_ucARSTAStatus & SAB_CRC) == SAB_CRC ) && ((g_ucARSTAStatus & VFR) == VFR) && ((g_ucARSTAStatus & RDO) != RDO ))
		{
			//AHDLC_Int_Flag = 1;		
			g_HDLCARxEndFlag = 1;	  //接受完成中断标志位  //WHH
			
			m_ucRBCL = SAF82525_Read_Register(ARBCL);
			m_ucRBCH = SAF82525_Read_Register(ARBCH);

			//计算得到数据的长度
			RxLen	= (((uint16_t)m_ucRBCH & 0x0f)<<8) | m_ucRBCL;	
			
			/* 帧长度 */
			*len = RxLen-1;		//将帧接受的长度返回,接受寄存器中最后一个字节为接受状态值
			
			/*计算一帧数据中，剩余的长度*/
			RecordTempLen = RxLen - (FrameSectionRxCount * SAF82525_FIFO_LEN)-1;		//RME中断之后，接受寄存器中最后一个字节为接受状态值
			
			//if((RecordTempLen>0) && (RecordTempLen <=32))
			//{
				for(i=0;i<RecordTempLen;i++)
				{
					buf[i+FrameSectionRxCount*SAF82525_FIFO_LEN] = SAF82525_Read_Register(AFIFO);
				}
				
				SetACMDRegister(RMC);//发送接受完成数据，让HSCX可以接受数据
			//}
			//SAF82525_ModifyBit_Register(AISTA,RME,0);//清除中断位，读取中断寄存器的时候，中断就被清除了。
		}
		
		else  /* 无效的帧 */
		{
			#ifdef DEBUG_PRINTF
			//			debugprintf("A_error!\r\n");
			#endif
			
			/* 复位接收FIFO及接收器RECEIVER     */
			SetACMDRegister(RHR);
		}
		
		FrameSectionRxCount=0;//接受一帧数据结束，将静态变量置0
	}
	 /* 对于沿触发中断方式的CPU,当还有中断为处理完时,以下指令可再次产生中断 手册40页 */	
	//	SAF82525_Write_Register(AMASK,0xFF);
	//	SAF82525_Write_Register(AMASK,(RSC|TIN|XPR/*|ICA|EXB|EXA*/));	//使能RPF, RME, XPR中断
}	

/*
//B通道接受数据
*/
void BChanleRxData(__IO uint8_t *buf,__IO uint16_t  *len)
{
	//	uint8_t  m_ucISTAStatus,m_ucEXIRStatus,m_ucRSTAStatus;
	uint8_t m_ucRBCL,m_ucRBCH;
	uint8_t  i=0;
	uint16_t RxLen,RecordTempLen;
	static uint8_t FrameSectionRxCount=0;
		
	//	m_ucISTAStatus = SAF82525_Read_Register(BISTA);
	//	m_ucEXIRStatus = SAF82525_Read_Register(BEXIR);
	//	m_ucRSTAStatus = SAF82525_Read_Register(BRSTA);
	//	
	#ifdef DEBUG_PRINTF
	//	debugprintf("BBBm_ISTA=0x%x,m_EXIR=0x%x,m_RSTA=0x%x\r\n",g_ucBISTAStatus,g_ucBEXIRStatus,g_ucBRSTAStatus);
	//	debugprintf("Bcount=%d\r\n",FrameSectionRxCount);
	#endif
	
	/*处理B通道EXIR指示中断*/
	/*判读这个寄存器 多此一举*/
	//	if((g_ucBISTAStatus & EXB) == EXB)
	//	{
		if((g_ucBEXIRStatus & RFO) == RFO)	//表示接收帧溢出
		{
			#ifdef DEBUG_PRINTF
	//					debugprintf("B_RFO!\r\n");
	//				debugprintf("B_溢出!\r\n");
	//				debugprintf("B_溢出!\r\n");
	//				debugprintf("B_溢出!\r\n");
	//				debugprintf("B_溢出!\r\n");
	//				debugprintf("B_溢出!\r\n");
	//				debugprintf("B_溢出!\r\n");
			#endif
			
				/* 接收帧溢出--RECEIVE FRAME OVERFLOW */
			  /* 复位接收FIFO及接收器RECEIVER     */
				SetBCMDRegister(RHR);
				return ;
		}
	//	}
	
	/* 处理B通道RPF中断  */
	if((g_ucBISTAStatus & RPF) == RPF)
	{
		#ifdef DEBUG_PRINTF
		//			debugprintf("B_RPF!\r\n");
		#endif
		
		 /* 收到>32字节的帧--RECEIVE POOL FULL   */
		for(i=0;i<SAF82525_FIFO_LEN;i++)
		{
			buf[i+FrameSectionRxCount*SAF82525_FIFO_LEN]=SAF82525_Read_Register(BFIFO);
		}
		SetBCMDRegister(RMC);//发送接受完成数据，让HSCX可以接受数据
		
		//SAF82525_ModifyBit_Register(BISTA,RPF,0);//清除中断位,其实在读取中断寄存器的时候，中断就被清除了。
		FrameSectionRxCount++;
	}
	
  /* 处理B通道RME中断  */
	if((g_ucBISTAStatus & RME) == RME)
	{
		#ifdef DEBUG_PRINTF
	//			debugprintf("B_RME!\r\n");
		#endif
		
		 /* 收到<32字节的帧--RECEIVE MESSAGE END */
		 /* 收到有效的帧--VALID FRAME RECEIVED,且CRC正确,没有RDO--RECEIVE DATA OVERFLOW  */
		if( ((g_ucBRSTAStatus & SAB_CRC) == SAB_CRC ) && ((g_ucBRSTAStatus & VFR) == VFR) && ((g_ucBRSTAStatus & RDO) != RDO ))
		{
			
			//BHDLC_Int_Flag = 1;		//设置B通道的接受中断标志，读取中断寄存器的时候，中断就被清除了。
			g_HDLCBRxEndFlag = 1;	  //接受完成中断标志位  //WHH
			
			m_ucRBCL = SAF82525_Read_Register(BRBCL);
			m_ucRBCH = SAF82525_Read_Register(BRBCH);

			//计算得到数据的长度
			RxLen	= (((uint16_t)m_ucRBCH & 0x0f)<<8) |m_ucRBCL;		
			/* 帧长度 */
			*len = RxLen-1;		//将帧接受的长度返回，接受寄存器中最后一个字节为接受状态值
			
			/*计算一帧数据中，剩余的长度*/
			RecordTempLen = RxLen - (FrameSectionRxCount * SAF82525_FIFO_LEN)-1;		//RME中断之后，接受寄存器中最后一个字节为接受状态值

			//if((RecordTempLen>0) && (RecordTempLen <=32))
			//{
				for(i=0;i<RecordTempLen;i++)
				{
					buf[i+FrameSectionRxCount*SAF82525_FIFO_LEN] = SAF82525_Read_Register(BFIFO);
				}
				SetBCMDRegister(RMC);//发送接受完成数据，让HSCX可以接受数据
			//}
			//SAF82525_ModifyBit_Register(BISTA,RME,0);//清除中断位   ,其实在读取中断寄存器的时候，中断就被清除了。
		}
		
		else  /* 无效的帧 */
		{
			#ifdef DEBUG_PRINTF
	//				debugprintf("B_error!\r\n");
			#endif
			
			/* 复位接收FIFO及接收器RECEIVER     */
			SetBCMDRegister(RHR);
		}
		
		FrameSectionRxCount=0;//接受一帧数据结束，将静态变量置0
	}
	 /* 对于沿触发中断方式的CPU,当还有中断为处理完时,以下指令可再次产生中断 手册40页 */	
	//	SAF82525_Write_Register(BMASK,0xFF);
	//	SAF82525_Write_Register(BMASK,(RSC|TIN|XPR/*|ICA|EXA|EXB*/));	//使能RPF, RME, XPR中断
}	

