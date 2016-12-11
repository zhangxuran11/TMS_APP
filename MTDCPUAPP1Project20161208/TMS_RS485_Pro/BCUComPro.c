#include "BCUComPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "CommunPro.h"
//#include "MTD_MFD_CurrentLoop.h"

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
	//	u8 CmdByte,mode;
		
	//	CmdByte=((u8 *)pInData)[0];
	//	if( CmdByte == BCU_SDR_COMMAND)
	//		mode = SEND_SDR;
	//	else if( CmdByte == BCU_TDR_COMMAND)
	//		mode = SEND_TDR;
	
	/*复制数据*/
	switch(mode)
	{
		case SEND_SDR:
			{
				BcuSdrFrame SdrDataFrame;		//声明变量		
				
				SdrDataFrame.SdrStx = BCU_STX;
				memcpy(&SdrDataFrame.BcuSdrData,pInData,sizeof(BcuSdrText));	 //将发送的文本信息全部拷贝给数据帧
				SdrDataFrame.SdrEtx = BCU_ETX;
			
				GenerateBcuBccChcek(pInData,sizeof(BcuSdrText),&BCC1,&BCC2);	//生成BCC
				
				SdrDataFrame.SdrBCC1 = BCC1;
				SdrDataFrame.SdrBCC2 = BCC2;
				//memcpy(pOutData,&SdrDataFrame,sizeof(BcuSdrFrame));
				//MTDMFD_CL_Send_Data((u8 *)&SdrDataFrame,sizeof(BcuSdrFrame));	//直接调用电流环发送函数
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
				//MTDMFD_CL_Send_Data((u8 *)&TdrDataFrame,sizeof(BcuTdrFrame));	//直接调用电流环发送函数
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
s8 GetBcuSdOrTd(u8 *pInData,void *pOutData)
{
	s8 returnvale=0;			//记录返回值
	u8 BCC1,BCC2;
	u8 BccCount;
		
	returnvale =	CheckBcuGetError(pInData);
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("BCU Get SDorTD Error！\r\n");
		#endif	
		return BCU_GetError;
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
		#if DEBUG_RS485==1
		printf("BCU Get SdOrTD Data Error!\r\n ");
		#endif
		return BCU_GetError;
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
s8 CheckBcuGetError(u8 *pInData)
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
				return BCU_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return BCU_FrameError;
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
void GenerateBcuBccChcek(void * pData,u8 len,u8* BCC1,u8* BCC2)
{
	u8 i=0;
	u8 *BccData;
	u8 DataTotalLen= 0;
	DataTotalLen = len +1;
	
	BccData = mymalloc(SRAMIN,len+1);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
																			/* 申请的长度比数据长度多1字节，为了存放ETX*/	
	if(BccData == NULL)
	{
		printf("BCC内存申请失败!\r\n");
	}
	/*copy 数据*/
	memcpy(BccData,pData,len);
	BccData[len] = BCU_ETX;
	
	for(i=0;i<DataTotalLen;i++)  	/*比传入的数据长度多了1字节*/
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


