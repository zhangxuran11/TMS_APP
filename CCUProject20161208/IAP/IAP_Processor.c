#include "IAP_Processor.h"
#include "iap.h"
#include "tools.h"
#ifndef inline
	#define inline __inline
#endif
#if defined(__APP__) || defined(__BOOT__)
void reboot_callback(CO_Data* d,UNS32 id)
{
		__disable_fault_irq();   
		NVIC_SystemReset();
}
#endif //defined(__APP__) || defined(__BOOT__)
void inline common_reponse(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len)
{
	send_UART_Pkg( dst_mNode, dst_sNode, funcCode,*(UNS32*)pCmdVal);
}
UNS32 get_soft_version_execute(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len)
{
	return IAP_info.softversion;
}
UNS32 reboot_execute(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len)
{
	#ifdef __BOOT__
	SetAlarm(NULL,0,reboot_callback,MS_TO_TIMEVAL(100),0);
	#endif
	return 0;
}
UNS32 program_ready_execute(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len)
{
#if defined(__BOOT__)
	return 1;//准备ok
#elif	defined(__APP__)//STM32 APP
	#include "tools.h"
	#include "iap.h"
	UNS32 writeAddr = IAP_FLAG_ADDR;
	UNS8 arg[8] = {0};
	arg[0] = 0xAB;
	arg[1] = src_mNode;
	arg[2] = src_sNode;
	arg[3] = dst_mNode;
	arg[4] = dst_sNode;
	arg[5] = funcCode;
	
	FLASH_If_Init();
	FLASH_If_Erase(writeAddr);
	FLASH_If_Write((uint32_t*)&writeAddr,(uint32_t*)&arg,sizeof(32));
	SetAlarm(NULL,0,reboot_callback,MS_TO_TIMEVAL(100),0);
	return 0;
#else
	return 0;//imx6q  通常不会进入此处
#endif
}
UNS32 programing_execute(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len)
{
#ifdef __BOOT__
#include "flash_if.h"
#include "iap.h"
	UNS8* _pCmdVal = pCmdVal;
	UNS32 app_address = MTD_APP1_ADDRES_FLASH;
	UNS32 app_end_address;
	UNS32 writeAddr;
	UNS16 fileCnt = *(UNS16*)_pCmdVal;
	UNS16 fileOffset = *(UNS16*)(_pCmdVal+2);
	UNS8 datLen = 0;
	UNS32 sector;
	if(IAP_info.ChooseBoard == 0x04 || IAP_info.ChooseBoard == 0x06 || IAP_info.ChooseBoard == 0x07)//跟据播码开关
		app_address = MTD_APP2_ADDRES_FLASH;
	if(fileOffset == 1)//保证程序在一个整块内
	{
		app_end_address = app_address+FILE_BLK_MAX_SIZE*fileCnt;
		FLASH_If_Init();
		for(sector = GetSector(app_address);sector <=GetSector(app_end_address);sector+=8 )
		{
			if (FLASH_EraseSector(sector, VoltageRange_3) != FLASH_COMPLETE)
			{
				/* Error occurred while page erase */
				return 2;//擦除失败
			}
		}
	}
	len -= 6;
	if(fileOffset != fileCnt){//最后一块的校验是整个文件的，故只校验最后一块之前的块
		if(CRC16((UNS8*)_pCmdVal+4,len) != *(UNS16*)(_pCmdVal+4+len))
			return 4;//块校验失败
	}
	datLen = len & 0x03 ? (len >>2 )+1 : len >> 2;
	writeAddr = app_address+FILE_BLK_MAX_SIZE*(fileOffset-1);

	if(0 != FLASH_If_Write((uint32_t*)&writeAddr,(uint32_t*)(_pCmdVal +4),datLen))
		return 3;//表示写入失败
//	printf("----------------programing_execute---------------------\r\n");
//	printf("CmdVal %x\r\n",*_pCmdVal);
//	printf("fileCnt %x\r\n",fileCnt);
//	printf("fileOffset %x\r\n",fileOffset);
//	printf("datLen %x\r\n",datLen);
//	printf("writeAddr %lx\r\n",writeAddr);
//	printf("-------------------------------------------------------\r\n");
	if(fileOffset == fileCnt){
		if(CRC16((UNS8*)app_address,FILE_BLK_MAX_SIZE*(fileOffset-1)+len) == *(UNS16*)(_pCmdVal+len+4))//校验整个文件
		{
			UNS32 flag = IAP_FLAG;
			writeAddr = IAP_FLAG_ADDR;
			FLASH_If_Erase(writeAddr);
			FLASH_If_Write((uint32_t*)&writeAddr,(uint32_t*)&flag,1);
			return 0;//表示校验OK
		}
		else
			return 5;//表示校验失败
	}
#endif//__BOOT__
	return 1;//表示写入成功
}

IAP_Processor_t iap_processor[FINAL_FUNC_CODE] = {
	{get_soft_version_execute,common_reponse},
	{program_ready_execute,common_reponse},
	{programing_execute,common_reponse},
	{reboot_execute,common_reponse},
};
