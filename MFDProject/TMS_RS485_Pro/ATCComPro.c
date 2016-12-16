#include "ATCComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
#include "MTD_MFD_RS485.h"
#include "main.h"
/*
	函数说明：
功能：监控设备发送给ATC的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendATCSdr(ATCSdrText * pInData/*,u8 * pOutData*/)
{
	ATCSdrFrame SdrDataFrame;
		
	SdrDataFrame.ATCSdrDle1 = ATC_DLE;
	SdrDataFrame.ATCSdrStx = ATC_STX;
	memcpy(&SdrDataFrame.ATCSdrData,pInData,sizeof(ATCSdrText));
	
	SdrDataFrame.ATCSdrDle2 = ATC_DLE;
	SdrDataFrame.ATCSdrEtx = ATC_ETX;
	SdrDataFrame.ATCSdrBcc =GenerateBccChcek (pInData,sizeof(ATCSdrText));
	
	//memcpy(pOutData,&SdrDataFrame,sizeof(ATCSdrFrame));
	MTDMFD_USART3_RS485_Send_Data((u8 *)&SdrDataFrame,sizeof(ATCSdrFrame));//直接调用Uart3发送数据
}


/*
	函数说明：
功能：监控设备接受ATCD的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到接受数据结构体)。

返回值：0,无错，  其他有错。
*/
s8 GetATCSd(u8 *pInData,ATCSdFrame *SdFrame)
{
	s8 returnvale;
	ATCSdFrame SdDataFrame;
	u8 checkBccData;
	
	returnvale=CheckATCGetError(pInData);		//检测接受是否错误
	
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return ATC_GetError;
	}
	
	memcpy(&SdDataFrame,pInData,sizeof(SdDataFrame));	//将串口接受的数据拷贝到结构体中
	
	checkBccData = GenerateBccChcek(&SdDataFrame.ATCSdData,sizeof(ATCSdText));	//得到数据的Bcc校验值
	
	if(checkBccData != SdDataFrame.ATCSdBcc)
	{
		#if DEBUG_RS485==1
			printf("ATC BCC校验值不符合\r\n");
		#endif		
		return ATC_BCCError;
	}
	
	else		//没有错误
	{
		memcpy(SdFrame,&SdDataFrame,sizeof(ATCSdFrame));
	}
		
	return 0;
}	


/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckATCGetError(u8 *pInData)
{
		//1.发送SDR 后，经过100ms 以上则没有正常收到SD 时,错误
	u8 i=0;
	
	/*判断10ms内是否接收到开始标志*/
	for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart3RxStartFlag == 1)
		{
			g_Uart3RxStartFlag =0;
			break;
		}	
		
		else
			delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表   (不适用软件定时，相当于打开定时器，直接执行下条语句，肯定不符合要求)
	}
	if(i>=10)
	{
		if(g_Uart3RxStartFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ATC 10ms内没有接受到数据\r\n");
			#endif
			return ATC_GetTimeoutError;
		}
	}
	
	/*再次判断数据是否接受完成*/
	for(i=0;i<100;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart3RxDataFlag == 1)
		{
			g_Uart3RxDataFlag =0;
			break;
		}	
		
		else
			delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
	}
	if(i>=100)
	{
		if(g_Uart3RxDataFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ATC 接受数据不完整!\r\n");
			#endif
			return ATC_GetTimeoutError;
		}
	}

	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart3FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		g_Uart3FrameErrorFlag = 0;  //清除帧错误，奇偶校验错误位
		
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return ATC_FrameError;
	}
	
	//3.检查接受的前字节数据是否匹配
	if(pInData[0]!=ATC_DLE ||pInData[1]!=ATC_STX || pInData[2]!=ATC_SD_TEXTCODEL8 || pInData[3]!=ATC_SD_TEXTCODEH8)
	{
		#if DEBUG_RS485==1
			printf("接受的开始符，地址，命令有一个不符合\r\n");
		#endif		
			return ATC_DataError;
	}

	//没有错误产生
	return 0;
}
