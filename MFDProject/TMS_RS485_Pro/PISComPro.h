#ifndef __PISCOMPRO_H__
#define __PISCOMPRO_H__

#include "TMS_ComunDefine.h"

#define  PIS_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  PIS_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

#define  PIS_STX							0x02	//定义发送 的开始字节为0x02
#define  PIS_SDR_DATALEN			0x1A
//#define  PIS_ETX							0x7E	

#define  PIS_SD_DATALEN				0x1A

/*定义错误编码值*/
#define  PIS_DataError  				-1			//接受的前4字节有不一致的错误
#define  PIS_GetTimeoutError	  -2			//发送请求之后，20ms以内没有接收到数据
#define  PIS_FrameError					-3			//检测到帧错误、奇偶校验错误时
#define  PIS_BCCError       	  -4  	 	//BCC 符号不符合计划值时
#define  PIS_GetError						-5			//表示接受数据错误


typedef struct
{	
	u8 PIS_DataLongth;					//数据区数据长度
	u8 PIS_TimeVaild;						//时间有效
	u8 Byte3Undef;							//
	
	/*byte4*/
	u8 PIS_PISClosed			:1;		//门关好	
	u8 PIS_ArrivePa				:1;		//到站广播	
	u8 PIS_PredictPa			:1;		//预报站广播
	u8 PIS_LeavePa				:1;		//离站广播
	u8 PIS_CarNumber			:1;		//位置
	u8 PIS_LeftPISOp			:1;		//本站开A侧门，开左门
	u8 PIS_RightPISOp			:1;		//本站开B侧门，开右门
	u8 PIS_Atomode				:1;		//ATO模式
	
	/*byte5*/						
	u8 Year;										//年		BCD
	u8 Month;										//月
	u8 Day;											//日
	u8 Hour;										//时
	u8 Minute;									//分
	u8 Second;									//秒	
	
	u8 PIS_TrainNumberH8;				//列车编号		
	u8 PIS_TrainNumberL8;	
	
	u8 Undef5Byte[5];
	
	u8 PIS_TrainDesIDH8;				//列车目的地序号
	u8 PIS_TrainDesIDL8;	
	
	u8 PIS_TrainNowIDH8;				//现在停站ID
	u8 PIS_TrainNowIDL8;
	
	u8 PIS_TrainNextIDH8;				//下一站ID
	u8 PIS_TrainNextIDL8;
	
	u8 Undef4Byte[4];
}PISSdrText;

typedef struct
{
	u8 PISSdrStx;
	PISSdrText PISSdrData;
	u8 PISSdrBcc;
}PISSdrFrame;

typedef struct
{
	u8 PIS_DataLongth;					//数据区数据长度
	u8 Byte2Undef;							//
	
	/*byte3*/
	u8 PIS_AutoPa					:1;		//自动广播模式
	u8 PIS_ManPa					:1;		//手动广播模式	
	u8 Byte3Undef6Bit			:6;
		
	/*byte4*/		
	u8 PIS_Pea1						:1;		//1车乘客紧急报警
	u8 PIS_Pea2						:1;		//2车乘客紧急报警
	u8 PIS_Pea3						:1;		//3车乘客紧急报警
	u8 PIS_Pea4						:1;		//4车乘客紧急报警
	u8 PIS_Pea5						:1;		//5车乘客紧急报警
	u8 PIS_Pea6						:1;		//6车乘客紧急报警
	u8 Byte4Undef2Bit			:2;
	
	u8 PIS_TrainOriIDH8;				//起始站ID
	u8 PIS_TrainOriIDL8;				
	u8 PIS_TrainNowIDH8;				//现在停站ID	
	u8 PIS_TrainNowIDL8;				
	u8 PIS_TrainNextIDH8;				//下一站ID
	u8 PIS_TrainNextIDL8;
	u8 PIS_TrainDesIDH8;				//终点站ID
	u8 PIS_TrainDesIDL8;
	
	u8 PIS_ControlLevel;				//主机状态		0=默认值；1=主；2=从；3=半主
	
	u8 Undef14Byte[14];
}PISSdText;		

typedef struct
{
	u8 PISSdStx;
	PISSdText PISSdData;
	u8 PISSdBcc;
}PISSdFrame;


	
/*
	函数说明：
功能：监控设备发送给PIS的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendPISSdr(PISSdrText * pInData/*,u8 * pOutData*/);  


/*
	函数说明：
功能：监控设备接受PISD的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。

返回值：0,无错，  其他有错。
*/
s8 GetPISSd(u8 *pInData,void *pOutData);		

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckPISGetError(u8 *pInData);




#endif  //__PISCOMPRO_H__
