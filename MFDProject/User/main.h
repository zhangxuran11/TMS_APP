#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include "os.h"
#include "stdbool.h"

/*事件标志组 位含义定义*/
#define  CAN2TxLED_FLAG  		0x01
#define  CAN2RxLED_FLAG 		0x02
#define  CAN2LEDFLAG_VALUE 	0x00

/*子系统协议的标志位*/
#define  TxSDRFlag			0x01			//发送SDR标志  （定时器时间到）
#define  TxSDREndFlag		0x02			//发送SDR结束标志
#define  RxStartFlag		0x04			//接受开始标志
#define  RxEndFlag			0x08			//接受结束标志
#define  RxOutTimeFlag	0x10
#define  RxParityFrameError	0x20	//奇偶校验错误 ,帧错误
#define	 RxBCCError			0x40			//BCC校验错误

#define TxSDROrTDRFlag			0x01			//BCU 可能发送SDR，或者TDR
#define TxSDROrTDREndFlag		0x02

/*BCU命令的标志位*/
extern	bool 	BCUTxTdrFlag; //是否需要发送TDR 0,不需要， 1 需要
extern 	bool 	BCUClearStatDataFlag	;
extern	bool	BCUSetTimeFlag ;


extern u8  ChooseBoard ;

//*测试使用的全局变量的定义*/
extern u8 g_Uart2RxStartFlag;
extern u8 g_Uart2RxDataFlag;
extern u8 g_Uart2FrameErrorFlag;

extern u8 g_Uart3RxStartFlag;
extern u8 g_Uart3RxDataFlag;
extern u8 g_Uart3FrameErrorFlag;

extern u8 g_Uart4RxStartFlag;
extern u8 g_Uart4RxDataFlag;
extern u8 g_Uart4FrameErrorFlag;
extern u8 g_BCURxSdOrTdFlag;
extern u8 g_Uart4NackFlag;

/*事件标志组*/
extern OS_FLAG_GRP  DoorEventFlags;			//门
extern OS_FLAG_GRP 	ACCEventFlags;			//空调事件标志组
extern OS_FLAG_GRP 	BCUEventFlags;			//制动事件标志组
extern OS_FLAG_GRP ACPEventFlags;				//ACP空压机 事件标志组
extern OS_FLAG_GRP SIVEventFlags;				//SIV辅助电源 事件标志组
extern OS_FLAG_GRP VVVF1EventFlags;			//VVVF1牵引1 事件标志组
extern OS_FLAG_GRP VVVF2EventFlags;			//VVVF2牵引2 事件标志组
extern OS_FLAG_GRP ATCEventFlags;				//ATC信号 事件标志组
extern OS_FLAG_GRP PISEventFlags;				//PIS广播 事件标志组

/*任务控制块*/
extern OS_TCB  USART3Rx_TCB;

#define  TPDO_18D_MFD1DI					0
#define  TPDO_18E_MFD2DI					1
#define  TPDO_18F_MFD3DI					2

#define  TPDO_190_DoorSta1				3
#define  TPDO_191_DoorSta2				4

/*子设备故障*/
#define  TPDO_1A9_SubDecFault			28
#define  TPDO_1AC_SubDecFault			31
#define  TPDO_1AD_SubDecFault			32
#define  TPDO_1AE_SubDecFault			33

/*ACC*/
#define  TPDO_192_ACCSta1					5
#define  TPDO_1AA_ACCSta2					29

/*BCU*/
#define  TPDO_193_BCUSta1					6
#define  TPDO_194_BCUSta2					7
#define  TPDO_195_BCUSta3					8

/*ATC*/
#define  TPDO_196_ATCSta1					9
#define  TPDO_197_ATCSta2					10
#define  TPDO_198_ATCSta3					11
#define  TPDO_199_ATCSta4					12
#define  TPDO_1AB_ATCTime					30

/*PIS*/
#define  TPDO_19A_PISSta1					13
#define  TPDO_19B_PISSta2					14

/*VVVF*/
#define  TDPO_19C_VVVFSta1				15
#define  TDPO_19D_VVVFSta2				16
#define  TDPO_19E_VVVFSta3				17
#define  TDPO_19F_VVVFSta4				18
#define  TDPO_1A0_VVVFSta5				19

#define  TDPO_1A1_VVVFSta1				20
#define  TDPO_1A2_VVVFSta2				21 
#define  TDPO_1A3_VVVFSta3				22
#define  TDPO_1A4_VVVFSta4				23
#define  TDPO_1A5_VVVFSta5				24  

/*SIV*/
#define  TDPO_1A6_SIVSta1					25
#define  TDPO_1A7_SIVSta2					26
#define  TDPO_1A8_SIVSta3					27

#endif //__MAIN_H__
