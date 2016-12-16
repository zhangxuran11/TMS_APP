#ifndef __TOOLS_H
#define __TOOLS_H
#include "canfestival.h"
#include <stdint.h>
#include <stdio.h>
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#endif

#define IAP_MSG_WARN(...) printf(__VA_ARGS__)
#ifndef DEBUG_LOG
#define DEBUG_LOG(...) printf("Filename %s, Function %s, Line %d > ", __FILE__, __FUNCTION__, __LINE__); \
                            printf(__VA_ARGS__); \
                            printf("\n");
#endif
extern UNS16 CRC16(UNS8* puchMsg, UNS32 usDataLen);

#if defined(__APP__) || defined(__BOOT__)
extern void	App_JumpFunction(uint32_t AppAddress);
#endif
#ifdef __APP__//FLASH ½Ó¿Ú
#define USER_FLASH_END_ADDRESS        0x080FFFFF
extern void FLASH_If_Init(void);
extern uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);
extern uint32_t FLASH_If_Erase(uint32_t StartSector);
#endif//__APP__

#endif//__TOOLS_H
