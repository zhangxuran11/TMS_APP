#include "IAP_UART.h"
#include "tools.h"
#include "IAP_Processor.h"
#include "iap.h"
static void parse_UART_Pkg(UNS8* buff,UNS16 count);
#define	 IAP_RECV_STATE_INIT	0
#define	 IAP_RECV_STATE_FLAG	(IAP_RECV_STATE_INIT+1)
#define	 IAP_RECV_STATE_LEN		(IAP_RECV_STATE_FLAG+1)
void IAP_UART_Proceed_Cell(UNS8 ch)
{
	static UNS8 rx_buff[UART_RX_BUFF_SIZE];
	static UNS16 rx_num = 0;
	static UNS16 len = 0;
	static UNS8 state = IAP_RECV_STATE_INIT;	
	switch(state)
	{
		case IAP_RECV_STATE_INIT:
			if(rx_num == 0 && ch == UART_RX_HEAD_FLAG1)
			{
				rx_buff[rx_num++] = ch;
			}
			else if(rx_num == 1  )
			{
				if(ch == UART_RX_HEAD_FLAG2)
				{
					state = IAP_RECV_STATE_FLAG;
					rx_buff[rx_num++] = ch;
				}
				else
				{
					state = IAP_RECV_STATE_INIT;
					rx_num = 0;
				}
			}
			break;
		case IAP_RECV_STATE_FLAG:
			if(rx_num == 2 )
			{
				rx_buff[rx_num++] = ch;
			}
			else if(rx_num == 3)
			{
				rx_buff[rx_num++] = ch;
				len = *(UNS16*)(rx_buff+2);
				if( len<= UART_RX_BUFF_SIZE)
				{
					state = IAP_RECV_STATE_LEN;
				}
				else
				{
					rx_num = 0;
					len = 0;
					state = IAP_RECV_STATE_INIT;
				}
			}
			break;
		case IAP_RECV_STATE_LEN:
			if(rx_num < len)
			{
				rx_buff[rx_num++] = ch;
			}
			//下面不能用else if 需要在接收最后一字节后立即判断是否满足长度
			if(rx_num == len)
			{
				UNS16 r_crc = *(UNS16*)(rx_buff+len - 2);
				if(r_crc == CRC16(rx_buff,len - 2))//crc检验通过
				{
					parse_UART_Pkg(rx_buff,len);
				}
				else
				{
					printf("");
				}
				//不论crc校验是否通过都会执行该步骤
				rx_num = 0;
				len = 0;
				state = IAP_RECV_STATE_INIT;
			}
			break;
	}
}
//解析UART包:
//	fflldcxxxx...xxxxcc
//两字节标志+两字节长度+一字节目标节点 +一字节功能码+iap功能码参数+两字节CRC
//目标节点 : 4b主节点+3b子节点+1b零 ,无论在命令流程还是响应流程，均对应于从PC到受控端的发送方
//长度		 : 整包长度，包括标志及crc校验，本地字节序与PC一致
static void parse_UART_Pkg(UNS8* buff,UNS16 count)
{
	UNS8 dst_mNode = buff[4] >> 4;
	UNS8 dst_sNode = buff[4] & 0x0F;
	UNS8 funcCode = buff[5] ;
	UNS8* pCmdVal = 	buff+6;
	UNS16 cmdValLen = count-8;//减去8个协议字节开销
	if(CRC16(buff,count-2)!= *(UNS16*)(pCmdVal+cmdValLen))
	{
		IAP_MSG_WARN("UART Pkg CRC invalid!\r\n");
		return;
	}
	if(IS_MAIN_DEVICE())
		IAP_SendCmd(*IAP_info.mCo_data->bDeviceNodeId, 0,dst_mNode,dst_sNode, funcCode, pCmdVal,cmdValLen);
	else
	 IAP_SendCmd(MAIN_DEVICE_NODE_ID(), *IAP_info.mCo_data->bDeviceNodeId,dst_mNode,dst_sNode, funcCode, pCmdVal,cmdValLen);

	return;
}

//发送到PC回应包
//	ffdcxxxxcc
//两字节标志+一字节目标节点 +一字节功能码+四字节iap功能码参数+两字节CRC
void send_UART_Pkg(UNS8 dst_mNode,UNS8 dts_sNode,UNS8 funcCode,UNS32 cmdVal)
{
	static UNS8 tx_buff[10];
	tx_buff[0] = UART_RX_HEAD_FLAG1;
	tx_buff[1] = UART_RX_HEAD_FLAG2;
	tx_buff[2] = dst_mNode<<4 | dts_sNode;
	tx_buff[3] = funcCode;
	*(UNS32*)(tx_buff+4) = cmdVal;
	*(UNS16*)(tx_buff+8) = CRC16(tx_buff,8);
	IAP_SEND_BYTES_TO_PC(tx_buff,10);
	
	
}
