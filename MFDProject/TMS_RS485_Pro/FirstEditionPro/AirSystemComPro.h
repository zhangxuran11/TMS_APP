#ifndef __AIRSYSTEMCOMPRO_H__
#define __AIRSYSTEMCOMPRO_H__

/*
(1)传输规格：
1 通讯方式 RS-485　3线式半双工
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
(1)通讯帧			//注意：使用奇校验方式
Start bit DATA bit Parity bit Stop bit
1bit 8bit 1bit(ODD) 1bit
*/

/*
(2)帧结构
开路PAD		起始标志			地址			控制		收发信息文本			CRC			标志
2～15Byte	1Byte			下位	上位	(13H)		2～255Byte			2Byte		1Byte
					(7EH)		 低字节								（可变长）							 （7EH）		
										(40H)	(01H)		
*/

/*
(3)状态资料的要求［SDR］（TMS→空调系统）
序号	bit7	bit6	bit 5	bit 4	bit 3	bit 2	bit 1	bit 0	
0	历史故障查询	顺序启动信号	环境温度有效	目标温度有效	时间有效	车辆标号
1～６(1号车～６号车)	1：激活
0：未激活
1	历史故障查询的信息编号
1～100	
2	年(10)	年(1)	BCD
3	月(10)	月(1)	BCD
4	日(10)	日(1)	BCD
5	小时(10)	小时(1)	BCD
6	分钟(10)	分钟(1)	BCD
7	秒(10)	秒(1)	BCD
8	网络控制制冷目标温度 	200～300(20.0～30.0℃ 1Bit:0.1℃)	
9	网络控制制冷目标温度 	200～300(20.0～30.0℃ 1Bit:0.1℃)		
10	环境温度 -32768～32767 (-3276.8～3276.7℃ 1Bit:0.1℃)	
11		
12	工作模式：0x01：停机，0x02：通风，0x04：半冷，
		0x08：全冷，0x10:自动	
13	备用	
14	备用	
*/

/*
（4）状态资料的应答［SD］（空调系统→TMS）
序号	bit7	bit6	bit 5	bit 4	bit 3	bit 2	bit 1	bit 0	
0	空调机组1故障信息	　

	压缩机1保护	压缩机1故障	冷凝风机2故障	冷凝风机1故障	通风机4故障	通风机3故障	通风机2故障	通风机1故障	
	CPP11	CPF11	CWF12	CWF11	EFF14	EFF13	EFF12	EFF11	
1							压缩机2保护	压缩机2故障	
							CPP12	CPF12	
2	空调机组2故障信息	
	压缩机1保护	压缩机1故障	冷凝风机2故障	冷凝风机1故障	通风机4故障	通风机3故障	通风机2故障	通风机1故障	
	CPP21	CPF21	CWF22	CWF21	EFF24	EFF23	EFF22	EFF21	
3							压缩机2保护	压缩机2故障	
					CP022	CP021	CPP22	CPF22	
4	回风温度(下位)	-32768～32767 (-3276.8～3276.7℃ 1Bit:0.1℃)	
5	回风温度(上位)		
6	环境温度（下位）	-32768～32767 (-3276.8～3276.7℃ 1Bit:0.1℃)	
7	环境温度（上位）		
8	空调机组1状态	1：激活     0：未激活
	工况
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：预冷
6：预热，7：紧急通风	工作模式
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：自动
6：网控,7：扩展供电，8：紧急通风	
9					压缩机2	压缩机1	冷凝风机	通风机	
10	空调机组2状态	
	工况
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：预冷
6：预热，7：紧急通风	工作模式
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：自动
6：网控,7：扩展供电，8：紧急通风	
11					压缩机2	压缩机1	冷凝风机	通风机	
12	记录故障信息数量	
13	备用	
14	备用	
15	备用	
*/

/*
（5）当查询历史信息位有效时将以下信息传递给主机（以下为一次历史故障的全部信息数据）
序号	bit7	bit6	bit 5	bit 4	bit 3	bit 2	bit 1	bit 0	
0	故障信息编号
1～100	
1	空调机组1故障信息	　
　
1：激活
 0：未激活
	压缩机1保护	压缩机1故障	冷凝风机2故障	冷凝风机1故障	通风机4故障	通风机3故障	通风机2故障	通风机1故障	
	CPP11	CPF11	CWF12	CWF11	EFF14	EFF13	EFF12	EFF11	
2							压缩机2保护	压缩机2故障	
							CPP12	CPF12	
3	空调机组2故障信息	
	压缩机1保护	压缩机1故障	冷凝风机2故障	冷凝风机1故障	通风机4故障	通风机3故障	通风机2故障	通风机1故障	
	CPP21	CPF21	CWF22	CWF21	EFF24	EFF23	EFF22	EFF21	
4							压缩机2保护	压缩机2故障	
					CP022	CP021	CPP22	CPF22	
5	回风温度(下位)	-32768～32767 (-3276.8～3276.7℃ 1Bit:0.1℃)	
6	回风温度(上位)		
7	环境温度（下位）	-32768～32767 (-3276.8～3276.7℃ 1Bit:0.1℃)	
8	环境温度（上位）		
9	空调机组1状态	1：激活     0：未激活
10	工况
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：预冷
6：预热，7：紧急通风	工作模式
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：自动
6：网控,7：扩展供电，8：紧急通风	
11					压缩机2	压缩机1	冷凝风机	通风机	
12	空调机组2状态	
	工况
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：预冷
6：预热，7：紧急通风	工作模式
0：通风,1：半冷,2：全冷
3：半暖,4：全暖,5：自动
6：网控,7：扩展供电，8：紧急通风	
13					压缩机2	压缩机1	冷凝风机	通风机	
14	年(10)	年(1)	BCD
15	月(10)	月(1)	BCD
16	日(10)	日(1)	BCD
17	小时(10)	小时(1)	BCD
18	分钟(10)	分钟(1)	BCD
19	秒(10)	秒(1)	BCD
20	备用	
21	备用	
22	备用	
*/

#include "TMS_ComunDefine.h"

#define  AIRSYSTEM_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  AIRSYSTEM_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始2字节的数据*/
#define  AIRSYSTEM_PAD1   					0x7E	//开路PAD1
#define  AIRSYSTEM_PAD2    					0x7E	//开路PAD2
#define  AIRSYSTEM_PAD3    					0x7E	//开路PAD3
#define  AIRSYSTEM_PAD4    					0x7E	//开路PAD4
#define  AIRSYSTEM_STARTFLAG				0x7E	//起始标志
#define  AIRSYSTEM_ADDRESSL8				0x40	//空调地址低8位
#define  AIRSYSTEM_ADDRESSH8				0x01	//空调地址高8位
#define  AIRSYSTEM_CONTROLBYTE			0x13	//控制字
#define  AIRSYSTEM_ENDFLAG					0x7E	//结束标志

/*定义错误编码值*/
#define  AIRSYSTEM_DataError  			-1			//接受的前4字节有不一致的错误
#define  AIRSYSTEM_GetTimeoutError  -2			//发送请求之后，20ms以内没有接收到数据
#define  AIRSYSTEM_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  AIRSYSTEM_CRCError         -4   		//BCC 符号不符合计划值时
#define  AIRSYSTEM_GetError					-5			//表示接受数据错误
#define  AIRSYSTEM_TransException		-6			//表示传输异常


typedef struct _TMS_TO_ARI_SDR_TEXT
{
//	u8 TrainNumberState;		//某节车空调系统状态	,下面定义成按位的定义		/*在stm32中数据是 memcpy从低位开始复制给每一位*/	
		u8 TrainNumber			 :3;			//车辆标号
		u8 TimeEffective		 :1;  		// 时间有效
		u8 TarTemperEffective:1;			//目标温度有效
		u8 EnvTemperEffective:1;			//环境温度有效
		u8 OrderStartSignal  :1;			//顺序启动信号
		u8 HistoryFaultQuery :1;			//历史故障查询
	
	u8 HistoryFaultNumber;	//历史故障查询的信息编号
	u8 Year;								//年	
	u8 Month;								//月
	u8 Day;									//日
	u8 Hour;								//时
	u8 Minute;							//分
	u8 Second;							//秒
	u8 RefriTargetTemperL8;	//网络控制制冷目标温度低8位
	u8 RefriTargetTemperH8;	//网络控制制冷目标温度高8位
	u8 EnvironmentTemperL8;	//环境温度低8位
	u8 EnvironmentTemperH8;	//环境温度高8位			//上层应用的时候注意温度有负值，用s16，将高低字节拼好放入
	u8 WorkMode;						//工作模式 0x01：停机，0x02：通风，0x04：半冷，0x08：全冷，0x10:自动
	u8 Dummy1;							//未使用1
	u8 Dummy2;							//未使用2
}S_TMS_AirSystem_SDR_Text;

typedef struct _AIR_SYSTEM_SDR_Frame		//请求数据帧
{
	u8 PAD1;											//空调开路 PAD1
	u8 PAD2;											//空调开路 PAD2
	u8 StartFlag;									//起始标志
	u8 AddressL8;									//地址低8位
	u8 AddressH8;									//地址高8位
	u8 ControlByte;								//控制字
	S_TMS_AirSystem_SDR_Text	DataText; //收发信息文本
	u8 CRCL8;											//CRC低字节
	u8 CRCH8;											//CRC高字节
	u8 EndFlag;										//结束标志

}S_TMS_AirSystem_SDR_Frame;


typedef struct _AIR_SYSTEM_StateSD_Text			//响应状态数据文本
{
//	u8 AHU1FaultL8;								//空调机组1故障信息低8位
	/*用位定义表示*/
	u8 EFF11			:1;							//通风机1故障
	u8 EFF12			:1;							//通风机2故障
	u8 EFF13			:1;							//通风机3故障
	u8 EFF14			:1;							//通风机4故障
	u8 CWF11			:1;							//冷凝风机1故障
	u8 CWF12			:1;							//冷凝风机2故障
	u8 CPF11			:1;							//压缩机1故障
	u8 CPP11			:1;							//压缩机1保护
	
//	u8 AHU1FaultH8;								//空调机组1故障信息高8位
	/*用位定义表示*/
	u8 CPF12			:1;							//压缩机2故障
	u8 CPP12			:1;							//压缩机2保护
	u8 undef1			:6;							//未使用6位
	
//	u8 AHU2FaultL8;								//空调机组2故障信息低8位
	/*用位定义表示*/
	u8 EFF21			:1;							//通风机1故障
	u8 EFF22			:1;							//通风机2故障
	u8 EFF23			:1;							//通风机3故障
	u8 EFF24			:1;							//通风机4故障
	u8 CWF21			:1;							//冷凝风机1故障
	u8 CWF22			:1;							//冷凝风机2故障
	u8 CPF21			:1;							//压缩机1故障
	u8 CPP21			:1;							//压缩机1保护
	
//	u8 AHU2FaultH8;								//空调机组2故障信息高8位
	/*用位定义表示*/
	u8 CPF22			:1;							//压缩机2故障
	u8 CPP22			:1;							//压缩机2保护
	u8 CP021			:1;
	u8 CP022			:1;
	u8 undef2			:4;							//未使用4位
	
	u8 ReturnAirTemperL8;					//回风温度低8位
	u8 ReturnAirTemperH8;					//回风温度高8位 	//上层应用的时候注意温度有负值，用s16，将高低字节拼好放入
	u8 EnvironmentTemperL8;				//环境温度低8位
	u8 EnvironmentTemperH8;				//环境温度高8位			//上层应用的时候注意温度有负值，用s16，将高低字节拼好放入

//	u8 AHU1State;									//空调机组1状态
	/*用位定义标志*/
	u8 AHU1WorkMode		:4;					//空调机组1工作模式
	u8 AHU1WorkState	:4;					//空调机组1工作状况
		
//	u8 AirState1;									//序号9 ，又疑问的字节
	/*用位定义标志*/
	u8 EF1State				:1;					//通风机状态
	u8 CW1State				:1;					//冷凝机状态
	u8 CP11State			:1;					//压缩机1状态
	u8 CP12State			:1;					//压缩机2状态
	u8 AHU1undefBit		:4;					//未使用4位。

//	u8 AHU2State;									//空调机组2状态
	/*用位定义标志*/
	u8 AHU2WorkMode		:4;					//空调机组2工作模式
	u8 AHU2WorkState	:4;					//空调机组2工作状况
	
//	u8 AirState2;									//序号11 ，又疑问的字节
	/*用位定义标志*/
	u8 EF2State				:1;					//通风机状态
	u8 CW2State				:1;					//冷凝机状态
	u8 CP21State			:1;					//压缩机1状态
	u8 CP22State			:1;					//压缩机2状态
	u8 AHU2undefBit		:4;					//未使用4位。

	u8 RecordFaultNumber;					//记录故障信息数量
	u8 Dummy1;										//未使用1
	u8 Dummy2;										//未使用2
	u8 Dummy3;										//未使用3

}S_TMS_AirSystem_State_SD_Text;

typedef struct _AIR_SYSTEM_StateSD_Frame			//响应状态数据帧
{
//	u8 PAD1;											//空调开路 PAD1
//	u8 PAD2;											//空调开路 PAD2
	u8 StartFlag;									//起始标志
	u8 AddressL8;									//地址低8位
	u8 AddressH8;									//地址高8位
	u8 ControlByte;								//控制字
	
	S_TMS_AirSystem_State_SD_Text DataText;	//响应状态信息文本
	
	u8 CRCL8;											//CRC低字节
	u8 CRCH8;											//CRC高字节
	u8 EndFlag;										//结束标志
	
}S_TMS_AirSystem_State_SD_Frame;


typedef struct _AIR_SYSTEM_HistoryFaultSD_Text			//响应历史故障数据文本
{
	u8 HistoryFaultNumber;				//故障信息编号
	
//	u8 AHU1FaultL8;								//空调机组1故障信息低8位
	/*用位定义表示*/
	u8 EFF11			:1;							//通风机1故障
	u8 EFF12			:1;							//通风机2故障
	u8 EFF13			:1;							//通风机3故障
	u8 EFF14			:1;							//通风机4故障
	u8 CWF11			:1;							//冷凝风机1故障
	u8 CWF12			:1;							//冷凝风机2故障
	u8 CPF11			:1;							//压缩机1故障
	u8 CPP11			:1;							//压缩机1保护
	
//	u8 AHU1FaultH8;								//空调机组1故障信息高8位
	/*用位定义表示*/
	u8 CPF12			:1;							//压缩机2故障
	u8 CPP12			:1;							//压缩机2保护
	u8 undef1			:6;							//未使用6位
	
//	u8 AHU2FaultL8;								//空调机组2故障信息低8位
	/*用位定义表示*/
	u8 EFF21			:1;							//通风机1故障
	u8 EFF22			:1;							//通风机2故障
	u8 EFF23			:1;							//通风机3故障
	u8 EFF24			:1;							//通风机4故障
	u8 CWF21			:1;							//冷凝风机1故障
	u8 CWF22			:1;							//冷凝风机2故障
	u8 CPF21			:1;							//压缩机1故障
	u8 CPP21			:1;							//压缩机1保护
	
//	u8 AHU2FaultH8;								//空调机组2故障信息高8位
	/*用位定义表示*/
	u8 CPF22			:1;							//压缩机2故障
	u8 CPP22			:1;							//压缩机2保护
	u8 CP021			:1;
	u8 CP022			:1;
	u8 undef2			:4;							//未使用4位	
	
	u8 ReturnAirTemperL8;					//回风温度低8位
	u8 ReturnAirTemperH8;					//回风温度高8位 	//上层应用的时候注意温度有负值，用s16，将高低字节拼好放入
	u8 EnvironmentTemperL8;				//环境温度低8位
	u8 EnvironmentTemperH8;				//环境温度高8位			//上层应用的时候注意温度有负值，用s16，将高低字节拼好放入

//	u8 AHU1State;									//空调机组1状态
	/*用位定义标志*/
	u8 AHU1WorkMode		:4;					//空调机组1工作模式
	u8 AHU1WorkState	:4;					//空调机组1工作状况
	
//	u8 AirState1;									//序号9 ，又疑问的字节
	/*用位定义标志*/
	u8 EF1State				:1;					//通风机状态
	u8 CW1State				:1;					//冷凝机状态
	u8 CP11State			:1;					//压缩机1状态
	u8 CP12State			:1;					//压缩机2状态
	u8 AHU1undefBit		:4;					//未使用4位。
	
//	u8 AHU2State;									//空调机组2状态
	/*用位定义标志*/
	u8 AHU2WorkMode		:4;					//空调机组2工作模式
	u8 AHU2WorkState	:4;					//空调机组2工作状况
	
//	u8 AirState2;									//序号11 ，又疑问的字节
	/*用位定义标志*/
	u8 EF2State				:1;					//通风机状态
	u8 CW2State				:1;					//冷凝机状态
	u8 CP21State			:1;					//压缩机1状态
	u8 CP22State			:1;					//压缩机2状态
	u8 AHU2undefBit		:4;					//未使用4位。
	
	u8 Year;								//年	
	u8 Month;								//月
	u8 Day;									//日
	u8 Hour;								//时
	u8 Minute;							//分
	u8 Second;							//秒
	u8 Dummy1;										//未使用1
	u8 Dummy2;										//未使用2
	u8 Dummy3;										//未使用3

}S_TMS_AirSystem_HisFault_SD_Text;

typedef struct _AIR_SYSTEM_HistoryFaultSD_Frame //响应历史故障数据帧
{
//	u8 PAD1;											//空调开路 PAD1
//	u8 PAD2;											//空调开路 PAD2
	u8 StartFlag;									//起始标志
	u8 AddressL8;									//地址低8位
	u8 AddressH8;									//地址高8位
	u8 ControlByte;								//控制字
	
	S_TMS_AirSystem_HisFault_SD_Text DataText;	//响应状态信息文本
	
	u8 CRCL8;											//CRC低字节
	u8 CRCH8;											//CRC高字节
	u8 EndFlag;		
}S_TMS_AirSystem_HisFault_SD_Frame;



/*
	函数说明：
功能：监控设备发送给空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendAirSystem_SDR(S_TMS_AirSystem_SDR_Text * pInData,u8 * pOutData);  


#define AIR_STATE_SD_MODE 	  0
#define AIR_HISFAULT_SD_MODE	1
/*
	函数说明：
功能：监控设备接受空调的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。
	GetMode:	STATE_SD_MODE  表示接受的数据为状态响应
						HISFAULT_SD_MODE	表示接受的数据为历史故障响应	
返回值：0,无错，  其他有错。
*/
s8 GetAirSystem_SD(u8 *pInData,void *pOutData,u8 GetMode);		

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckAirSystemGetError(u8 *pInData);


/*
	函数说明：
功能：将10进制数转换为BCD码
参数: 
			输入：u8 Dec   待转换的十进制数据
返回值：  转换后的BCD码
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 DecToBcd(u8 Dec);

/*
	函数说明：
功能：将BCD码转换为10进制数
参数: 
			输入：u8 Bcd   待转换BCD码
返回值：  转换后的10进制数
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 BcdToDec(u8 Bcd);


void test_airsystem(void);

#endif  //__AIRSYSTEMCOMPRO_H__

