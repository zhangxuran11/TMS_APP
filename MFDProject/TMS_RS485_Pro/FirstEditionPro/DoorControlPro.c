#include "DoorControlPro.h"
#include "string.h"
#include "Uart.h"
#include "malloc.h"	
#include "delay.h"
#include "main.h"

/*
	函数说明：
功能：发送车门控制的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针   
	pOutData:指向底层串口发送的数据
返回值：无返回值
*/
void SendDoorControl_SDR(S_TMS_DOOR_SDR_Text * pInData,u8 * pOutData)  //发送车门控制的状态请求数据。
{
		S_TMS_DOOR_SDR_Frame DataFrame;
	
		#if	 DEBUG_RS485==1
			printf("监控设备向车门控制设置发送请求数据\r\n");
		#endif
	
		DataFrame.SDR_STX=DOOR_STX;
		memcpy(&DataFrame.SDR_Data,pInData,sizeof(S_TMS_DOOR_SDR_Text));

		//其他数据在应用层有用户赋值；
	
		DataFrame.SDR_Bcc=GenerateDoorBccChcek(&(DataFrame.SDR_Data),sizeof(S_TMS_DOOR_SDR_Text));
	
		memcpy(pOutData,&DataFrame,sizeof(S_TMS_DOOR_SDR_Frame));
}


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_DOOR_SD_State结构体)。
返回值：0,无错，  其他有错。
*/
s8 GetDoorControl_SD(u8 *pInData,S_TMS_DOOR_SD_Frame *DoorSDFrame,S_DOOR_SD_State *pOutData)		//监控设备接受车门控制的状态响应数据。
{
	s8 returnvale;
	S_TMS_DOOR_SD_Frame DataFrame;
	u8 checkBccData;
	returnvale=CheckDoorGetError(pInData);		//检测接受是否错误
	if(returnvale !=0)
	{
		#if DEBUG_RS485==1
			printf("有错误发生！\r\n");
		#endif	
		return DOOR_GetError;
	}
	
	memcpy(&DataFrame,pInData,sizeof(DataFrame));	//将串口接受的数据拷贝到结构体中

	memcpy(DoorSDFrame,&DataFrame,sizeof(DataFrame));//whh 增加
	
	checkBccData = GenerateDoorBccChcek(&DataFrame.SD_Data,sizeof(S_TMS_DOOR_SD_Text));	//得到数据的Bcc校验值
	
	if(checkBccData != DataFrame.SD_Bcc)
	{
		#if DEBUG_RS485==1
			printf("BCC校验值不符合\r\n");
		#endif		
		return DOOR_BCCError;
	}
	
	if(returnvale==0)	//表示没有错误
	{
		/*车门状态*/
		pOutData->SignalDetection = GetDoorDataBit(&DataFrame.SD_Data.CarDoorState,DOOR_SIGNALDETECTION_POSIT);
		pOutData->DoorIsolation = GetDoorDataBit(&DataFrame.SD_Data.CarDoorState,DOOR_DOORISOLATION_POSIT);
		pOutData->DoorSwitchAction = GetDoorDataBit(&DataFrame.SD_Data.CarDoorState,DOOR_DOORSWITCHACTION_POSIT);
		pOutData->DoorCloseState = GetDoorDataBit(&DataFrame.SD_Data.CarDoorState,DOOR_DOORCLOSESTATE_POSIT);
		pOutData->DoorOpenState = GetDoorDataBit(&DataFrame.SD_Data.CarDoorState,DOOR_DOOROPENSTATE_POSIT);
		
		/*车门故障*/
		pOutData->CloseDoorExtrusion = GetDoorDataBit(&DataFrame.SD_Data.CarDoorFault,DOOR_CLOSEDOOREXTRUSION_POSIT);
		pOutData->OpenDoorExtrusion = GetDoorDataBit(&DataFrame.SD_Data.CarDoorFault,DOOR_OPENDOOREXTRUSION_POSIT);
		pOutData->DoorSystemFault	=	GetDoorDataBit(&DataFrame.SD_Data.CarDoorFault,DOOR_DOORSYSTEMFAULT_POSIT);
		pOutData->EmergencyDeviceOper = GetDoorDataBit(&DataFrame.SD_Data.CarDoorFault,DOOR_EMERGENCYDEVICEOPER_POSIT);
		
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
s8 CheckDoorGetError(u8 *pInData)
{
	//1.发送SDR 后，经过20ms 以上则没有正常收到SD 时,错误
	{
		u8 i=0;
		for(i=0;i<10;i++)		//这样处理的好处是，可以及时判断数据是否到来，响应会及时些
		{
			delay_ms(1);//延时1ms后，判断接受中断标志位的值				//W H H 这里可以用个软件定时器 替代， 暂时使用延时代表   (不适用软件定时，相当于打开定时器，直接执行下条语句，肯定不符合要求)
			if(g_Uart3RxDataFlag == 1)
			{
				g_Uart3RxDataFlag =0;
				break;
			}	
		}
		if(i>=10)
		{
			if(g_Uart3RxDataFlag == 0)	//说明没有接受到数据
			{
				#if DEBUG_RS485==1
					printf("10ms内没有接受到数据\r\n");
				#endif
				return DOOR_GetTimeoutError;
			}
		}
	}
	
	//2.检测到帧错误、奇偶校验错误时,的错误
	if(g_Uart3FrameErrorFlag == 1)	////检测到帧错误、奇偶校验错误时
	{
		#if DEBUG_RS485==1
			printf("检测到帧错误、奇偶校验错误\r\n");
		#endif
		return DOOR_FrameError;
	}
	
	//3.检查接受的前字节数据是否匹配
	if(pInData[0]!=DOOR_STX || pInData[1]<0x01 ||pInData[1]>0x08 || pInData[2]!=DOOR_SUB_ADDRES ||pInData[3]!=DOOR_COMMAND )
	{
		#if DEBUG_RS485==1
			printf("接受的开始符，地址，命令有一个不符合\r\n");
		#endif		
			return DOOR_DataError;
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
返回值：返回生成的BCC码
*/
u8 GenerateDoorBccChcek(void * pData,u8 len)
{
	u8 i=0;
	u8 BccCheckCode=0xFF;
	u8 *BccData;
	BccData = mymalloc(SRAMIN,len);		//动态申请内存 ,在keil里不能使用BccData[len],编译会错误，提示数组长度未定义
	memcpy(BccData,pData,len);
	for(i=0;i<len;i++)
	{
		BccCheckCode=BccCheckCode^BccData[i];
	};
	myfree(SRAMIN,BccData);			//释放内存
	
	return BccCheckCode;
}

	//要获得某位的数据，先将数据移动到最高位，然后在移到最最低位。
/*
函数说明：
功能：要获得某位的数据
参数: 
	Data:入口参数,数据指针。
	GetPosit： 要得到数据的位置
返回值：返回得到某位的值
*/
u8 GetDoorDataBit(u8 * Data,u8 GetPosit)
{
	u8 vale;
	vale=*Data <<(7-GetPosit);
	return vale>>7;
}


