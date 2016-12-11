#include "MTD_REP_CAN3_MCP2515.h"
#include "MCP2515_BitTime.h"
#include "Uart.h"
#include "delay.h"
#include "os.h"
#include "string.h"


u8 MCP2515_CAN3_PORT	=0x03;
/*RXB0's data registers:*/ 
unsigned char RXB0D[8]={RXB0D0,RXB0D1,RXB0D2,RXB0D3,RXB0D4,RXB0D5,RXB0D6,RXB0D7}; 
/*TXB0's data registers:*/ 
unsigned char TXB0D[8]={TXB0D0,TXB0D1,TXB0D2,TXB0D3,TXB0D4,TXB0D5,TXB0D6,TXB0D7};

#define MCP2515_INT1_RCC           RCC_AHB1Periph_GPIOE
#define MCP2515_INT1_GPIO          GPIOE
#define MCP2515_INT1_GPIO_PIN      GPIO_Pin_11
#define MCP2515_INT1_EXIT_PORT		 EXTI_PortSourceGPIOE
#define MCP2515_INT1_EXIT_PIN			 EXTI_PinSource11

#define MCP2515_REST1_RCC          RCC_AHB1Periph_GPIOE
#define MCP2515_REST1_GPIO         GPIOE
#define MCP2515_REST1_GPIO_PIN     GPIO_Pin_12
#define MCP2515_REST1()						 GPIO_ResetBits(MCP2515_REST1_GPIO,MCP2515_REST1_GPIO_PIN)
#define MCP2515_NOREST1()					 GPIO_SetBits(MCP2515_REST1_GPIO,MCP2515_REST1_GPIO_PIN)

void MCP2515_REST1_GpioInit(void)
{
	/*rest Gpio init*/
	  GPIO_InitTypeDef  GPIO_InitStructure;
   
		RCC_AHB1PeriphClockCmd( MCP2515_REST1_RCC , ENABLE); 
	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = MCP2515_REST1_GPIO_PIN;
		GPIO_Init(MCP2515_REST1_GPIO, &GPIO_InitStructure);
	
		MCP2515_REST1();		//初始化之后产生复位
		delay_ms(50);
		MCP2515_NOREST1();

}


void	MCP2515_INT1_GpioInit(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(MCP2515_INT1_RCC, ENABLE); 		//使能IO时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //设置上接
  GPIO_InitStructure.GPIO_Pin = MCP2515_INT1_GPIO_PIN;  //IO口为11
  GPIO_Init(MCP2515_INT1_GPIO, &GPIO_InitStructure);    //如上配置gpio

  SYSCFG_EXTILineConfig(MCP2515_INT1_EXIT_PORT, MCP2515_INT1_EXIT_PIN);//实始化中断线11

  EXTI_InitStructure.EXTI_Line = EXTI_Line11;                   //配置中断线为中断线11
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;          //配置中断模式
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;      //配置为下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                    //配置中断线使能
  EXTI_Init(&EXTI_InitStructure);                              

  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;             
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void MCP2515_INT1_IRQ(void)
{
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif	

    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
		{
			CanRxMsg frame;
			u8 i;
			printf("can3,rec int!\r\n");
	
			memset(&frame,0,sizeof(CanRxMsg));
			
				MCP2515_CAN3_Interrupt(&frame);
			
        EXTI_ClearITPendingBit(EXTI_Line11);
			
			for(i=0;i<8 ;i++)
			{
				printf("can3[%d]=0x%x\r\n",i,frame.Data[i]);
			}
    }
				
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    	//退出中断
	#endif
}



unsigned char MTDREP_CAN3_MCP2515_Init(const unsigned char CanBaudRate)
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
	
	MCP2515_REST1_GpioInit();  //复位引脚初始化
	
	MCP2515_INT1_GpioInit();	//中断引脚初始化
	
	MTDREP_SPI1_Init();		//初始化MCU的SPI总线
	
	debugprintf("mpc2515_init!\r\n");
	
	/*Flip into the Configuration Mode*/ 
	MTDREP_CAN3_MCP2515_Reset();						//发送复位指令软件复位MCP2515
	
	res = MTDREP_CAN3_MCP2515_SetControlRegister_Mode(REQOP_CONFIG); 	 //设置为配置模式
	if(res == MCP2515_FAIL)	//设置失败直接返回
	{
		return res; 	/* function exit on error */
	}
	
	res = MTDREP_CAN3_MCP2515_Config_Rate(CanBaudRate);		//设置波特率
	if(res == MCP2515_OK)
	{
		MTDREP_CAN3_MCP2515_Init_CAN_Buffer();  //清空屏蔽，滤波，发送，接受寄存器
		
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		MTDREP_CAN3_MCP2515_Register_BitModify(RXB0CTRL,RXM | BUKT,RXM_VALID_ALL | BUKT_ROLLOVER);	//设置接受控制寄存器0,接受模式00，接受滚存
		
		MTDREP_CAN3_MCP2515_Register_BitModify(RXB1CTRL,RXM,RXM_VALID_ALL);								//设置接受控制寄存器1，接受所有id
		
		//enable recive interrupt
		MTDREP_CAN3_MCP2515_Register_BitModify(CANINTE,RX0IE | RX1IE,G_RXIE_ENABLED);  //0,1全部接受中断使能
	}
	
	res = MTDREP_CAN3_MCP2515_SetControlRegister_Mode(REQOP_NORMAL);		//设置为正常模式
	if(res == MCP2515_FAIL)
	{
		return res;/* function exit on error */
	}
	return res;
	
	
//	//Set the baudrate 				Fsoc=20Mhz
//	//set CNF1,SJW=00,lengthe is 1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/20M=5us 
//	SPISendByte_MCP2515(CNF1,CAN_125Kbps); 
//		
//	//set CNF2,SAM=0,caputre one time,PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ 
//	SPISendByte_MCP2515(CNF2,0x80|PHSEG1_7TQ|PRSEG_2TQ);	
//	
//	//set CNF3,PHSEG2=(2+1)TQ=3TQ,when CANCTRL.CLKEN=1,set CLKOUT pin to output 
//  SPISendByte_MCP2515(CNF3,PHSEG2_6TQ); 
	
//	/* 
//    3 Transimit Buffers:TXB0,TXB1,TXB2 
//    2 Receive   Buffers:RXB0,RXB1 
//    6 ID Filter:RXF0~RXF5 
//    2 ID Mask Regieter:RXM0,RXM1 
//  */ 
//	
//	//Configurate TXB0's ID and DLC registers 
//    SPISendByte_MCP2515(TXB0SIDH,0x00);//Standard ID 	//发送缓冲器0标准标识符高位
//    SPISendByte_MCP2515(TXB0SIDL,0x00);//Standard ID 	//发送缓冲器0标准标识符低位
////    SPISendByte_MCP2515(TXB0DLC,DLC_1);//DLC 
////		SPISendByte_MCP2515(TXB0EID8,0xFF);//发送缓冲器0拓展标识符高位
////		SPISendByte_MCP2515(TXB0EID0,0xFF);//发送缓冲器0拓展标识符低位
//	
//	//configurate RXB0' registers 
//		SPISendByte_MCP2515(RXB0SIDH,0x00);//清空接收缓冲器0的标准标识符高位
//		SPISendByte_MCP2515(RXB0SIDL,0x00);//清空接收缓冲器0的标准标识符低位
//		SPISendByte_MCP2515(RXB0CTRL,0x60);//Receive all frames from CAN bus 
////    SPISendByte_MCP2515(RXB0CTRL,0x20);//RXB0 just receive the standard frames  //仅仅接收标准标识符的有效信息
//		SPISendByte_MCP2515(RXB0DLC,DLC_8);//设置接收数据的长度为8个字节
//		
//    SPISendByte_MCP2515(RXF0SIDH,0x00);//Fileter register 						//配置验收滤波寄存器n标准标识符高位
//    SPISendByte_MCP2515(RXF0SIDL,0x00);//Just receive ID=0x7FF frame 	//配置验收滤波寄存器n标准标识符低位
//    SPISendByte_MCP2515(RXF1SIDH,0x00);//Fileter register 						
//    SPISendByte_MCP2515(RXF1SIDL,0x00);//Just receive ID=0x7EE frame 	

//		SPISendByte_MCP2515(RXM0SIDH,0x00);//Mask register 								//配置验收屏蔽寄存器n标准标识符高位
//    SPISendByte_MCP2515(RXM0SIDL,0x00); 															//配置验收屏蔽寄存器n标准标识符低位	
// 	
//  /*Set the RXB0 or RXB1 interrupt enableling*/ 
//		SPISendByte_MCP2515(CANINTF,0x00);//清空CAN中断标志寄存器的所有位(必须由MCU清空)
//		SPISendByte_MCP2515(CANINTE,RX0IE);//Enable RXB0 interrupt		//配置CAN中断使能寄存器的接收缓冲器0满中断使能,其它位禁止中断
//	
//    SPISendByte_MCP2515(CANCTRL,REQOP_NORMAL | CLKOUT_ENABLED);//Go into the normal mode //将MCP2515设置为正常模式,退出配置模式
//    
//		dummy=SPIReadByte_MCP2515(CANSTAT);//读取CAN状态寄存器的值    
//		if (OPMODE_NORMAL != (dummy & 0xE0)) //判断MCP2515是否已经进入正常模式
//    {
//				SPISendByte_MCP2515(CANCTRL,REQOP_NORMAL | CLKOUT_ENABLED);//If normal ok //再次将MCP2515设置为正常模式,退出配置模式       	
//		}	
}	

////CAN发送任意长度字节
//void CAN_Send_anylength(u8 *CAN_TX_Buf,u8 length1)
//{
//     
//  unsigned char tempdata,j;
//  tempdata=SPIReadByte_MCP2515(SPICAN_READ_STATUS);
//  SPISendByte_MCP2515(TXB0DLC,length1);
//  for(j=0;j<length1;j++)
//  {
//    SPISendByte_MCP2515(TXB0D0+j,CAN_TX_Buf[j]);
//  }

//  if(tempdata&0x04)//判断TXREQ标志位
//  {
//    delay_ms(1);
//    SPISendByte_MCP2515(TXB0CTRL,0);//清除TXREQ标志位
//    while(SPIReadByte_MCP2515(SPICAN_READ_STATUS)&0x04);//等待TXREQ清零
//  }
//  MTDCPU_SPI_CS_LOW();
//  MTDCPU_SPI1_SendByte(SPICAN_RTS_TXB0);//发送缓冲器0请求发送
//  MTDCPU_SPI_CS_HIGH();
//}



/** 
  * @brief  Reset the MCP2515 then U go into the configruation mode 
  * @param  none 
  * @retval none 
  */ 
void MTDREP_CAN3_MCP2515_Reset(void)	     //MCP2515器件复位
{
   MTDREP_SPI1_CS_LOW();										/* 片选口线置低选中  */
   MTDREP_SPI1_SendByte(SPICAN_RESET);		//发送寄存器复位命令
   MTDREP_SPI1_CS_HIGH();									/* 片选口线置高不选中 */
	 delay_ms(10);
}

/*读取寄存器的值*/
unsigned char MTDREP_CAN3_MCP2515_Read_Register(const unsigned char Addr)
{
    unsigned char Data;   
	
    MTDREP_SPI1_CS_LOW();                //置MCP2515的CS为低电平
    MTDREP_SPI1_SendByte(SPICAN_READ);   //发送读命令
    MTDREP_SPI1_SendByte(Addr); 				 //发送地址	
    Data = MTDREP_SPI1_ReadByte();   		 //读一字节 
    MTDREP_SPI1_CS_HIGH();               //置MCP2515的CS为高电平 
    return Data; 
}

/*********read the several register*********************/
void MTDREP_CAN3_MCP2515_Read_RegisterS(const unsigned char Addr, unsigned char RegValues[], const unsigned char RegNum)
{
		unsigned char i;
		
	  MTDREP_SPI1_CS_LOW();                //置MCP2515的CS为低电平
		MTDREP_SPI1_SendByte(SPICAN_READ);   //发送读命令
    MTDREP_SPI1_SendByte(Addr); 				 //发送地址	
	
		// mcp2515 has auto-increment of address-pointer	
		for(i=0;i<RegNum;i++)
		{
			RegValues[i]=MTDREP_SPI1_ReadByte();	//读取数据
		}
		MTDREP_SPI1_CS_HIGH();               //置MCP2515的CS为高电平 
}

//设置寄存器的值
/** 
  * @brief  Send a byte to MCP2515 through SPI to return corresponding status you need 
  * @param  Add:Address of register 
  * @param  Data:the data U need to be sent from MCU to MCP2515 
  * @retval none 
  */ 
void MTDREP_CAN3_MCP2515_Set_Register(const unsigned char Addr,const unsigned char Data)
{
	  MTDREP_SPI1_CS_LOW();   						//使能片选     
    MTDREP_SPI1_SendByte(SPICAN_WRITE); //发送写命令
    MTDREP_SPI1_SendByte(Addr); 				//发送地址				
    MTDREP_SPI1_SendByte(Data); 				//写入数据
    MTDREP_SPI1_CS_HIGH(); 							//关闭片选
}


void MTDREP_CAN3_MCP2515_Set_RegisterS(const unsigned char Addr,const unsigned char SetValues[],const unsigned char RegNum) //设置多个寄存器的值
{
		unsigned char i;
		MTDREP_SPI1_CS_LOW();   						//使能片选
	  MTDREP_SPI1_SendByte(SPICAN_WRITE); //发送写命令
    MTDREP_SPI1_SendByte(Addr); 				//发送地址	
	
		// mcp2515 has auto-increment of address-pointer
		for(i=0;i<RegNum;i++)
		{
			MTDREP_SPI1_SendByte(SetValues[i]);//写入数据
		}
    MTDREP_SPI1_CS_HIGH(); 							//关闭片选		
}


/** 
  * @brief  Modify the bit of the register 
  * @param  Add:Address of register 
  * @param  Mask:1-->U can modify the corresponding bit;0-->U can not modify the corresponding bit 
  * @param  Data:the data U need to be sent from MCU to MCP2515 
  * @retval none 
  */ 
void MTDREP_CAN3_MCP2515_Register_BitModify(const unsigned char Addr,const unsigned char Mask,unsigned char Data) 
{ 
    MTDREP_SPI1_CS_LOW();                			//使能片选
    MTDREP_SPI1_SendByte(SPICAN_BIT_MODIFY); 	//发送位修改命令
    MTDREP_SPI1_SendByte(Addr); 							//发送地址
    MTDREP_SPI1_SendByte(Mask); 							//发送掩码
    MTDREP_SPI1_SendByte(Data); 							//写入数据
    MTDREP_SPI1_CS_HIGH();    								//关闭片选
}

//读取寄存器状态
unsigned char MTDREP_CAN3_MCP2515_Read_Status(void)
{
		unsigned char State;
		MTDREP_SPI1_CS_LOW();                				//使能片选
		MTDREP_SPI1_SendByte(SPICAN_READ_STATUS); 	//发送读状态命令
		State = MTDREP_SPI1_ReadByte();							//读取状态
		MTDREP_SPI1_CS_HIGH();											//关闭片选
		
		return State;
}


 //读取接受的状态
unsigned char MTDREP_CAN3_MCP2515_ReadRx_Status(void)
{
		unsigned char RxState;
		MTDREP_SPI1_CS_LOW();                				//使能片选
		MTDREP_SPI1_SendByte(SPICAN_RX_STATUS); 		//发送读状态命令
		RxState = MTDREP_SPI1_ReadByte();						//读取状态
		MTDREP_SPI1_CS_HIGH();											//关闭片选
		
		return RxState;
}


/*
//设置控制寄存器 的工作模式
	返回0成功，返回1设置失败。
*/
unsigned char MTDREP_CAN3_MCP2515_SetControlRegister_Mode(const unsigned char NewMode)	
{
		unsigned char mode;
		MTDREP_CAN3_MCP2515_Register_BitModify(CANCTRL,REQOP,NewMode);	//修改模式
		
		//verify as advised in datasheet
		mode = MTDREP_CAN3_MCP2515_Read_Register(CANCTRL);
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
*/
unsigned char MTDREP_CAN3_MCP2515_Config_Rate(const unsigned char  BaudRate)
{
		unsigned char Set,cfg1,cfg2,cfg3;
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
			//Set the baudrate 				Fsoc=20Mhz
			//set CNF1,SJW=00,lengthe is 1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/20M=5us 
			MTDREP_CAN3_MCP2515_Set_Register(CNF1,cfg1);		//设置波特率
			//set CNF2,SAM=0,caputre one time,PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ 
			MTDREP_CAN3_MCP2515_Set_Register(CNF2,cfg2);
			//set CNF3,PHSEG2=(2+1)TQ=3TQ,when CANCTRL.CLKEN=1,set CLKOUT pin to output 
			MTDREP_CAN3_MCP2515_Set_Register(CNF3,cfg3);
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
void MTDREP_CAN3_MCP2515_Read_CAN_ID(const unsigned char RxIdReg_Addr, unsigned char* IDE_ExtFlag, unsigned int* Std_id,unsigned int * Ext_id)
{
		unsigned char  IdBufData[4];
		
		/*初始化*/
		*IDE_ExtFlag = CAN_ID_STD;
		*Std_id	=	0;
		*Ext_id = 0;
	
		MTDREP_CAN3_MCP2515_Read_RegisterS(RxIdReg_Addr,IdBufData,4);
		
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
void MTDREP_CAN3_MCP2515_Read_CAN_Message(const unsigned char RxBuf_SIDH_Addr, CanRxMsg* Message)
{
		unsigned char RxReg_Addr, RxControlReg_Value;
	
		RxReg_Addr = RxBuf_SIDH_Addr;
		
		/*获得id*/
		MTDREP_CAN3_MCP2515_Read_CAN_ID(RxReg_Addr,&(Message->IDE),&(Message->StdId),&(Message->ExtId));	//得到id
		
		RxControlReg_Value = MTDREP_CAN3_MCP2515_Read_Register(RxReg_Addr-1);		//得多接受控制寄存器值
	
		Message->DLC = MTDREP_CAN3_MCP2515_Read_Register(RxReg_Addr+4);					//读取数据长度
	
		if(RxControlReg_Value & RXRTR_REMOTE/*0x08*/  || (Message->DLC &0x40) )		//说明是远程帧
		{
				Message->RTR = CAN_RTR_REMOTE;		//表示为远程帧
		}
		else
		{
				Message->RTR = CAN_RTR_DATA;			//表示为数据帧
		}
		
		Message->DLC &= 0x0F;   //寄存器的低4位，才是数据长度
		
		MTDREP_CAN3_MCP2515_Read_RegisterS(RxReg_Addr+5,&(Message->Data[0]),Message->DLC);		//读取数据
}


/*
//写入要发送的canid
IDE_ExtFlag =CAN_ID_EXT 为扩展帧
*/
void MTDREP_CAN3_MCP2515_Write_CAN_ID(const unsigned char TxIdReg_Addr, const unsigned char IDE_ExtFlag,const unsigned int can_id)
{
		unsigned int canid;
		unsigned char IdBufData[4];
		
		if(IDE_ExtFlag ==CAN_ID_EXT)		//表示为扩展帧
		{
			canid =can_id;
			IdBufData[3] = (unsigned char )(canid & 0xFF);	//EID0
			IdBufData[2] = (unsigned char )(canid >> 8);		//EID8
			
			canid >>= 16;		//获得高16位
			
			IdBufData[1] = (unsigned char )(canid & 0x03);
			IdBufData[1] +=(unsigned char )((canid & 0x1C)<<3);
			IdBufData[1] |= EXIDE_SET;
			IdBufData[0] = (unsigned char )(canid >> 5);
		}
		else
		{
			canid = (unsigned short)(can_id & 0xFFFF);
			
			IdBufData[0] = (unsigned char )(canid >>3);
			IdBufData[1] = (unsigned char )(canid &0x07 <<5); 
			IdBufData[2] = 0;
			IdBufData[3] = 0;
		}
		
		MTDREP_CAN3_MCP2515_Set_RegisterS(TxIdReg_Addr,IdBufData,4);		//将id写入寄存器
}

/*
	写入要发送的can消息   
// Buffer can be TXBUF0 TXBUF1 or TXBUF2
*/
void MTDREP_CAN3_MCP2515_Write_CAN_Message(const unsigned char TxBuf_SIDH_Addr, const CanTxMsg* Message)
{
		unsigned char TxReg_Addr, Dlc;		//地址，数据的长度
	
		TxReg_Addr = TxBuf_SIDH_Addr;
		Dlc = Message->DLC;
		
		MTDREP_CAN3_MCP2515_Set_RegisterS(TxReg_Addr+5,&(Message->Data[0]),Dlc);		//向寄存器写入数据
		
		if(Message->IDE == CAN_ID_EXT)	//扩展帧
		{
				MTDREP_CAN3_MCP2515_Write_CAN_ID(TxReg_Addr,Message->IDE,Message->ExtId);		//写入canid
		}
		else		//标准帧
		{
				MTDREP_CAN3_MCP2515_Write_CAN_ID(TxReg_Addr,Message->IDE,Message->StdId);
		}
		
		if(Message->RTR ==	CAN_RTR_REMOTE )		//表示远程帧
		{
			Dlc |= 0x40;		//手册21页
		}
		
		MTDREP_CAN3_MCP2515_Set_Register((TxReg_Addr+4),Dlc);		//写入RTR和DLC
}		


/*
 ** Start the transmission from one of the tx buffers.
// Buffer can be TXBUF0 TXBUF1 or TXBUF2
 */
void MTDREP_CAN3_MCP2515_Start_Transimt(const unsigned char TxBuf_SIDH_Addr)
{
		MTDREP_CAN3_MCP2515_Register_BitModify(TxBuf_SIDH_Addr-1,TXREQ,TXREQ);
}


/*
	获得 一个空的物理发送缓冲区
*/
unsigned char MTDREP_CAN3_MCP2515_Get_Free_TxBuf(unsigned char * TxBufNum)
{
		unsigned char TempState, i, ControlRegValue;
		unsigned char ControlReg[3] ={TXB0CTRL,TXB1CTRL,TXB2CTRL};
		
		TempState = MCP2515_ALLTXBUSY;		//初始化发送缓冲区状态。所有的发送缓冲区满	
		*TxBufNum = 0x00;
		
		// check all 3 TX-Buffers
		for(i=0;i<3;i++)
		{
			ControlRegValue = MTDREP_CAN3_MCP2515_Read_Register(ControlReg[i]);		//读取控制寄存器的值
			if((ControlRegValue & TXREQ )== 0)
			{
				debugprintf("Selected TX-Buffer %d\r\n", i+1);
				*TxBufNum = ControlReg[i]+1; 	// return SIDH-address of Buffer
				TempState = MCP2515_OK;
				return TempState;
			}
			
		}
		
		return TempState;
}


/*
//设置屏蔽寄存器，滤波寄存器，初始化发送缓冲区

*/
void MTDREP_CAN3_MCP2515_Init_CAN_Buffer(void)
{
		unsigned char i,TxContr1,TxContr2,TxContr3;
		// TODO: check why this is needed to receive extended and standard frames
		// Mark all filter bits as don't care:  	屏蔽寄存器全部为0，接受所有的canid
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXM0SIDH,0,0);	//初始化接受屏蔽寄存器0，为0，不屏蔽id
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXM1SIDH,0,0);	//初始化接受屏蔽寄存器1

		// Anyway, set all filters to 0:		/*接受缓冲区0 */
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF0SIDH,1,0);	// RXB0: extended 
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF1SIDH,0,0);	//    AND standard
	
																				/*接受缓冲区1，滤波寄存器配置*/
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF2SIDH, 1, 0); // RXB1: extended 
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF3SIDH, 0, 0); //  AND standard
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF4SIDH, 0, 0);
		MTDREP_CAN3_MCP2515_Write_CAN_ID(RXF5SIDH, 0, 0);
		
	// Clear, deactivate the three transmit buffers		清空，停用三个发送缓冲区
	// TXBnCTRL -> TXBnD7
	
		TxContr1 = TXB0CTRL;
		TxContr2 = TXB1CTRL;
		TxContr3 = TXB2CTRL;
		for(i=0;i<14;i++)				// in-buffer loop
		{
			MTDREP_CAN3_MCP2515_Set_Register(TxContr1,0);
			MTDREP_CAN3_MCP2515_Set_Register(TxContr2,0);
			MTDREP_CAN3_MCP2515_Set_Register(TxContr3,0);
			
			TxContr1++;
			TxContr2++;
			TxContr3++;
		}
		
		// and clear, deactivate the two receive buffers. 清空，停用两个接受缓存区
		MTDREP_CAN3_MCP2515_Set_Register(RXB0CTRL,0);
		MTDREP_CAN3_MCP2515_Set_Register(RXB1CTRL,0);
}


/*接受can3消息*/
unsigned char MCP2515_CAN3_Receive_Message(CanRxMsg* msg)
{
	unsigned char state, res;
	state = MTDREP_CAN3_MCP2515_Read_Status();
	
	if(state & RX0IF)
	{
		//Msg in Buffer 0
		MTDREP_CAN3_MCP2515_Read_CAN_Message(RXB0SIDH,msg);			//读取接收到的信息
		MTDREP_CAN3_MCP2515_Register_BitModify(CANINTF,0xFC | RX0IF,0);		//清除中断满标志位
		res= CAN_OK;
	}
	else if(state & RX1IF)
	{
		// Msg in Buffer 1
		MTDREP_CAN3_MCP2515_Read_CAN_Message(RXB1SIDH,msg);
		MTDREP_CAN3_MCP2515_Register_BitModify(CANINTF,0xFC | RX1IF,0);		//清除中断满标志位
		res= CAN_OK;
	}
	else
	{
		res =CAN_NOMSG;
	}
	return res;
}

/* returns either 
 #define CAN_MSGAVAIL   (3) - a message has been received
 #define CAN_NOMSG      (4) - no new message
*/
unsigned char MCP2515_CAN3_Check_Receive(void)
{
	unsigned char res;
	res = MTDREP_CAN3_MCP2515_Read_Status();	// RXnIF in Bit 1 and 0
	
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
	unsigned char eflg = MTDREP_CAN3_MCP2515_Read_Register(EFLG);

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
	CanRxMsg *tempmsg;
	unsigned char state=CAN_FAIL;
	tempmsg = msg;
	state = MCP2515_CAN3_Receive_Message(tempmsg);
	
}





///** 
//  * @brief  Read 8 bytes to CAN bus 
//  * @param  CAN_TX_Buf:the address of data'indicator,8bytes 
//  * @retval none 
//  */ 
//void MTDREP_CAN3_SPICAN_Send8bytes(unsigned char *CAN_TX_Buf) 
//{ 
//    unsigned char tempdata; 
//    tempdata=SPIReadByte_MCP2515(SPICAN_READ_STATUS); 
//    SPISendByte_MCP2515(TXB0D0,CAN_TX_Buf[0]); 
//    SPISendByte_MCP2515(TXB0D1,CAN_TX_Buf[1]); 
//    SPISendByte_MCP2515(TXB0D2,CAN_TX_Buf[2]); 
//    SPISendByte_MCP2515(TXB0D3,CAN_TX_Buf[3]); 
//    SPISendByte_MCP2515(TXB0D4,CAN_TX_Buf[4]); 
//    SPISendByte_MCP2515(TXB0D5,CAN_TX_Buf[5]); 
//    SPISendByte_MCP2515(TXB0D6,CAN_TX_Buf[6]); 
//    SPISendByte_MCP2515(TXB0D7,CAN_TX_Buf[7]); 
//    if(tempdata&0x04)			//check the TXREQ bit 
//    { 
//        delay_ms(1); 
//        MCP2515_BitModify(TXB0CTRL,0x80,0x00);	//clear TXREQ bit 
//        while(SPI_CMD_MCP2515(SPICAN_READ_STATUS)&0x04);//wait until TXREQ bit matchs 0 
//    } 
//    MTDCPU_SPI_CS_LOW(); 
//    MTDCPU_SPI1_SendByte(SPICAN_RTS_TXB0);//TXB0 send-request 
//    MTDCPU_SPI_CS_HIGH(); 
//}

///** 
//  * @brief  Send a byte to CAN bus by TXB0 
//  * @param  Data:data to be sent 
//  * @retval none 
//  */ 
//void SPICAN_SendOneByte(unsigned char Data) 
//{ 
//    unsigned char tempdata; 
///*Test the driver between MCP2515 and the SPI periphere :tempdata<--------------*/ 
//    tempdata=SPI_CMD_MCP2515(SPICAN_READ_STATUS); 
//    if(tempdata&0x04)//check the TXREQ bit 
//    { 
//        delay_ms(1); 
//        MCP2515_BitModify(TXB0CTRL,0x08,0x00);//clear TXREQ bit 
//        while(SPI_CMD_MCP2515(SPICAN_READ_STATUS)&0x04);//wait until TXREQ bit matchs 0 
//    } 
//    SPISendByte_MCP2515(TXB0D0,Data); 
//    MTDCPU_SPI_CS_LOW(); 
//    MTDCPU_SPI1_SendByte(SPICAN_RTS_TXB0);//TXB0 send-request 
//    MTDCPU_SPI_CS_HIGH(); 
//}

///** 
//  * @brief  Receive a byte from MCP2515 RXB0 
//  * @param  none 
//  * @retval Data:return the effectively data from RXB0 
//  */ 
//unsigned char CAN_Receive_onebyte() 
//{ 
//    unsigned char tempdata; 
//    tempdata=SPI_CMD_MCP2515(SPICAN_READ_STATUS);//confirm receive data=RXB0IF 
//    if(tempdata&0x01)    
//    { 
//        tempdata=SPI_CMD_MCP2515(SPICAN_RX_STATUS); 
//        if(tempdata&0x40)//true means RXB0's standard frame 
//        {            
//            tempdata=SPIReadByte_MCP2515(RXB0D0); 
//        } 
//        else 
//            {tempdata=254;}//no standard frame receive 
//        MCP2515_BitModify(CANINTF,0x01,0x00);    
//    } 
//    else 
//        {tempdata=255;}//no frame receive 
//    return tempdata; 
//}

///** 
//  * @brief   Send n bytes with a given standard ID  corresponding to frame type 
//  * @param   CAN_TX_Buf: data to be sent 
//  * @param     DLC:DLC<=8 
//  * @param   SID:<=0x7FF 
//  * @param   CAN_FrameType:CAN_STD,CAN_RTR        
//  * @retval None 
//  */ 
//void SPICAN_SendData(unsigned char *CAN_TX_Buf,Frame_TypeDef *Frame ) 
//{ 
//    unsigned char tempdata; 
//    unsigned char HSID,LSID; 
//    if(Frame->Type==CAN_STD) 
//    { 
//        /*Set the ID of the frame*/ 
//        HSID=(unsigned char)(Frame->SID>>3); 
//        LSID=(unsigned char)((Frame->SID<<5)&0xE0); 
//        SPISendByte_MCP2515(TXB0SIDH,HSID); 
//        SPISendByte_MCP2515(TXB0SIDL,LSID); 
//        /*Set the DLC and the type of the frame*/ 
//        SPISendByte_MCP2515(TXB0DLC,Frame->DLC|CAN_STD); 
//        /*Write the data into the TXB0 data registers */ 
//        for(tempdata=0;tempdata<Frame->DLC;tempdata++) 
//					SPISendByte_MCP2515(TXB0D[tempdata],CAN_TX_Buf[tempdata]);        
///*      SPISendByte_MCP2515(TXB0D0,CAN_TX_Buf[0]); 
//        SPISendByte_MCP2515(TXB0D1,CAN_TX_Buf[1]); 
//        SPISendByte_MCP2515(TXB0D2,CAN_TX_Buf[2]); 
//        SPISendByte_MCP2515(TXB0D3,CAN_TX_Buf[3]); 
//        SPISendByte_MCP2515(TXB0D4,CAN_TX_Buf[4]); 
//        SPISendByte_MCP2515(TXB0D5,CAN_TX_Buf[5]); 
//        SPISendByte_MCP2515(TXB0D6,CAN_TX_Buf[6]); 
//        SPISendByte_MCP2515(TXB0D7,CAN_TX_Buf[7]);*/ 
//    } 
//    else  /*if(CAN_FrameType==CAN_RTR)*/ 
//    { 
//        /*Set the ID of the frame*/ 
//        HSID=(unsigned char)(Frame->SID>>3); 
//        LSID=(unsigned char)((Frame->SID<<5)&0xE0); 
//        SPISendByte_MCP2515(TXB0SIDH,HSID); 
//        SPISendByte_MCP2515(TXB0SIDL,LSID); 
//        /*Set the type of the frame*/ 
//        SPISendByte_MCP2515(TXB0DLC,CAN_RTR); 
//    }

//    tempdata=SPI_CMD_MCP2515(SPICAN_READ_STATUS); 
//    if(tempdata&0x04) 
//    { 
//        delay_ms(1); 
//        MCP2515_BitModify(TXB0CTRL,0x80,0x00); 
//        while(SPI_CMD_MCP2515(SPICAN_READ_STATUS)&0x04); 
//    } 
//    /*Send the SPI_RTS_TXB0 request command to MCP2515 to send the data loaded in the data register*/ 
//    MTDCPU_SPI_CS_LOW(); 
//    MTDCPU_SPI1_SendByte(SPICAN_RTS_TXB0); 
//    MTDCPU_SPI_CS_HIGH(); 
//}

///** 
//  * @brief  Receive n bytes from MCP2515 RXB0 
//  * @param  none 
//  * @retval Data:return the effectively data from RXB0 
//  */ 
//unsigned char SPICAN_ReceiveData(unsigned char *CAN_RX_Buf,Frame_TypeDef *Frame) 
//{ 
//    unsigned char tempdata; 
//    unsigned int CAN_ID; 
////    tempdata=SPI_CMD_MCP2515(SPI_READ_STATUS);//check if it is received a  frame 
//    tempdata=SPI_CMD_MCP2515(SPICAN_RX_STATUS);

//    if(tempdata&0x40)//RXB0 
//    { 
//        CAN_ID=(unsigned int)((SPIReadByte_MCP2515(RXB0SIDH))<<8); 
//        CAN_ID|=(unsigned int)(SPIReadByte_MCP2515(RXB0SIDL)); 
//        CAN_ID>>=5; 
//        /*Check the CAN_ID you received that if it is matched with the SID you wish*/ 
//        if(/*CAN_ID==Frame->SID*/1) 
//        { 
//            for(tempdata=0;tempdata<Frame->DLC;tempdata++) 
//							CAN_RX_Buf[tempdata]=SPIReadByte_MCP2515(RXB0D[tempdata]); 
///*          CAN_RX_Buf[0]=ReadByte_MCP2515(RXB0D0); 
//            CAN_RX_Buf[1]=ReadByte_MCP2515(RXB0D1); 
//            CAN_RX_Buf[2]=ReadByte_MCP2515(RXB0D2); 
//            CAN_RX_Buf[3]=ReadByte_MCP2515(RXB0D3); 
//            CAN_RX_Buf[4]=ReadByte_MCP2515(RXB0D4); 
//            CAN_RX_Buf[5]=ReadByte_MCP2515(RXB0D5); 
//            CAN_RX_Buf[6]=ReadByte_MCP2515(RXB0D6); 
//            CAN_RX_Buf[7]=ReadByte_MCP2515(RXB0D7);*/ 
//					MCP2515_BitModify(CANINTF,0x01,0x00); 	//清除中断标志位(中断标志寄存器必须由MCU清零)
//        } 
//        else 
//        { 
//            return 0;//ID is not correct 
//        } 
//        return 1;//receive ok 
//    } 
//    else 
//    return 2;//no standard frame receive 
//}

