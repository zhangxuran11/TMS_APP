#ifndef __DOORCOMPRO_H__
#define __DOORCOMPRO_H__

/*
(1)传输规格：
1 通讯方式 3 线式半双层通讯方式
2 通讯速度 19200bps±0.2%
3 同步方式 调同步
4 控制步骤 BASIC 顺序
5 连接方法 轮询选择
6 传输符号 NRZ
7 调制方式 基带
8 误控 垂直奇偶校验［奇数］、水平奇偶校验［奇数］
9 电路连接 1:8 连续					(主从总线的方式)
10 适用电线 公称120Ω 平衡,带屏蔽3 芯电缆
11 线路电压 DC5V±10%
*/

/*
(1)通讯帧			//注意：使用的奇校验方式
Start bit DATA bit Parity bit Stop bit
1bit 8bit 1bit(ODD) 1bit
*/

/*
(2)帧结构
No.0				No.1～n						No.n+1
STX(02H)  TEXT（DATA 字符串）  BCC 符号
*/

/*
(3)状态数据要求［SDR］（监控装置→车门控制装置）
char
No.
bit7 bit6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
0 STX(02H)
1 Address(01H～08H)
2 Sub-address(00H)
3 Command(20H)
4 																		设置时钟*1(bit0)
5 年
6 月
7 日
8 点
9 分
10 秒
11 Speed 00H～FFH（0～255km/h） [1km/h:1bit]
12 Dummy (00H) 未使用
13 BCC 符号
*1：设置时钟时，输出“1”一秒钟以上。
*/

/*
(4)状态数据响应［SD］（车门控制装置→监控装置）
char
No.
bit7 bit6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
0 STX(02H)
1 Address(01H～08H)
2 Sub-address(00H)
3 Command(20H)
4				5km/h		门隔离 .	开,关门动	车门			车门
			信号检测						作中 			关闭状态		敞开状态
5			关门防挤		开门防挤		门系统		紧急装置
			压				压 .			故障 .	操作	
6 Dummy(00H) 未使用
7 BCC 符号

*/

#include "TMS_ComunDefine.h"

#define  DOOR_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度		//可以有用户自定义
#define  DOOR_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始4字节的数据*/
#define  DOOR_STX							0x02	//定义发送 的开始字节为0x20
#define  DOOR_ADDRES_MIN 			0x01	//地址 (0x01 ~ 0x08 )
#define  DOOR_ADDRES_MAX			0x08
#define  DOOR_SUB_ADDRES			0x00	//子地址
#define  DOOR_COMMAND    			0x20	//命令码

/*定义错误编码值*/
#define  DOOR_DataError  				-1			//接受的前4字节有不一致的错误
#define  DOOR_GetTimeoutError	  -2			//发送请求之后，20ms以内没有接收到数据
#define  DOOR_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  DOOR_BCCError       	  -4  	 	//BCC 符号不符合计划值时
#define  DOOR_GetError					-5			//表示接受数据错误

typedef struct 		//请求数据文本
{
	u8 Address;				//地址
	u8 Sub_Address;		//子地址
	u8 Command;				//命令字
  u8 SetClock;			//设置时钟(只有最后一位有效，故只有0 ，1)
	u8 Year;					//年	
	u8 Month;					//月
	u8 Day;						//日
	u8 Hour;					//时
	u8 Minute;				//分
	u8 Second;				//秒
	u8 Speed;					//00H～FFH（0～255km/h） [1km/h:1bit]
	u8 Dummy2;				//未使用2
}DoorSdrText;//DoorSdrText;

typedef struct 		//监控设备 发送到 车门控制的请求数据帧
{
	u8 DoorSdrStx	;						//开始字符 0x02
	DoorSdrText DoorSdrData;	//数据段
	u8 DoorSdrBcc;						//bcc算法
}DoorSdrFrame;	


typedef struct 			//响应数据文本
{
	u8 Address;						//地址
	u8 Sub_Address;				//子地址
	u8 Command;						//命令字

	/*Byte4*/
		/*车门状态*/
	u8 DoorOpenState			:1;	//  车门 打开状态
	u8 DoorCloseState			:1;	//	车门 关闭状态
	u8 DoorOpenCloseing		:1;	//	车门 开，关动作中
	u8 Byte4UndefBit3			:1;
	u8 DoorIsolation			:1;	//	门隔离	
	u8 SpeedDetection5km	:1;	//	5km/h信号检测
	u8 Byte4UndefBit6			:1;
	u8 Byte4UndefBit7			:1;
	
	/*byte5*/
	/*车门故障*/
	u8 EmergUnlock				:1;	//紧急装置操作
	u8 DoorSystemFault		:1;	//门系统故障
	u8 Byte5UndefBit2			:1;
	u8 Byte5UndefBit3			:1;
	u8 OpenDoorExtrusion	:1;	//开门防挤压
	u8 CloseDoorExtrusion	:1;	//关门防挤压
	u8 Byte5UndefBit6			:1;
	u8 Byte5UndefBit7			:1;

	u8 Dummy;							//未使用
}DoorSdText;

typedef struct    // 监控设备 接收到 车门控制设备的响应帧
{
	u8 DoorSdStx;							//开始字符 0x02
	DoorSdText	DoorSdData;
	u8 DoorSdBcc;							//Bcc算法
}DoorSdFrame;

/*
	函数说明：
功能：发送车门控制的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendDoorSdr(DoorSdrText * pInData/*,u8 * pOutData*/);  //发送车门控制的状态请求数据。


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_DOOR_SD_State结构体)。
返回值：0,无错，  其他有错。
*/
s8 GetDoorSd(u8 *pInData,DoorSdFrame *DoorSdFrame);	//监控设备接受车门控制的状态响应数据。

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckDoorGetError(u8 *pInData);

///*
//函数说明：
//功能：根据输入的数据生成bcc码
//参数: 
//	pData:入口参数，需要生成bcc的数据。
//	len： pData的数据长度 (字节单位 )
//返回值：返回生成的BCC码
//*/
//u8 GenerateDoorBccChcek(void * pData,u8 len);


void testDoor(void);
#endif  //__DOORCOMPRO_H__

