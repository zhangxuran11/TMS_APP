#include "MTD_CPU_CAN3_MCP2515.h"
#include "MCP2515_BitTime.h"
#include "delay.h"
#include "Uart.h"
#include "os.h"
#include "string.h"
#include "can_stm32.h"
#include "canfestival.h"

extern CO_Data MTDCAN3Master_Data;
extern OS_MUTEX	CAN3_MUTEX;		//定义一个互斥信号量,用于can3发送接受任务的互斥

#define MCP2515_INT1_RCC           RCC_AHB1Periph_GPIOG
#define MCP2515_INT1_GPIO          GPIOG
#define MCP2515_INT1_GPIO_PIN      GPIO_Pin_7
#define MCP2515_INT1_EXIT_PORT		 EXTI_PortSourceGPIOG
#define MCP2515_INT1_EXIT_PIN			 EXTI_PinSource7


void	MCP2515_INT1_GpioInit(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(MCP2515_INT1_RCC, ENABLE); 		//使能IO时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //设置上接
  GPIO_InitStructure.GPIO_Pin = MCP2515_INT1_GPIO_PIN;  //IO口为7
  GPIO_Init(MCP2515_INT1_GPIO, &GPIO_InitStructure);    //如上配置gpio

  SYSCFG_EXTILineConfig(MCP2515_INT1_EXIT_PORT, MCP2515_INT1_EXIT_PIN);//实始化中断线7

  EXTI_InitStructure.EXTI_Line = EXTI_Line7;                   //配置中断线为中断线7
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

/*mcp2515 中断线7*/
//extern void MCP2515_INT1_IRQ(void);
void EXTI9_5_IRQHandler(void)
{
		MCP2515_INT1_IRQ();
}


void MCP2515_INT1_IRQ(void)
{
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	

    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
		{
			OS_ERR err;
			
			//发送信号量
			OSSemPost(&can3recv_sem,OS_OPT_POST_1,&err);		
			//printf("RxOs3sem=%d\r\n",can3recv_sem.Ctr);	
			
			//g_can3RxCount++;
			//printf("RxOs3=%d\r\n",g_can3RxCount);
			EXTI_ClearITPendingBit(EXTI_Line7);		//清除中断标志 中断线7
    }
				
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    	//退出中断
	#endif
}


unsigned char MTDCPU_CAN3_MCP2515_Init(const unsigned int CanBaudRate)
{  
//	unsigned char dummy; 
	unsigned char res;
	
    /*First initialize the SPI periphere*/ 
    /*Then initialize the MCP2515.Step as follow: 
    1>Get into configuration mode by reset MCP2515 or write the bit of CANCTRL.REQOP. 
    2>Check the CANSTAT by ReadByte_MCP2515(CANSTAT) if MCP2515 is already into configuration mode. 
    3>Configurate the registers to set the baudrate:CN1,CN2,CN3. 
    4>Configurate TXRTSCTRL to set the function of TXnRTS pin. 
    5>Configurate TRXBnCTRL to set the pirority of transmit mailbox:TXB0,TXB1,TXB2. 
    6>Configurate TXBn'SID ,EID and DLC.This step can configurate when you are in normal mode. 
    7>Configurate RXFnSIDH and RXFnSIDL to resceive the specific ID. 
    8>Configurate RXMnSIDH and RXMnSIDL to mask the RXFnSID's ID. 
    9>Configurate CANINTE to enable or disable the interrupts. 
    10>Return to the normal mode and double check if it is return to the normal mode by CANSTAT. 
    */   
	
	//	MCP2515_REST1_GpioInit();  //复位引脚初始化		MTDCPU 板子的复位方式是硬件电源直接复位
	
	MCP2515_INT1_GpioInit();	//中断引脚初始化
	
	MTDCPU_SPI1_Init();		//初始化MCU的SPI总线
	
	debugprintf("mpc2515_init!\r\n");
	
	/*Flip into the Configuration Mode*/ 
	do{
		 //MCP2515复位,在上电或复位时，器件会自动进入配置模式
		MTDCPU_CAN3_MCP2515_Reset();		//发送复位指令软件复位MCP2515
		res = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT) & REQOP_CONFIG ;
	}while(res != REQOP_CONFIG);	// 判断是否进入配置模式

	
	//	res = MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(REQOP_CONFIG); 	 //设置为配置模式
	//	if(res == MCP2515_FAIL)	//设置失败直接返回
	//	{
	//		return res; 	/* function exit on error */
	//	}
	
	//设置为配置模式
	MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,REQOP_CONFIG); 
	
	res = MTDCPU_CAN3_MCP2515_Config_Rate(CanBaudRate);		//设置波特率
	if(res == MCP2515_OK)
	{
		MTDCPU_CAN3_MCP2515_Init_CAN_Buffer();  //清空屏蔽，滤波，发送，接受寄存器
		
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		MTDCPU_CAN3_MCP2515_Register_BitModify(RXB0CTRL,RXM /*| BUKT*/,/*RXM_VALID_ALL*/RXM_RCV_ALL /*| BUKT_ROLLOVER*/);	//设置接受控制寄存器0,接受模式00，接受滚存
		MTDCPU_CAN3_MCP2515_Register_BitModify(RXB1CTRL,RXM,/*RXM_VALID_ALL*/ RXM_RCV_ALL);								//设置接受控制寄存器1，接受所有id
		//	MTDCPU_CAN3_MCP2515_Set_Register(RXB0CTRL,0x00);// 输入缓冲器0控制寄存器, 接收所有符合滤波条件的报文，滚存禁止
		//	MTDCPU_CAN3_MCP2515_Set_Register(RXB1CTRL,0x00);
		
		
		//配置中断
		/*错误标志寄存器 不能使能，会导致进入错误中断*/
		//MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTE,MERRE | ERRIE | RX0IE /*| RX1IE*/,IVRE_ENABLED|ERRIE_ENABLED |RX0IE_ENABLED /*G_RXIE_ENABLED*/);  //0,1全部接受中断使能  //使能ERRIE：错误中断使能位\报文接收中断RX0IE\RX1IE
		MTDCPU_CAN3_MCP2515_Set_Register(CANINTE,/*MERRE|*/ERRIE|RX0IE/*|RX1IE|TX0IE|TX1IE|TX2IE*/);
		MTDCPU_CAN3_MCP2515_Set_Register(CANINTF,0x00);
		
		//复位MCP2515的错误标志寄存器
		MTDCPU_CAN3_MCP2515_Set_Register(EFLG,0x00);

		//配置相关引脚
		MTDCPU_CAN3_MCP2515_Set_Register(TXRTSCTRL,0x00);	// TXnRST作为数字引脚，非发送请求引脚
		MTDCPU_CAN3_MCP2515_Set_Register(BFPCTRL,0x00);		// RXnRST禁止输出
	}

	//设置为正常模式
	MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,REQOP_NORMAL); 
	do{
		res = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT) & REQOP_NORMAL ;
	}while( res != REQOP_NORMAL);

	//	res= MTDCPU_CAN3_MCP2515_Read_Register(CNF2);		//读取波特率寄存器
	//	debugprintf("cnf2=0x%x\r\n",res);
	
	return res;		
				
}	


/** 
  * @brief  Reset the MCP2515 then U go into the configruation mode 
  * @param  none 
  * @retval none 
  */ 
void MTDCPU_CAN3_MCP2515_Reset(void)	     //MCP2515器件复位
{
   MTDCPU_SPI1_CS_LOW();										/* 片选口线置低选中  */
   MTDCPU_SPI1_SendByte(SPICAN_RESET);		//发送寄存器复位命令
   MTDCPU_SPI1_CS_HIGH();									/* 片选口线置高不选中 */
	 delay_ms(10);
}

/*读取寄存器的值*/
unsigned char MTDCPU_CAN3_MCP2515_Read_Register(const unsigned char Addr)
{
    unsigned char Data;   
	
    MTDCPU_SPI1_CS_LOW();                //置MCP2515的CS为低电平
    MTDCPU_SPI1_SendByte(SPICAN_READ);   //发送读命令
    MTDCPU_SPI1_SendByte(Addr); 				 //发送地址	
    Data = MTDCPU_SPI1_ReadByte();   		 //读一字节 
    MTDCPU_SPI1_CS_HIGH();               //置MCP2515的CS为高电平 
    return Data; 
}

/*********read the several register*********************/
void MTDCPU_CAN3_MCP2515_Read_RegisterS(const unsigned char Addr, unsigned char RegValues[], const unsigned char RegNum)
{
		unsigned char i;
		
	  MTDCPU_SPI1_CS_LOW();                //置MCP2515的CS为低电平
		MTDCPU_SPI1_SendByte(SPICAN_READ);   //发送读命令
    MTDCPU_SPI1_SendByte(Addr); 				 //发送地址	
	
		// mcp2515 has auto-increment of address-pointer	
		for(i=0;i<RegNum;i++)
		{
			RegValues[i]=MTDCPU_SPI1_ReadByte();	//读取数据
		}
		MTDCPU_SPI1_CS_HIGH();               //置MCP2515的CS为高电平 
}

//设置寄存器的值
/** 
  * @brief  Send a byte to MCP2515 through SPI to return corresponding status you need 
  * @param  Add:Address of register 
  * @param  Data:the data U need to be sent from MCU to MCP2515 
  * @retval none 
  */ 
void MTDCPU_CAN3_MCP2515_Set_Register(const unsigned char Addr,const unsigned char Data)
{
	  MTDCPU_SPI1_CS_LOW();   						//使能片选     
    MTDCPU_SPI1_SendByte(SPICAN_WRITE); //发送写命令
    MTDCPU_SPI1_SendByte(Addr); 				//发送地址				
    MTDCPU_SPI1_SendByte(Data); 				//写入数据
    MTDCPU_SPI1_CS_HIGH(); 							//关闭片选
}


void MTDCPU_CAN3_MCP2515_Set_RegisterS(const unsigned char Addr,const unsigned char SetValues[],const unsigned char RegNum) //设置多个寄存器的值
{
		unsigned char i;
		MTDCPU_SPI1_CS_LOW();   						//使能片选
	  MTDCPU_SPI1_SendByte(SPICAN_WRITE); //发送写命令
    MTDCPU_SPI1_SendByte(Addr); 				//发送地址	
	
		// mcp2515 has auto-increment of address-pointer
		for(i=0;i<RegNum;i++)
		{
			MTDCPU_SPI1_SendByte(SetValues[i]);//写入数据
		}
    MTDCPU_SPI1_CS_HIGH(); 							//关闭片选		
		
		//		unsigned char i;
		//		unsigned char address;
		//		address = Addr;
		//		for(i=0;i<RegNum;i++)
		//		{
		//			MTDCPU_CAN3_MCP2515_Set_Register(address,SetValues[i]);
		//			address ++;
		//		}
}


/** 
  * @brief  Modify the bit of the register 
  * @param  Add:Address of register 
  * @param  Mask:1-->U can modify the corresponding bit;0-->U can not modify the corresponding bit 
  * @param  Data:the data U need to be sent from MCU to MCP2515 
  * @retval none 
  */ 
void MTDCPU_CAN3_MCP2515_Register_BitModify(const unsigned char Addr,const unsigned char Mask,unsigned char Data) 
{ 
    MTDCPU_SPI1_CS_LOW();                			//使能片选
    MTDCPU_SPI1_SendByte(SPICAN_BIT_MODIFY); 	//发送位修改命令
    MTDCPU_SPI1_SendByte(Addr); 							//发送地址
    MTDCPU_SPI1_SendByte(Mask); 							//发送掩码
    MTDCPU_SPI1_SendByte(Data); 							//写入数据
    MTDCPU_SPI1_CS_HIGH();    								//关闭片选
}

//读取寄存器状态
unsigned char MTDCPU_CAN3_MCP2515_Read_Status(unsigned char cmd)
{
		unsigned char State;
		MTDCPU_SPI1_CS_LOW();                				//使能片选
		MTDCPU_SPI1_SendByte(/*SPICAN_READ_STATUS*/cmd); 	//发送读状态命令
		State = MTDCPU_SPI1_ReadByte();							//读取状态
		MTDCPU_SPI1_CS_HIGH();											//关闭片选
		
		return State;
}


 //读取接受的状态
unsigned char MTDCPU_CAN3_MCP2515_ReadRx_Status(void)
{
		unsigned char RxState;
		MTDCPU_SPI1_CS_LOW();                				//使能片选
		MTDCPU_SPI1_SendByte(SPICAN_RX_STATUS); 		//发送读状态命令
		RxState = MTDCPU_SPI1_ReadByte();						//读取状态
		MTDCPU_SPI1_CS_HIGH();											//关闭片选
		
		return RxState;
}


/*
//设置控制寄存器 的工作模式
	返回0成功，返回1设置失败。
*/
unsigned char MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(const unsigned char NewMode)	
{
		unsigned char mode;
		MTDCPU_CAN3_MCP2515_Register_BitModify(CANCTRL,REQOP,NewMode);	//修改模式
		
		//verify as advised in datasheet
		mode = MTDCPU_CAN3_MCP2515_Read_Register(CANCTRL);
		mode &= REQOP;
		if(mode == NewMode)
		{
			return MCP2515_OK;
		}
		else
		{
			return MCP2515_FAIL;
		}
}


/*
	配置mcp2515 的波特率
	返回0成功，返回1失败。
     * The CAN bus bit time (tbit) is determined by:
     *   tbit = (SyncSeg + PropSeg + PS1 + PS2) * TQ
     * with:
     *     SyncSeg = 1
     *     sample point (between PS1 and PS2) must be at 60%-70% of the bit time
     *     PropSeg + PS1 >= PS2
     *     PropSeg + PS1 >= Tdelay
     *     PS2 > SJW
     *     1 <= PropSeg <= 8, 1 <= PS1 <=8, 2 <= PS2 <= 8
     * SJW = 1 is sufficient in most cases.z
     * Tdelay is usually 1 or 2 TQ.
     
*/
unsigned char MTDCPU_CAN3_MCP2515_Config_Rate(const unsigned int  BaudRate)
{
		unsigned char Set,cfg1,cfg2,cfg3;
	
	//unsigned char canctrl;
	//	{
	//		int tqs; /* tbit/TQ */
	//    int brp;
	//    int ps1, ps2, propseg, sjw;
	//		
	//	/* Determine the BRP value that gives the requested bit rate. */
	//    for(brp = 0; brp < 25; brp++) 
	//		{
	//			tqs = CANFOSC/*20M晶振*/ / (2 * (brp + 1)) / BaudRate;
	//			if (tqs >= 5 && tqs <= 25	&& (CANFOSC/*20M晶振*/ / (2 * (brp + 1)) / tqs) == BaudRate)
	//				break;
	//    }
	//		 /* The CAN bus bit time (tbit) is determined by:
	//     *   tbit = (SyncSeg + PropSeg + PS1 + PS2) * TQ
	//     * with:
	//     *     SyncSeg = 1
	//     *     sample point (between PS1 and PS2) must be at 60%-70% of the bit time
	//     *     PropSeg + PS1 >= PS2
	//     *     PropSeg + PS1 >= Tdelay
	//     *     PS2 > SJW
	//     *     1 <= PropSeg <= 8, 1 <= PS1 <=8, 2 <= PS2 <= 8
	//     * SJW = 1 is sufficient in most cases.z
	//     * Tdelay is usually 1 or 2 TQ.
	//     */
	//    propseg = ps1 = ps2 = (tqs - 1) / 3;

	//    if (tqs - (1 + propseg + ps1 + ps2) == 2)
	//			ps1++;
	//    if (tqs - (1 + propseg + ps1 + ps2) == 1)
	//			ps2++;
	//    sjw = 1;
	//		
	//		debugprintf("bit rate: BRP = %d, Tbit = %d TQ, PropSeg = %d, PS1 = %d, PS2 = %d, SJW = %d\n", \
	//	    brp, tqs, propseg, ps1, ps2, sjw);
	//		canctrl = MTDCPU_CAN3_MCP2515_Read_Register(CANCTRL);			//读出当前状态		whh
	//		MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(REQOP_CONFIG);//修改模式为 配置模式、	whh
	//		
	//		MTDCPU_CAN3_MCP2515_Set_Register(CNF1,((sjw-1) << 6) | brp);		//设置波特率
	//		MTDCPU_CAN3_MCP2515_Set_Register(CNF2,BTLMODE | ((ps1-1) << 3) | (propseg-1));
	//		MTDCPU_CAN3_MCP2515_Register_BitModify(CNF3,PHSEG2, (ps2-1));
	//		
	//		MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,canctrl);	//返回为之前的模式		whh
	//		
	//		return MCP2515_OK;
	//	}
		
	
		Set = 0;
		
		switch (BaudRate)
		{
			case (MCP2515_CAN_BAUD_125K):
						cfg1 = MCP2515_20MHz_CAN_BAUD_125K_CFG1;
						cfg2 = MCP2515_20MHz_CAN_BAUD_125K_CFG2;
						cfg3 = MCP2515_20MHz_CAN_BAUD_125K_CFG3;
						Set =1;
						break;
			case	MCP2515_CAN_BAUD_500K:
						cfg1 = MCP2515_20MHz_CAN_BAUD_500K_CFG1;
						cfg2 = MCP2515_20MHz_CAN_BAUD_500K_CFG2;
						cfg3 = MCP2515_20MHz_CAN_BAUD_500K_CFG3;
						Set = 1;
						break;
			default :
						Set = 0;
						break;
		}
		
		if(Set)
		{
	//			canctrl = MTDCPU_CAN3_MCP2515_Read_Register(CANCTRL);			//读出当前状态		whh
	//			MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(REQOP_CONFIG);//修改模式为 配置模式、	whh
			//Set the baudrate 				Fsoc=20Mhz
			//set CNF1,SJW=00,lengthe is 1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/20M=5us 
			MTDCPU_CAN3_MCP2515_Set_Register(CNF1,cfg1);		//设置波特率
			//set CNF2,SAM=0,caputre one time,PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ 
			MTDCPU_CAN3_MCP2515_Set_Register(CNF2,cfg2);
			//set CNF3,PHSEG2=(2+1)TQ=3TQ,when CANCTRL.CLKEN=1,set CLKOUT pin to output 
			MTDCPU_CAN3_MCP2515_Set_Register(CNF3,cfg3);

	//			MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,canctrl);	//返回为之前的模式		whh			
			return MCP2515_OK;
		}
		else
		{
			return MCP2515_FAIL;
		}
}

/*
	//读取接收到的canid标识
	返回 是否是扩展帧，CAN_ID_STD表示标准，CAN_ID_EXT表示扩展，以及canid	到参数里面
*/
void MTDCPU_CAN3_MCP2515_Read_CAN_ID(const unsigned char RxIdReg_Addr, unsigned char* IDE_ExtFlag, unsigned int* Std_id,unsigned int * Ext_id)
{
		unsigned char  IdBufData[4];
		
		/*初始化*/
		*IDE_ExtFlag = CAN_ID_STD;
		*Std_id	=	0;
		*Ext_id = 0;
	
		MTDCPU_CAN3_MCP2515_Read_RegisterS(RxIdReg_Addr,IdBufData,4);
		
		/*接受的是标准帧id*/
		*Std_id = (IdBufData[0]<<3) + (IdBufData[1]>>5);		//看手册29页，RXBnSIDH，在数组第一个存放
	
		/*接受到的是扩展帧*/
		if((IdBufData[1] & EXIDE_SET) == EXIDE_SET)	//说明是扩展帧
		{
			// extended id
			*Ext_id = (*Std_id<<2) + (IdBufData[1] & 0x03);
			*Ext_id <<= 16;
		  *Ext_id = *Ext_id +((u16)IdBufData[2]<<8) + IdBufData[3];
      *IDE_ExtFlag = CAN_ID_EXT;		//将扩展标识符置于扩展标识符
			*Std_id = 0;  								//将标准id置于0
		}
}


	// Buffer can be RXBUF0 or RXBUF1 。读取接受的can消息数据
void MTDCPU_CAN3_MCP2515_Read_CAN_Message(const unsigned char RxBuf_SIDH_Addr, CanRxMsg* Message)
{
		unsigned char RxReg_Addr, RxControlReg_Value;
	
		RxReg_Addr = RxBuf_SIDH_Addr;
		
		/*获得id*/
		MTDCPU_CAN3_MCP2515_Read_CAN_ID(RxReg_Addr,&(Message->IDE),&(Message->StdId),&(Message->ExtId));	//得到id
		
		RxControlReg_Value = MTDCPU_CAN3_MCP2515_Read_Register(RxReg_Addr-1);		//得多接受控制寄存器值
	
		Message->DLC = MTDCPU_CAN3_MCP2515_Read_Register(RxReg_Addr+4);					//读取数据长度
	
		if(RxControlReg_Value & RXRTR_REMOTE/*0x08*/  || (Message->DLC &0x40) )		//说明是远程帧
		{
				Message->RTR = CAN_RTR_REMOTE;		//表示为远程帧
		}
		else
		{
				Message->RTR = CAN_RTR_DATA;			//表示为数据帧
		}
		
		Message->DLC &= 0x0F;   //寄存器的低4位，才是数据长度
		
		MTDCPU_CAN3_MCP2515_Read_RegisterS(RxReg_Addr+5,&(Message->Data[0]),Message->DLC);		//读取数据
}


/*
//写入要发送的canid
IDE_ExtFlag =CAN_ID_EXT 为扩展帧
*/
void MTDCPU_CAN3_MCP2515_Write_CAN_ID(const unsigned char TxIdReg_Addr, const unsigned char IDE_ExtFlag,const unsigned int can_id)
{
		unsigned int canid;
		unsigned char IdBufData[4];
		
		if(IDE_ExtFlag ==CAN_ID_EXT)		//表示为扩展帧
		{
			canid =can_id;
			IdBufData[3] = (unsigned char )(canid & 0xFF);				//EID0
			IdBufData[2] = (unsigned char )(canid >> 8)& 0xFF;		//EID8
			
			canid >>= 16;		//获得高16位
			
			IdBufData[1] = (unsigned char )(canid & 0x03);
			IdBufData[1] +=(unsigned char )((canid & 0x1C)<<3);
			IdBufData[1] |= EXIDE_SET;
			IdBufData[0] = (unsigned char )(canid >> 5)&0xFF;
		}
		else
		{
			canid = (unsigned short)(can_id & 0xFFFF);
			
			IdBufData[0] = (unsigned char )(canid >>3)&0xFF;
			IdBufData[1] = (unsigned char )((canid &0x07) <<5);		//W H H 少了一个括号，导致id错误 
			IdBufData[2] = 0;
			IdBufData[3] = 0;
		}
		
		MTDCPU_CAN3_MCP2515_Set_RegisterS(TxIdReg_Addr,IdBufData,4);		//将id写入寄存器
}

/*
	写入要发送的can消息   
// Buffer can be TXBUF0 TXBUF1 or TXBUF2
*/
void MTDCPU_CAN3_MCP2515_Write_CAN_Message(const unsigned char TxBuf_SIDH_Addr, const CanTxMsg* Message)
{
		unsigned char TxReg_Addr, Dlc;		//地址，数据的长度
	
		TxReg_Addr = TxBuf_SIDH_Addr;
		Dlc = Message->DLC;
		
		MTDCPU_CAN3_MCP2515_Set_RegisterS(TxReg_Addr+5,&(Message->Data[0]),Dlc);		//向寄存器写入数据
		
		if(Message->IDE == CAN_ID_EXT)	//扩展帧
		{
				MTDCPU_CAN3_MCP2515_Write_CAN_ID(TxReg_Addr,Message->IDE,Message->ExtId);		//写入canid
		}
		else		//标准帧
		{
				MTDCPU_CAN3_MCP2515_Write_CAN_ID(TxReg_Addr,Message->IDE,Message->StdId);
		}
		
		if(Message->RTR ==	CAN_RTR_REMOTE )		//表示远程帧
		{
			Dlc |= 0x40;		//手册21页
		}
		
		MTDCPU_CAN3_MCP2515_Set_Register((TxReg_Addr+4),Dlc);		//写入RTR和DLC
		MTDCPU_CAN3_MCP2515_Set_Register(TxBuf_SIDH_Addr-1,TXREQ );	//请求发送  //WHH
}		


/*
 ** Start the transmission from one of the tx buffers.
// Buffer can be TXBUF0 TXBUF1 or TXBUF2
 */
void MTDCPU_CAN3_MCP2515_Start_Transimt(const unsigned char TxBuf_SIDH_Addr)
{
		MTDCPU_CAN3_MCP2515_Register_BitModify(TxBuf_SIDH_Addr-1,TXREQ,TXREQ);
}


/*
	获得 一个空的物理发送缓冲区
*/
unsigned char MTDCPU_CAN3_MCP2515_Get_Free_TxBuf(unsigned char * TxBufNum)
{
		unsigned char TempState, i, ControlRegValue;
		unsigned char ControlReg[3] ={TXB0CTRL,TXB1CTRL,TXB2CTRL};
		
		TempState = MCP2515_ALLTXBUSY;		//初始化发送缓冲区状态。所有的发送缓冲区满	
		*TxBufNum = 0x00;
		
		// check all 3 TX-Buffers
		for(i=0;i<3;i++)
		{
			ControlRegValue = MTDCPU_CAN3_MCP2515_Read_Register(ControlReg[i]);		//读取控制寄存器的值
			if((ControlRegValue & TXREQ )== 0)
			{
				//debugprintf("Selected TX-Buffer %d\r\n", i+1);
				*TxBufNum = ControlReg[i]+1; 	// return SIDH-address of Buffer
				TempState = MCP2515_OK;
				return TempState;
			}
			
		}
		
//		/*WHH*/
//		ControlRegValue = MTDCPU_CAN3_MCP2515_Read_Status(SPICAN_READ_STATUS);
//		
//		if (!(ControlRegValue & CAN_STATE_TX0REQ)) 
//		{
//        *TxBufNum = ControlReg[0];
//				TempState = MCP2515_OK;		
//    }
//		else if (!(ControlRegValue & CAN_STATE_TX1REQ)) 
//		{
//        *TxBufNum = ControlReg[1];
//				TempState = MCP2515_OK;
//    }
//		else if (!(ControlRegValue & CAN_STATE_TX2REQ))
//		{
//        *TxBufNum = ControlReg[2];
//				TempState = MCP2515_OK;	 
//    }		
		
		return TempState;
}


/*
//设置屏蔽寄存器，滤波寄存器，初始化发送缓冲区

*/
void MTDCPU_CAN3_MCP2515_Init_CAN_Buffer(void)
{
		unsigned char i,TxContr1,TxContr2,TxContr3;
	
		/*whh 滤波器，屏蔽寄存器只能在配置模式下设置，为了防止不在配置模式下*/
		//		unsigned char canctrl;
		//		canctrl = MTDCPU_CAN3_MCP2515_Read_Register(CANCTRL);			//读出当前状态		whh
		//		MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(REQOP_CONFIG);//修改模式为 配置模式、	whh	
	
		// TODO: check why this is needed to receive extended and standard frames
		// Mark all filter bits as don't care:  	屏蔽寄存器全部为0，接受所有的canid
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXM0SIDH,0,0);	//初始化接受屏蔽寄存器0，为0，不屏蔽id
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXM1SIDH,0,0);	//初始化接受屏蔽寄存器1

		// Anyway, set all filters to 0:		/*接受缓冲区0 */
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF0SIDH,CAN_ID_EXT,0);	// RXB0: extended 
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF1SIDH,0,0);	//    AND standard
	
																				/*接受缓冲区1，滤波寄存器配置*/
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF2SIDH, CAN_ID_EXT, 0); // RXB1: extended 
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF3SIDH, 0, 0); //  AND standard
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF4SIDH, 0, 0);
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXF5SIDH, 0, 0);
		
	// Clear, deactivate the three transmit buffers		清空，停用三个发送缓冲区
	// TXBnCTRL -> TXBnD7
	
		TxContr1 = TXB0CTRL;
		TxContr2 = TXB1CTRL;
		TxContr3 = TXB2CTRL;
		for(i=0;i<14;i++)				// in-buffer loop
		{
			MTDCPU_CAN3_MCP2515_Set_Register(TxContr1,0);
			MTDCPU_CAN3_MCP2515_Set_Register(TxContr2,0);
			MTDCPU_CAN3_MCP2515_Set_Register(TxContr3,0);
			
			TxContr1++;
			TxContr2++;
			TxContr3++;
		}
		//		MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,canctrl);	//返回为之前的模式		whh
		
		// and clear, deactivate the two receive buffers. 清空，停用两个接受缓存区
		MTDCPU_CAN3_MCP2515_Set_Register(RXB0CTRL,0);
		MTDCPU_CAN3_MCP2515_Set_Register(RXB1CTRL,0);
}

/*
中断错误置位MCP25515芯片
*/
void MTDCPU_CAN3_MCP2515_IntError_Init(const unsigned int  BaudRate)
{
	unsigned char res;

	/*Flip into the Configuration Mode*/ 
	do{
		 //MCP2515复位,在上电或复位时，器件会自动进入配置模式	
	
		//MTDCPU_CAN3_MCP2515_Reset();		//发送复位指令软件复位MCP2515
		MTDCPU_SPI1_CS_LOW();										/* 片选口线置低选中  */
		MTDCPU_SPI1_SendByte(SPICAN_RESET);			//发送寄存器复位命令
		MTDCPU_SPI1_CS_HIGH();									/* 片选口线置高不选中 */
		
		res = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT) & REQOP_CONFIG ;
		debugprintf("bbvbvb!\r\n");
	}while(res != REQOP_CONFIG);	// 判断是否进入配置模式

	res = MTDCPU_CAN3_MCP2515_Config_Rate(BaudRate);		//设置波特率
	if(res == MCP2515_OK)
	{
		//MTDCPU_CAN3_MCP2515_Init_CAN_Buffer();  //清空屏蔽，滤波，发送，接受寄存器
		// TODO: check why this is needed to receive extended and standard frames
		// Mark all filter bits as don't care:  	屏蔽寄存器全部为0，接受所有的canid
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXM0SIDH,0,0);	//初始化接受屏蔽寄存器0，为0，不屏蔽id
		MTDCPU_CAN3_MCP2515_Write_CAN_ID(RXM1SIDH,0,0);	//初始化接受屏蔽寄存器1

		// and clear, deactivate the two receive buffers. 清空，停用两个接受缓存区
		MTDCPU_CAN3_MCP2515_Set_Register(RXB0CTRL,0);
		MTDCPU_CAN3_MCP2515_Set_Register(RXB1CTRL,0);
		
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		MTDCPU_CAN3_MCP2515_Register_BitModify(RXB0CTRL,RXM /*| BUKT*/,/*RXM_VALID_ALL*/RXM_RCV_ALL /*| BUKT_ROLLOVER*/);	//设置接受控制寄存器0,接受模式00，接受滚存
		MTDCPU_CAN3_MCP2515_Register_BitModify(RXB1CTRL,RXM,/*RXM_VALID_ALL*/ RXM_RCV_ALL);								//设置接受控制寄存器1，接受所有id
		//	MTDCPU_CAN3_MCP2515_Set_Register(RXB0CTRL,0x00);// 输入缓冲器0控制寄存器, 接收所有符合滤波条件的报文，滚存禁止
		//	MTDCPU_CAN3_MCP2515_Set_Register(RXB1CTRL,0x00);
		
		//enable recive interrupt
		//MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTE,MERRE | ERRIE | RX0IE /*| RX1IE*/,IVRE_ENABLED|ERRIE_ENABLED |RX0IE_ENABLED /*G_RXIE_ENABLED*/);  //0,1全部接受中断使能  //使能ERRIE：错误中断使能位\报文接收中断RX0IE\RX1IE
		MTDCPU_CAN3_MCP2515_Set_Register(CANINTE,/*MERRE|*/ERRIE|RX0IE/*|RX1IE*/);
		MTDCPU_CAN3_MCP2515_Set_Register(CANINTF,0x00);
		MTDCPU_CAN3_MCP2515_Set_Register(EFLG,0x00);

		/*W H H */
		MTDCPU_CAN3_MCP2515_Set_Register(TXRTSCTRL,0x00);	// TXnRST作为数字引脚，非发送请求引脚
		/*W H H */
		MTDCPU_CAN3_MCP2515_Set_Register(BFPCTRL,0x00);		// RXnRST禁止输出
	}
	
		MTDCPU_CAN3_MCP2515_Set_Register(CANCTRL,REQOP_NORMAL); 	//设置为正常模式
	do{
		res = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT) & REQOP_NORMAL ;
	}while( res != REQOP_NORMAL);
}


/*
中断错误的处理函数
*/
void Pro_CAN3_ERROR(void)
{
	unsigned char EflgState;
	EflgState=MTDCPU_CAN3_MCP2515_Read_Register( EFLG ); // 读错误标志寄存器，判断错误类型
	if( EflgState & EWARN )	 // 错误警告寄存器，当TEC或REC大于或等于96时置1
	{
		debugprintf("31\r\n");
		 MTDCPU_CAN3_MCP2515_Set_Register(TEC, 0);
		 MTDCPU_CAN3_MCP2515_Set_Register(REC, 0);
	}
	if( EflgState & RXWAR )		  // 当REC大于或等于96时置1
	{debugprintf("RXWAR 处于主动错误状态\r\n");;}
	if( EflgState & TXWAR )		  // 当TEC大于或等于96时置1
	{debugprintf("TXWAR 处于主动错误状态\r\n");;}
	if( EflgState & RXEP )		  // 当REC大于或等于128时置1
	{
		debugprintf("RXEP处于被动错误状态\r\n");
		MTDCPU_CAN3_MCP2515_Set_Register(REC, 0);
	}
	if( EflgState & TXEP )		  // 当TEC大于或等于128时置1
	{
		debugprintf("TXEP处于被动错误状态\r\n");
		MTDCPU_CAN3_MCP2515_Set_Register(TEC, 0);
	}
	if( EflgState & TXBO )      // 当TEC大于或等于255时置1
	{
		debugprintf("MCP2515处于总线关闭状态\r\n");
		MTDCPU_CAN3_MCP2515_IntError_Init(MCP2515_CAN_BAUD_500K);//复位CAN3  //W HH
	}   
	if( EflgState & RX0OVR )		      // 接收缓冲区0溢出
	{
		 debugprintf("接收缓冲区0溢出\r\n");
		 MTDCPU_CAN3_MCP2515_Register_BitModify( EFLG, RX0OVR,0 ); 
		// 清中断标志，根据实际情况处理，一种处理办法是发送远程桢，请求数据重新发送
	}
	if( EflgState & RX1OVR )			 // 接收缓冲区1溢出
	{
		debugprintf("接收缓冲区1溢出\r\n");
		MTDCPU_CAN3_MCP2515_Register_BitModify( EFLG, RX1OVR,0 ); // 清中断标志;
	}	
	
	MTDCPU_CAN3_MCP2515_Set_Register(EFLG,0x00);	//WHH 清除EFLG寄存器
}	


/*接受can3消息*/
unsigned char MCP2515_CAN3_Receive_Message(CanRxMsg* msg)
{
	unsigned char state=0, res=CAN_NOMSG,SetMode=0/*,canstate=0*/;
	
	//	state = MTDCPU_CAN3_MCP2515_Read_Register(CANINTF);
	//	//debugprintf("TF=0x%x\r\n",state);

	//	if(state == 0x00)
	//	{
	//		//CanRxMsg msg;
	//		debugprintf("000!\r\n");
	//		
	//	//		canstate = MTDCPU_CAN3_MCP2515_Read_Register(TEC);
	//	//		debugprintf("TEC=0x%x\r\n",canstate);
	//			
	//	//		canstate = MTDCPU_CAN3_MCP2515_Read_Register(REC);
	//	//		debugprintf("REC=0x%x\r\n",canstate);
	//		canstate= MTDCPU_CAN3_MCP2515_Read_Register(CNF2);
	//		debugprintf("cnf2=0x%x\r\n",canstate);
	//		
	//		res= CAN_NOMSG;
	//		return res;
	//	//		
	//	//		canstate = MTDCPU_CAN3_MCP2515_Read_Register(CANCTRL);
	//	//		debugprintf("stat=0x%x\r\n",canstate);
	//	//		
	//	//		MTDCPU_CAN3_MCP2515_Read_CAN_Message(RXB0SIDH,&msg);
	//	//		printf("l=%d\r\n",msg.DLC);
	//	//		for(i=0;i<msg.DLC;i++)
	//	//		{
	//	//			debugprintf("a%d=0x%x\r\n",i,msg.Data[i]);
	//	//		}
	//	//		canstate = MTDCPU_CAN3_MCP2515_Read_Register(RXB1D0);
	//	//		printf("RDO=%d\r\n",canstate);
	//			
	//	//		canstate = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT);
	//	//		debugprintf("stat=0x%x\r\n",state);
	//	//	
	//	//		can_eflg = MTDCPU_CAN3_MCP2515_Read_Register(EFLG);
	//	//		debugprintf("eflg=0x%x\r\n",state);

	//	//		state = MTDCPU_CAN3_MCP2515_Read_Status(SPICAN_READ_STATUS);
	//	//		debugprintf("can3 int SR=0x%x\r\n",state);
	//	//		
	//	//		for(i=0;i<9;i++)
	//	//		{
	//	//			state = MTDCPU_CAN3_MCP2515_Read_Register(CANINTF);
	//	//			debugprintf("TF=0x%x\r\n",state);
	//	//		}
	//			
	//			//if(state ==0x00)
	//	//		{
	//	//				//MTDCPU_CAN3_MCP2515_IntError_Init(MCP2515_CAN_BAUD_125K);	//复位CAN3  //W HH,这里不一定要复位
	//	//				//printf("err errr errr can3 err int !\r\n");
	//	//		}
	//	}
	
	while(1)
	{
		state = MTDCPU_CAN3_MCP2515_Read_Register(CANINTF);
		if(state == 0x00)	//不是以上所有中断
		{
			break;
		}
		
		if(state & MERRF)  //报文错误标志位
		{
			debugprintf("MERRF报文错误中断！\r\n");
							//MTDCPU_CAN3_MCP2515_IntError_Init(MCP2515_CAN_BAUD_500K);	//复位CAN3  //W HH  /*WHH*/
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF,MERRF,0);	// 清报文错误中断标志
		}
		
		if(state & WAKIF) // 唤醒中断
		{		
			debugprintf("WAKIF唤醒中断！\r\n");
	
			MTDCPU_CAN3_MCP2515_SetControlRegister_Mode(REQOP_NORMAL);		// 唤醒后，在仅监听模式，须设置进入正常工作模式
			do{
				SetMode = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT) & REQOP_NORMAL ;
			}while( SetMode != REQOP_NORMAL);
	
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF,WAKIF,0);	// 清唤醒中断标志
		}
		
		if(state & ERRIF) // 错误中断
		{
			debugprintf("ERRIF错误中断！！\r\n");
			Pro_CAN3_ERROR( );   //分别处理各个错误
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF,ERRIF,0);	// 清错误中断标志
						//MTDCPU_CAN3_MCP2515_IntError_Init(MCP2515_CAN_BAUD_500K);	//复位CAN3 whh
		}

		if(state & TX2IF)  // 发送2成功中断
		{
			//debugprintf("发送2成功中断\r\n");
			MTDCPU_CAN3_MCP2515_Register_BitModify( CANINTF, TX2IF,0 ); // 清中断标志
		}
		if(state & TX1IF)  // 发送1成功中断
		{
			//debugprintf("发送1成功中断\r\n");
			MTDCPU_CAN3_MCP2515_Register_BitModify( CANINTF, TX1IF,0 );// 清中断标志
		}
		if(state & TX0IF)    // 发送0成功中断
		{
			//debugprintf("发送0成功中断\r\n");
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF, TX0IF,0);  // 清中断标志
		}		
		
		if(state & RX0IF) 
		{
			//Msg in Buffer 0
			//debugprintf("Rx0 中断!\r\n");
			MTDCPU_CAN3_MCP2515_Read_CAN_Message(RXB0SIDH,msg);					//读取接收到的信息
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF,RX0IF,0);		//清除中断满标志位
			res= CAN_OK;
		}
		
		if(state & RX1IF)
		{
			// Msg in Buffer 1
			//debugprintf("Rx1 中断!\r\n");
			MTDCPU_CAN3_MCP2515_Read_CAN_Message(RXB1SIDH,msg);
			MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTF,RX1IF,0);		//清除中断满标志位
			res= CAN_OK;
		}

	}
		
		//	state = MTDCPU_CAN3_MCP2515_Read_Register(CANINTF);
		//	debugprintf("TF=0x%x\r\n",state);
		//		
		//	canstate = MTDCPU_CAN3_MCP2515_Read_Register(CANSTAT);
		//	debugprintf("stat=0x%x\r\n",state);	
		//	/*WHH 重新设置中断，使能中断*/
		//	MTDCPU_CAN3_MCP2515_Set_Register(CANINTE,0);		//关闭所有中断
		//	MTDCPU_CAN3_MCP2515_Register_BitModify(CANINTE,ERRIE | RX0IE | RX1IE,ERRIE_ENABLED | G_RXIE_ENABLED);  //0,1全部接受中断使能  //使能ERRIE：错误中断使能位\报文接收中断RX0IE\RX1IE
		//	MTDCPU_CAN3_MCP2515_Set_Register(CANINTF,0x00);
		//	MTDCPU_CAN3_MCP2515_Set_Register(EFLG,0x00);
				
		//	MTDCPU_CAN3_MCP2515_Set_Register(CANINTE,0);		//关闭所有中断
		//	MTDCPU_CAN3_MCP2515_Set_Register(CANINTE,MERRE|ERRIE|RX0IE/*|RX1IE*/);
	
		MTDCPU_CAN3_MCP2515_Set_Register(CANINTF,0x00);
		MTDCPU_CAN3_MCP2515_Set_Register(EFLG,0x00);
	
	return res;	
}

/* returns either 
 #define CAN_MSGAVAIL   (3) - a message has been received
 #define CAN_NOMSG      (4) - no new message
*/
unsigned char MCP2515_CAN3_Check_Receive(void)
{
	unsigned char res;
	res = MTDCPU_CAN3_MCP2515_Read_Status(SPICAN_READ_STATUS);	// RXnIF in Bit 1 and 0
	
	if(res &( RX0IF|RX1IF))
	{
		return CAN_MSGAVAIL;
	}
	else
	{
		return CAN_NOMSG;
	}
	
}

/* checks Controller-Error-State, returns CAN_OK or CAN_CTRLERROR 
   only errors (and not warnings) lead to "CTRLERROR" */
unsigned char MCP2515_CAN3_Check_Error(void)
{
	unsigned char eflg = MTDCPU_CAN3_MCP2515_Read_Register(EFLG);

	if ( eflg &  (0xF8) /* 5 MS-Bits */ ) 
	{
		return CAN_CTRLERROR;
	}
	else
	{
		return CAN_OK;
	}

}

/*中断接受函数*/
void MCP2515_CAN3_Interrupt(CanRxMsg *msg)
{
	//	CanRxMsg *tempmsg;
	//	unsigned char state=CAN_FAIL;
	//	tempmsg = msg;
	//	state = MCP2515_CAN3_Receive_Message(tempmsg);
	
}

