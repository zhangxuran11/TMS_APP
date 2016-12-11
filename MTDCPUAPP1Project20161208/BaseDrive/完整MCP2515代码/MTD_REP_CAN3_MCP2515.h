#ifndef __MTD_REP_CAN3_MCP2515_H__
#define __MTD_REP_CAN3_MCP2515_H__

#include "MTD_REP_SPI1.h"
#include "MCP2515_RegisterDefine.h"


/*接受缓冲区个数，发送缓冲区*/
#define MCP2515_TXBUF_NUM  (3)		//MCP2515 有3个发送缓冲区

#define MCP2515_TXBUF_0 (TXB0SIDH)
#define MCP2515_TXBUF_1 (TXB1SIDH)
#define MCP2515_TXBUF_2 (TXB2SIDH)

#define MCP2515_RXBUF_0 (RXB0SIDH)
#define MCP2515_RXBUF_1 (RXB1SIDH)

/*返回值设置*/
#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)
#define MCP2515_ALLTXBUSY  (2)		//所有的发送缓冲区满

#define CAN_OK         (0)
#define CAN_FAILINIT   (1)	 //初始化失败
#define CAN_FAILTX     (2)	 //发送失败
#define CAN_MSGAVAIL   (3)	 //消息可用
#define CAN_NOMSG      (4)	 //没有消息
#define CAN_CTRLERROR  (5)	 //错误
#define CAN_FAIL       (0xff)	//失败

/*波特率的选择*/
#define MCP2515_CAN_BAUD_5K     (1)
#define MCP2515_CAN_BAUD_10K    (MCP2515_CAN_BAUD_5K + 1)
#define MCP2515_CAN_BAUD_20K    (MCP2515_CAN_BAUD_10K + 1)
#define MCP2515_CAN_BAUD_40K    (MCP2515_CAN_BAUD_20K + 1)
#define MCP2515_CAN_BAUD_50K    (MCP2515_CAN_BAUD_40K + 1)
#define MCP2515_CAN_BAUD_80K    (MCP2515_CAN_BAUD_50K + 1)
#define MCP2515_CAN_BAUD_100K   (MCP2515_CAN_BAUD_80K + 1)
#define MCP2515_CAN_BAUD_125K   (MCP2515_CAN_BAUD_100K + 1)
#define MCP2515_CAN_BAUD_200K   (MCP2515_CAN_BAUD_125K + 1)
#define MCP2515_CAN_BAUD_250K   (MCP2515_CAN_BAUD_200K + 1)
#define MCP2515_CAN_BAUD_400K   (MCP2515_CAN_BAUD_250K + 1)
#define MCP2515_CAN_BAUD_500K   (MCP2515_CAN_BAUD_400K + 1)
#define MCP2515_CAN_BAUD_800K   (MCP2515_CAN_BAUD_500K + 1)
#define MCP2515_CAN_BAUD_1M     (MCP2515_CAN_BAUD_800K + 1)


extern 	u8 MCP2515_CAN3_PORT;
#define CAN3 	(CAN_TypeDef *)(&MCP2515_CAN3_PORT)

///*Frame Definition*/ 
//typedef enum 
//{ 
//    CAN_STD=(unsigned char)(0x00), 
//    CAN_RTR=(unsigned char)(0x40) 
//}CAN_FrameType_TypeDef;

//typedef struct Frame_Strut 
//{ 
//    unsigned int SID; 
//    unsigned char DLC; 
//    CAN_FrameType_TypeDef Type; 
//}Frame_TypeDef;

/* Exported variables --------------------------------------------------------*/ 
extern unsigned char RXB0D[8]; 
extern unsigned char TXB0D[8]; 

/* Exported functions --------------------------------------------------------*/ 
unsigned char MTDREP_CAN3_MCP2515_Init(const unsigned char CanBaudRate); 
void MTDREP_CAN3_MCP2515_Reset(void); 			//发送复位指令，复位mcp2515
unsigned char MTDREP_CAN3_MCP2515_Read_Register(const unsigned char Addr);	/*读取一个寄存器的值*/
void MTDREP_CAN3_MCP2515_Read_RegisterS(const unsigned char Addr, unsigned char RegValues[], const unsigned char RegNum);  /*读取多个寄存器的值*/
void MTDREP_CAN3_MCP2515_Set_Register(const unsigned char Addr,const unsigned char Data); //设置一个寄存器的值
void MTDREP_CAN3_MCP2515_Set_RegisterS(const unsigned char Addr,const unsigned char SetValues[],const unsigned char RegNum); //设置多个寄存器的值
void MTDREP_CAN3_MCP2515_Register_BitModify(const unsigned char Addr,const unsigned char Mask, unsigned char Data); //修改寄存器某位的值
unsigned char MTDREP_CAN3_MCP2515_Read_Status(void);		//读取寄存器状态
unsigned char MTDREP_CAN3_MCP2515_ReadRx_Status(void);	 //读取接受的状态
unsigned char MTDREP_CAN3_MCP2515_SetControlRegister_Mode(const unsigned char NewMode);		//设置控制寄存器 的工作模式
unsigned char MTDREP_CAN3_MCP2515_Config_Rate(const unsigned char  BaudRate);							//配置mcp2515 的波特率
void MTDREP_CAN3_MCP2515_Read_CAN_ID(const unsigned char RxIdReg_Addr, unsigned char* IDE_ExtFlag, unsigned int* Std_id,unsigned int * Ext_id);	//读取接收到的canid标识
void MTDREP_CAN3_MCP2515_Read_CAN_Message(const unsigned char RxBuf_SIDH_Addr, CanRxMsg* Message);			//读取接受的can消息数据
void MTDREP_CAN3_MCP2515_Write_CAN_ID(const unsigned char TxIdReg_Addr, const unsigned char IDE_ExtFlag, const unsigned int can_id);		//写入要发送的canid
void MTDREP_CAN3_MCP2515_Write_CAN_Message(const unsigned char TxBuf_SIDH_Addr, const CanTxMsg* Message);		//写入要发送的can消息
void MTDREP_CAN3_MCP2515_Start_Transimt(const unsigned char TxBuf_SIDH_Addr);				//开始发送消息
unsigned char MTDREP_CAN3_MCP2515_Get_Free_TxBuf(unsigned char * TxBufNum);
void MTDREP_CAN3_MCP2515_Init_CAN_Buffer(void);//设置屏蔽寄存器，滤波寄存器，初始化发送缓冲区

/*
把can3 的初始化，发送，都放入 can_stm32.c文件中，	
*/
/*接受消息*/
unsigned char MCP2515_CAN3_Receive_Message(CanRxMsg* msg);
unsigned char MCP2515_CAN3_Check_Receive(void);
unsigned char MCP2515_CAN3_Check_Error(void);
void MCP2515_CAN3_Interrupt(CanRxMsg *msg);

void MCP2515_REST1_GpioInit(void);
void MCP2515_INT1_GpioInit(void);
void MCP2515_INT1_IRQ(void);




//void MTDREP_CAN3_SPISendByte_MCP2515(unsigned char Addr,unsigned char Data); 
//unsigned char MTDREP_CAN3_SPIReadByte_MCP2515(unsigned char Addr); 
//unsigned char MTDREP_CAN3_SPI_CMD_MCP2515(unsigned char CMD); 
//void MTDREP_CAN3_SPICAN_SendData(unsigned char *CAN_TX_Buf,Frame_TypeDef *Frame); 
//unsigned char MTDREP_CAN3_SPICAN_ReceiveData(unsigned char *CAN_RX_Buf,Frame_TypeDef *Frame);
//void MTDREP_CAN3_Send_anylength(u8 *CAN_TX_Buf,u8 length1);



#endif  //__MTD_REP_CAN3_MCP2515_H__
