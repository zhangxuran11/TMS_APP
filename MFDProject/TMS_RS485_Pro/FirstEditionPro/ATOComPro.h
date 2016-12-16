#ifndef	 __ATOCOMPRO_H__
#define  __ATOCOMPRO_H__
	
#include "TMS_ComunDefine.h"

#define  ATO_RX_BUF_SIZE  		20  	//定义接受 数据缓冲区的长度
#define  ATO_TX_BUF_SIZE  		20  	//定义发送数据缓冲区的长度

/*定义开始2字节的数据*/
#define  ATO_PAD1   						0x7E	//开路PAD1
#define  ATO_PAD2    						0x7E	//开路PAD2
#define  ATO_PAD3    						0x7E	//开路PAD3
#define  ATO_PAD4    						0x7E	//开路PAD4
#define  ATO_STARTFLAG					0x7E	//起始标志
#define  ATO_ADDRESSL8					0x60	//ATO地址低8位
#define  ATO_ADDRESSH8					0x01	//ATO地址高8位
#define  ATO_CONTROLBYTE				0x13	//控制字
#define  ATO_ENDFLAG						0x7E	//结束标志

#define  ATO_SDR_COMMAND				0x20	//状态数据要求SDR 命令字（列车监控装置→车内引导装置）
#define  ATO_SD_COMMAND					0x30	//状态数据响应SD  命令字（车内引导装置→列车监控装置）


/*定义错误编码值*/
#define  ATO_DataError  				-1			//接受的前4字节有不一致的错误
#define  ATO_GetTimeoutError  	-2			//发送请求之后，20ms以内没有接收到数据
#define  ATO_FrameError					-3			//检测到帧错误、奇偶校验错误时
#define  ATO_CRCError         	-4   		//BCC 符号不符合计划值时
#define  ATO_GetError						-5			//表示接受数据错误
#define  ATO_TransException			-6			//表示传输异常


#endif //__ATOCOMPRO_H__
