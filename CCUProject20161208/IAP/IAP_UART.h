#ifndef __IAP_UART_H
#define __IAP_UART_H
#include "canfestival.h"
							
#define UART_RX_HEAD_FLAG1										0xAF
#define UART_RX_HEAD_FLAG2										0xFA
#define UART_RX_BUFF_SIZE											260

void IAP_UART_Proceed_Cell(UNS8 ch);
extern void send_UART_Pkg(UNS8 dst_mNode,UNS8 dts_sNode,UNS8 funcCode,UNS32 cmdVal);
#endif//__IAP_UART_H
