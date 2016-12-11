#include "BrakeComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"

/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针 (指向的类型为，S_TMS_BRAKE_SDR_Text,或者 S_TMS_BRAKE_TDR_Text)   
	pOutData:指向底层串口发送的数据
//	u8 mode:发送的数据为SDR，还是TDR	; 
返回值：无返回值
*/
void SendBrake_SDR_Or_TDR(void * pInData,u8 * pOutData /*,u8 mode*/)
{
	u8 BCC1Even ,BCC1Odd;		//存放BCC校验位

	u8 a,mode;
	a=((u8 *)pInData)[0];
	if( a == BRAKE_SDR_COMMAND)
		mode = SEND_SDR;
	else if( a == BRAKE_TDR_COMMAND)
		mode = SEND_TDR;
	
	
	switch(mode)
	{
		case SEND_SDR:
		{
				S_TMS_BRAKE_SDR_Frame DataFrame;
				
				DataFrame.SDR_STX = BRAKE_STX;
				
				memcpy(&DataFrame.SDR_Data,pInData,sizeof(S_TMS_BRAKE_SDR_Text));	 //将发送的文本信息全部拷贝给数据帧

				DataFrame.SDR_ETX = BRAKE_ETX;
				
				GenerateBrakeBccChcek(pInData,sizeof(S_TMS_BRAKE_SDR_Text),&BCC1Even,&BCC1Odd);
				
				DataFrame.SDR_BCC1EVEN = BCC1Even;
				DataFrame.SDR_BCC1ODD = BCC1Odd;
				
				memcpy(pOutData,&DataFrame,sizeof(DataFrame));
		}	
			break;
		
		case SEND_TDR:
		{
			S_TMS_BRAKE_TDR_Frame DataFrame;
			
			DataFrame.TDR_STX = BRAKE_STX;
			
			memcpy(&DataFrame.TDR_Data,pInData,sizeof(S_TMS_BRAKE_TDR_Text));	 //将发送的文本信息全部拷贝给数据帧

			DataFrame.TDR_ETX = BRAKE_ETX;
			
			GenerateBrakeBccChcek(pInData,sizeof(S_TMS_BRAKE_TDR_Text),&BCC1Even,&BCC1Odd);
			
			DataFrame.TDR_BCC1EVEN = BCC1Even;
			DataFrame.TDR_BCC1ODD = BCC1Odd;
			
			memcpy(pOutData,&DataFrame,sizeof(DataFrame));
		}
			break;
		
		default :
			break;
	}
}


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_TMS_BRAKE_SD_Text结构体)。
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetBrake_SD_Or_TD(u8 *pInData,void *pOutData)
{
	s8 returnvale=0;			//记录返回值
	u8 BCC1Even,Bcc1Odd;
	u8 BccCount=0;
		
	returnvale =	CheckBrakeGetError(pInData);
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	

		return BRAKE_GetError;
	}
	
	
	if(pInData[1] == BRAKE_SD_COMMAND)			//接受的为SD响应
	{
		S_TMS_BRAKE_SD_Frame DataFrame;
		
		memcpy(&DataFrame,pInData,sizeof(DataFrame));  //将输入的数据复制给变量
		GenerateBrakeBccChcek(&DataFrame.SD_Data,sizeof(S_TMS_BRAKE_SD_Text),&BCC1Even,&Bcc1Odd);
		
		if(BCC1Even != DataFrame.SD_BCC1EVEN )
			BccCount++;
		if(Bcc1Odd != DataFrame.SD_BCC1ODD)
			BccCount ++;
		
		if(BccCount >0)
		{
			#if DEBUG_RS485==1
			printf("BCC校验值不符合\r\n");
			#endif		
			return BRAKE_BCCError;
		}
	}
		
	else if(pInData[1] == BRAKE_TD_COMMAND ) 	//接受的为TD响应
	{
		if(pInData[2] == 0x00)				//TD0  模块编号No（OOH）
		{
			S_TMS_BRAKE_TD0_Frame DataFrame;
			
			memcpy(&DataFrame,pInData,sizeof(DataFrame));  //将输入的数据复制给变量
			
			GenerateBrakeBccChcek(&DataFrame.TD0_Data,sizeof(S_TMS_BRAKE_TD0_Text),&BCC1Even,&Bcc1Odd);
			
			if(BCC1Even != DataFrame.TD0_BCC1EVEN )
				BccCount++;
			if(Bcc1Odd != DataFrame.TD0_BCC1ODD)
				BccCount ++;
			
			if(BccCount >0)
			{
				#if DEBUG_RS485==1
				printf("BCC校验值不符合\r\n");
				#endif		
				return BRAKE_BCCError;
			}
		
		}
			
		else													//TD1	 模块编号（01~40）
		{
			S_TMS_BRAKE_TD1_Frame DataFrame;
			
			memcpy(&DataFrame,pInData,sizeof(DataFrame));  //将输入的数据复制给变量
			
			GenerateBrakeBccChcek(&DataFrame.TD1_Data,sizeof(S_TMS_BRAKE_TD1_Text),&BCC1Even,&Bcc1Odd);
			
			if(BCC1Even != DataFrame.TD1_BCC1EVEN )
				BccCount++;
			if(Bcc1Odd != DataFrame.TD1_BCC1ODD)
				BccCount ++;
			
			if(BccCount >0)
			{
				#if DEBUG_RS485==1
				printf("BCC校验值不符合\r\n");
				#endif		
				return BRAKE_BCCError;
			}
		}
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
s8 CheckBrakeGetError(u8 *pInData)
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
				return BRAKE_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return BRAKE_FrameError;
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
void GenerateBrakeBccChcek(void * pData,u8 len,u8* BCC1Even,u8* BCC1Odd)
{
	u8 i=0;
	u8 *BccData;
	u8 DataTotalLen= 0;
	DataTotalLen = len +1;
	
	BccData = mymalloc(SRAMIN,len+1);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
																			/* 申请的长度比数据长度多1字节，为了存放ETX*/	
	memcpy(BccData,pData,len);
	BccData[len] = BRAKE_ETX;
	
	
	for(i=0;i<DataTotalLen;i++)  	/*比传入的数据长度多了1字节*/
	{
		if((i&0x01) ==0 )	//判断为偶数列
		{
			*BCC1Even = (*BCC1Even) ^BccData[i]; 
		}
		
		else		//否则为奇数列
		{
			*BCC1Odd = (*BCC1Odd) ^BccData[i]; 
		}
	};
	myfree(SRAMIN,BccData);			//释放内存
}

