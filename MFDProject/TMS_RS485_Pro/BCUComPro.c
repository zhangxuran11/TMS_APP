#include "BCUComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
#include "MTD_MFD_CurrentLoop.h"
#include "main.h"
/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针 (指向的类型为，BcuSdrText,或者 BcuSdText)   
	pOutData:指向底层串口发送的数据
//	u8 mode:发送的数据为SDR，还是TDR	; 
返回值：无返回值
*/
void SendBcuSdrOrTdr(void * pInData/*,u8 * pOutData */,u8 mode)
{
	u8 BCC1 ,BCC2;		//存放BCC校验位

	/*复制数据*/
	switch(mode)
	{
		case SEND_SDR:
			{
				BcuSdrFrame SdrDataFrame;		//声明变量		
				
				SdrDataFrame.SdrStx = BCU_STX;
				memcpy(&SdrDataFrame.BcuSdrData,pInData,sizeof(BcuSdrText));	 //将发送的文本信息全部拷贝给数据帧
				SdrDataFrame.SdrEtx = BCU_ETX;
			
				GenerateBcuBccChcek((u8*)pInData,sizeof(BcuSdrText),&BCC1,&BCC2);	//生成BCC
				
				SdrDataFrame.SdrBCC1 = BCC1;
				SdrDataFrame.SdrBCC2 = BCC2;
				//memcpy(pOutData,&SdrDataFrame,sizeof(BcuSdrFrame));
				
				//printf("BCUsize=%d\r\n",sizeof(BcuSdrFrame));
				MTDMFD_CL_Send_Data((u8 *)&SdrDataFrame,sizeof(BcuSdrFrame));	//直接调用电流环发送函数
			}	
			break;
		
		case SEND_TDR:
			{
				BcuTdrFrame TdrDataFrame;		//声明变量	
				
				TdrDataFrame.TdrStx = BCU_STX;
				memcpy(&TdrDataFrame.BcuTdrData,pInData,sizeof(BcuTdrText));	 //将发送的文本信息全部拷贝给数据帧
				TdrDataFrame.TdrEtx = BCU_ETX;
				
				GenerateBcuBccChcek(pInData,sizeof(BcuTdrText),&BCC1,&BCC2);		//生成BCC
				
				TdrDataFrame.TdrBCC1 = BCC1;
				TdrDataFrame.TdrBCC2 = BCC2;
				//memcpy(pOutData,&TdrDataFrame,sizeof(BcuTdrFrame));
				MTDMFD_CL_Send_Data((u8 *)&TdrDataFrame,sizeof(BcuTdrFrame));	//直接调用电流环发送函数
			}
			break;
		
		default :
			#if	 DEBUG_RS485==1
				printf("BCU Frame Format Error！\r\n");
			#endif
			break;
	}
}


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_TMS_BCU_SD_Text结构体)。
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetBcuSdOrTd(u8 *pInData,void *pOutData,u8 mode,u8 GetNum)
{
	s8 returnvale=0;			//记录返回值
	u8 BCC1,BCC2;
	u8 BccCount;
		
	if(GetNum == GET_NUM1)
	{
		returnvale =	CheckBcuGetError(pInData,mode);			//第一次获得数据
	}
	else 
	{
		returnvale = SecondCheckBcuGetError(pInData,mode);	//表示第二次获得数据
	}
	
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("BCU Get SDorTD Error！\r\n");
		#endif	
		return returnvale;
	}
	
	/*BCC校验*/
	if(pInData[1] == BCU_SD_COMMAND)			//接受的为SD响应
	{
		BcuSdFrame SdDataFrame;		//定义变量
		BccCount =0;
		
		memcpy(&SdDataFrame,pInData,sizeof(BcuSdFrame));  //将输入的数据复制给变量
		GenerateBcuBccChcek(&SdDataFrame.BcuSdData,sizeof(BcuSdText),&BCC1,&BCC2);//生成BCC
		
		if(BCC1 != SdDataFrame.SdBCC1 )
				BccCount++;
		if(BCC2 != SdDataFrame.SdBCC2)
				BccCount ++;
		
		if(BccCount >0)		//BCC校验错误
		{
			#if DEBUG_RS485==1
			printf("BCU BCC校验值不符合\r\n");
			#endif		
			return BCU_BCCError;
		}
		
		else		//BCC正确
		{
			memcpy(pOutData,&SdDataFrame,sizeof(BcuSdFrame));		//将数据copy出去
		}
	}
		
	else if(pInData[1] == BCU_TD_COMMAND ) 	//接受的为TD响应
	{
			BcuTdFrame TdDataFrame;		//定义变量
			BccCount =0;
		
			memcpy(&TdDataFrame,pInData,sizeof(BcuTdFrame));  //将输入的数据复制给变量
			GenerateBcuBccChcek(&TdDataFrame.BcuTdData,sizeof(BcuTdText),&BCC1,&BCC2);
			
			if(BCC1 != TdDataFrame.TdBCC1 )
				BccCount++;
			if(BCC2 != TdDataFrame.TdBCC2)
				BccCount ++;
			
			if(BccCount >0)
			{
				#if DEBUG_RS485==1
					printf("BCU BCC校验值不符合\r\n");
				#endif		
				return BCU_BCCError;
			}
			else
			{
				memcpy(pOutData,&TdDataFrame,sizeof(BcuTdFrame));
			}
	}
	
	else
	{
		#if DEBUG_RS485==0
		printf("BCU Get SdOrTD Data Error!\r\n ");
		#endif
		return BCU_GetError;
	}
	return 0;
}

/*
	函数说明：
功能：检测接受数据时候的错误
参数: 
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckBcuGetError(u8 *pInData,u8 mode)
{
	static u16 Uart4OldCount=0;	//老的串口接收数据值
	u8 i=0;
	//1.发送SDR 后，经过15ms 以上则没有正常收到SD 时,错误
	
	//1.无应答超时，第一次(TM1),否则标志正常接受到数据
	for(i=0;i<15;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart4RxStartFlag == 1)
		{
			g_Uart4RxStartFlag =0;
			break;
		}	
		
		else
			delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表   (不适用软件定时，相当于打开定时器，直接执行下条语句，肯定不符合要求)
	}
	if(i>=15)
	{
		if(g_Uart4RxStartFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("BCU 15ms内没有接受到数据\r\n");
			#endif
			return BCU_GetTIM1OutError;
		}
	}	
	
	//2.表示接受到NACK消息
	if(g_Uart4NackFlag ==1)
	{
		g_Uart4NackFlag = 0;
		
			#if DEBUG_RS485==1
				printf("BCU NACK Error!\r\n");
			#endif
		return BCU_NACKError;
	}
	
	else if(g_Uart4RxDataFlag == 1)			//表示接受数据完成，则不用判断下面else数据
	{
		g_Uart4RxDataFlag = 0;
	}
	
	//3.接受字符之间超时
	//4.接受信息出错
	else  //表示正常接受消息
	{
		Uart4OldCount = USART4_CL_RxLength;		//将串口接受数据长度赋值给Uart4OledCount;
		
		if(mode == GET_SD)			//表示获取的为SD,并且接受字节小于54时
		{
			while(Uart4OldCount<54)	//循环判断数据
			{
				for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
				{
					if(USART4_CL_RxLength > Uart4OldCount)
					{
						break;
					}		
					else
						delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
				}
				if(i>=10)
				{
					#if DEBUG_RS485==1
						printf("BCU 10ms内没有下一字节数据!\r\n");
					#endif
					return BCU_GetTIM4OutError;
				}
				
				Uart4OldCount = USART4_CL_RxLength;		//将串口接受数据长度赋值给Uart4OledCount;
			}
			
			g_Uart4RxDataFlag = 0;  //执行到这里接受数据已经完成，清除接受完成标志
		}
		
		else if(mode == GET_TD)			//表示获取的为TD,并且接受字节小于56时
		{
			while(Uart4OldCount<56)	//循环判断数据  
			{
				for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
				{
					if(USART4_CL_RxLength > Uart4OldCount)
					{
						break;
					}		
					else
						delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
				}
				if(i>=10)
				{
					#if DEBUG_RS485==1
						printf("BCU 10ms内没有下一字节数据!\r\n");
					#endif
					return BCU_GetTIM4OutError;
				}
				
				Uart4OldCount = USART4_CL_RxLength;	//将串口接受数据长度赋值给Uart4OledCount;
			}
			
			g_Uart4RxDataFlag = 0;  //执行到这里接受数据已经完成，清除接受完成标志
		}	

		else
		{
			printf("BCU Get Error!\r\n");
		}
	}

				
	//3.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart4FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		g_Uart4FrameErrorFlag =0; //清除奇偶错误标志
		
		#if DEBUG_RS485==1
			printf("BCU 检测到帧错误、奇偶校验错误\r\n");
		#endif
		return BCU_FrameError;
	}
	//没有错误产生
	return 0;
}


/*
	第二次检测BCU错误
	函数在BCU通信接受错误的时候发送NCK，再次等待接受数据
*/
s8 SecondCheckBcuGetError(u8 *pInData,u8 mode)
{
	static u16 Uart4OldCount=0;	//老的串口接收数据值
	u8	IsNoACK = 0;
	u8 i=0;
	//需要先发送NACK，表示接受出错 ,然后重新接受数据
	IsNoACK = BCU_NACK;
	MTDMFD_CL_Send_Data(&IsNoACK,1);
	
	
	//1.无应答超时，第二次(TM2),否则标志正常接受到数据
	for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
	{
		if(g_Uart4RxStartFlag == 1)
		{
			g_Uart4RxStartFlag =0;
			break;
		}	
		
		else
			delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表   (不适用软件定时，相当于打开定时器，直接执行下条语句，肯定不符合要求)
	}
	if(i>=10)		
	{
		if(g_Uart4RxStartFlag == 0)	//说明没有接受到数据
		{
			#if DEBUG_RS485==1
				printf("BCU 15ms内没有接受到数据\r\n");
			#endif
			return BCU_GetTIM1OutError;
		}
	}	


	//2.接受字符之间超时
	//3.接受信息出错
	
	if(g_Uart4RxDataFlag == 1)	//表示接受数据完成，则不用判断下面else数据
	{
		g_Uart4RxDataFlag = 0;
	}
	
	else  //表示正常接受消息
	{
		Uart4OldCount = USART4_CL_RxLength;		//将串口接受数据长度赋值给Uart4OledCount;
		
		if(mode == GET_SD)			//表示获取的为SD,并且接受字节小于54时
		{
			while(Uart4OldCount<54)	//循环判断数据
			{
				for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
				{
					if(USART4_CL_RxLength > Uart4OldCount)
					{
						break;
					}		
					else
						delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
				}
				if(i>=10)
				{
					#if DEBUG_RS485==1
						printf("BCU 10ms内没有下一字节数据!\r\n");
					#endif
					return BCU_GetTIM4OutError;
				}
				
				Uart4OldCount = USART4_CL_RxLength;		//将串口接受数据长度赋值给Uart4OledCount;
			}
			
			g_Uart4RxDataFlag = 0;  //执行到这里接受数据已经完成，清除接受完成标志
		}
		
		else if(mode == GET_TD)			//表示获取的为TD,并且接受字节小于56时
		{
			while(Uart4OldCount<56)	//循环判断数据  
			{
				for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
				{
					if(USART4_CL_RxLength > Uart4OldCount)
					{
						break;
					}		
					else
						delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
				}
				if(i>=10)
				{
					#if DEBUG_RS485==1
						printf("BCU 10ms内没有下一字节数据!\r\n");
					#endif
					return BCU_GetTIM4OutError;
				}
				
				Uart4OldCount = USART4_CL_RxLength;	//将串口接受数据长度赋值给Uart4OledCount;
			}
			
			g_Uart4RxDataFlag = 0;  //执行到这里接受数据已经完成，清除接受完成标志
		}	

		else
		{
			printf("BCU Get Error!\r\n");
		}
	}

				
	//3.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart4FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		g_Uart4FrameErrorFlag =0; //清除奇偶错误标志
		
		#if DEBUG_RS485==1
			printf("BCU 检测到帧错误、奇偶校验错误\r\n");
		#endif
		return BCU_FrameError;
	}
	//没有错误产生
	return 0;
}


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
void GenerateBcuBccChcek(void * pData,u8 len,u8* BCC1,u8* BCC2)
{
	u8 i=0;
	u8 *BccData;
	u8 DataTotalLen= 0;
	DataTotalLen = len +1;
	
	BccData = mymalloc(SRAMIN,len+1);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
																			/* 申请的长度比数据长度多1字节，为了存放ETX*/	
	/*copy 数据*/
	memcpy(BccData,pData,len);
	BccData[len] = BCU_ETX;
	*BCC1 = BccData[0];		//赋初始值
	*BCC2 = BccData[1];
	
	for(i=2;i<DataTotalLen;i++)  	/*比传入的数据长度多了1字节，初始值已经有了，故从2开始*/
	{
		if((i&0x01) ==0 )	//判断为偶数列
		{
			*BCC1 = (*BCC1) ^BccData[i]; 
		}
		
		else		//否则为奇数列
		{
			*BCC2 = (*BCC2) ^BccData[i]; 
		}
	};
	myfree(SRAMIN,BccData);			//释放内存
}


