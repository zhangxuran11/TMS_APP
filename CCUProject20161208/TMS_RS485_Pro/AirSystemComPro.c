#include "AirSystemComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
#include "CRC_CCITT.h"
#include "stdlib.h"
#include "math.h"

/*
	函数说明：
功能：监控设备发送给空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendAirSystem_SDR(S_TMS_AirSystem_SDR_Text * pInData,u8 * pOutData)
{
	S_TMS_AirSystem_SDR_Frame DataFrame;
	uint16_t CrcValue;
	
	DataFrame.PAD1 = AIRSYSTEM_PAD1;
	DataFrame.PAD2 = AIRSYSTEM_PAD2;
	DataFrame.StartFlag = AIRSYSTEM_STARTFLAG;
	DataFrame.AddressL8 = AIRSYSTEM_ADDRESSL8;
	DataFrame.AddressH8 = AIRSYSTEM_ADDRESSH8;
	DataFrame.ControlByte = AIRSYSTEM_CONTROLBYTE;
	
	memcpy(&DataFrame.DataText,pInData,sizeof(S_TMS_AirSystem_SDR_Text));	 //将发送的文本信息全部拷贝给数据帧
	/*对年，月，日，时，分，秒，转换为BCD码的形式*/
	DataFrame.DataText.Year = DecToBcd(pInData->Year);
	DataFrame.DataText.Month = DecToBcd(pInData->Month);
	DataFrame.DataText.Day = DecToBcd(pInData->Day);
	DataFrame.DataText.Hour = DecToBcd(pInData->Hour);
	DataFrame.DataText.Minute = DecToBcd(pInData->Minute);
	DataFrame.DataText.Second = DecToBcd(pInData->Second);
	
	CrcValue = crcCompute((u8 *)&DataFrame.AddressL8,sizeof(S_TMS_AirSystem_SDR_Text)+3);
	DataFrame.CRCL8 =	CrcValue&0xff;
	DataFrame.CRCH8	= (CrcValue>>8)&0xff;
	DataFrame.EndFlag = AIRSYSTEM_ENDFLAG;
	
	memcpy(pOutData,&DataFrame,sizeof(DataFrame));
}


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
s8 GetAirSystem_SD(u8 *pInData,void *pOutData,u8 GetMode)
{
	s8 returnvale=0;			//记录返回值
	u8 i=0 /*,j=0*/;						//i记录接受数据的0x7E的个数,  j用于循环将前几个0x7E拷贝到缓冲区
	u8 CRCH8,CRCL8;
	u8 *pDireInData;			//定义一个指针，指向输入的数据 pInData;
//	u8 *pDireOutData;			//定义一个指针，指向输出的数据 pOutData;
	u8 s_AIRSYSTEMRxErrCount = 0;							 //静态局部变量，检测到连续5 次接收失败时，则判定为传输异常	
	
	pDireInData = pInData;
//	pDireOutData = pOutData;
	
	returnvale =	CheckAirSystemGetError(pInData);
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		
		s_AIRSYSTEMRxErrCount++;
		if(s_AIRSYSTEMRxErrCount == 5)
		{
			s_AIRSYSTEMRxErrCount = 0;
			return AIRSYSTEM_TransException;
		}
		
		return AIRSYSTEM_GetError;
	}
			
	s_AIRSYSTEMRxErrCount = 0 ;	//将不连续的标志清零
	
	while(*pDireInData == AIRSYSTEM_PAD1)			//计算有几个字节是0x7E
	{
		i++;
		pDireInData++;
	}
	
	if(GetMode == AIR_STATE_SD_MODE)
	{
		S_TMS_AirSystem_State_SD_Frame DataFrame;
		uint16_t CrcValue;
		
		memcpy(&DataFrame,pInData+i-1,sizeof(DataFrame));		//pInData+i-1 ，表示从startflag开始复制数据		
																												//注意这里得到的年月日，时分秒，还是BCD码，并没有转换为10进制
	
		CrcValue = crcCompute((u8 *)&(DataFrame.AddressL8),sizeof(S_TMS_AirSystem_State_SD_Text)+3);
		CRCH8 =	(CrcValue>>8) & 0xff;	//得到数据的CRC校验值
		CRCL8 =  CrcValue & 0xff;		//得到数据的CRC校验值
			
		if(CRCH8 != DataFrame.CRCH8 && CRCL8 != DataFrame.CRCL8)
		{
			#if DEBUG_RS485==1
				printf("BCC校验值不符合\r\n");
			#endif		
			return AIRSYSTEM_CRCError;
		}
		
		/*将数据拷贝给pOutData所指向的空间*/
//		for(j=0;j<i-1;j++)			//拷贝前几个字节的0x7E
//		{	
//			*pDireOutData = AIRSYSTEM_PAD1;
//			pDireOutData++;
//		}
		memcpy((u8*)pOutData/*+i-1*/,&DataFrame,sizeof(DataFrame));
		
	}
	
	else if(GetMode == AIR_HISFAULT_SD_MODE )
	{
		S_TMS_AirSystem_HisFault_SD_Frame DataFrame;
		uint16_t CrcValue;
		
		memcpy(&DataFrame,pInData+i-1,sizeof(DataFrame));		//pInData+i-1 ，表示从startflag开始复制数据
																												//注意这里得到的年月日，时分秒，还是BCD码，并没有转换为10进制
		
		CrcValue = crcCompute((u8 *)&(DataFrame.AddressL8),sizeof(S_TMS_AirSystem_HisFault_SD_Text)+3);
		CRCH8 = (CrcValue>>8) & 0xff;	//得到数据的CRC校验值
		CRCL8 =  CrcValue & 0xff;		//得到数据的CRC校验值
			
		if(CRCH8 != DataFrame.CRCH8 && CRCL8 != DataFrame.CRCL8)
		{
			#if DEBUG_RS485==1
				printf("BCC校验值不符合\r\n");
			#endif		
			return AIRSYSTEM_CRCError;
		}
		
		/*将数据拷贝给pOutData所指向的空间*/
//		for(j=0;j<i-1;j++)			//拷贝前几个字节的0x7E
//		{	
//			*pDireOutData = AIRSYSTEM_PAD1;
//			pDireOutData++;
//		}
		memcpy((u8*)pOutData/*+i-1*/,&DataFrame,sizeof(DataFrame));
		
	}
		
	//没有错误返回0
	return 0;
}

extern 	u8 g_RxDataFlag;
extern	u8 g_FrameErrorFlag;

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckAirSystemGetError(u8 *pInData)
{
		//1.发送SDR 后，经过20ms 以上则没有正常收到SD 时,错误
	{
		u8 i=0;
		for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
		{
			delay_ms(5);//延时5ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
			if(g_RxDataFlag == 1)
			{
				break;
			}				
		}
		if(i>=10)
		{
			if(g_RxDataFlag == 0)	//说明没有接受到数据
			{
				#if DEBUG_RS485==1
					printf("50ms内没有接受到数据\r\n");
				#endif
				return AIRSYSTEM_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return AIRSYSTEM_FrameError;
	}
	
//	//3.检查接受的前字节数据是否匹配
//	if(pInData[0]!=AIR_STX || pInData[1]!=AIR_ADDRES || pInData[2]!=AIR_SUB_ADDRES ||pInData[3]!=AIR_COMMAND)
//	{
//		#if DEBUG_RS485==1
//			printf("接受的开始符，地址，命令有一个不符合\r\n");
//		#endif		
//			return AIRSYSTEM_DataError;
//	}

	//没有错误产生
	return 0;
}


///*
//	函数说明：
//功能：将10进制数转换为BCD码
//参数: 
//			输入：u8 Dec   待转换的十进制数据 输入的数据只能是0~99
//返回值：  转换后的BCD码
//思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
//*/
//u8 DecToBcd(u8 Dec)
//{
//	if(Dec>=99)
//	{
//		Dec %=100;
//	}
//	return ((Dec/10)<<4) | ((Dec%10) & 0x0F);
//}

///*
//	函数说明：
//功能：将BCD码转换为10进制数
//参数: 
//			输入：u8 Bcd   待转换BCD码		只有一个字节
//返回值：  转换后的10进制数
//思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
//*/
//u8 BcdToDec(u8 Bcd)
//{
//	return (Bcd>>4)*10 + (Bcd & 0x0f);
//}


void test_airsystem(void)
{
	{
		/*将数组 复制给 位定义*/
		u8 Buffer[30]={0xCE};
//		u8 datbuff[30];
//		u8 i;
		S_TMS_AirSystem_SDR_Text data;
		memset(&Buffer[1],0x88,sizeof(Buffer)-1);
		memcpy(&data,Buffer,sizeof(Buffer));
		
		printf("\r\n");
		printf("bit7=%d\r\n",data.HistoryFaultQuery);
		printf("bit6=%d\r\n",data.OrderStartSignal);
		printf("bit5=%d\r\n",data.EnvTemperEffective);
		printf("bit4=%d\r\n",data.TarTemperEffective);
		printf("bit3=%d\r\n",data.TimeEffective);
		printf("bit2~0=%d\r\n",data.TrainNumber);
		
		printf("HistoryFaultNumber=0x%.2X\r\n",data.HistoryFaultNumber);
		printf("Year=0x%.2X\r\n",data.Year);
		printf("\r\n");

	}
	
	{
		/*测试数据的拷贝*/
		/*将位定义的数据复制给数组*/
		S_TMS_AirSystem_SDR_Frame SDRData;
		S_TMS_AirSystem_State_SD_Frame sdData;
		u8 datbuff[30];
		u16 i=0;
		
		SDRData.PAD1=AIRSYSTEM_PAD1;
		SDRData.PAD2=AIRSYSTEM_PAD2;
		SDRData.StartFlag= AIRSYSTEM_STARTFLAG;
		SDRData.AddressL8=AIRSYSTEM_ADDRESSL8;
		SDRData.AddressH8=AIRSYSTEM_ADDRESSH8;
		SDRData.ControlByte=AIRSYSTEM_CONTROLBYTE;
		SDRData.DataText.TrainNumber=6;
		SDRData.DataText.TimeEffective=1;
		SDRData.DataText.TarTemperEffective=0;
		SDRData.DataText.EnvTemperEffective=0;
		SDRData.DataText.OrderStartSignal=1;
		SDRData.DataText.HistoryFaultQuery=1;
		
		SDRData.DataText.HistoryFaultNumber=1;
		SDRData.DataText.Year=16;
		SDRData.DataText.Month=6;
		SDRData.DataText.Day=30;
		SDRData.DataText.Hour=12;
		SDRData.DataText.Minute=12;
		SDRData.DataText.Second=60;
		SDRData.DataText.RefriTargetTemperL8=8;
		SDRData.DataText.RefriTargetTemperH8=9;
		SDRData.DataText.EnvironmentTemperL8=10;
		SDRData.DataText.EnvironmentTemperH8=11;
		SDRData.DataText.WorkMode=12;
		SDRData.DataText.Dummy1=13;
		SDRData.DataText.Dummy2=14;
		SDRData.CRCL8=0xAA;
		SDRData.CRCH8=0xbb;
		SDRData.EndFlag=AIRSYSTEM_ENDFLAG;
		
		memcpy(datbuff,&SDRData,sizeof(SDRData));
		printf("\r\n");
		for(i=0;i<sizeof(SDRData);i++)
		{
			printf("data%d=0x%x\r\n",i,datbuff[i]);
		}
		printf("\r\n");

		/*crc校验*/
		i=0;
		i=crcCompute((u8 *)&SDRData.DataText,sizeof(S_TMS_AirSystem_SDR_Text));
		printf("crc=0x%x\r\n",i);
		
		/**/
		SendAirSystem_SDR(&SDRData.DataText,datbuff);
		printf("\r\n");
		for(i=0;i<sizeof(SDRData);i++)
		{
			printf("send%d=0x%x\r\n",i,datbuff[i]);
		}
		printf("\r\n");
		
		GetAirSystem_SD(datbuff,&sdData,AIR_STATE_SD_MODE);
		
		printf("1=0x%x\r\n",sdData.StartFlag);
		printf("adH8=0x%x\r\n",sdData.AddressH8);
	}
	
	{
		u8 a=0;
		a=DecToBcd(16);
		printf("a1=%x\r\n",a);		//16进制
		
		a=BcdToDec(0x16);
		printf("a2=%d\r\n",a);		//10进制
	}
	
}


