#include "SIVComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
#include "CRC_CCITT.h"

/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针 (指向的类型为，SIVSdrText,或者 SIVSdText,SIVTcrText)   
	pOutData:指向底层串口发送的数据
	u8 mode:发送的数据为SDR，还是TDR	TCR; 
返回值：无返回值
*/
void SendSIVSdrOrTdrOrTcr(void * pInData,u8 * pOutData,u8 mode)
{
	u16	CrcVale;
	
	switch(mode)
	{
		case	SIV_SEND_SDR:
		{
			SIVSdrFrame SdrDataFrame;	//定义变量
			
			SdrDataFrame.StartFlag = SIV_STARTFLAG;
			SdrDataFrame.Address1 = SIV_ADDRESS1;
			SdrDataFrame.Address2 = SIV_ADDRESS2;
			SdrDataFrame.Control = SIV_CONTROLBYTE;
			memcpy(&SdrDataFrame.SIVSdrData,pInData,sizeof(SIVSdrText));//复制文本数据
			SdrDataFrame.SIVSdrData.Command = SIV_SDR_COMMAND;
			
			CrcVale = crcCompute((u8*)pInData+1,sizeof(SIVSdrText)+3);//生成CRC校验值,校验的是地址位，控制位和文本数据帧的数据
			SdrDataFrame.CRCH8 = (u8)(CrcVale>>8)&0xFF; 
			SdrDataFrame.CRCL8 = (u8)(CrcVale)&0xFF;
			SdrDataFrame.EndFlag = SIV_ENDFLAG;
			
			memcpy(pOutData,&SdrDataFrame,sizeof(SIVSdrFrame));
		}
		break;
		
		case	SIV_SEND_TDR:
		{
			SIVTdrFrame TdrDataFrame;	//定义变量
			
			TdrDataFrame.StartFlag = SIV_STARTFLAG;
			TdrDataFrame.Address1 = SIV_ADDRESS1;
			TdrDataFrame.Address2 = SIV_ADDRESS2;
			TdrDataFrame.Control = SIV_CONTROLBYTE;
			memcpy(&TdrDataFrame.SIVTdrData,pInData,sizeof(SIVTdrText));//复制文本数据
			TdrDataFrame.SIVTdrData.Command = SIV_TDR_COMMAND;
			
			CrcVale = crcCompute((u8*)pInData+1,sizeof(SIVSdrText)+3);//生成CRC校验值,校验的是地址位，控制位和文本数据帧的数据
			TdrDataFrame.CRCH8 = (u8)(CrcVale>>8)&0xFF; 
			TdrDataFrame.CRCL8 = (u8)(CrcVale)&0xFF;
			TdrDataFrame.EndFlag = SIV_ENDFLAG;
			
			memcpy(pOutData,&TdrDataFrame,sizeof(SIVSdrFrame));			
		}
		break;
		
		case	SIV_SEND_TCR:
		{
			SIVTcrFrame TcrDataFrame;	//定义变量
			
			TcrDataFrame.StartFlag = SIV_STARTFLAG;
			TcrDataFrame.Address1 = SIV_ADDRESS1;
			TcrDataFrame.Address2 = SIV_ADDRESS2;
			TcrDataFrame.Control = SIV_CONTROLBYTE;
			memcpy(&TcrDataFrame.SIVTcrData,pInData,sizeof(SIVTcrText));//复制文本数据
			TcrDataFrame.SIVTcrData.Command = SIV_TDR_COMMAND;
			
			CrcVale = crcCompute((u8*)pInData+1,sizeof(SIVTcrText)+3);//生成CRC校验值,校验的是地址位，控制位和文本数据帧的数据
			TcrDataFrame.CRCH8 = (u8)(CrcVale>>8)&0xFF; 
			TcrDataFrame.CRCL8 = (u8)(CrcVale)&0xFF;
			TcrDataFrame.EndFlag = SIV_ENDFLAG;
			
			memcpy(pOutData,&TcrDataFrame,sizeof(SIVSdrFrame));				
		}
		break;
		
		default:
			break;
	}
}  


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到SIVSdText结构体)。
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetSIVSdOrTd(u8 *pInData,void *pOutData)
{
	s8 returnvale=0;			//记录返回值
	u8 CommandVale=0;
	u16 SourCrcVale,ComputeCrcVale;
	
	returnvale =	CheckSIVGetError(pInData);
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("SIV Get SDorTD Error！\r\n");
		#endif	
		return SIV_GetError;
	}
	
	CommandVale = *(pInData+4);		//得到命令字节  3+1 = 0x7E,地址2字节，控制字1字节
	switch(CommandVale)
	{
		case	SIV_SD_COMMAND:
		{
			SIVSdFrame SdDataFrame;
			memcpy(&SdDataFrame,pInData,sizeof(SIVSdFrame));
			
			ComputeCrcVale = crcCompute(&SdDataFrame.Address1,sizeof(SIVSdText)+3);
			SourCrcVale = ((u16)SdDataFrame.CRCH8<<8 )| (SdDataFrame.CRCL8);
			
			if(SourCrcVale == ComputeCrcVale)
			{
				memcpy(pOutData,pInData,sizeof(SIVSdFrame));//去除前面的0x7E
			}
			else
			{
				#if DEBUG_RS485==1
				printf("SIV Sd校验值不符合\r\n");
				#endif		
				return SIV_CRCError;
			}
		}
		break;
		case	SIV_TD_COMMAND:
		{
			SIVTdFrame TdDataFrame;
			memcpy(&TdDataFrame,pInData,sizeof(SIVTdFrame));
			
			ComputeCrcVale = crcCompute(&TdDataFrame.Address1,sizeof(SIVSdText)+3);
			SourCrcVale = ((u16)TdDataFrame.CRCH8<<8 )| (TdDataFrame.CRCL8);
					
			if(SourCrcVale == ComputeCrcVale)
			{
				memcpy(pOutData,pInData,sizeof(SIVTdFrame));//去除前面的0x7E
			}
			else
			{
				#if DEBUG_RS485==1
				printf("SIV Sd校验值不符合\r\n");
				#endif		
				return SIV_CRCError;
			}
			
		}
		break;
		
		default:
			break;
	}
	
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
s8 CheckSIVGetError(u8 *pInData)
{
		//1.发送SDR 后，经过100ms 以上则没有正常收到SD 时,错误
	{
		u8 i=0;
		for(i=0;i<8;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
		{
			delay_ms(10);//延时10ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
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
				return SIV_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return SIV_FrameError;
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
