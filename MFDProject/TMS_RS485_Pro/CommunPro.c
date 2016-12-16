#include "CommunPro.h"
#include "string.h"
#include "malloc.h"


/*定义全局数组，用于表示DI采集对应的下标数据*/

u8 MCMFD1Di110VIndex[12]={MC_MFD1_LeftOpenDoor,MC_MFD1_LeftCloseDoor,MC_MFD1_RightOpenDoor,MC_MFD1_RightCloseDoor,
											MC_MFD1_LDoorOCACmd,MC_MFD1_RDoorOCACmd,MC_MFD1_BCUFAult,MC_MFD1_ParkingBrake,MC_MFD1_BCUIsolate,
											MC_MFD1_EmergBrakeBypass,MC_MFD1_ACPStart,MC_MFD1_VVVFPowerVaild,};

u8 MCMFD1Di24VIndex[2]= {MC_MFD1_ATPEmergBrake,MC_MFD1_ATPServiceBrake};

u8 MCMFD2DiIndex[7] = {MC_MFD2_HeardRelay,MC_MFD2_TailRelay,MC_MFD2_BrakeCmd,MC_MFD2_EmergBrakeRelay,
										MC_MFD2_ATCPowering,MC_MFD2_ATCBypass,MC_MFD2_TractionCmd};

u8 MCMFD3DiIndex[4]={MC_MFD3_DoorClosedAll,MC_MFD3_CabACCNormal,MC_MFD3_CabACCFault,MC_MFD3_EmergEvacuDoorSta};

u8 TDiIndex[7]={T_MFD1_BHBStatus,T_MFD1_BLBStatus,T_MFD1_SIVContactor,T_MFD1_SIVPower,T_MFD1_BCUFault,  
								T_MFD1_ParkingBrake,T_MFD1_BCUIsolate };

u8 MDiIndex[6]={M_MFD1_BHBStatus,M_MFD1_BLBStatus,M_MFD1_BCUFault,M_MFD1_ParkingBrake,M_MFD1_BCUIsolate, 
								M_MFD1_VVVFPowerVaild};				

u8 T1DiIndex[6]={T1_MFD1_ExtendPowering,T1_MFD1_SIVContactor,T1_MFD1_SIVPower,T1_MFD1_BCUFault,
								T1_MFD1_ParkingBrake,T1_MFD1_BCUIsolate};

u8 T2DiIndex[3] = {T2_MFD1_BCUFault,T2_MFD1_ParkingBrake,T2_MFD1_BCUIsolate};								
/*
函数说明：
功能：要获得某位的数据
参数: 
	Data:入口参数,数据指针。
	GetPosit： 要得到数据的位置
返回值：返回得到某位的值
*/
u8 GetDataBit(u8 * Data,u8 GetPosit)
{
	u8 vale;
	vale=*Data <<(7-GetPosit);
	return vale>>7;
}



/*
	函数说明：
功能：将10进制数转换为BCD码
参数: 
			输入：u8 Dec   待转换的十进制数据 输入的数据只能是0~99
返回值：  转换后的BCD码
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 DecToBcd(u8 Dec)
{
	if(Dec>=99)
	{
		Dec %=100;
	}
	return ((Dec/10)<<4) | ((Dec%10) & 0x0F);
}

/*
	函数说明：
功能：将BCD码转换为10进制数
参数: 
			输入：u8 Bcd   待转换BCD码		只有一个字节
返回值：  转换后的10进制数
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 BcdToDec(u8 Bcd)
{
	return (Bcd>>4)*10 + (Bcd & 0x0f);
}


/*
函数说明：
功能：根据输入的数据生成bcc码
参数: 
	pData:入口参数，需要生成bcc的数据。
	len： pData的数据长度 (字节单位 )
返回值：返回生成的BCC码
*/
u8 GenerateBccChcek(void * pData,u8 len)
{
	u8 i=0;
	u8 BccCheckCode=0xFF;
	u8 *BccData;
	BccData = mymalloc(SRAMIN,len);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
	memcpy(BccData,pData,len);
	for(i=0;i<len;i++)
	{
		BccCheckCode=BccCheckCode^BccData[i];
	};
	myfree(SRAMIN,BccData);			//释放内存
	
	return BccCheckCode;
}













