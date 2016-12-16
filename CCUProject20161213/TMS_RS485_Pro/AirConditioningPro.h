#ifndef __AIRCONDITIONINGPRO_H__
#define __AIRCONDITIONINGPRO_H__

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
(3)状态数据要求［SDR］（监控装置→空调装置）
char
No.
bit7 bit6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
0 STX(02H)
1 Address(01H)
2 Sub-address(00H)
3 Command(20H)
4 																		设置时钟*1(bit0)
5 年
6 月
7 日
8 点
9 分
10 秒
11 Dummy (00H) 未使用
12 Dummy (00H) 未使用
13 BCC 符号
*1：设置时钟时，输出“1”一秒钟以上。
*/

/*
(4)状态数据响应［SD］（空调装置→监控装置）
char
No.
bit7 bit6 bit 5 bit 4 bit 3 bit 2 bit 1 bit 0
0 STX(02H)
1 Address(01H)
2 Sub-address(00H)
3 Command(20H)
4 全暖 半暖  0   强制冷 弱制冷 0 送风 自动 Unit1动作状态
5 全暖 半暖  0 强制冷 弱制冷 0 送风 自动 Unit2动作状态
6 冷却机1	冷却机2	通风机1	通风机2	通风机3		Unit1
	故障		故障		故障		故障			故障		故障
COF11 COF12 VEF11 VEF12 VEF13
7 压缩机1		压缩机1	压缩机2		压缩机2			Unit1
	圧力保護			故障	圧力保護		故障				故障
	CPP11 CPF11 CPP12 CPF12
8 冷却机1		冷却机2	通风机1		通风机2		通风机3		Unit2
	故障			故障		故障			故障			故障			故障
	COF21 COF22 VEF21 VEF22 VEF23	
9 压缩机1		压缩机1		压缩机2		压缩机2			Unit2
	压力保护		故障			压力保護			故障			故障

	CPP21 CPF21 CPP22 CPF22
10 客室温度 D0H～F3H（5～40℃） [1℃:1bit]
11 Dummy（00H） 未使用
12 BCC 符号

*/

#include "TMS_ComunDefine.h"

#define  MONITOR_AIR_DEVICE    1 		//如果定义1，为代码运行在监控设备上
#define  AIRCONDIR_DEVICE			 0		//如果定义1，为代码运行在空调设备上

#define  AIR_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  AIR_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始4字节的数据*/
#define  AIR_STX						0x02	//定义发送 的开始字节为0x20
#define  AIR_ADDRES   			0x01	//地址
#define  AIR_SUB_ADDRES			0x00	//子地址
#define  AIR_COMMAND    		0x20	//命令码

/*定义错误编码值*/
#define  AIR_DataError  			-1			//接受的前4字节有不一致的错误
#define  AIR_GetTimeoutError  -2		//发送请求之后，20ms以内没有接收到数据
#define  AIR_FrameError				-3		//检测到帧错误、奇偶校验错误时
#define  AIR_BCCError         -4   //BCC 符号不符合计划值时
#define  AIR_GetError					-5		//表示接受数据错误

/*空调状态标识的位置*/
#define  AIR_ALLWAEM_POSIT				7	//全暖所在位置
#define  AIR_HALFWAEM_POSIT				6
#define  AIR_HIGHCOOL_POSIT				4
#define  AIR_WEAKCOLD_POSIT				3
#define  AIR_BLOWING_POSIT				1
#define  AIR_AUTOMATIC_POSIT			0

/*冷却机 通风机*/
#define  AIR_COOLER1FAULT_POSIT			7
#define  AIR_COOLER2FAULT_POSIT			6
#define  AIR_VETILATOR1FAULT_POSIT 	3
#define  AIR_VETILATOR2FAULT_POSIT	2
#define  AIR_VETILATOR3FAULT_POSIT 	1

/*压缩机*/
#define  AIR_COMPRESSOR1PROTECT_POSIT	 7
#define  AIR_COMPRESSOR1FAULT_POSIT		 6
#define  AIR_COMPRESSOR2PROTECT_POSIT	 1
#define  AIR_COMPRESSOR2FAULT_POSIT		 0


typedef struct SDR_TEXT		//请求数据文本
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
	u8 Dummy1;				//未使用1
	u8 Dummy2;				//未使用2
	
}S_TMS_AIR_SDR_Text;

typedef struct SDR_Frame		//监控设备 发送到 空调的请求数据帧
{
	u8 SDR_STX	;					//开始字符 0x02
	S_TMS_AIR_SDR_Text SDR_Data;	//数据段
	u8 SDR_Bcc;						//bcc算法
} S_TMS_AIR_SDR_Frame;


typedef struct SD_Text			//响应数据文本
{
	u8 Address;						//地址
	u8 Sub_Address;				//子地址
	u8 Command;						//命令字
	u8 U1ActiveState;			//单元1的动作状态
	u8 U2ActiveState;			//单元2的动作状态
	u8 U1Fault1;					//单元1故障1的状态
	u8 U1Fault2;					//单元1故障2的状态
	u8 U2Fault1;					//单元2故障1的状态
	u8 U2Fault2;					//单元2故障2的状态
	u8 RoomTemperature;		//室内温度
	u8 Dummy;							//未使用
	
}S_TMS_AIR_SD_Text;

typedef struct SD_Frame   // 监控设备 接收到 空调设备的响应帧
{
	u8 SD_STX;						//开始字符 0x02
	S_TMS_AIR_SD_Text	SD_Data;
	u8 SD_Bcc;						//Bcc算法
} S_TMS_AIR_SD_Frame;

typedef struct Air_SD_State		//空调的响应状态数据,表示某位的状态位
{
	u8	U1AllWarm;				//u1全暖状态位
	u8	U1HalfWarm;				//u1半暖状态位
	u8  U1HighCool;				//u1强制冷状态位
	u8  U1WeakCold;				//u1弱制冷状态位
	u8  U1Blowing; 				//u1送风状态位
	u8 	U1Automatic;  	 	//u1自动状态位
	
	u8	U2AllWarm;				//u2全暖状态位
	u8	U2HalfWarm;				//u2半暖状态位
	u8  U2HighCool;				//u2强制冷状态位
	u8  U2WeakCold;				//u2弱制冷状态位
	u8  U2Blowing; 			//u2送风状态位
	u8 	U2Automatic;   		//u2自动状态位
	
	u8	U1Cooler1Fault;					//u1冷却机1故障状态位
	u8	U1Cooler2Fault;					//u1冷却机2故障状态位
	u8  U1Vetilator1Fault;			//u1通风机1故障状态位
	u8  U1Vetilator2Fault;			//u1通风机2故障状态位
	u8  U1Vetilator3Fault; 			//u1通风机3故障状态位
	
	u8 	U1Compressor1Protect; 	//u1压缩机1压力保护
	u8	U1Compressor1Fault;			//u1压缩机1故障
	u8 	U1Compressor2Protect; 	//u1压缩机2压力保护
	u8	U1Compressor2Fault;			//u1压缩机2故障	
	
	u8	U2Cooler1Fault;					//u2冷却机1故障状态位
	u8	U2Cooler2Fault;					//u2冷却机2故障状态位
	u8  U2Vetilator1Fault;			//u2通风机1故障状态位
	u8  U2Vetilator2Fault;			//u2通风机2故障状态位
	u8  U2Vetilator3Fault; 			//u2通风机3故障状态位
	
	u8 	U2Compressor1Protect; 	//u2压缩机1压力保护
	u8	U2Compressor1Fault;			//u2压缩机1故障
	u8 	U2Compressor2Protect; 	//u2压缩机2压力保护
	u8	U2Compressor2Fault;			//u2压缩机2故障	
	
	u8  RoomTemperature;				//客室温度			D0H～F3H（5～40℃） [1℃:1bit]
	
}S_AIR_SD_State;

/*
	函数说明：
功能：发送空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendAirCondit_SDR(S_TMS_AIR_SDR_Text * pInData,u8 * pOutData);  //发送空调的状态请求数据。


/*
	函数说明：
功能：接受空调的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:若监控设备，则得到每个状态标识(放到S_AIR_SD_State结构体)。
					若空调设备，接受的数据就是文本数据
返回值：0,无错，  其他有错。
*/
s8 GetAirCondit_SD(u8 *pInData,S_AIR_SD_State *pOutData);		//接受空调的状态响应数据。

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckGetError(u8 *pInData);

///*
//函数说明：
//功能：根据输入的数据生成bcc码
//参数: 
//	pData:入口参数，需要生成bcc的数据。
//	len： pData的数据长度 (字节单位 )
//返回值：返回生成的BCC码
//*/
//u8 GenerateBccChcek(void * pData,u8 len);

	
//要获得某位的数据，先将数据移动到最高位，然后在移到最最低位。	//W H H 统一放到CommunPro.h中
/*
函数说明：
功能：要获得某位的数据
参数: 
	Data:入口参数,数据指针。
	GetPosit： 要得到数据的位置
返回值：返回得到某位的值
*/
//u8 GetDataBit(u8 * Data,u8 GetPosit);

void test(void);
#endif  //__AIRCONDITIONINGPRO_H__

