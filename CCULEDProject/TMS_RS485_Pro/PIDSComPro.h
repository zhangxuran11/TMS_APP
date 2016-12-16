#ifndef __AIRSYSTEMCOMPRO_H__
#define __AIRSYSTEMCOMPRO_H__

/*
(1)传输规格：
1 通讯方式 RS-485　3线式半双工
2 通讯速度 19200bps±0.2%
3 同步方式 调同步
4 控制步骤 遵照HDLC 步骤
5 连接方法 轮询选择
6 传输符号 NRZ
7 调制方式 基带
8 误控 CRC-CCITT
9 电路连接 RS-485
10 适用电线 公称120Ω 平衡,带屏蔽3 芯电缆
11 线路电压 DC5V±10%
*/

/*
////////(1)通讯帧			//注意：使用奇校验方式
////////Start bit DATA bit Parity bit Stop bit
////////1bit 8bit 1bit(ODD) 1bit
*/

/*
(2)帧结构
开路PAD		起始标志			地址			控制		收发信息文本			CRC			标志
2～4Byte	1Byte			下位	上位	(13H)			N Byte				2Byte		1Byte
					(7EH)		 低字节								（可变长）							 （7EH）		
										(60H)	(01H)		
*/

/*
(3)状态数据要求SDR（列车监控装置→车内引导装置 PIDS）
TEXT							Bit No						备 注
				7 	6 	5	 4 	3 	2	 1	 0
0 Command ：20H
1 － － － － － － 时钟设置 时钟有効 *1
2当前站		列车编号		站间距离		－	下一站			列车目的地		起点站			线路
 ID 有效	ID 有效		有效					ID 有效		ID 有效			ID 有效		ID 有效
3	左侧门		右侧门		 1 车	 ATO	 折返		出发车站  到达车站		到达车站预告
	状态		状态
4 备用 ：00H
5 年 (10 位) 年 (1 位)			公历的后面	2 位数 BCD
6 月 (10 位) 月 (1 位) BCD
7 日 (10 位) 日 (1 位) BCD
8 时 (10 位) 时 (1 位) BCD
9 分 (10 位) 分 (1 位) BCD
10 秒 (10 位) 秒 (1 位) BCD
11 列车编号ID ：1000 位 BCD *4
12 列车编号ID ： 100 位 BCD *4
13 列车编号ID ： 10 位 BCD *4
14 列车编号ID ： 1 位 BCD *4
15 线路ID ：00H
16 起点站ID(H)		：0000H～FFFFH (2字节)
17 起点站ID(L)
18 列车目的地ID(H)	：0000H～FFFFH *4,*5
19 列车目的地ID(L)
20 当前站ID(H)	：0000H～FFFFH *4,*5
21 当前站ID(L)
22 下一站ID(H)：0000H～FFFFH *4,*5
23 下一站ID(L)
24 备用 ：00H
25 备用 ：00H
26 站间距离(H)	：0～65535m *5
27 站间距离(L)
28 备用 ：00H
29 备用 ：00H
*/

/*
（4）状态数据响应SD（车内引导装置→列车监控装置）
TEXT				Bit No				备 注
				7 6 5 4 3 2 1 0
0 Command ：30H
1
|	备用		：00H
27
*/

#include "TMS_ComunDefine.h"

#define  PIDS_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  PIDS_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始2字节的数据*/
#define  PIDS_PAD1   						0x7E	//开路PAD1
#define  PIDS_PAD2    					0x7E	//开路PAD2
#define  PIDS_PAD3    					0x7E	//开路PAD3
#define  PIDS_PAD4    					0x7E	//开路PAD4
#define  PIDS_STARTFLAG					0x7E	//起始标志
#define  PIDS_ADDRESSL8					0x60	//PIDS地址低8位
#define  PIDS_ADDRESSH8					0x01	//PIDS地址高8位
#define  PIDS_CONTROLBYTE				0x13	//控制字
#define  PIDS_ENDFLAG						0x7E	//结束标志

#define  PIDS_SDR_COMMAND				0x20	//状态数据要求SDR 命令字（列车监控装置→车内引导装置）
#define  PIDS_SD_COMMAND				0x30	//状态数据响应SD  命令字（车内引导装置→列车监控装置）


/*定义错误编码值*/
#define  PIDS_DataError  				-1			//接受的前4字节有不一致的错误
#define  PIDS_GetTimeoutError   -2			//发送请求之后，20ms以内没有接收到数据
#define  PIDS_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  PIDS_CRCError          -4   		//BCC 符号不符合计划值时
#define  PIDS_GetError					-5			//表示接受数据错误
#define  PIDS_TransException		-6			//表示传输异常



typedef struct _TMS_TO_PIDS_SDR_TEXT
{
	/*0*/
	u8 Command	;						//PIDS命令字
	
	/*1*/
	/*位定义，在stm32中数据是 memcpy从低位开始复制给每一位*/	
	u8 TimeEffective 			:1;		//时钟有效
	u8 TimeSet			 			:1;		//时钟设置
	u8 undef1							:6;		//未使用的6位
	
	/*2*/
	u8 LineIDEffective		:1;		//线路ID有效
	u8 StartStationIDEff 	:1;		//起点站ID 有效
	u8 TrainDestIDEff			:1;		//列车目的地ID 有效
	u8 NextStationIDEff		:1;		//下一站ID 有效
	u8 undef2							:1;		//未使用的1位
	u8 StationDistanceEff :1;		//站间距离有效
	u8 TrainNumIDEff			:1;		//列车编号ID 有效
	u8 CurrentStationIDEff:1;		//当前站ID 有效
	
	/*3*/
	u8 ArrivStationForecast :1;	//到达车站预告
	u8 ArrivStation					:1;	//到达车站
	u8 LeaveStation					:1;	//出发车站
	u8 FoldBack							:1;	//折返
	u8 ATO									:1;	//ATO有效
	u8 Train1								:1;	//1车
	u8 RightDoorState				:1;	//右侧门状态
	u8 LeftDoorState				:1;	//左侧门状态
	
	/*4*/
	u8 Reserve1;									//保留 00H
	
	/*5~10*/
	u8 Year;										//年	
	u8 Month;										//月
	u8 Day;											//日
	u8 Hour;										//时
	u8 Minute;									//分
	u8 Second;									//秒
	
	/*11~14*/
	u8 TrainNumID4;							//列车编号ID ：1000 位(10进制的千位)
	u8 TrainNumID3;							//列车编号ID ：100 位(10进制的百位)
	u8 TrainNumID2;							//列车编号ID ：10 位(10进制的十位)
	u8 TrainNumID1;							//列车编号ID ：1 位(10进制的个位)
	
	/*15*/
	u8 LineID;									//线路ID ：00H
	
	/*16 17*/
	u8 StartStationIDH8;				//起始站ID高8位
	u8 StartStationIDL8;				//起始站ID低8位
	
	/*18 19*/
	u8 TrainDestIDH8;						//列车目的地ID(H8位）
	u8 TrainDestIDL8;						//列车目的地ID(L8位）	
	
	/*20 21*/
	u8 CurrentStationIDH8;			//当前站ID(H)
	u8 CurrentStationIDL8;			//当前站ID(L)
	
	/*22 23*/
	u8 NextStationIDH8;					//下一站ID(H)
	u8 NextStationIDL8;					//下一站ID(L)
	
	/*24 25*/				
	u8 Reserve2;								//备用 ：00H
	u8 Reserve3;								//备用 ：00H
	
	/*26 27*/
	u8 StationDistanceH8;				//	站间距离(H)
	u8 StationDistanceL8;				//	站间距离(L)	
	
	/*28 29*/
	u8 Reserve4;								//备用 ：00H
	u8 Reserve5;								//备用 ：00H

}S_TMS_PIDS_SDR_Text;

typedef struct _PIDS_SDR_Frame		//请求数据帧
{
	u8 PAD1;											//空调开路 PAD1
	u8 PAD2;											//空调开路 PAD2
	u8 StartFlag;									//起始标志
	u8 AddressL8;									//地址低8位
	u8 AddressH8;									//地址高8位
	u8 ControlByte;								//控制字
	S_TMS_PIDS_SDR_Text	DataText; //收发信息文本
	u8 CRCL8;											//CRC低字节
	u8 CRCH8;											//CRC高字节
	u8 EndFlag;										//结束标志

}S_TMS_PIDS_SDR_Frame;


typedef struct _PIDS_StateSD_Text			//响应状态数据文本
{
	/*0*/
	u8 Command	;						//PIDS命令字
	
	/*1 - 27*/
	u8 Reserve[26];				 //备用 ：00H

}S_TMS_PIDS_SD_Text;

typedef struct _PIDS_SD_Frame			//响应状态数据帧
{
//	u8 PAD1;											//空调开路 PAD1
//	u8 PAD2;											//空调开路 PAD2
	u8 StartFlag;									//起始标志
	u8 AddressL8;									//地址低8位
	u8 AddressH8;									//地址高8位
	u8 ControlByte;								//控制字
	
	S_TMS_PIDS_SD_Text DataText;	//响应状态信息文本
	
	u8 CRCL8;											//CRC低字节
	u8 CRCH8;											//CRC高字节
	u8 EndFlag;										//结束标志
	
}S_TMS_PIDS_SD_Frame;

	
/*
	函数说明：
功能：监控设备发送给PIDS的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendPIDS_SDR(S_TMS_PIDS_SDR_Text * pInData,u8 * pOutData);  


/*
	函数说明：
功能：监控设备接受PISD的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。

返回值：0,无错，  其他有错。
*/
s8 GetPIDS_SD(u8 *pInData,void *pOutData);		

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckPIDSGetError(u8 *pInData);




void test_pids(void);

#endif  //__AIRSYSTEMCOMPRO_H__

