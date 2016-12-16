#ifndef __IAP_CFG_H
#define __IAP_CFG_H
#define IAP_CMD_CSDO_PARA_INDEX				0x1280	//用于发送IAP命令的客户端SDO参数字典索引

/*****************************不需改变**********************************/
#define IAP_MAIN_INDEX						0X5fff	//IAP字典主索引
#define IAP_SEG_INDEX							0X01	//IAP命令字典子索引
#define IAP_CMD_SUB_INDEX							0X02	//IAP命令字典子索引
#define IAP_CMD_REPONSE_SUB_INDEX							0X03	//IAP命令响应字典子索引
#define IAP_SOFT_VERSION_SUB_INDEX							0X04	//IAP版本字典子索引
/***********************************************************************/
#define CAN3_NODE_ID 0x08	//主卡can3口id
#define CCU1_LED	0x11
#define CCU2_LED	0x21

/***********************************************************************/
#endif//__IAP_CFG_H
