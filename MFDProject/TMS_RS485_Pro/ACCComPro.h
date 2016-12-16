#ifndef __ACCCOMPRO_H__
#define __ACCCOMPRO_H__

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

#define  MONITOR_ACC_DEVICE    1 		//如果定义1，为代码运行在监控设备上
#define  ACCCONDIR_DEVICE			 0		//如果定义1，为代码运行在空调设备上

#define  ACC_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  ACC_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始4字节的数据*/
#define  ACC_STX						0x02	//定义发送 的开始字节为0x20
#define  ACC_ADDRES   			0x01	//地址
#define  ACC_SUB_ADDRES			0x00	//子地址
#define  ACC_COMMAND    		0x20	//命令码

/*定义错误编码值*/
#define  ACC_DataError  			-1			//接受的前4字节有不一致的错误
#define  ACC_GetTimeoutError  -2			//发送请求之后，20ms以内没有接收到数据
#define  ACC_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  ACC_BCCError         -4  	  //BCC 符号不符合计划值时
#define  ACC_GetError					-5			//表示接受数据错误


typedef struct 			//请求数据文本
{
	u8 Address;				//地址
	u8 Sub_Address;		//子地址
	u8 Command;				//命令字
	
	/*byte4*/
  u8 SetClock							:1;			//设置时钟(只有最后一位有效，故只有0 ，1)
	u8 TargetTempVaild			:1;
	u8 Byte4Undef6Bit				:6;
	
	u8 Year;					//年	
	u8 Month;					//月
	u8 Day;						//日
	u8 Hour;					//时
	u8 Minute;				//分
	u8 Second;				//秒
	
	u8 NetTargetTemp;		//网络控制目标温度10～30(10.0～30.0℃ 1Bit:1℃)  //网络目标温度为10进制
	
	/*A7协议增加预留 2字节*/
	u8 Undef2Byte[2];
}ACCSdrText;

typedef struct 			//监控设备 发送到 空调的请求数据帧
{
	u8 ACCSdrStx	;					//开始字符 0x02
	ACCSdrText ACCSdrData;	//数据段
	u8 ACCSdrBcc;						//bcc算法
}ACCSdrFrame; 


typedef struct 					//响应数据文本
{
	u8 Address;						//地址
	u8 Sub_Address;				//子地址
	u8 Command;						//命令字	

	/*byte4*/
	u8 ACCU1_AutoMode					:1;		//自动模式
	u8 ACCU1_VentilMode				:1;		//通风
	u8 ACCU1_StopMode					:1;		//停止
	u8 ACCU1_HalfColdeMode		:1;		//弱冷
	u8 ACCU1_AllColdMode			:1;		//强冷
	u8 ACCU1_EmergVentilMode	:1;		//紧急通风
	u8 ACCU1_HalfWarmMode			:1;		//半暖
	u8 ACCU1_AllWarmMode			:1;		//全暖
	
	/*byte5*/
	u8 ACCU2_AutoMode					:1;		//自动模式
	u8 ACCU2_VentilMode				:1;		//通风
	u8 ACCU2_StopMode					:1;		//停止
	u8 ACCU2_HalfColdeMode		:1;		//弱冷
	u8 ACCU2_AllColdMode			:1;		//强冷
	u8 ACCU2_EmergVentilMode	:1;		//紧急通风
	u8 ACCU2_HalfWarmMode			:1;		//半暖
	u8 ACCU2_AllWarmMode			:1;		//全暖	
	
	/*byte6*/
	u8 ACCU1_Ventil4Fault			:1;		//空调机组1通风机4故障
	u8 ACCU1_Ventil3Fault			:1;		//空调机组1通风机3故障
	u8 ACCU1_Ventil2Fault			:1;		//空调机组1通风机2故障
	u8 ACCU1_Ventil1Fault			:1;		//空调机组1通风机1故障
	u8 Byte6Undef2Bit					:2;
	u8 ACCU1_Cold2Fault				:1;		//空调机组1冷凝风机2故障
	u8 ACCU1_Cold1Fault				:1;		//空调机组1冷凝风机1故障
	
	/*byte7*/
	u8 ACCU1_Comp2Fault				:1;		//空调机组1压缩机2故障
	u8 ACCU1_Comp2PrePro			:1;		//空调机组1压缩机2压力保护
	u8 Byte7Undef4Bit					:4;		
	u8 ACCU1_Comp1Fault				:1;		//空调机组1压缩机1故障
	u8 ACCU1_Comp1PrePro			:1;		//空调机组1压缩机1压力保护
	
	/*byte8*/
	u8 ACCU2_Ventil4Fault			:1;		//空调机组2通风机4故障
	u8 ACCU2_Ventil3Fault			:1;		//空调机组2通风机3故障
	u8 ACCU2_Ventil2Fault			:1;		//空调机组2通风机2故障
	u8 ACCU2_Ventil1Fault			:1;		//空调机组2通风机1故障
	u8 Byte8Undef2Bit					:2;
	u8 ACCU2_Cold2Fault				:1;		//空调机组2冷凝风机2故障
	u8 ACCU2_Cold1Fault				:1;		//空调机组2冷凝风机1故障

	/*byte9*/
	u8 ACCU2_Comp2Fault				:1;		//空调机组2压缩机2故障
	u8 ACCU2_Comp2PrePro			:1;		//空调机组2压缩机2压力保护
	u8 Byte9Undef4Bit					:4;		
	u8 ACCU2_Comp1Fault				:1;		//空调机组2压缩机1故障
	u8 ACCU2_Comp1PrePro			:1;		//空调机组2压缩机1压力保护
	
	u8 RoomTemperature;		//室内温度
	
	/*
	u8 OtherFault;				//其余设备故障
	u8 DriveRoomACCStatu;	//司机室空调集控转换开关状态
	*/
	
	/*byte11*/
	u8 ACC_WasteVentil1Fault	:1;		//废排风机1故障
	u8 ACC_WasteVentil2Fault	:1;		//废排风机2故障	
	u8 ACC_EmergInvetFault		:1;		//紧急通风逆变器故障
	u8 ACC_PowerFault					:1;		//空调电源故障
	u8 ACC_FreshSensor1Fault	:1;		//新风温度传感器1故障
	u8 ACC_FreshSensor2Fault	:1;		//新风温度传感器2故障
	u8 ACC_ReturnSensor1Fault	:1;		//回风温度传感器1故障
	u8 ACC_ReturnSensor2Fault	:1;		//回风温度传感器1故障
	
	/*byte12,司机室空调集控转换开关状态*/
	u8 ACC_DrivAllColdState		:1;		//强冷
	u8 ACC_DrivHalfColdState	:1;		//弱冷
	u8 ACC_DrivVentilState		:1;		//通风
	u8 ACC_DrivStopState			:1;		//停止
	u8 ACC_DrivAutoState			:1;		//自动
	u8 ACC_DrivNetContrState	:1;		//网控
	u8 ACC_DRivHalfWarmState	:1;		//半暖
	u8 ACC_DrivAllWarmState		:1;		//全暖
	
}ACCSdText;

typedef struct    // 监控设备 接收到 空调设备的响应帧
{
	u8 ACCSdStx;						//开始字符 0x02
	ACCSdText	ACCSdData;
	u8 ACCSdBcc;						//Bcc算法
}ACCSdFrame; 



/*
	函数说明：
功能：发送空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendACCSdr(ACCSdrText * pInData/*,u8 * pOutData*/);  //发送空调的状态请求数据。


/*
	函数说明：
功能：接受空调的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:若监控设备，则得到每个状态标识(放到S_ACC_SD_State结构体)。
					若空调设备，接受的数据就是文本数据
返回值：0,无错，  其他有错。
*/
s8 GetACCSd(u8 *pInData,ACCSdText *pOutData);		//接受空调的状态响应数据。

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

void test_ACC(void);
#endif  //__ACCCOMPRO_H__

