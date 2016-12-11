#include "PISComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
//#include "MTD_MFD_RS485.h"

/*
	函数说明：
功能：监控设备发送给PIS的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendPISSdr(PISSdrText * pInData/*,u8 * pOutData*/)
{
	PISSdrFrame  SdrDataFrame;
	
	SdrDataFrame.PISSdrStx = PIS_STX;
	memcpy(&SdrDataFrame.PISSdrData,pInData,sizeof(PISSdrText));
	SdrDataFrame.PISSdrBcc =GenerateBccChcek (pInData,sizeof(PISSdrText));
	
	//memcpy(pOutData,&SdrDataFrame,sizeof(PISSdrFrame));
	//MTDMFD_USART2_RS485_Send_Data((u8 *)&SdrDataFrame,sizeof(PISSdrFrame));//直接调用Uart2发送数据
}


/*
	函数说明：
功能：监控设备接受PISD的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。

返回值：0,无错，  其他有错。
*/
s8 GetPISSd(u8 *pInData,void *pOutData)
{
	s8 returnvale;
	PISSdFrame SdDataFrame;
	u8 checkBccData;
	returnvale=CheckPISGetError(pInData);		//检测接受是否错误
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return PIS_GetError;
	}
	
	memcpy(&SdDataFrame,pInData,sizeof(SdDataFrame));	//将串口接受的数据拷贝到结构体中
	
	checkBccData = GenerateBccChcek(&SdDataFrame.PISSdData,sizeof(PISSdrText));	//得到数据的Bcc校验值
	
	if(checkBccData != SdDataFrame.PISSdBcc)
	{
		#if DEBUG_RS485==1
			printf("PIS BCC校验值不符合\r\n");
		#endif		
		return PIS_BCCError;
	}
	
	else		//没有错误
	{
		memcpy(pOutData,&SdDataFrame,sizeof(PISSdFrame));
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
s8 CheckPISGetError(u8 *pInData)
{
		//1.发送SDR 后，经过100ms 以上则没有正常收到SD 时,错误
	{
		u8 i=0;
		for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
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
				return PIS_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return PIS_FrameError;
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
