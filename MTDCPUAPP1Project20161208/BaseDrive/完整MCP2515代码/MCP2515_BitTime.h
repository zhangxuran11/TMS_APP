#ifndef __MCP2515_BITTIME_H__
#define __MCP2515_BITTIME_H__

#include "MCP2515_RegisterDefine.h"

#define MCP2515_20MHz_CAN_BAUD_125K_CFG1  	0x04
#define MCP2515_20MHz_CAN_BAUD_125K_CFG2		0xB5  //0x80|7TQ|6TQ	 1:7:6:2
#define MCP2515_20MHz_CAN_BAUD_125K_CFG3		0x01

#define MCP2515_20MHz_CAN_BAUD_500K_CFG1  	0x01
#define MCP2515_20MHz_CAN_BAUD_500K_CFG2		0x9A  //0x80|4TQ|3TQ		1:4:3:2	
#define MCP2515_20MHz_CAN_BAUD_500K_CFG3		0x01

#endif //__MCP2515_BITTIME_H__
