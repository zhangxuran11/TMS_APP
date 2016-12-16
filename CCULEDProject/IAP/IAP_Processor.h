#ifndef __IAP_PROCESSOR_H
#define __IAP_PROCESSOR_H
#include "canfestival.h"
enum{
	GET_SOFT_VERSION = 0,
	PROGRAM_READY,
	PROGRAMING,
	REBOOT,
	FINAL_FUNC_CODE
};
typedef struct __IAP_Processor_t
{
	UNS32 (*execute)(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len);
	void (*reponse)(UNS8 src_mNode,UNS8 src_sNode,UNS8 dst_mNode,UNS8 dst_sNode,UNS8 funcCode,void* pCmdVal,UNS8 len);
}IAP_Processor_t;

extern IAP_Processor_t iap_processor[];
#endif//__IAP_PROCESSOR_H
