#ifndef __BCUCOMPRO_H__
#define __BCUCOMPRO_H__

#include "TMS_ComunDefine.h"
/*
(1)传输规格：
通信方式		四线式双向通信方式
通信速度		9600bps±0.1%
同期方式		异步同期
控制顺序		依照BASIC 基本顺序
连接方法		父节点对子节点（在具有父子关系的通信系统中分配发送数据权的方式。）
传送符号		NRZ
调制方式		基带传输
误控			水平奇偶，BCC
连接线路		20mA 电流环路
适用电线		标准120Ω平衡，屏蔽扁平网络线
线路电压		DC24V ±10%
线路电流		标记（‘1’）：17-23mA，，间隙（‘0’）：0-2mA
线路网			1 对1
*/

/*
(1)通讯帧			//注意：使用的奇校验方式
Start bit DATA bit Parity bit Stop bit
	1bit  	8bit 1		bit(ODD) 	1bit
*/

/*
(2)帧结构
No.0				No.1～n							ETX		BCC1(偶数列的校验)		BCC2(奇数列的校验)
STX(02H)  TEXT（DATA 字符串） 		03H			1byte								1byte
*/

#define  BCU_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度		//可以有用户自定义
#define  BCU_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义格式需要数据*/
#define  BCU_STX						0x02		//定义发送 的开始字节为0x20
#define  BCU_ETX 						0x03		

/*制动器 指令定义*/
#define  BCU_SDR_COMMAND		0x20	 	//状态数据要求指令
#define  BCU_TDR_COMMAND		0x21		//故障扫描数据要求指令
#define  BCU_SD_COMMAND			0x20	 	//状态数据应答指令
#define  BCU_TD_COMMAND			0x21		//故障扫描数据应答指令

/*定义错误编码值*/
#define  BCU_DataError  				-1			//接受的前4字节有不一致的错误
#define  BCU_GetTimeoutError	  -2			//发送请求之后，20ms以内没有接收到数据
#define  BCU_FrameError					-3			//检测到帧错误、奇偶校验错误时
#define  BCU_BCCError       	  -4  	 	//BCC 符号不符合计划值时
#define  BCU_GetError						-5			//表示接受数据错误

#define SEND_SDR			1
#define SEND_TDR			2

#define RECEIVE_SD		1
#define RECEIVE_TD0		2
#define RECEIVE_TD1		3

/*SDR*/
typedef struct 
{
	u8 Command;				//命令字
	u8 Year;					//年	00~99（公历后面2 位数） BCD
	u8 Month;					//月	BCD
	u8 Day;						//日	BCD
	u8 Hour;					//时	BCD
	u8 Minute;				//分	BCD
	u8 Second;				//秒	BCD
	u8 BrakeRise;			// 制动力上升指令 ： FFH＊１
	u8 Reserve2;			//00H
	u8 Reserve3;			//00H

	u8 BCU_TraceDataClea 	:1; //清除状态数据＊2  清除：１该字节保持约 1 秒（相当于传送周期 5）时，删除 BCU 内部所有的状态数据，记录件数为 0
	u8 BCU_TimeSet 				:1; //时间设定＊３  时间设定：1 该字节保持约 1 秒（相当于传送周期 5）时，基于启动时的时钟信息 Text １～６设定制动控制装置内部的时钟
	u8 BCU_unUse6Bit 			:6; //没有使用的位
	
}BcuSdrText;

typedef struct 		//监控设备 发送到 车门控制的请求数据帧
{
	u8 SdrStx;											//开始字符 0x02
	BcuSdrText 		BcuSdrData;				//数据段
	u8 SdrEtx;											//扩展字段 0x3
	u8 SdrBCC1;											//偶数列bcc算法 
	u8 SdrBCC2;											//奇数列bcc算法 
	
} BcuSdrFrame;


/*SD*/
typedef struct 
{
	u8 Command;									//命令
	
	/*第1Byte*/
	u8 BCU_LoadValueErrtx 			:1;		//空重车行驶信号异常（牵引力载荷信号异常）
	u8 BCU_ElecticBrakeCmdErr		:1;		//电制动模式信号异常
	u8 BCU_PressureValErr2			:1;		//滑行防止阀异常（后转向架）
	u8 BCU_PressureValErr1			:1;		//滑行防止阀异常（前转向架）
	u8 BCU_MagnetValueErr				:1;		//常用制动器控制阀异常
	u8 BCU_FunctionFaul					:1;		//功能异常
	u8 BCU_RAMCheckErr					:1;		//RAM检测出错
	u8 BCU_TestErro							:1;		//初始测试异常
	
	/*第2Byte*/
	u8 BCU_Axle4SpeedErr				:1;		//第4轴速度异常
	u8 BCU_Axle3SpeedErr				:1;		//第3轴速度异常
	u8 BCU_Axle2SpeedErr				:1;		//第2轴速度异常
	u8 BCU_Axle1SpeedErr				:1;		//第1轴速度异常
	u8 BCU_12VFault							:1;		//控制电源异常
	u8 BCU_TramWireLogicCheck		:1;		//贯通线逻辑异常
	u8 BCU_PneuBrakeReduceCmdErr:1;		//空气制动减算指令异常
	u8 BCU_ElecticBrakeFbCmdErr	:1;		//电制动反馈信号异常
	
	/*第3Byte*/
	u8 BCU_CompulsoryRelease		:1;		//强制缓解中
	u8 BCU_NonreleaseBrakeDet		:1;		//不缓解检测
	u8 BCU_InsufficientBraketDet:1;		//不足检测（常用制动）
	u8 BCU_Axle4Skid						:1;		//第4轴滑行
	u8 BCU_Axle3Skid						:1;		//第3轴滑行
	u8 BCU_Axle2Skid						:1;		//第2轴滑行
	u8 BCU_Axle1Skid						:1;		//第1轴滑行
	u8 BCU_Test									:1;		//试验
	
	/*第4Byte*/
	u8 BCU_PowerSignal					:1;		//重力行驶信号（牵引信号？）
	u8 BCU_ElecticBrakeFail			:1;		//电制动失效预告信号
	u8 BCU_ElecticBrakeSignal		:1;		//电制动信号
	u8 BCU_ACPressureSensorErr	:1;		//AC压力传感器异常
	u8 BCU_BCPressureSensorErr	:1;		//BC压力传感器异常
	u8 BCU_AS1PressureSensorErr	:1;		//AS1压力传感器异常
	u8 BCU_AS2PressureSensorErr	:1;		//AS2压力传感器异常
	u8 BCU_ASSignalErrTCar			:1;		//T车的空重车信号异常（AS压力异常?）
	
	/*第5Byte*/
	u8 BCU_ForcedEase						:1;		//强制开放指令（强制缓解？）
	u8 Byte5Bit2Undef						:1;		//
	u8 BCU_ATOMode							:1;		//ATO模式信号
	u8 BCU_BrakeSignal					:1;		//制动信号
	u8 BCU_HillStartCmd					:1;		//坡起指令
	u8 BCU_ATPMaxCmd						:1;		//ATP常用最大指令
	u8 BCU_PwmToOther						:1;		//常用制动PWM至另一场
	u8 BCU_ASCompensated				:1;		//控制AS压力已偿还
	
	/*第6Byte*/
	u8 BCU_BrakeForceUping			:1;		//制动力上升控制中
	u8 BCU_BrakeForceUpCmd			:1;		//制动力上升指令
	u8 BCU_HBEase								:1;		//HB开放指令
	u8 BCU_HBCmd								:1;		//HB作用指令
	u8 BCU_HBOperating					:1;		//HB作用中
	u8 BCU_30kmSignal						:1;		//30公里速度信号
	u8 BCU_BCPressureExist			:1;		//BC压力有无信号
	u8 BCU_EmergencyBrakeCmd		:1;		//紧急制动信号
	
	u8 BCU_AS1Pressure;								//AS1压力
	u8 BCU_AS2Pressure;								//AS2压力 
	u8 BCU_PropulsionLoadCmd;					//空重车行驶信号(负载补偿指令?) 
	u8 BCU_ElecticBrakeMode;					//电制动力模式信号
	u8 BCU_ElecticBrakeFB;						//电制动力反馈信号
	u8 BCU_PneuBrakeReducCmd;					//空气制动减算指令
	u8 BCU_ASPressureTcar;						//T车的空重车信号（AS压力信号？）
	u8 BCU_MagnetControlValCmd;				//电磁阀电流指令
	u8 BCU_ACPressure;								//AC压力  
	u8 BCU_BCPressure;								//BC压力 
	u8 BCU_PneuBrakeForce;						//空气制动力
	u8 BCU_CarWeight;									//车辆重量
	u8 BCU_PWMCmd;										//常用制动PWM指令
	u8 BCU_CarSpeedControl;						//车辆速度控制
	u8 BCU_Axle1Speed;								//第1轴速度
	u8 BCU_Axle2Speed;								//第2轴速度	
	u8 BCU_Axle3Speed;								//第3轴速度	
	u8 BCU_Axle4Speed;								//第4轴速度

	/*25 - 47*/
	u8 BCU_Dummy[23];										//预备（OOH) 25字节
	
	u8 BCU_Record1								:1;			//记录1
	u8 BCU_Record2								:1;			//记录2
	u8 BCU_Record3								:1;			//记录3
	u8 BCU_Record4								:1;			//记录4
	u8 BCU_Record5								:1;			//记录5
	u8 BCU_Record6								:1;			//记录6
	u8 Byte48Bit7Undef						:1;			//未使用
	u8 Byte48Bit8Undef						:1;			//未使用
	
	u8 BCU_Byte49Dummy;												//
	
} BcuSdText;

typedef struct 
{
	u8 SdStx;												//开始字符 0x02
	BcuSdText 		BcuSdData;				//数据段
	u8 SdEtx;												//扩展字段 0x3
	u8 SdBCC1;											//偶数列bcc算法 
	u8 SdBCC2;											//奇数列bcc算法 
} BcuSdFrame;

/*TDR*/
typedef struct
{
	u8 Command;									//命令字
	u8 BCU_TraceDataNoRq;				//扫描编号		备注1）扫描编号在1 到5 范围进行指定。过去的数据可以设为1.
	u8 BCU_TraceDataBlackNoRq;	//模块编号		备注2）模块编号在0 到40 范围内进行指定。
}BcuTdrText;

typedef struct
{
	u8 TdrStx;												//开始字符 0x02
	BcuTdrText 		BcuTdrData;					//数据段
	u8 TdrEtx;												//扩展字段 0x3
	u8 TdrBCC1;												//偶数列bcc算法 
	u8 TdrBCC2;												//奇数列bcc算法 
}BcuTdrFrame;


/*TD，响应的抽样数据都是为51+1（命令）字节*/
typedef struct
{
	u8 Command;        //命令字
	u8 ModuleNumber;	//模块编号	（00H）	
	u8 FaultCode;			//故障代码
	u8 Year;					//年	00~99（公历后面2 位数） BCD
	u8 Month;					//月	BCD
	u8 Day;						//日	BCD
	u8 Hour;					//时	BCD
	u8 Minute;				//分	BCD
	u8 Reserve1;			//00H
	
	u8 SampleData[42];	//为00H时，抽样是42字节。
}BcuTdHeardText;

typedef struct
{
	u8 Command;        	//命令字
	u8 ModuleNumber;		//模块编号	（00H）	
	
	u8 SampleData[50];	//1~40的抽样数据
}BcuTdOtherText;

typedef struct
{
	u8 Command;        	//命令字
	u8 ModuleNumber;		//模块编号	（00H）	
	
	u8 SampleData[50];	//1~40的抽样数据	
}BcuTdText;

typedef struct
{
	u8 TdStx;												//开始字符 0x02
	BcuTdText 		BcuTdData;					//数据段
	u8 TdEtx;												//扩展字段 0x3
	u8 TdBCC1;												//偶数列bcc算法 
	u8 TdBCC2;												//奇数列bcc算法 
}BcuTdFrame;

typedef struct
{
	/*第1Byte*/
	u8 BCU_LoadValueErrtx 			:1;		//空重车行驶信号异常（牵引力载荷信号异常）
	u8 BCU_ElecticBrakeCmdErr		:1;		//电制动模式信号异常
	u8 BCU_PressureValErr2			:1;		//滑行防止阀异常（后转向架）
	u8 BCU_PressureValErr1			:1;		//滑行防止阀异常（前转向架）
	u8 BCU_MagnetValueErr				:1;		//常用制动器控制阀异常
	u8 BCU_FunctionFaul					:1;		//功能异常
	u8 BCU_RAMCheckErr					:1;		//RAM检测出错
	u8 BCU_TestErro							:1;		//初始测试异常
	
	/*第2Byte*/
	u8 BCU_Axle4SpeedErr				:1;		//第4轴速度异常
	u8 BCU_Axle3SpeedErr				:1;		//第3轴速度异常
	u8 BCU_Axle2SpeedErr				:1;		//第2轴速度异常
	u8 BCU_Axle1SpeedErr				:1;		//第1轴速度异常
	u8 BCU_12VFault							:1;		//控制电源异常
	u8 BCU_TramWireLogicCheck		:1;		//贯通线逻辑异常
	u8 BCU_PneuBrakeReduceCmdErr:1;		//空气制动减算指令异常
	u8 BCU_ElecticBrakeFbCmdErr	:1;		//电制动反馈信号异常
	
	/*第3Byte*/
	u8 BCU_CompulsoryRelease		:1;		//强制缓解中
	u8 BCU_NonreleaseBrakeDet		:1;		//不缓解检测
	u8 BCU_InsufficientBraketDet:1;		//不足检测（常用制动）
	u8 BCU_Axle4Skid						:1;		//第4轴滑行
	u8 BCU_Axle3Skid						:1;		//第3轴滑行
	u8 BCU_Axle2Skid						:1;		//第2轴滑行
	u8 BCU_Axle1Skid						:1;		//第1轴滑行
	u8 BCU_Test									:1;		//试验
	
	/*第4Byte*/
	u8 BCU_PowerSignal					:1;		//重力行驶信号（牵引信号？）
	u8 BCU_ElecticBrakeFail			:1;		//电制动失效预告信号
	u8 BCU_ElecticBrakeSignal		:1;		//电制动信号
	u8 BCU_ACPressureSensorErr	:1;		//AC压力传感器异常
	u8 BCU_BCPressureSensorErr	:1;		//BC压力传感器异常
	u8 BCU_AS1PressureSensorErr	:1;		//AS1压力传感器异常
	u8 BCU_AS2PressureSensorErr	:1;		//AS2压力传感器异常
	u8 BCU_ASSignalErrTCar			:1;		//T车的空重车信号异常（AS压力异常?）
	
	/*第5Byte*/
	u8 BCU_ForcedEase						:1;		//强制开放指令（强制缓解？）
	u8 Byte5Bit2Undef						:1;		//
	u8 BCU_ATOMode							:1;		//ATO模式信号
	u8 BCU_BrakeSignal					:1;		//制动信号
	u8 BCU_HillStartCmd					:1;		//坡起指令
	u8 BCU_ATPMaxCmd						:1;		//ATP常用最大指令
	u8 BCU_PwmToOther						:1;		//常用制动PWM至另一场
	u8 BCU_ASCompensated				:1;		//控制AS压力已偿还
	
	/*第6Byte*/
	u8 BCU_BrakeForceUping			:1;		//制动力上升控制中
	u8 BCU_BrakeForceUpCmd			:1;		//制动力上升指令
	u8 BCU_HBEase								:1;		//HB开放指令
	u8 BCU_HBCmd								:1;		//HB作用指令
	u8 BCU_HBOperating					:1;		//HB作用中
	u8 BCU_30kmSignal						:1;		//30公里速度信号
	u8 BCU_BCPressureExist			:1;		//BC压力有无信号
	u8 BCU_EmergencyBrakeCmd		:1;		//紧急制动信号
	
	u8 BCU_AS1Pressure;								//AS1压力
	u8 BCU_AS2Pressure;								//AS2压力 
	u8 BCU_PropulsionLoadCmd;					//空重车行驶信号(负载补偿指令?) 
	u8 BCU_ElecticBrakeMode;					//电制动力模式信号
	u8 BCU_ElecticBrakeFB;						//电制动力反馈信号
	u8 BCU_PneuBrakeReducCmd;					//空气制动减算指令
	u8 BCU_ASPressureTcar;						//T车的空重车信号（AS压力信号？）
	u8 BCU_MagnetControlValCmd;				//电磁阀电流指令
	u8 BCU_ACPressure;								//AC压力  
	u8 BCU_BCPressure;								//BC压力 
	u8 BCU_PneuBrakeForce;						//空气制动力
	u8 BCU_CarWeight;									//车辆重量
	u8 BCU_PWMCmd;										//常用制动PWM指令
	u8 BCU_CarSpeedControl;						//车辆速度控制
	u8 BCU_Axle1Speed;								//第1轴速度
	u8 BCU_Axle2Speed;								//第2轴速度	
	u8 BCU_Axle3Speed;								//第3轴速度	
	u8 BCU_Axle4Speed;								//第4轴速度
	
	u8 Bcu_Reserve;											//00H

}BcuSampleData;


/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针 (指向的类型为，BcuSdrText,或者 BcuSdText)   
	pOutData:指向底层串口发送的数据
//	u8 mode:发送的数据为SDR，还是TDR	; 
返回值：无返回值
*/
void SendBcuSdrOrTdr(void * pInData/*,u8 * pOutData*/,u8 mode);  


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_TMS_BRAKE_SD_Text结构体)。
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetBcuSdOrTd(u8 *pInData,void *pOutData);

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckBcuGetError(u8 *pInData);

/*
函数说明：
功能：根据输入的数据生成bcc码
参数: 
	pData:入口参数，需要生成bcc的数据。
	len： pData的数据长度 (字节单位 )
	BCC1Even：得到的bccEven校验码
	Bcc1Odd:	得到的bccOdd校验码
返回值：无
*/
void GenerateBcuBccChcek(void * pData,u8 len,u8* BCC1,u8* BCC2);


void testBcu(void);


#endif
