#ifndef __COMMUNPRO_H__
#define	__COMMUNPRO_H__

#include "TMS_ComunDefine.h"

//#define MC1_MTD_NODEID	 		3		//MC1车监控装置：节点号 3
//#define T_MTD_NODEID   	 		4		//T车监控装置：节点号4
//#define M_MTD_NODEID		 		5		//M车监控装置： 节点号5
//#define T1_MTD_NODEID				6		//T1车监控装置：节点号6
//#define T2_MTD_NODEID   		7		//T2车监控装置：节点号7
//#define MC2_MTD_NODEID   		8		//MC2车监控装置：节点号8

#define MC1_MTD1_MFD1_NODEID				0x31
#define MC1_MTD1_MFD2_NODEID				0x32
#define MC1_MTD1_MFD3_NODEID				0x33
#define MC1_MTD1_MFD4_NODEID				0x34

#define T_MTD2_MFD1_NODEID					0x41
#define T_MTD2_MFD2_NODEID					0x42
#define T_MTD2_MFD3_NODEID					0x43
#define T_MTD2_MFD4_NODEID					0x44
		
#define M_MTD3_MFD1_NODEID					0x51
#define M_MTD3_MFD2_NODEID					0x52
#define M_MTD3_MFD3_NODEID					0x53
#define M_MTD3_MFD4_NODEID					0x54
		
#define T1_MTD4_MFD1_NODEID					0x61
#define T1_MTD4_MFD2_NODEID					0x62
#define T1_MTD4_MFD3_NODEID					0x63
#define T1_MTD4_MFD4_NODEID					0x64
		
#define T2_MTD5_MFD1_NODEID					0x71
#define T2_MTD5_MFD2_NODEID					0x72
#define T2_MTD5_MFD3_NODEID					0x73
#define T2_MTD5_MFD4_NODEID					0x74
		
#define MC2_MTD6_MFD1_NODEID				0x81
#define MC2_MTD6_MFD2_NODEID				0x82
#define MC2_MTD6_MFD3_NODEID				0x83
#define MC2_MTD6_MFD4_NODEID				0x84



/*DI节点板卡含义的定义*/

/*MC1/MC2 - MFD1子卡*/
/*110V*/
#define MC_MFD1_LeftOpenDoor   			2			//左侧开门指令
#define MC_MFD1_LeftCloseDoor				4			//左侧关门指令
#define MC_MFD1_RightOpenDoor				6			//右侧开门指令
#define MC_MFD1_RightCloseDoor			8			//右侧关门指令
#define MC_MFD1_LDoorOCACmd					10		//左侧门再开闭指令
#define MC_MFD1_RDoorOCACmd					12		//右侧门再开闭指令
                                  
#define MC_MFD1_BCUFAult				 		1			//制动单元故障
#define MC_MFD1_ParkingBrake		 		3			//停放制动
#define MC_MFD1_BCUIsolate		 			5			//制动隔离塞门
#define MC_MFD1_EmergBrakeBypass		7			//紧急制动短路
#define MC_MFD1_ACPStart						9			//空压机启动
#define MC_MFD1_VVVFPowerVaild			11		//VVVF电源有效

/*24v*/
#define MC_MFD1_ATPEmergBrake				2			//ATP紧急制动
#define MC_MFD1_ATPServiceBrake			4			//ATP常用制动

/*MC-MFD2子卡*/
/*110V*/
#define	MC_MFD2_HeardRelay					2			//头继电器
#define MC_MFD2_TailRelay						4			//尾继电器
#define MC_MFD2_BrakeCmd						6			//制动指令
#define	MC_MFD2_EmergBrakeRelay			8			//紧急制动继电器
#define MC_MFD2_ATCPowering					10		//ATC工作状态
#define MC_MFD2_ATCBypass						12		//ATC切除开关

#define MC_MFD2_TractionCmd					1			//牵引指令

/*MC-MFD3子卡*/
/*110V*/
#define MC_MFD3_DoorClosedAll				6			//门关好继电器
#define MC_MFD3_CabACCNormal				10		//司机室空调运行正常
#define MC_MFD3_CabACCFault					12		//司机室空调故障

#define MC_MFD3_EmergEvacuDoorSta		3			//紧急疏散门状态


/*T-MFD1子卡*/
#define T_MFD1_BHBStatus					 	2			//BHB
#define T_MFD1_BLBStatus						4			//BLB
#define T_MFD1_SIVContactor					8			//SIV输出接触器
#define T_MFD1_SIVPower							10		//SIV电源建立

#define T_MFD1_BCUFault							1			//制动单元故障
#define T_MFD1_ParkingBrake		 			3			//停放制动
#define T_MFD1_BCUIsolate		 				5			//制动隔离塞门


/*M-MFD1子卡*/
#define M_MFD1_BHBStatus					 	2			//BHB
#define M_MFD1_BLBStatus						4			//BLB
#define M_MFD1_BCUFault							6			//制动单元故障
#define M_MFD1_ParkingBrake		 			8			//停放制动
#define M_MFD1_BCUIsolate		 				10		//制动隔离塞门

#define M_MFD1_VVVFPowerVaild				9			//VVVF电源有效

/*T1_MFD1子卡*/
#define T1_MFD1_ExtendPowering			4			//扩展供电
#define T1_MFD1_SIVContactor				10		//SIV输出接触器
#define T1_MFD1_SIVPower						12		//SIV电源建立

#define T1_MFD1_BCUFault						1			//制动单元故障
#define T1_MFD1_ParkingBrake		 		3			//停放制动
#define T1_MFD1_BCUIsolate		 			5			//制动隔离塞门


/*T2-MFD1子卡*/
#define T2_MFD1_BCUFault				  	12		//制动单元故障
#define T2_MFD1_ParkingBrake		 		1			//停放制动
#define T2_MFD1_BCUIsolate		 			3			//制动隔离塞门

/*定义全局数组，用于表示DI采集对应的下标数据*/
extern u8 MCMFD1Di110VIndex[12];
extern u8 MCMFD1Di24VIndex[2];
extern u8 MCMFD2DiIndex[7];
extern u8 MCMFD3DiIndex[4];
extern u8 TDiIndex[7];
extern u8 MDiIndex[6];
extern u8 T1DiIndex[6];
extern u8 T2DiIndex[3];


/*
函数说明：
功能：要获得某位的数据值
参数: 
	Data:入口参数,数据指针。
	GetPosit： 要得到数据的位置
返回值：返回得到某位的值
*/
u8 GetDataBit(u8 * Data,u8 GetPosit);

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

/*
函数说明：
功能：根据输入的数据生成bcc码
参数: 
	pData:入口参数，需要生成bcc的数据。
	len： pData的数据长度 (字节单位 )
返回值：返回生成的BCC码
*/
u8 GenerateBccChcek(void * pData,u8 len);

#endif //__COMMUNPRO_H__

