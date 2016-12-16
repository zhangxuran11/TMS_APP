#include "ACPComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
#include "CRC_CCITT.h"
#include "MTD_MFD_RS485.h"
#include "main.h"
/*
	函数说明：
功能：发送空压机的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据缓存
返回值：无返回值
*/
void SendACPSdr(ACPSdrFrame * pInData/*,u8 * pOutData*/)  
{
		ACPSdrFrame SdrDataFrame;
		u16	CrcVale;
		
		#if	 DEBUG_RS485==1
			printf("监控设备发送请求数据\r\n");
		#endif
		
		SdrDataFrame.ACPDeviceAddress = ACP_DEVICEADDRES;
		SdrDataFrame.Command	= ACP_COMMAND;
		memcpy(&SdrDataFrame.StartAddressH8,&pInData->StartAddressH8,4);		//cope地址，寄存器值
	
		CrcVale = crcCompute((u8*)&SdrDataFrame,sizeof(ACPSdrFrame)-2);
		SdrDataFrame.CRCH8 = (u8)(CrcVale>>8)&0xFF; 
		SdrDataFrame.CRCL8 = (u8)(CrcVale)&0xFF;
	
		//memcpy(pOutData,&SdrDataFrame,sizeof(ACPSdrFrame));		//复制数据
		MTDMFD_USART3_RS485_Send_Data((u8 *)&SdrDataFrame,sizeof(ACPSdrFrame));//直接调用Uart3发送数据
}

/*
	函数说明：
功能：接受空压机的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据缓存的指针 
	pOutData:监控设备，得到每个状态标识(放到S_ACPre_SD_StateBit结构体)。
返回值：0,无错，  其他有错。
*/
s8 GetACPSd(u8 *pInData,ACPSdFrame *pOutData)
{
	s8 returnvale=0;
	ACPSdFrame SdDataFrame;
	u16 SourCrcVale,ComputeCrcVale;
	
	returnvale=CheckACPGetError(pInData);		//检测接受是否错误
	
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return ACP_GetError;
	}
	
	memcpy(&SdDataFrame,pInData,sizeof(ACPSdFrame));	//将串口接受的数据拷贝到结构体中

	SourCrcVale = ((u16)SdDataFrame.CRCH8<<8 )| (SdDataFrame.CRCL8);	//源crc值
	ComputeCrcVale = crcCompute((u8*)&SdDataFrame,sizeof(ACPSdFrame)-2);		//计算得到的crc值

	if(SourCrcVale == ComputeCrcVale)
	{
		memcpy(pOutData,pInData,sizeof(ACPSdFrame));		//copy数据
	}
	else
	{
		#if DEBUG_RS485==1
			printf("BCC校验值不符合\r\n");
		#endif		
		return ACP_CRCError;
	}
		//没有错误返回0
		return 0;
}


//extern 	u8 g_Uart3RxDataFlag;
//extern	u8 g_Uart3FrameErrorFlag;
/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckACPGetError(u8 *pInData)
{
	//1.发送SDR 后，经过200ms 以上则没有正常收到SD 时,错误
	u8 i=0;
	/*判断200ms内是否接收到开始标志*/
	for(i=0;i<20;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart3RxStartFlag == 1)
		{
			g_Uart3RxStartFlag =0;
			break;
		}	
		
		else
			delay_ms(10);//延时10ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表   (不适用软件定时，相当于打开定时器，直接执行下条语句，肯定不符合要求)
	}
	if(i>=20)
	{
		if(g_Uart3RxStartFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ACP 200ms内没有接受到数据\r\n");
			#endif
			return ACP_GetTimeoutError;
		}
	}	
	
	/*判断100ms后是够接受完成标志*/
	for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart3RxDataFlag == 1)
		{
			g_Uart3RxDataFlag =0;
			break;
		}	
		else
			delay_ms(10);		//延时10ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
	}
	if(i>=10)
	{
		if(g_Uart3RxDataFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ACP 没有接受到完整数据\r\n");
			#endif
			return ACP_GetTimeoutError;
		}
	}

	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart3FrameErrorFlag == 1)	//检测到帧错误、奇偶校验错误时
	{
		g_Uart3FrameErrorFlag = 0;  //清除帧错误，奇偶校验错误位
		
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return ACP_FrameError;
	}
	
	//3.检查接受的前字节数据是否匹配
	if(pInData[0]!=ACP_DEVICEADDRES || pInData[1]!=ACP_COMMAND )
	{
		#if DEBUG_RS485==1
			printf("ACP 接受的开始符，地址，命令有一个不符合\r\n");
		#endif		
			return ACP_DataError;
	}
	
	//没有错误产生
	return 0;
}



void test_ACP(void)
{
	
		
}

