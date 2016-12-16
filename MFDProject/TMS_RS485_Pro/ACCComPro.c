#include "ACCComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
//#include "stm32f4xx_it.h"		//有全局变量的标志
#include "delay.h"
#include "CommunPro.h"
#include "MTD_MFD_RS485.h"
#include "main.h"

u8 TMS_RxBuffer[ACC_RX_BUF_SIZE];
u8 TMS_TxBuffer[ACC_TX_BUF_SIZE];

/*
	函数说明：
功能：发送空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendACCSdr(ACCSdrText *pInData/*,u8 * pOutData*/)
{
		ACCSdrFrame SdrDataFrame;
	
		#if	 DEBUG_RS485==1
			printf("监控设备发送请求数据\r\n");
		#endif
	
		SdrDataFrame.ACCSdrStx = ACC_STX;
		memcpy(&SdrDataFrame.ACCSdrData,pInData,sizeof(ACCSdrText));		//其他数据在应用层有用户赋值；
		SdrDataFrame.ACCSdrBcc=GenerateBccChcek(&(SdrDataFrame.ACCSdrData),sizeof(ACCSdrText));
		
		//memcpy(pOutData,&SdrDataFrame,sizeof(ACCSdrFrame));
		MTDMFD_USART2_RS485_Send_Data((u8 *)&SdrDataFrame,sizeof(ACCSdrFrame));//直接调用Uart2发送数据
}


/*
	函数说明：
功能：接受空调的状态响应的数据 转换为使用 的状态数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:若监控设备，则得到每个状态标识(放到ACCSdText结构体)。
					若空调设备，接受的数据就是文本数据
返回值：0,无错，  其他有错。
*/
s8 GetACCSd(u8 *pInData,ACCSdText *pOutData)
{
	s8 returnvale=0;
	ACCSdFrame 	SdDataFrame;
	u8 checkBccData;
	
	returnvale=CheckGetError(pInData);		//检测接受是否错误
	
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return returnvale;
	}
	
	memcpy(&SdDataFrame,pInData,sizeof(ACCSdFrame));	//将串口接受的数据拷贝到结构体中
	checkBccData = GenerateBccChcek(&SdDataFrame.ACCSdData,sizeof(ACCSdText));	//得到数据的Bcc校验值
	
	if(checkBccData != SdDataFrame.ACCSdBcc)
	{
		#if DEBUG_RS485==1
			printf("BCC校验值不符合\r\n");
		#endif		
		return ACC_BCCError;
	}
	
	else//表示没有错误
	{
		memcpy(pOutData,&SdDataFrame.ACCSdData,sizeof(ACCSdText));	//copy数据
	}
		
	//没有错误返回0
	return 0;
}

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckGetError(u8 *pInData)
{
	//1.发送SDR 后，经过20ms 以上则没有正常收到SD 时,错误
	u8 i=0;
	
	for(i=0;i<20;i++)
	{
		if(g_Uart2RxStartFlag == 1)
		{
			g_Uart2RxStartFlag = 0;
			break;
		}
		
		else
			delay_ms(1);//延时20ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
	}
	if(i>=20)
	{
		if(g_Uart2RxStartFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ACC 20ms内没有接受到数据!\r\n");
			#endif
			return ACC_GetTimeoutError;
		}
	}	
	
	
	for(i=0;i<10;i++)
	{
		if(g_Uart2RxDataFlag == 1)
		{
			g_Uart2RxDataFlag = 0;
			break;
		}
		
		else
			delay_ms(1);//延时20ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
	}
	if(i>=10)
	{
		if(g_Uart2RxDataFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("ACC 接受数据不完整！\r\n");
			#endif
			return ACC_GetTimeoutError;
		}
	}

	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart2FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		g_Uart2FrameErrorFlag = 0;
		
		#if DEBUG_RS485==1
			printf("ACC 检测到帧错误、奇偶校验错误\r\n");
		#endif
		return ACC_FrameError;
	}
	
	//3.检查接受的前字节数据是否匹配
	if(pInData[0]!=ACC_STX || pInData[1]!=ACC_ADDRES || pInData[2]!=ACC_SUB_ADDRES ||pInData[3]!=ACC_COMMAND)
	{
		#if DEBUG_RS485==1
			printf("接受的开始符，地址，命令有一个不符合\r\n");
		#endif		
			return ACC_DataError;
	}
	
	//没有错误产生
	return 0;
}



//测试打印信息
void test_ACC(void)
{
//			u8 i,a;
//			ACCSdrText rx;
//		ACCSdText	State;
//			rx.Address=0x01;
//			rx.Sub_Address=0x00;
//			rx.Command=0x20;
//			rx.Year=16;
//			rx.Month=6;
//			rx.Day=24;
//			rx.Hour=1;
//			rx.Minute=2;
//			rx.Second=3;
//			rx.SetClock=1;
//		//	rx.Dummy1=0;
//		//	rx.Dummy2=0;

//			#if	 DEBUG_RS485==1
//			printf("rx_size=%d\r\n",sizeof(rx));
//		//	memcpy(TMS_TxBuffer,&rx,sizeof(rx));
//		//	for(i=0;i<sizeof(rx);i++)
//		//	{
//		//		printf("rx=%d\r\n",TMS_TxBuffer[i]);
//		//	}
//			
//			printf("SendACCCondit_SDR  test\r\n");
//			SendACCCondit_SDR(&rx,TMS_TxBuffer);
//			for(i=0;i<14;i++)
//			{
//				printf("rx=%d\r\n",TMS_TxBuffer[i]);
//			}
//			printf("\r\n");
//			
//			GetACCCondit_SD(TMS_TxBuffer,&State);
//			
//			i=GenerateBccChcek( &rx,sizeof(ACCSdrText));	//测试bcc
//			printf("code=%x,%d\r\n",i,i);
//			
//			
//			{
//				a=0xDB;
//				i=0;
//				i=GetDataBit(&a,7);
//				printf("i=%d\r\n",i);
//					i=GetDataBit(&a,6);
//				printf("i=%d\r\n",i);
//				i=GetDataBit(&a,5);
//				printf("i=%d\r\n",i);
//					i=GetDataBit(&a,3);
//				printf("i=%d\r\n",i);
//					i=GetDataBit(&a,2);
//				printf("i=%d\r\n",i);
//			}
//			
//			#endif 
//			

}
