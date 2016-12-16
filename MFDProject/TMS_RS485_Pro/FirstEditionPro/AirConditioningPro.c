
#include "AirconditioningPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
//#include "stm32f4xx_it.h"		//有全局变量的标志
#include "delay.h"
#include "CommunPro.h"

u8 TMS_RxBuffer[AIR_RX_BUF_SIZE];
u8 TMS_TxBuffer[AIR_TX_BUF_SIZE];

/*
	函数说明：
功能：发送空调的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendAirCondit_SDR(S_TMS_AIR_SDR_Text *pInData,u8 * pOutData)
{
		S_TMS_AIR_SDR_Frame DataFrame;
	
		#if	 DEBUG_RS485==1
			printf("监控设备发送请求数据\r\n");
		#endif
	
		DataFrame.SDR_STX=AIR_STX;
		memcpy(&DataFrame.SDR_Data,pInData,sizeof(S_TMS_AIR_SDR_Text));
	
		//其他数据在应用层有用户赋值；
	
		DataFrame.SDR_Bcc=GenerateBccChcek(&(DataFrame.SDR_Data),sizeof(S_TMS_AIR_SDR_Text));
	
		memcpy(pOutData,&DataFrame,sizeof(S_TMS_AIR_SDR_Frame));
}


/*
	函数说明：
功能：接受空调的状态响应的数据 转换为使用 的状态数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:若监控设备，则得到每个状态标识(放到S_AIR_SD_State结构体)。
					若空调设备，接受的数据就是文本数据
返回值：0,无错，  其他有错。
*/
s8 GetAirCondit_SD(u8 *pInData,S_AIR_SD_State *pOutData)
{
	s8 returnvale;
	S_TMS_AIR_SD_Frame DataFrame;
	u8 checkBccData;
	returnvale=CheckGetError(pInData);		//检测接受是否错误
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return AIR_GetError;
	}
	
	memcpy(&DataFrame,pInData,sizeof(DataFrame));	//将串口接受的数据拷贝到结构体中

	checkBccData = GenerateBccChcek(&DataFrame.SD_Data,sizeof(S_TMS_AIR_SD_Text));	//得到数据的Bcc校验值
	
	if(checkBccData != DataFrame.SD_Bcc)
	{
		#if DEBUG_RS485==1
			printf("BCC校验值不符合\r\n");
		#endif		
		return AIR_BCCError;
	}
	
	if(returnvale==0)	//表示没有错误
	{
		/*Unit1 动作状态*/
		pOutData->U1AllWarm = GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_ALLWAEM_POSIT);
		pOutData->U1HalfWarm = GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_HALFWAEM_POSIT);
		pOutData->U1HighCool = GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_HIGHCOOL_POSIT);
		pOutData->U1WeakCold = GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_WEAKCOLD_POSIT);
		pOutData->U1Blowing	=	GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_BLOWING_POSIT);
		pOutData->U1Automatic	=	GetDataBit(&DataFrame.SD_Data.U1ActiveState,AIR_AUTOMATIC_POSIT);
		
		/*Unit2 动作状态*/
		pOutData->U2AllWarm = GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_ALLWAEM_POSIT);
		pOutData->U2HalfWarm = GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_HALFWAEM_POSIT);
		pOutData->U2HighCool = GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_HIGHCOOL_POSIT);
		pOutData->U2WeakCold = GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_WEAKCOLD_POSIT);
		pOutData->U2Blowing	=	GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_BLOWING_POSIT);
		pOutData->U2Automatic	=	GetDataBit(&DataFrame.SD_Data.U2ActiveState,AIR_AUTOMATIC_POSIT);
		
		/*Unit1	故障*/
		pOutData->U1Cooler1Fault = GetDataBit(&DataFrame.SD_Data.U1Fault1,AIR_COOLER1FAULT_POSIT);
		pOutData->U1Cooler2Fault = GetDataBit(&DataFrame.SD_Data.U1Fault1,AIR_COOLER2FAULT_POSIT);
		pOutData->U1Vetilator1Fault = GetDataBit(&DataFrame.SD_Data.U1Fault1,AIR_VETILATOR1FAULT_POSIT);
		pOutData->U1Vetilator2Fault = GetDataBit(&DataFrame.SD_Data.U1Fault1,AIR_VETILATOR2FAULT_POSIT);
		pOutData->U1Vetilator3Fault = GetDataBit(&DataFrame.SD_Data.U1Fault1,AIR_VETILATOR3FAULT_POSIT);
		
		pOutData->U1Compressor1Protect = GetDataBit(&DataFrame.SD_Data.U1Fault2,AIR_COMPRESSOR1PROTECT_POSIT);
		pOutData->U1Compressor1Fault	= GetDataBit(&DataFrame.SD_Data.U1Fault2,AIR_COMPRESSOR1FAULT_POSIT);
		pOutData->U1Compressor2Protect = GetDataBit(&DataFrame.SD_Data.U1Fault2,AIR_COMPRESSOR2PROTECT_POSIT);
		pOutData->U1Compressor2Fault	=	GetDataBit(&DataFrame.SD_Data.U1Fault2,AIR_COMPRESSOR2FAULT_POSIT);
		
		/*Uint2 故障*/
		pOutData->U2Cooler1Fault = GetDataBit(&DataFrame.SD_Data.U2Fault1,AIR_COOLER1FAULT_POSIT);
		pOutData->U2Cooler2Fault = GetDataBit(&DataFrame.SD_Data.U2Fault1,AIR_COOLER2FAULT_POSIT);
		pOutData->U2Vetilator1Fault = GetDataBit(&DataFrame.SD_Data.U2Fault1,AIR_VETILATOR1FAULT_POSIT);
		pOutData->U2Vetilator2Fault = GetDataBit(&DataFrame.SD_Data.U2Fault1,AIR_VETILATOR2FAULT_POSIT);
		pOutData->U2Vetilator3Fault = GetDataBit(&DataFrame.SD_Data.U2Fault1,AIR_VETILATOR3FAULT_POSIT);
		
		pOutData->U2Compressor1Protect = GetDataBit(&DataFrame.SD_Data.U2Fault2,AIR_COMPRESSOR1PROTECT_POSIT);
		pOutData->U2Compressor1Fault	= GetDataBit(&DataFrame.SD_Data.U2Fault2,AIR_COMPRESSOR1FAULT_POSIT);
		pOutData->U2Compressor2Protect = GetDataBit(&DataFrame.SD_Data.U2Fault2,AIR_COMPRESSOR2PROTECT_POSIT);
		pOutData->U2Compressor2Fault	=	GetDataBit(&DataFrame.SD_Data.U2Fault2,AIR_COMPRESSOR2FAULT_POSIT);
		
		/*客室温度*/
		pOutData->RoomTemperature = (DataFrame.SD_Data.RoomTemperature - 0xD0)+5;		//客室温度 D0H～F3H（5～40℃） [1℃:1bit]		

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
s8 CheckGetError(u8 *pInData)
{
	//1.发送SDR 后，经过20ms 以上则没有正常收到SD 时,错误
	{
		u8 i=0;
		for(i=0;i<10;i++)
		{
			delay_ms(2);//延时20ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表 
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
					printf("20ms内没有接受到数据\r\n");
				#endif
				return AIR_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return AIR_FrameError;
	}
	
	//3.检查接受的前字节数据是否匹配
	if(pInData[0]!=AIR_STX || pInData[1]!=AIR_ADDRES || pInData[2]!=AIR_SUB_ADDRES ||pInData[3]!=AIR_COMMAND)
	{
		#if DEBUG_RS485==1
			printf("接受的开始符，地址，命令有一个不符合\r\n");
		#endif		
			return AIR_DataError;
	}
	
	//没有错误产生
	return 0;
}

///*
//函数说明：
//功能：根据输入的数据生成bcc码
//参数: 
//	pData:入口参数，需要生成bcc的数据。
//	len： pData的数据长度 (字节单位 )
//返回值：返回生成的BCC码
//*/
//u8 GenerateBccChcek(void * pData,u8 len)
//{
//	u8 i=0;
//	u8 BccCheckCode=0;
//	u8 *BccData;
//	BccData = mymalloc(SRAMIN,len);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
//	memcpy(BccData,pData,len);
//	for(i=0;i<len;i++)
//	{
//		BccCheckCode=BccCheckCode^BccData[i];
//	};
//	myfree(SRAMIN,BccData);			//释放内存
//	
//	return BccCheckCode;
//}

///*
//函数说明：
//功能：要获得某位的数据
//参数: 
//	Data:入口参数,数据指针。
//	GetPosit： 要得到数据的位置
//返回值：返回得到某位的值
//*/
//u8 GetDataBit(u8 * Data,u8 GetPosit)
//{
//	u8 vale;
//	vale=*Data <<(7-GetPosit);
//	return vale>>7;
//}


//测试打印信息
void test(void)
{
	u8 i,a;
	S_TMS_AIR_SDR_Text rx;
S_AIR_SD_State	State;
	rx.Address=0x01;
	rx.Sub_Address=0x00;
	rx.Command=0x20;
	rx.Year=16;
	rx.Month=6;
	rx.Day=24;
	rx.Hour=1;
	rx.Minute=2;
	rx.Second=3;
	rx.SetClock=1;
//	rx.Dummy1=0;
//	rx.Dummy2=0;

	#if	 DEBUG_RS485==1
	printf("rx_size=%d\r\n",sizeof(rx));
//	memcpy(TMS_TxBuffer,&rx,sizeof(rx));
//	for(i=0;i<sizeof(rx);i++)
//	{
//		printf("rx=%d\r\n",TMS_TxBuffer[i]);
//	}
	
	printf("SendAirCondit_SDR  test\r\n");
	SendAirCondit_SDR(&rx,TMS_TxBuffer);
	for(i=0;i<14;i++)
	{
		printf("rx=%d\r\n",TMS_TxBuffer[i]);
	}
	printf("\r\n");
	
	GetAirCondit_SD(TMS_TxBuffer,&State);
	
	i=GenerateBccChcek( &rx,sizeof(S_TMS_AIR_SDR_Text));	//测试bcc
	printf("code=%x,%d\r\n",i,i);
	
	
	{
		a=0xDB;
		i=0;
		i=GetDataBit(&a,7);
		printf("i=%d\r\n",i);
			i=GetDataBit(&a,6);
		printf("i=%d\r\n",i);
		i=GetDataBit(&a,5);
		printf("i=%d\r\n",i);
			i=GetDataBit(&a,3);
		printf("i=%d\r\n",i);
			i=GetDataBit(&a,2);
		printf("i=%d\r\n",i);
	}
	
	#endif 
	

}
