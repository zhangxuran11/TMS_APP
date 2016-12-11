#ifndef __MTD_CCU_CAN3_MCP2515_H__
#define __MTD_CCU_CAN3_MCP2515_H__

#include "MTD_CCU_SPI.h"

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

#define CAN_OK         		 (0)
#define CAN_FAILINIT   		 (1)	 //初始化失败
#define CAN_FAILTX     		 (2)	 //发送失败
#define CAN_MSGAVAIL   		 (3)	 //消息可用
#define CAN_NOMSG      		 (4)	 //没有消息
#define CAN_CTRLERROR  		 (5)	 //错误
#define CAN_FAIL       		 (0xff)	//失败

/*波特率的选择*/
#define MCP2515_CAN_BAUD_5K     5000
#define MCP2515_CAN_BAUD_10K    10000
#define MCP2515_CAN_BAUD_20K    20000
#define MCP2515_CAN_BAUD_40K    40000
#define MCP2515_CAN_BAUD_50K    50000
#define MCP2515_CAN_BAUD_80K    80000
#define MCP2515_CAN_BAUD_100K   10000
#define MCP2515_CAN_BAUD_125K   125000
#define MCP2515_CAN_BAUD_200K   200000
#define MCP2515_CAN_BAUD_250K   250000
#define MCP2515_CAN_BAUD_400K   400000
#define MCP2515_CAN_BAUD_500K   500000
#define MCP2515_CAN_BAUD_800K   800000
#define MCP2515_CAN_BAUD_1M     1000000

#define CANFOSC			20000000		// 20MHz

#define MCP2515_CAN3_PORT			((uint32_t)0x03)
#define CAN3 									((CAN_TypeDef *) MCP2515_CAN3_PORT)

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

///* Exported variables --------------------------------------------------------*/ 
//extern unsigned char RXB0D[8]; 
//extern unsigned char TXB0D[8]; 

/* Exported functions --------------------------------------------------------*/ 
unsigned char MTDCCU_CAN3_MCP2515_Init(const unsigned int CanBaudRate); 
void MTDCCU_CAN3_MCP2515_Reset(void); 			//发送复位指令，复位mcp2515
unsigned char MTDCCU_CAN3_MCP2515_Read_Register(const unsigned char Addr);	/*读取一个寄存器的值*/
void MTDCCU_CAN3_MCP2515_Read_RegisterS(const unsigned char Addr, unsigned char RegValues[], const unsigned char RegNum);  /*读取多个寄存器的值*/
void MTDCCU_CAN3_MCP2515_Set_Register(const unsigned char Addr,const unsigned char Data); //设置一个寄存器的值
void MTDCCU_CAN3_MCP2515_Set_RegisterS(const unsigned char Addr,const unsigned char SetValues[],const unsigned char RegNum); //设置多个寄存器的值
void MTDCCU_CAN3_MCP2515_Register_BitModify(const unsigned char Addr,const unsigned char Mask, unsigned char Data); //修改寄存器某位的值
unsigned char MTDCCU_CAN3_MCP2515_Read_Status(unsigned char cmd);		//读取寄存器状态
unsigned char MTDCCU_CAN3_MCP2515_ReadRx_Status(void);	 //读取接受的状态
unsigned char MTDCCU_CAN3_MCP2515_SetControlRegister_Mode(const unsigned char NewMode);		//设置控制寄存器 的工作模式
unsigned char MTDCCU_CAN3_MCP2515_Config_Rate(const unsigned int  BaudRate);							//配置mcp2515 的波特率
void MTDCCU_CAN3_MCP2515_Read_CAN_ID(const unsigned char RxIdReg_Addr, unsigned char* IDE_ExtFlag, unsigned int* Std_id,unsigned int * Ext_id);	//读取接收到的canid标识
void MTDCCU_CAN3_MCP2515_Read_CAN_Message(const unsigned char RxBuf_SIDH_Addr, CanRxMsg* Message);			//读取接受的can消息数据
void MTDCCU_CAN3_MCP2515_Write_CAN_ID(const unsigned char TxIdReg_Addr, const unsigned char IDE_ExtFlag, const unsigned int can_id);		//写入要发送的canid
void MTDCCU_CAN3_MCP2515_Write_CAN_Message(const unsigned char TxBuf_SIDH_Addr, const CanTxMsg* Message);		//写入要发送的can消息
void MTDCCU_CAN3_MCP2515_Start_Transimt(const unsigned char TxBuf_SIDH_Addr);				//开始发送消息
unsigned char MTDCCU_CAN3_MCP2515_Get_Free_TxBuf(unsigned char * TxBufNum);
void MTDCCU_CAN3_MCP2515_Init_CAN_Buffer(void);//设置屏蔽寄存器，滤波寄存器，初始化发送缓冲区
void Pro_CAN3_ERROR(void);
void MTDCCU_CAN3_MCP2515_IntError_Init(const unsigned int  BaudRate);
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


//void MTDCCU_MCP2515_Init(void); 
//void MCP2515_Reset(void); 
//void MCP2515_BitModify(unsigned char Addr,unsigned char Mask,unsigned char Data); 
//void SPISendByte_MCP2515(unsigned char Addr,unsigned char Data); 
//unsigned char SPIReadByte_MCP2515(unsigned char Addr); 
//unsigned char SPI_CMD_MCP2515(unsigned char CMD); 
//void SPICAN_Send8bytes(unsigned char *CAN_TX_Buf); 
//void SPICAN_SendOneByte(unsigned char Data); 
//unsigned char SPICAN_Receive_onebyte(void); 
//void SPICAN_SendData(unsigned char *CAN_TX_Buf,Frame_TypeDef *Frame); 
//unsigned char SPICAN_ReceiveData(unsigned char *CAN_RX_Buf,Frame_TypeDef *Frame);

//void MTDCCU_CAN_Send_anylength(u8 *CAN_TX_Buf,u8 length1);


////MCP2515波特率预分频
//#define	CAN_10Kbps	0x31
//#define CAN_25Kbps	0x13
//#define CAN_50Kbps	0x09
//#define CAN_100Kbps	0x04
//#define CAN_125Kbps	0x03
//#define CAN_250Kbps	0x01
//#define	CAN_500Kbps	0x07


/* Configuration Registers */
#define CANSTAT         0x0E
#define CANCTRL         0x0F
#define BFPCTRL         0x0C
#define TEC             0x1C
#define REC             0x1D
#define CNF3            0x28
#define CNF2            0x29
#define CNF1            0x2A
#define CANINTE         0x2B
#define CANINTF         0x2C
#define EFLG            0x2D
#define TXRTSCTRL       0x0D

/*  Recieve Filters */
#define RXF0SIDH        0x00
#define RXF0SIDL        0x01
#define RXF0EID8        0x02
#define RXF0EID0        0x03
#define RXF1SIDH        0x04
#define RXF1SIDL        0x05
#define RXF1EID8        0x06
#define RXF1EID0        0x07
#define RXF2SIDH        0x08
#define RXF2SIDL        0x09
#define RXF2EID8        0x0A
#define RXF2EID0        0x0B
#define RXF3SIDH        0x10
#define RXF3SIDL        0x11
#define RXF3EID8        0x12
#define RXF3EID0        0x13
#define RXF4SIDH        0x14
#define RXF4SIDL        0x15
#define RXF4EID8        0x16
#define RXF4EID0        0x17
#define RXF5SIDH        0x18
#define RXF5SIDL        0x19
#define RXF5EID8        0x1A
#define RXF5EID0        0x1B

/* Receive Masks */
#define RXM0SIDH        0x20
#define RXM0SIDL        0x21
#define RXM0EID8        0x22
#define RXM0EID0        0x23
#define RXM1SIDH        0x24
#define RXM1SIDL        0x25
#define RXM1EID8        0x26
#define RXM1EID0        0x27

/* Tx Buffer 0 */
#define TXB0CTRL        0x30
#define TXB0SIDH        0x31
#define TXB0SIDL        0x32
#define TXB0EID8        0x33
#define TXB0EID0        0x34
#define TXB0DLC         0x35
#define TXB0D0          0x36
#define TXB0D1          0x37
#define TXB0D2          0x38
#define TXB0D3          0x39
#define TXB0D4          0x3A
#define TXB0D5          0x3B
#define TXB0D6          0x3C
#define TXB0D7          0x3D

/* Tx Buffer 1 */
#define TXB1CTRL        0x40
#define TXB1SIDH        0x41
#define TXB1SIDL        0x42
#define TXB1EID8        0x43
#define TXB1EID0        0x44
#define TXB1DLC         0x45
#define TXB1D0          0x46
#define TXB1D1          0x47
#define TXB1D2          0x48
#define TXB1D3          0x49
#define TXB1D4          0x4A
#define TXB1D5          0x4B
#define TXB1D6          0x4C
#define TXB1D7          0x4D

/* Tx Buffer 2 */
#define TXB2CTRL        0x50
#define TXB2SIDH        0x51
#define TXB2SIDL        0x52
#define TXB2EID8        0x53
#define TXB2EID0        0x54
#define TXB2DLC         0x55
#define TXB2D0          0x56
#define TXB2D1          0x57
#define TXB2D2          0x58
#define TXB2D3          0x59
#define TXB2D4          0x5A
#define TXB2D5          0x5B
#define TXB2D6          0x5C
#define TXB2D7          0x5D

/* Rx Buffer 0 */
#define RXB0CTRL        0x60
#define RXB0SIDH        0x61
#define RXB0SIDL        0x62
#define RXB0EID8        0x63
#define RXB0EID0        0x64
#define RXB0DLC         0x65
#define RXB0D0          0x66
#define RXB0D1          0x67
#define RXB0D2          0x68
#define RXB0D3          0x69
#define RXB0D4          0x6A
#define RXB0D5          0x6B
#define RXB0D6          0x6C
#define RXB0D7          0x6D

/* Rx Buffer 1 */
#define RXB1CTRL        0x70
#define RXB1SIDH        0x71
#define RXB1SIDL        0x72
#define RXB1EID8        0x73
#define RXB1EID0        0x74
#define RXB1DLC         0x75
#define RXB1D0          0x76
#define RXB1D1          0x77
#define RXB1D2          0x78
#define RXB1D3          0x79
#define RXB1D4          0x7A
#define RXB1D5          0x7B
#define RXB1D6          0x7C
#define RXB1D7          0x7D


/*******************************************************************
 *               Bit register masks                                *
 *******************************************************************/

/* TXBnCTRL */
#define TXREQ           0x08
#define TXP             0x03

/* RXBnCTRL */
#define RXM             0x60
#define BUKT            0x04

/* CANCTRL */
#define REQOP           0xE0
#define ABAT            0x10
#define	OSM             0x08
#define CLKEN           0x04
#define CLKPRE          0x03

/* CANSTAT */
#define REQOP           0xE0
#define ICOD            0x0E

/* CANINTE */
#define RX0IE           0x01
#define RX1IE           0x02
#define TX0IE           0x04
#define TX1IE           0x08
#define TX2IE           0x10
#define ERRIE           0x20
#define WAKIE           0x40
#define MERRE           0x80

/* CANINTF */
#define RX0IF           0x01
#define RX1IF           0x02
#define TX0IF           0x04
#define TX1IF           0x08
#define TX2IF           0x10
#define ERRIF           0x20
#define WAKIF           0x40
#define MERRF           0x80

/* BFPCTRL */
#define B1BFS           0x20
#define B0BFS           0x10
#define B1BFE           0x08
#define B0BFE           0x04
#define B1BFM           0x02
#define B0BFM           0x01

/* CNF1 Masks */
#define SJW             0xC0
#define BRP             0x3F

/* CNF2 Masks */
#define BTLMODE         0x80
#define SAM             0x40
#define PHSEG1          0x38
#define PRSEG           0x07

/* CNF3 Masks */
#define WAKFIL          0x40
#define PHSEG2          0x07

/* TXRTSCTRL Masks */
#define TXB2RTS         0x04
#define TXB1RTS         0x02
#define TXB0RTS         0x01

/*EFLG Masks*/
#define RX1OVR					0x80
#define RX0OVR					0x40
#define TXBO						0x20
#define TXEP						0x10
#define RXEP						0x08
#define TXWAR						0x04
#define RXWAR						0x02
#define EWARN						0x01

/*******************************************************************
 *                    Bit Timing Configuration                     *
 *******************************************************************/
 
/* CNF1 */
#define SJW_1TQ         0x40
#define SJW_2TQ         0x80
#define SJW_3TQ         0x90
#define SJW_4TQ         0xC0

/* CNF2 */
#define BTLMODE_CNF3    0x80
#define BTLMODE_PH1_IPT 0x00

#define SMPL_3X         0x40
#define SMPL_1X         0x00

#define PHSEG1_8TQ      0x38
#define PHSEG1_7TQ      0x30
#define PHSEG1_6TQ      0x28
#define PHSEG1_5TQ      0x20
#define PHSEG1_4TQ      0x18
#define PHSEG1_3TQ      0x10
#define PHSEG1_2TQ      0x08
#define PHSEG1_1TQ      0x00

#define PRSEG_8TQ       0x07
#define PRSEG_7TQ       0x06
#define PRSEG_6TQ       0x05
#define PRSEG_5TQ       0x04
#define PRSEG_4TQ       0x03
#define PRSEG_3TQ       0x02
#define PRSEG_2TQ       0x01
#define PRSEG_1TQ       0x00

/* CNF3 */
#define PHSEG2_8TQ      0x07
#define PHSEG2_7TQ      0x06
#define PHSEG2_6TQ      0x05
#define PHSEG2_5TQ      0x04
#define PHSEG2_4TQ      0x03
#define PHSEG2_3TQ      0x02
#define PHSEG2_2TQ      0x01
#define PHSEG2_1TQ      0x00

#define SOF_ENABLED     0x80
#define WAKFIL_ENABLED  0x40
#define WAKFIL_DISABLED 0x00


/*******************************************************************
 *                  Control/Configuration Registers                *
 *******************************************************************/

/* CANINTE */
#define RX0IE_ENABLED   0x01
#define RX0IE_DISABLED  0x00
#define RX1IE_ENABLED   0x02
#define RX1IE_DISABLED  0x00
#define G_RXIE_ENABLED  0x03
#define G_RXIE_DISABLED 0x00

#define TX0IE_ENABLED   0x04
#define TX0IE_DISABLED  0x00
#define TX1IE_ENABLED   0x08
#define TX2IE_DISABLED  0x00
#define TX2IE_ENABLED   0x10
#define TX2IE_DISABLED  0x00
#define G_TXIE_ENABLED  0x1C
#define G_TXIE_DISABLED 0x00

#define ERRIE_ENABLED   0x20
#define ERRIE_DISABLED  0x00
#define WAKIE_ENABLED   0x40
#define WAKIE_DISABLED  0x00
#define IVRE_ENABLED    0x80
#define IVRE_DISABLED   0x00

/* CANINTF */
#define RX0IF_SET       0x01
#define RX0IF_RESET     0x00
#define RX1IF_SET       0x02
#define RX1IF_RESET     0x00
#define TX0IF_SET       0x04
#define TX0IF_RESET     0x00
#define TX1IF_SET       0x08
#define TX2IF_RESET     0x00
#define TX2IF_SET       0x10
#define TX2IF_RESET     0x00
#define ERRIF_SET       0x20
#define ERRIF_RESET     0x00
#define WAKIF_SET       0x40
#define WAKIF_RESET     0x00
#define IVRF_SET        0x80
#define IVRF_RESET      0x00

/* CANCTRL */ 
#define REQOP_CONFIG    0x80
#define REQOP_LISTEN    0x60
#define REQOP_LOOPBACK  0x40
#define REQOP_SLEEP     0x20
#define REQOP_NORMAL    0x00

#define ABORT           0x10

#define OSM_ENABLED     0x08

#define CLKOUT_ENABLED  0x04
#define CLKOUT_DISABLED 0x00
#define CLKOUT_PRE_8    0x03
#define CLKOUT_PRE_4    0x02
#define CLKOUT_PRE_2    0x01
#define CLKOUT_PRE_1    0x00

/* CANSTAT */
#define OPMODE_CONFIG   0x80
#define OPMODE_LISTEN   0x60
#define OPMODE_LOOPBACK 0x40
#define OPMODE_SLEEP    0x20
#define OPMODE_NORMAL   0x00


/* RXBnCTRL */
#define RXM_RCV_ALL     0x60
#define RXM_VALID_EXT   0x40
#define RXM_VALID_STD   0x20
#define RXM_VALID_ALL   0x00

#define RXRTR_REMOTE    0x08
#define RXRTR_NO_REMOTE 0x00

#define BUKT_ROLLOVER    0x04
#define BUKT_NO_ROLLOVER 0x00

#define FILHIT0_FLTR_1  0x01
#define FILHIT0_FLTR_0  0x00

#define FILHIT1_FLTR_5  0x05
#define FILHIT1_FLTR_4  0x04
#define FILHIT1_FLTR_3  0x03
#define FILHIT1_FLTR_2  0x02
#define FILHIT1_FLTR_1  0x01
#define FILHIT1_FLTR_0  0x00


/* TXBnCTRL */
#define TXREQ_SET       0x08
#define TXREQ_CLEAR     0x00

#define TXP_HIGHEST     0x03
#define TXP_INTER_HIGH  0x02
#define TXP_INTER_LOW   0x01
#define TXP_LOWEST      0x00
    

/*******************************************************************
 *                  Register Bit Masks                             *
 *******************************************************************/
 
 #define DLC_0          0x00
 #define DLC_1          0x01
 #define DLC_2          0x02
 #define DLC_3          0x03
 #define DLC_4          0x04
 #define DLC_5          0x05
 #define DLC_6          0x06
 #define DLC_7          0x07    
 #define DLC_8          0x08
 

/*******************************************************************
 *                  CAN SPI commands                               *
 *******************************************************************/

#define SPICAN_RESET       0xC0
#define SPICAN_READ        0x03
#define SPICAN_WRITE       0x02
#define SPICAN_RTS         0x80
#define SPICAN_RTS_TXB0    0x81
#define SPICAN_RTS_TXB1    0x82
#define SPICAN_RTS_TXB2    0x84
#define SPICAN_READ_STATUS 0xA0
#define SPICAN_BIT_MODIFY  0x05  
#define SPICAN_RX_STATUS   0xB0
#define SPICAN_READ_RX		 0x90				//从缓冲区读取接受的数据
#define SPICAN_WRITE_TX    0X40  			//将缓冲区的数据发送出去


#define CAN_STATE_TX2IF				0x80
#define CAN_STATE_TX2REQ			0x40
#define CAN_STATE_TX1IF				0x20
#define CAN_STATE_TX1REQ			0x10
#define CAN_STATE_TX0IF				0x08
#define CAN_STATE_TX0REQ			0x04
#define CAN_STATE_RX1IF				0x02
#define CAN_STATE_RX0IF				0x01

#define CAN_STATE_TX012REQ		0xA4

/*******************************************************************
 *                  Miscellaneous                                  *
 *******************************************************************/

#define DUMMY_BYTE      0x00
#define TXB0            0x31
#define TXB1            0x41
#define TXB2            0x51
#define RXB0            0x61
#define RXB1            0x71
#define EXIDE_SET       0x08
#define EXIDE_RESET     0x00

#endif  //__MTD_CCU_CAN3_MCP2515_H__

