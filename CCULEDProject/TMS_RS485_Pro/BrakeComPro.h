#ifndef  __BRAKECOMPRO_H__
#define  __BRAKECOMPRO_H__

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

/*
（1）数据要求指令（监控器制动器装置）
	指令	说明						记号		收发信息文本的文本长度
1 20H 状态数据要求				SDR 		11Byte(字节)
2 21H 故障扫描数据要求		TDR 		3Byte(字节)

//状态数据的要求   11字节文本格式
（1）状态数据要求（监控器 -> 制动器装置）
	0 字节	1~10字节
	指令	状态数据要求
	20H			10Byte
	
1	年：00~99（公历后面2 位数） BCD
2 月：01~12 BCD
3 日：01~31 BCD
4 时：00~23 BCD
5 分：00~59 BCD
6 秒：00~59 BCD
7 预备（00H）
8 预备（00H）
9 预备（00H）
10 牵引载荷			时间设定		清除扫描数据
	有效信号				3）bit1			1） bit0
	2）bit7

注1）清除扫描数据时设定为1
注2）VVVF 的控制状态在KA 行时设定为1
注3）要求设定时间为1 表示约为1 秒钟的意思
（制动器控制装置由0 变化到1 时，则要对相同文本的时间进行设定）


//故障扫描数据要求	 3字节文本
（1）要求故障扫描数据（监控器制动器控制装置）
0 					1							 						2
指令21H		要求扫描编号NO（1Byte)		要求模块编号

备注1）扫描编号在1 到5 范围进行指定。过去的数据可以设为1.
备注2）模块编号在0 到40 范围内进行指定。


(2)应答指令（制动器装置监控器）
	指令		说明					 记号	收发信息文本的文本长度
1 20H 	状态数据应答				SD 		50Byte(字节)
2 21H 	故障扫描数据应答		TD 		51Byte(字节)

//状态数据应答			50字节
0					1~49
指令			状态数据
20H			（49Byte)

（a) 状态数据的详细内容如下所示。
	Bit 7			 6			 	5			 4			 3			 2			 1			 0	 备注
1			初期测试异常																								异常：1	
			RAM检测出错
			性能异常
			常用制动器控制阀异常
			空转用压力控制阀异常（ 1 转向架）
			空转用压力控制阀异常（ 2 转向架）
			再生制动器模式信号异常
			KA 行负载补偿（装置）信号异常
				
2 		再生制动器反映信号异常																					异常：1
			空气制动器减法指令异常
			交叉线路异常
			0（空）
			第1 轴速度异常
			第2 轴速度异常
			第3 轴速度异常
			第4 轴速度异常
																											检测或是有操作：1						
3 		试验
			第1 轴空转
			第2 轴空转
			第3 轴空转
			第4 轴空转
			检测制动器出现问题
			检测出有不缓解
			强制缓解
	
4																		异常或是收信： 1（ 加压）
			T 车的AS压力异常
			AS2 压力异常
			AS1 压力异常
			BC 压力异常
			AC 压力异常
			电控节流阀有效信号
			再生失效预告信号
			KA 行有效信号

5 									（备注4）备注4）常用制动器指令1 到3 是指在加压时发送信息1。非常制动器指令时指在非常制动器不起作用时发送信息1。
			0
			0
			0
			非常制动器指令
			常用制动器指令3
			常用制动器指令2
			常用制动器指令1
			0

6 AS1 压力0~765kPa ：OOH~FFH (3Kpa/bit) 空簧压缩
7 AS2 压力0~765kPa ：OOH~FFH (3Kpa/bit)
8 负载补偿指令0~25.5V：OOH~FFH （0.1v/bit)
9 再生制动力模式指令0~25.5V：OOH~FFH （0.1v/bit)
10 再生制动力反应指令0~25.5V：OOH~FFH （0.1v/bit)
11 空气制动器减法指令0~102% :OOH~FFH （0.4%/bit)
12 T 车的AS 压力信号0~102% :OOH~FFH （0.4%/bit)
13 电磁阀电流指令0~510mA :OOH~FFH （2mA/bit)
14 AC 压力0~765kPa ：OOH~FFH (3Kpa/bit)
15 BC 压力0~765kPa ：OOH~FFH (3Kpa/bit)
16 空气制动力0~102kN :OOH~FFH （0.4KN /bit)
17 车辆重量15~66ton :OOH~FFH （0.2ton /bit)
18 第1 轴速度0~127.5km/h :OOH~FFH （0.5km/h/bit)
19 第2 轴速度0~127.5km/h :OOH~FFH （0.5km/h/bit)
20 第3 轴速度0~127.5km/h :OOH~FFH （0.5km/h/bit)
21 第4 轴速度0~127.5km/h :OOH~FFH （0.5km/h/bit)
22 控制用的车辆速度0~127.5km/h :OOH~FFH （0.5km/h/bit)： 预备（OOH)
47 预备（OOH)
48 0 0 0   	记录5 记录4 记录3 记录2 记录1 						有记录：1
49 预备（OOH）

备注1）空白处为预备（数据为0)处理
备注2）第1 字节的bit5 表示第1 字节的bit4，bit6 与第4 字节的bit3 之间的理论只和。
备注3）第48 字节的记录1 到5 表示记录完成的扫描数据。


//故障扫描数据的应答（制动器控制装置 ->	监控器）
0 									1~51
指令21H						扫描数据（50Byte)

(a) 要求模块设为0 时（故障标题）
Bit7 6 5 4 3 2 1 0 备注
1 模块编号No（OOH）
2 故障代码
3 年：00~99（公历年份的后2 位数）
4 月：00~12 BCD
5 日：01~31 BCD
6 时：00~23 BCD
7 秒：00~59 BCD
8 预备（OOH）
9
：	抽样数据（42Byte)
51

（b) 要求模块编号为1 到40 时
Bit7 6 5 4 3 2 1 0 备注
1 模块编号（01~40）
2
:		抽样数据（50Byte)
51
备注1）启动时间设定在第0 模块
备注2）最后模块没有数据时，作为空数据可录入OOH。
备注3）抽样数据是指从第0 模块的第9byte（位数）开始到第40 模块的第8 位数为止连续

*/


/*
c)故障代码
故障代码					故障名称						故障水平
90H 				KA 行负载指令异常					C
91H 				再生制动器模式信号异常			C
92H 				常用制动器控制阀异常			A 或B
93H 			1 转向架空转用的控制阀异常		B
94H 			2 转向架空转用的控制阀异常		B
95H			  再生制动器反应信号异常				C
96H 				RAM 检测出错            A 或B
97H 				初期测试异常B
98H 				交叉电线逻辑异常						B
9AH 				第1 轴速度异常							C
9BH 				第2 轴速度异常							C
9CH 				第3 轴速度异常							C
9DH 				第4 轴速度异常							C
9EH 			空气制动器减法指令异常			A 或B
9FH 			AC 压力异常								A 或B
AOH 			BC 压力异常									B
A1H 			AS1 压力异常									B
A2H 			AS2 压力异常									B
A3H 			T 车AS 压力异常							B
A4H				检测出制动器问题						A 或B
A5H 			检测出不缓解								A 或B
A:可以运行到下一站。
B:运行到终点站后停止行使
C：当天运行结束后必须进行维修
D：只对维修情况进行记录。

*/

#include "TMS_ComunDefine.h"

#define  BRAKE_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度		//可以有用户自定义
#define  BRAKE_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义格式需要数据*/
#define  BRAKE_STX						0x02		//定义发送 的开始字节为0x20
#define  BRAKE_ETX 						0x03		

/*制动器 指令定义*/
#define  BRAKE_SDR_COMMAND		0x20	 	//状态数据要求指令
#define  BRAKE_TDR_COMMAND		0x21		//故障扫描数据要求指令
#define  BRAKE_SD_COMMAND			0x20	 	//状态数据应答指令
#define  BRAKE_TD_COMMAND			0x21		//故障扫描数据应答指令

/*定义错误编码值*/
#define  BRAKE_DataError  				-1			//接受的前4字节有不一致的错误
#define  BRAKE_GetTimeoutError	  -2			//发送请求之后，20ms以内没有接收到数据
#define  BRAKE_FrameError					-3			//检测到帧错误、奇偶校验错误时
#define  BRAKE_BCCError       	  -4  	 	//BCC 符号不符合计划值时
#define  BRAKE_GetError						-5			//表示接受数据错误

typedef struct _BRAKE_SDR_TEXT		//请求数据文本
{
	u8 Command;				//命令字
	u8 Year;					//年	00~99（公历后面2 位数） BCD
	u8 Month;					//月	BCD
	u8 Day;						//日	BCD
	u8 Hour;					//时	BCD
	u8 Minute;				//分	BCD
	u8 Second;				//秒	BCD
	u8 Reserve1;			//00H
	u8 Reserve2;			//00H
	u8 Reserve3;			//00H
	
	u8 ClearScanDataBit :1;		//清除扫描数据		清除扫描数据时设定为1
	u8 TimeSetBit				:1;		//时间设定位  
														//注3）要求设定时间为1 表示约为1 秒钟的意思
														//（制动器控制装置由0 变化到1 时，则要对相同文本的时间进行设定）
	u8 undef5bit				:5;		//5位为使用
	u8 TracLoadSigEff		:1;		//牵引载荷有效信号
	
}S_TMS_BRAKE_SDR_Text;

typedef struct _BRAKE_SDR_Frame		//监控设备 发送到 车门控制的请求数据帧
{
	u8 SDR_STX;											//开始字符 0x02
	S_TMS_BRAKE_SDR_Text SDR_Data;	//数据段
	u8 SDR_ETX;											//扩展字段 0x3
	u8 SDR_BCC1EVEN;								//偶数列bcc算法 
	u8 SDR_BCC1ODD;									//奇数列bcc算法 
} S_TMS_BRAKE_SDR_Frame;


typedef struct _BRAKE_TDR_TEXT		//请求故障扫描数据文本
{
	u8 Command;				//命令字
	u8 ScanNumber;		//扫描编号		备注1）扫描编号在1 到5 范围进行指定。过去的数据可以设为1.
	u8 ModuleNumber;	//模块编号		备注2）模块编号在0 到40 范围内进行指定。
}S_TMS_BRAKE_TDR_Text;

typedef struct _BRAKE_TDR_Frame		//监控设备 发送到 车门控制的请求数据帧
{
	u8 TDR_STX;											//开始字符 0x02
	S_TMS_BRAKE_TDR_Text TDR_Data;	//数据段
	u8 TDR_ETX;											//扩展字段 0x3
	u8 TDR_BCC1EVEN;								//偶数列bcc算法 
	u8 TDR_BCC1ODD;									//奇数列bcc算法 
} S_TMS_BRAKE_TDR_Frame;


typedef struct _Receive_ComDataFormat
{
	u8 KALoadCompensSigExce 	 :1;	//KA负载补偿信号异常
	u8 RegenerBrakModeSigExce  :1;	//再生制动器模式信号异常
	u8 IdlPressureControl1Exce :1;	//空转用压力控制阀1异常
	u8 IdlPressureControl2Exce :1;	//空转用压力控制阀2异常
	u8 CommonBrakeControlExce	 :1;	//常用制动器控制阀异常
	u8 PerformanceExce				 :1;	//性能异常
	u8 RAMDetectionError			 :1;	//RAM检测出错
	u8 EarlyTestExce					 :1;	// 初期测试异常
	
	u8 Axis4SpeedExce					 :1;	//第4 轴速度异常
	u8 Axis3SpeedExce					 :1;	//第3 轴速度异常
	u8 Axis2SpeedExce					 :1;	//第2 轴速度异常
	u8 Axis1SpeedExce					 :1;	//第1 轴速度异常
	u8 undef2thByte4Bit				 :1;	//未使用1
	u8 CrossLineExce					 :1;	//交叉线路异常
	u8 AirBrakeSubInstruExce	 :1;	//空气制动器减法指令异常
	u8 RegenerBrakReflectInforExce :1;	//再生制动器反映信息异常
	
	u8 ForceRemission				 	 :1;	//强制缓解
	u8 CheckNoRemission			 	 :1;	//检测出有不缓解
	u8 CheckBrakeFault			 	 :1;	//检测制动器出现问题
	u8 Axis4Idle							 :1;	//第4 轴空转
	u8 Axis3Idle							 :1;	//第3 轴空转
	u8 Axis2Idle							 :1;	//第2 轴空转
	u8 Axis1Idle							 :1;	//第1 轴空转
	u8 Test										 :1;	//试验
	
	u8 KAEffSignal						 :1;	//KA 行有效信号
	u8 RegenerFailWarnSig			 :1;	//再生失效预告信号
	u8 EleControlThrottleEffSig :1;	//电控节流阀有效信号
	u8 ACPressureExce					 :1;	//AC压力异常
	u8 BCPressureExce					 :1;	//BC压力异常
	u8 AS1PressureExce				 :1;	//AS1压力异常
	u8 AS2PressureExce				 :1;	//AS2压力异常
	u8 TASPressureExce				 :1;	//TAS压力异常			
	
	u8 undef5thByte0Bit				 :1;	//第五字节第0位未使用
	u8 ComUseBrakeInstruct1		 :1;	//常用制动器指令1	
	u8 ComUseBrakeInstruct2		 :1;	//常用制动器指令2	
	u8 ComUseBrakeInstruct3		 :1;	//常用制动器指令3	
	u8 unComUseBrakeInstruct	 :1;	//非常用制动器指令
	u8 undef5thByte1Bit5to7Bit :3;	//第五字节第5到7位未使用
	
	u8 AS1Pressure;									//AS1压力
	u8 AS2Pressure;									//AS2压力
	u8 LoadCompensInstruct;					//负载补偿指令
	u8 RegenerBrakModeInstruct;			//再生制动器模式指令
	u8 RegenerBrakReflectInstruct;	//再生制动器反映指令
	u8 AirBrakeSubInstruct;					//空气制动器减法指令
	u8 TASPressureSingal;						//T 车的AS 压力信号
	u8 SolenoidCurrentInstruct;			//电磁阀电流指令
	u8 ACPressure;									//AC压力
	u8 BCPressure;									//BC压力
	u8 AirBrakeForce;								//空气制动力
	u8 VehicleWeight;								//车辆重量
	u8 Axis1Speed;									//第1 轴速度
	u8 Axis2Speed;									//第2 轴速度
	u8 Axis3Speed;									//第3 轴速度
	u8 Axis4Speed;									//第4 轴速度
	u8 ControlTrainSpeed;						//控制用的车辆速度

}S_TMS_BRAKE_RX_ComDataFormat;

typedef struct _BRAKE_StateSD_Text			//状态响应数据文本
{
	u8 Command;											//命令字
	
	S_TMS_BRAKE_RX_ComDataFormat SD_Text;

	/*23 - 47*/
	u8 Dummy[25];										//预备（OOH) 25字节

	u8 Record1								:1;			//记录1
	u8 Record2								:1;			//记录2
	u8 Record3								:1;			//记录3
	u8 Record4								:1;			//记录4
	u8 Record5								:1;			//记录5
	u8 undef48thByte5to7Bit		:3;			//未使用
	
}S_TMS_BRAKE_SD_Text;

typedef struct _BRAKE_SD_Frame   // 监控设备 接收到 车门控制设备的响应帧
{
	u8 SD_STX;										//开始字符 0x02
	S_TMS_BRAKE_SD_Text	SD_Data;
	u8 SD_ETX;										//扩展字段 0x3
	u8 SD_BCC1EVEN;							//偶数列bcc算法 
	u8 SD_BCC1ODD;								//奇数列bcc算法 

} S_TMS_BRAKE_SD_Frame;


typedef struct _BRAKE_StateTD0_Text
{
	u8 Command;        //命令字
	u8 ModuleNumber;	//模块编号	（00H）	
	u8 FaultCode;			//故障代码
	u8 Year;					//年	00~99（公历后面2 位数） BCD
	u8 Month;					//月	BCD
	u8 Day;						//日	BCD
	u8 Hour;					//时	BCD
	u8 Minute;				//分	BCD
	u8 Second;				//秒	BCD
	u8 Reserve1;			//00H
	
	S_TMS_BRAKE_RX_ComDataFormat TD0SampleData;	//抽样数据（42Byte)
	u8 Dummy0[42-sizeof(S_TMS_BRAKE_RX_ComDataFormat)]; //00H
	
}S_TMS_BRAKE_TD0_Text;

typedef struct _BRAKE_TD0_Frame   // 监控设备 接收到 车门控制设备的响应帧
{
	u8 TD0_STX;										//开始字符 0x02
	S_TMS_BRAKE_TD0_Text	TD0_Data;
	u8 TD0_ETX;										//扩展字段 0x3
	u8 TD0_BCC1EVEN;							//偶数列bcc算法 
	u8 TD0_BCC1ODD;								//奇数列bcc算法 

} S_TMS_BRAKE_TD0_Frame;


typedef struct _BRAKE_StateTD1_Text
{
	u8 Command;       	 //命令字
	u8 ModuleNumber;		//模块编号	（01-0x40H）	
	
	S_TMS_BRAKE_RX_ComDataFormat TD1SampleData;	//抽样数据（42Byte)
	u8 Dummy1[50-sizeof(S_TMS_BRAKE_RX_ComDataFormat)]; //00H
	
}S_TMS_BRAKE_TD1_Text;

typedef struct _BRAKE_TD1_Frame   // 监控设备 接收到 车门控制设备的响应帧
{
	u8 TD1_STX;										//开始字符 0x02
	S_TMS_BRAKE_TD1_Text	TD1_Data;
	u8 TD1_ETX;										//扩展字段 0x3
	u8 TD1_BCC1EVEN;							//偶数列bcc算法 
	u8 TD1_BCC1ODD;								//奇数列bcc算法 

} S_TMS_BRAKE_TD1_Frame;



#define SEND_SDR			1
#define SEND_TDR			2

#define RECEIVE_SD		1
#define RECEIVE_TD0		2
#define RECEIVE_TD1		3

/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
//	u8 mode:发送的数据为SDR，还是TDR	; 
返回值：无返回值
*/
void SendBrake_SDR_Or_TDR(void * pInData,u8 * pOutData/*,u8 mode*/);  


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_TMS_BRAKE_SD_Text结构体)。
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetBrake_SD_Or_TD(u8 *pInData,void *pOutData);

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckBrakeGetError(u8 *pInData);

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
void GenerateBrakeBccChcek(void * pData,u8 len,u8* BCC1Even,u8* BCC1Odd);


void testBrake(void);

#endif  //__BRAKECOMPRO_H__
