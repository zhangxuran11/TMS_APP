#ifndef __SAF82525_H__
#define __SAF82525_H__

#include "stm32f4xx.h"

#define SAF82525_INT_RCC           RCC_AHB1Periph_GPIOE
#define SAF82525_INT_GPIO          GPIOE
#define SAF82525_INT_GPIO_PIN      GPIO_Pin_6
#define SAF82525_INT_EXIT_PORT		 EXTI_PortSourceGPIOE
#define SAF82525_INT_EXIT_PIN			 EXTI_PinSource6

#define SAF82525_REST_RCC           RCC_AHB1Periph_GPIOF
#define SAF82525_REST_GPIO          GPIOF
#define SAF82525_REST_GPIO_PIN      GPIO_Pin_6
#define SAF82525_REST_OFF() 				GPIO_ResetBits(SAF82525_REST_GPIO,SAF82525_REST_GPIO_PIN);
#define SAF82525_REST_ON()					GPIO_SetBits(SAF82525_REST_GPIO,SAF82525_REST_GPIO_PIN);		//高电平进行复位,至少持续1.8us

#define SAF_BAUD_RATE_125000		0x02		//0x02 为12M晶振	
#define SAF_BAUD_RATE_115200		0x02		//
#define SAF_BAUD_RATE_19200			0x14  	//0x11 为11.0592M 晶振。  0x14为12M
#define SAF_BAUD_RATE_9600			0x26		//0x23 为11.0592M 晶振。  0x26为12M


extern __IO uint8_t HDLCARxBuf[200];
extern __IO uint8_t	HDLCBRxBuf[200];
extern __IO uint16_t HDLCARxLen;
extern __IO uint16_t HDLCBRxLen;

extern u8  g_HDLCARxStartFlag;		//A通道接受开始标志
extern u8  g_HDLCARxEndFlag;			//A通道接受结束标志
extern u8  g_HDLCBRxStartFlag;		//B通道接受开始标志
extern u8  g_HDLCBRxEndFlag;			//B通道接受结束标志

void SAF82525_INT_GPIO_Init(void);
void SAF82525_Rest_CPIO_Init(void);
void SAF82525_GPIO_Init(void);
void SAF82525_Init(uint8_t HDLC_BautRate);
uint8_t SAF82525_Read_Register(const uint8_t ReadAddr);
void SAF82525_Write_Register(const uint8_t WriteAddr,uint8_t WriteData);
void SAF82525_ModifyBit_Register(uint8_t Addr,uint8_t Mask,uint8_t Data);
uint8_t CheckASTAR(void);		//检测SAB82525状态函数        
uint8_t CheckBSTAR(void);   //检测B通道状态
void SetACMDRegister(uint8_t CMD_Data);	//这两个函数可以合为一个
void SetBCMDRegister(uint8_t CMD_Data);

//void WaitAXFIFOWriteEnable(uint8_t ChanleRegister);			(使用了内联 __inline)			//发送缓冲区写使能

void AChanleTxData(uint8_t *buf,int len);		//A通道发送数据
void BChanleTxData(uint8_t *buf,int len);		//B通道发送数据
void AChanleRxData(__IO uint8_t *buf,__IO uint16_t *len);		//A通道接受数据
void BChanleRxData(__IO uint8_t *buf,__IO uint16_t *len);		//B通道接受数据

/*CHANNEL A REGISTER ADDRESS*/
#define AFIFO       0x00    /*//read\write*/	//Receive/Transmit FIFO
#define AISTA       0x20    /*/READ*/       	//Interrupt STAtus
#define AMASK       0x20    /*/WRITE*/      	//Mask
#define ASTAR       0x21    /*/read*/       	//STAtus
#define ACMDR       0x21    /*/write*/      	//COManD
#define AMODE       0x22    /*/read\write */	//MODE
#define ATIMR       0x23    /*/read\write*/ 	//TIMer
#define AEXIR       0x24    /*/read*/       	//EXtended Interrupt
#define AXAD1       0x24    /*/write*/      	//Transmit ADdress 1
#define ARBCL       0x25    /*/read*/       	//Receive Byte Count Low
#define AXAD2       0x25    /*/write*/      	//Transmit ADdress 2
#define ARAH1       0x26    /*/WRITE*/      	//Receive Address High 1
#define ARSTA       0x27    /*/read*/       	//Receive STAtus
#define ARAH2       0x27    /*/write*/      	//Rec. Addr. High 2
#define ARAL1       0x28    /*/read\write*/ 	//Receive Address Low 1
#define ARHCR       0x29                    	//Receive HDLC Control
#define ARAL2       0x29                    	//Receive Addr. Low 2
#define AXBCL       0x2a                    	//Transmit Byte Count Low
#define ABGR        0x2b    									//Baudrate Generator Register
#define ACCR2       0x2c    									//Channel Configuration Register 2
#define ARBCH       0x2d    									//Receive Byte Count High
#define AXBCH       0x2d    									//Transmit Byte Count High
#define AVSTR       0x2e    									//Version STatus
#define ARLCR       0x2e											//Receive Frame Length Check
#define ACCR1       0x2f    									//Channel Configuration Register 1
#define ATSAX       0x30    						  	 	//时间分配发送
#define ATSAR       0x31    							 		//时间槽分配接受
#define AXCCR       0x32    									//Transmit Channel Capacity 发送通道容量
#define ARCCR       0x33    									//Receive Channel Capacity  接受通道容量

/*CHANNEL B REGISTER ADDRESS*/
#define BFIFO       0x40  
#define BISTA       0x60  
#define BMASK       0x60    
#define BSTAR       0x61    
#define BCMDR       0x61    
#define BMODE       0x62    
#define BTIMR       0x63    
#define BEXIR       0x64    
#define BXAD1       0x64   
#define BRBCL       0x65    
#define BXAD2       0x65    
#define BRAH1       0x66    
#define BRSTA       0x67    
#define BRAH2       0x67    
#define BRAL1       0x68    
#define BRHCR       0x69    
#define BRAL2       0x69    
#define BXBCL       0x6a    
#define BBGR        0x6b    
#define BCCR2       0x6c    
#define BRBCH       0x6d    
#define BXBCH       0x6d    
#define BVSTR       0x6e    
#define BRLCR       0x6e    
#define BCCR1       0x6f    
#define BTSAX       0x70    
#define BTSAR       0x71    
#define BXCCR       0x72    
#define BRCCR       0x73    


//W H H读取中断寄存器的时候，中断就被清除了。
/*iSTA*/
#define RME         0x80			//接受消息结束
#define RPF         0x40			//接受池满
#define RSC         0x20			//接受状态改变（在自动模式下）
#define XPR         0x10			//发送池就绪
#define TIN         0x08			//定时器中断
#define ICA         0x04			//通道A中断
#define EXA         0x02			//通道A扩展中断
#define EXB         0x01			//通道B扩展中断


/*exir*/
#define XMR         0x80			//发送报文重复
#define XDU_EXE     0x40			//发送数据欠载 或 扩展发送结束
#define PCE         0x20			//协议错误
#define RFO         0x10			//接受帧溢出
#define CSC         0x08			//清除发送状态改变
#define RFS         0x04			//接受帧开始

/*star*/
#define XDOV        0x80			//发送数据溢出
#define XFW         0x40			//发送FIFO写使能		XFW:1: Transmit FIFO Write Enable.Data can be written to the XFIFO.
#define XRNR        0x20			//发送RNR（自动模式下）
#define RRNR        0x10			//接受RNR（自动模式下）
#define RLI         0x08			//接受线路无效
#define CEC         0x04			//命令执行			
#define CTS         0x02			//清除发送状态
#define WFA         0x01			//等待确认（自动模式）
#define XFW_CEC     0x44			//只有CEC为0是，XFW才有效

/*rsta*/  //RFIFO的最后一个字节中存储
#define VFR         0x80			//有效帧(1 有效，0 无效)
#define RDO         0x40			//接受数据溢出()
#define SAB_CRC     0x20			//CRC比较检查(0 失败，1 0k)
#define RAB         0x10			//接受消息中止(CPU必须丢弃改帧)
#define HA          0x0C			//高位字节地址进行比较
#define C_R         0x02			//命令/响应
#define LA          0x01			//低字节地址进行比较

/*cmdr*/
#define RMC         0x80			//接受消息完成
#define RHR         0x40			//复位HDLC接受								Reset HDLC Receiver.All data in the RFIFO and the HDLC receiver deleted.
#define RNR_XREP    0x20			//接受不就绪/发送重复
#define STI         0x10			//启动定时器
#define XTF         0x08			//传输透明帧
#define XIF         0x04			//发送I帧（自动模式下）
#define XME         0x02			//发送报文结束（中断模式）
#define XRES        0x01			//发送复位 (可中止帧传输)		Transmit Reset.The contents of the XFIFO is deleted and IDLE is transmitted.

/*control*/
#define UFRAME      0x00
#define IFRAME      0x01

/*mode register bit*/
#define MDS1				0x80			//模式选择
#define MDS0 				0x40
#define ADM					0x20			//地址模式
#define TMD					0x10			//定时器模式
#define RAC					0x08			//接收器有效		
#define RTS					0x04			//请求发送		(0,HSCX自动控制，1，cpu控制)
#define TRS					0x02			//定时器分辨率
#define TLP					0x01			//测试循环	

/*CCR2 bit*/
#define SOC1			  0x80			//特殊输出控制
#define SOC0				0x40			
#define BR9					0x80			//波特率 第9位
#define BR8					0x40		
#define BDF					0x20			//波特率分频因子
#define TSS					0x10			//发送时钟源选择
#define XCS0				0x20			//发送/接受时钟移位，bit0
#define RCS0				0x10			//
#define TIO					0x08			//发送时钟输入输出开关
#define CIE					0x04			//清除发送中断使能
#define RIE					0x02			//接受开始中断使能(RFS=1)
#define DIV					0x01			//数据翻转 （用于NRZ模式时）

/*CCR1 bit*/
#define PU					0x80 			//上电、掉电切换(0掉电，1上电)
#define SC1					0x40			//串行端口配置  ( 00 NRZ,10 NRZI,01定时1,11 定时2)
#define SC0					0x20			
#define ODS					0x10			//输出驱动选择	（0 漏极开路，1 推免输出）
#define ITF_OIN			0x08			//帧间时间填充/插入(选择OIN)
#define CM2					0x04			//时钟模式选择
#define CM1					0x02
#define CM0					0x01


#endif //__SAF82525_H__
