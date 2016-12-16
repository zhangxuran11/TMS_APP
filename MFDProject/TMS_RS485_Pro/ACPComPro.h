#ifndef __ACPCOMPRO_H__
#define __ACPCOMPRO_H__

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
9 电路连接 1:1 连续					(故为点对点的传输方式)
10 适用电线 公称120Ω 平衡,带屏蔽3 芯电缆
11 线路电压 DC5V±10%
*/

/*
(1)通讯帧			//注意：使用的偶校验方式
Start bit DATA bit Parity bit Stop bit
1bit 8bit 1bit(EVEN) 1bit
*/

/*
(2)帧结构
Byte0			BYte1			Byte2			Byte3		Byte4			Byte5			Byte6			Byte7	
空压机			功能码		起始地址高	 起始地址	寄存器数	 寄存器数		CRC高			CRC低字节
原地址			字节			低字节		高字节			低字节			字节
0xB0 			0x03			00				00				00			00											

*/

/*
(3)状态数据要求［SDR］（监控装置→空压机电源）
No. bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
0 0XB0 设备地址
1 0X03 功能码
2 0x 00 起始地址高字节
3 0x 01 起始地址低字节
4 0x 00 寄存器数高字节
5 0x 03 寄存器数低字节
6 CRC 高字节 CRC 高字节
7 CRC 低字节 CRC 低字节
*/

/*
(4)状态数据响应［SD］（空压机电源→监控装置）
char
No. bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 备注
0 0XB0 设备地址
1 0X03 功能码
2 0x05 数据长度
3 网压欠压 网压过压 FC 欠压 输出过载1 输出过载2 输出过载3 输出短路 输出缺相  故障代码
4 IGBT 或散热器过热 自检故障 KM2闭合故障 IGBT故障 其它故障 预留 预留 预留 故障代码
5 保留 保留 保留 保留 保留 Bit10 Bit9 Bit8 输入电压0-2048V 分辨率：
6 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 1V
7 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 输入电流分辨 率0-70A 分辨率：1A
8 保留 保留 保留 保留 保留 保留 保留 Bit8 输出电压0-512V 分辨率：
9 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 1V
10 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 输出电流分辨率0-70A 分辨率：1A
11 保留 保留 保留 保留 保留 保留 保留 保留 输出频率
0 – 50HZ（范围0-250）分辨率：
12 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 0.2 HZ
13 bit 7 bit 6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0 温度
-127 - 127 分辨率：1 度
14 CRC 高字节 CRC 高字节
15 CRC 低字节 CRC 低字节
*/

#include "TMS_ComunDefine.h"

#define  ACP_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  ACP_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始2字节的数据*/
#define  ACP_DEVICEADDRES   	0xB0	//空压机源地址
#define  ACP_COMMAND    			0x03	//命令码

/*定义错误编码值*/
#define  ACP_DataError  			-1			//接受的前4字节有不一致的错误
#define  ACP_GetTimeoutError  -2			//发送请求之后，20ms以内没有接收到数据
#define  ACP_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  ACP_CRCError         -4   		//BCC 符号不符合计划值时
#define  ACP_GetError					-5			//表示接受数据错误

/*空压机状态标识的位置*/
/*故障1*/
#define  ACP_NETVOLTLOW_POSIT			7		//网压欠压
#define  ACP_NETVOLTHIG_POSIT			6
#define  ACP_FCLOW_POSIT					5
#define  ACP_OUTOVER1_POSIT				4
#define  ACP_OUTOVER2_POSIT				3
#define  ACP_OUTOVER3_POSIT				2
#define  ACP_OUTSHORT_POSIT	 			1
#define  ACP_OUTDEFPHASE_POSIT		0

/*故障2*/
#define  ACP_IGBT_OR_RAHeat_POSIT		7
#define  ACP_SELFFAULT_POSIT				6
#define  ACP_KM2CLOSEFAULT_POSIT 		5
#define  ACP_IGBTFAULT_POSIT				4
#define  ACP_OTHERFAULT_POSIT 			3

typedef struct 		//请求数据文本帧
{
	u8 ACPDeviceAddress;					//空压机源地址(设备地址)
	u8 Command;										//命令字		功能码
  u8 StartAddressH8;						//起始地址高字节
	u8 StartAddressL8;						//起始地址低字节	
	u8 RegisterNumberH8;					//寄存器数高字节
	u8 RegisterNumberL8;					//寄存器数低字节
	u8 CRCH8;											//CRC高字节
	u8 CRCL8;											//CRC低字节
}ACPSdrFrame;

typedef struct 			//响应数据文本帧
{
	u8 ACPDeviceAddress;			//设备地址
	u8 Command;								//功能码
	u8 Datalen;								//数据长度

	/*byte3*/
	u8 ACPOutPhaseLoss		:1;	//输出缺 相
	u8 ACPOutShorCircuit	:1;	//输出短 路
	u8 ACPOutOverload3		:1;	//输出过 载3
	u8 ACPOutOverload2		:1;	//输出过 载2
	u8 ACPOutOverload1		:1;	//输出过 载1
	u8 ACP_FCUnderVoltage	:1;	//FC 欠压
	u8 ACP_NetOverVoltage	:1;	//网压过压
	u8 ACP_NetUnderVoltage:1;	//网压欠压
	
	/*byte4*/
	u8 Byte4Undef3Bit			:3;
	u8 ACP_OtherFault			:1;	//其它故障
	u8 ACP_IGBTFault			:1;	//IGBT 故障
	u8 ACP_KM2CloseFault	:1;	//KM2 闭合故障
	u8 ACP_SelfCheckFault	:1;	//自检故障
	u8 ACP_IGBTOrRadOverheat	:1;	//IGBT 或散热器过热
	
	u8 InputVoltageH8;				//输入电压高8位
	u8 InputVoltageL8;				//输入电压低8位	
	u8 InputElectricRate;			//输入电流分辨率
	
	u8 OutputVoltageH8;				//输出电压高8位
	u8 OutputVoltageL8;				//输出电压低8位
	u8 OutputElectricRate;		//输出电流分辨率
	
	u8 OutputHzH8;						//输出频率高8位
	u8 OutputHzL8;						//输出频率低8位
	s8 RoomTemperature;				//温度 	(-127 -- 127)
	
	u8 CRCH8;									//CRC高字节
	u8 CRCL8;									//CRC低字节
	
}ACPSdFrame;


/*
	函数说明：
功能：监控设备发送空压机的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendACPSdr(ACPSdrFrame * pInData/*,u8 * pOutData*/);  


/*
	函数说明：
功能：接受空压机的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到S_ACPre_SD_StateBit结构体)。
返回值：0,无错，  其他有错。
*/
s8 GetACPSd(u8 *pInData,ACPSdFrame *pOutData);		

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckACPGetError(u8 *pInData);



void test_ACP(void);

#endif  //__ACPCOMPRO_H__

