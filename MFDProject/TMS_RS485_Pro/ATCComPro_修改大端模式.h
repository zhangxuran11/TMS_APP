#ifndef __ATCCOMPRM_H__
#define __ATCCOMPRM_H__

#include "TMS_ComunDefine.h"

#define  ATC_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  ATC_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

#define  ATC_DLE							0x10
#define  ATC_STX							0x02	//定义发送 的开始字节为0x20
#define  ATC_ETX							0x03

#define  ATC_SDR_TEXTCODEH8		0x05
#define  ATC_SDR_TEXTCODEL8		0x0F

/*定义错误编码值*/
#define  ATC_DataError  				-1			//接受的前4字节有不一致的错误
#define  ATC_GetTimeoutError	  -2			//发送请求之后，20ms以内没有接收到数据
#define  ATC_FrameError					-3			//检测到帧错误、奇偶校验错误时
#define  ATC_BCCError       	  -4  	 	//BCC 符号不符合计划值时
#define  ATC_GetError						-5			//表示接受数据错误


typedef struct
{
	u8 ATC_TextCodeH8;				//测试码（Text Code）
	u8 ATC_TextCodeL8;
	u8 ATC_TotalweightH8;			//列车总重量
	u8 ATC_TotalweightL8;
	
	//u8 TrainSpeed;			
	
	/*byte5*/
	u8 Byte5Undef5Bit						:5;		
	u8 ATC_No6VVVF1Vaild				:1;		//6号车VVVF1动作
	//u8 ATC_No6VVVF2Vaild				:1;		//6号车VVVF2动作	
	u8 ATC_No3VVVF1Vaild				:1;		//3号车VVVF1动作
	//u8 ATC_No3VVVF2Vaild				:1;		//3号车VVVF2动作	
	u8 ATC_No1VVVF1Vaild				:1;		//1号车VVVF1动作
	//u8 ATC_No1VVVF2Vaild				:1;		//1号车VVVF2动作

	
	/*byte6*/
	u8 Byte6Undef5Bit						:5;
	u8 ATC_No6VVVF1EleBrake			:1;
	//u8 ATC_No6VVVF2EleBrake			:1;
	u8 ATC_No3VVVF1EleBrake			:1;
	//u8 ATC_No3VVVF2EleBrake			:1;	
	u8 ATC_No1VVVF1EleBrake			:1;		//1 号车VVVF1电制有效
	//u8 ATC_No1VVVF2EleBrake			:1;

	/*Byte7*/
	u8 Byte7Undef2Bit						:2;
	u8 ATC_BCU6vaild						:1;
	u8 ATC_BCU5vaild						:1;
	u8 ATC_BCU4vaild						:1;
	u8 ATC_BCU3vaild						:1;
	u8 ATC_BCU2vaild						:1;
	u8 ATC_BCU1vaild						:1; 	//1 号车BCU 动作
	
	//u8 OperatStatus;
	/*Byte8*/
	u8 UndefByte8;
	
	/*byte9*/
	u8 Byte9Undef6Bit						:6;
	u8 ATC_Slide6								:1;		//6 号车滑行
	u8 ATC_Slide1								:1;		//1 号车滑行
	
	/*byte10*/
	u8 Byte10Undef4Bit					:4;
	u8 ATC_No6VVVF1SlideStatu		:2;
	//u8 ATC_No6VVVF2SlideStatu		:2;
	u8 ATC_No1VVVF1SlideStatu		:2;		//1 号车 VVVF1滑行状态
	//u8 ATC_No1VVVF2SlideStatu		:2;		
	
	
	u8 ATC_TrainNumber;								//列车编号  BCD
	u8 Byte12Spare;					
	u8 ATC_TimeVaild;								//时间有效 0 1
	
	u8 ATC_Year;										//年		BCD
	u8 ATC_Month;                   //月
	u8 ATC_Day;		                  //日
	u8 ATC_Hour;	                  //时
	u8 ATC_Minute;                  //分
	u8 ATC_Second;                  //秒	
	
	u8 ATC_TBFroce1;								//1车牵引/制动力
	u8 ATC_TBFroce2;								//6车牵引/制动力
	
	u8 ATC_DoorStatu;								//车门状态 ： [00H：全车门关闭 ]， [01H：左侧车门敞开]，[02H：右侧车门敞开]， [03H：双侧车门敞开]
	u8 ATC_LoadRate;								//（平均）乘车率
	
	u8 Undef24Byte[24];
}ATCSdrText;

typedef struct
{
	u8 ATCSdrDle1;									//开始字节DLE
	
	u8 ATCSdrStx;										//开始字符 0x02
	ATCSdrText 		ATCSdrData;				//数据段
	u8 ATCSdrDle2;									//结束字节DLE
	
	u8 ATCSdrEtx;
	u8 ATCSdrBcc;
}ATCSdrFrame;


/*SD文本是先放低字节，在放入高字节*/		//WHHH
typedef struct
{
	u8 ATC_TextCodeL8;
	u8 ATC_TextCodeH8;					//测试码（Text Code）
	
	/*byte3,ATC模式*/
	u8 Byte3Undef5Bit				:5;
	u8 ATC_AMC_AVAIL				:1;	//AMC_AVAIL;
	u8 ATC_MCS_AVAIL				:1;	//MCS_AVAIL
	u8 ATC_RM_AVAIL					:1;	//RM_AVAIL
	
	
	/*byte4*/
		u8 ATC_BLOCK						:1;//BLOCK
		u8 ATC_CBTC							:1;//CBTC
		u8 Byte4Undef2bit						:2;
		u8 ATC_AMC							:1;//AMC
		u8 ATC_MCS							:1;//MCS
	u8 ATC_RMR							:1;//RMR
	u8 ATC_RMF							:1;//RMF





	
	u8 ATC_Speed;							//速度
	u8 ATC_ObjSpeed;					//目标速度
	
	/*byte7*/
	u8 ATC_EmergenBrak			:1;//紧急制动
	u8 ATC_OverSpeed				:1;//超速
	u8 ATC_DrivReq					:1;//开车要求
	u8 Byte7Undef5Bit				:5;
	
	u8 ATC_CrewNumberH8;			//乘务员编号
	u8 ATC_CrewNumberL8;
	u8 ATC_TrainOperatNoH8;		//列车运用编号
	u8 ATC_TrainOperatNoL8;
	u8 ATC_TrainDestNoH8;			//列车目的地编号
	u8 ATC_TrainDestNoL8;
	
	/*byte18*/
	u8 Byte18Undef3Bit			:3;
	u8 ATC_HoldBrakCmd			:1;//保持制动指令
	u8 ATC_LeftDoorLockSta	:1;//左车门锁闭状态
	u8 ATC_RightDoorLockSta	:1;//右车门锁闭状态
	u8 Byte18Undef2Bit			:2;
	
	/*19*/
	u8 ATC_PrePaSig					:1;	//预报站信号
	u8 ATC_ArrivePa					:1;	//到站信号
	u8 ATC_LeavePa					:1;	//出发车站
	u8 Byte19Undef5Bit			:5;
	
	u8 ATC_TrainNowIDH8;				//现在停站ID	
	u8 ATC_TrainNowIDL8;				
	u8 ATC_TrainNextIDH8;				//下一站ID
	u8 ATC_TrainNextIDL8;
	
	u8 Undef4Byte[4];
	
		/*byte5*/						
	u8 ATC_Year;								//年		10进制
	u8 ATC_Month;								//月
	u8 ATC_Day;									//日
	u8 ATC_Hour;								//时
	u8 ATC_Minute;							//分
	u8 ATC_Second;							//秒	

	u8 ATC_DoorControlMode;			//车门控制模式 ：[00H:手动开关]，[01H:自动开?手动关]，[02H:自动开合]
	
	/*35byte*/
	u8 ATC_RightOpenCmd		 	:1;
	u8 ATC_RightCloseCmd	 	:1;
	u8 ATC_LeftOpenCmd		 	:1;
	u8 ATC_LeftCloseCmd	 	 	:1;
	u8 Byte35Undef4Bit			:4;
	
	u8 Spare4Byte[4];
	
	/*byte40*/
	u8 ATC_BMModeRelays			:1;	//BM模式继电器
	u8 ATC_EBRD1Mode				:1;//信号系统EBRD1状态
	u8 ATC_EBRD2Mode				:1;//信号系统EBRD2状态
	u8 ATC_TractionRemove		:1;//牵引切除
	u8 ATC_HoldBrakeApplied	:1;//保持制动施加
	u8 ATC_AAideVaild				:1;//A侧门使能信号
	u8 ATC_BSideVaild				:1;//B侧门使能信号
	u8 ATC_TractionForward	:1;//牵引向前信号
	
	/*byte41*/
	u8 Byte41Bit0						:1;
	u8 ATC_TractionCmd			:1;//牵引指令
	u8 ATC_BrakeCmd					:1;//制动指令
	u8 Undef4Bit						:4;
	u8 ATC_DriveRoomMaster	:1;//司机室主控
	
	/*byte42*/
	u8 ATC_CBTCBMButton			:1;//CBTC/BM模式转换按钮
	u8 ATC_RMModeButt				:1;//RM模式按钮
	u8 Undef6Bit						:6;
	
	u8 Undef7Byte[7];		//43-49
	
}ATCSdText;

typedef struct
{
	u8 ATCSdStx;												//开始字符 0x02
	ATCSdText 		ATCSdData;				//数据段
	u8 ATCsdEtx;
	u8 ATCSdBcc;
}ATCSdFrame;

/*
	函数说明：
功能：监控设备发送给ATC的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendATCSdr(ATCSdrText * pInData/*,u8 * pOutData*/);  


/*
	函数说明：
功能：监控设备接受ATCD的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。

返回值：0,无错，  其他有错。
*/
s8 GetATCSd(u8 *pInData,void *pOutData);		

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckATCGetError(u8 *pInData);


#endif  //__ATCCOMPRM_H__
