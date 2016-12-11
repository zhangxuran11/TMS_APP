#include "can_stm32.h"
#include "canfestival.h"
#include "CAN3Master.h"
#include "CAN3MasterSlave.h"
#include "MTD_CPU_CAN3_MCP2515.h"
#include "objdictdef.h"			//W H H  这两个头文件必须添加，否则无法获取节点的id
#include "sysdep.h"
#include "os.h"
#include "stdbool.h"
#include "bsp.h"

/**
  * @brief  Main program
  * @param  None
  * @retval : None
  */
extern OS_MUTEX	CAN3_MUTEX;		//定义一个互斥信号量,用于can3发送接受任务的互斥

extern void TIM4_start(void);
int  test_can3master(void);

void can3master_recv_thread(void* p_arg)
{
	OS_ERR err;
	CanRxMsg RxMessage;
	Message *pmsg;
	uint32_t i, next;
	
	debugprintf("can3init_500K\r\n");
	canInit(CAN3,MCP2515_CAN_BAUD_500K);			//W H H ,MTDCPU用的can3
	
	can3rx_save = 0;
	can3rx_read = 0;

	can3tx_save = 0;
	can3tx_read = 0;
	
	test_can3master();		//调用主站测试函数

  /* Infinite loop*/
  while(1)
  {	
		//请求信号量 ，等待信号量
		OSSemPend(&can3recv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		if(err)
		{
			printf("can3Rx_err=0x%x\r\n",err);
		}
		
		else /*if(err == OS_ERR_NONE)*/
						//if(g_can3RxCount>0 )
						//printf("sem3=%d\r\n",g_can3RxCount);
		//whh 9-13	 接受数据放入到缓冲区				
		{
			OSMutexPend (&CAN3_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
			
			memset(&RxMessage,0,sizeof(CanRxMsg));		
			MCP2515_CAN3_Receive_Message(&RxMessage);	//W H H,上修改为下面这句
			if(RxMessage.DLC >0)
			{
				next = (can3rx_save + 1) ;
				if(next >= RX_BUF_LEN) next = 0;
				//				if(next == can3rx_read)
				//				{
				//					printf("can3 err!\r\n");
				//					return;				//WHH ，循环缓冲区 覆盖
				//				}
				pmsg = &can3rx_msg_buf[can3rx_save];
				pmsg->cob_id = (UNS16)RxMessage.StdId;		/**< message's ID */
				if(RxMessage.RTR == CAN_RTR_REMOTE)				/**< remote transmission request. (0 if not rtr message, 1 if rtr message) */
					pmsg->rtr = 1;		
				else
					pmsg->rtr = 0;

				pmsg->len  = (UNS8)RxMessage.DLC;					/**< message's length (0 to 8) */
				for(i = 0; i < pmsg->len; i++)
					pmsg->data[i] = RxMessage.Data[i];

				can3rx_save = next;
				
				//释放接受处理信号量
				OSSemPost(&can3RxProc_sem,OS_OPT_POST_1,&err);			 /*WHH 信号量的放松必须放到这里，这样接受处理任务的速度才快*/	
			}
			
			/*WHH*/
			//						printf("id3=0x%x\r\n",can3rx_msg_buf[can3rx_read].cob_id);
			//						for(i=0;i<8;i++)
			//						{
			//							printf("rx3=0x%x\r\n",can3rx_msg_buf[can3rx_read].data[i]);
			//						}
			
			OSMutexPost(&CAN3_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量

		}	
		//OSSched();		//W H  H 进行任务调度,会产生错误，调度太快，低优先级的任务无法执行到。无法使任务切换到低有限的任务
		//BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
  }
}

//can3接受处理任务函数
void can3RxProcthread(void *p_arg)
{
	OS_ERR err;
	uint32_t next;
	Message *pmsg;
	
	while(1)
	{
		//请求信号量 ，阻塞等待信号量
		OSSemPend(&can3RxProc_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		
		if(err)
		{
			printf("can3Rx_err=0x%x\r\n",err);
		}
		else /*if(err == OS_ERR_NONE)*/
		{
			next = can3rx_read;
			pmsg = &can3rx_msg_buf[next];
				
			//debugprintf("MC can3 id =0x%x\r\n",pmsg->cob_id);		
			
			//TIM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);		//关定时器中断
			/* Disable the Interrupt sources */
			TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&MTDCAN3Master_Data, pmsg);
			/* Enable the Interrupt sources */
			TIM4->DIER |= TIM_IT_CC1;
			//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);		//开定时器中断

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			can3rx_read = next;
			
			//debugprintf("can3RxProc ！！\r\n");
		}
	}
}


void can3master_send_thread(void *p_arg)
{
	OS_ERR err;
	uint32_t next;
	uint8_t  mailbox_no,TxBufNum;
	CanTxMsg *ptx_msg;	
	u8 timeout=0;
	
	while(1)
	{	
		//printf("can3 sendthread\r\n");
		//请求信号量 ，等待信号量
		//		OSSemPend(&can3tran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		//		if(err)
		//		{
		//			printf("can3Tx_err=%d\r\n",err);
		//			printf("can3post=%d\r\n",err);
		//		}
		
		//W H H  ，使用全局变量来代替信号量
		//else /*if(err == OS_ERR_NONE)*/
		
		if(g_can3TxCount >0)
		{
			next = can3tx_read;
			ptx_msg = &can3tx_msg_buf[next];
		
			OSMutexPend (&CAN3_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
			
			do{
					mailbox_no = MTDCPU_CAN3_MCP2515_Get_Free_TxBuf(&TxBufNum); // info = addr.
					timeout++;
					if (timeout == 0xFF)
					{		
						//printf(" 3 no mailbox!\r\n");
						break;
					}
			} while (mailbox_no == MCP2515_ALLTXBUSY);
			MTDCPU_CAN3_MCP2515_Write_CAN_Message(TxBufNum, ptx_msg);
				next++;
				if(next >= TX_BUF_LEN) next = 0;
				can3tx_read = next;	
				
				//W H H  ，使用全局变量来代替信号量
				g_can3TxCount--;
			if (timeout != 0xFF)			//获得邮箱成功,可以发送数据
			{	
				
				//debugprintf("can3,ok!\r\n");
			}
			else					//表示还没有可以发送的邮箱
			{
				 printf("can3 no mailbox !\r\n");
				//发送信号量
				//OSSemPost(&can3tran_sem,OS_OPT_POST_1,&err);
			}
		
			OSMutexPost(&CAN3_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量
		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}


/*can3口 回调函数的定义*/

/*DI硬线采集回调函数定义*/
UNS32 DICallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
		//			can1_DI_MC1[0] = can3_RxMFD1DI[0];
		//			can1_DI_MC1[1] = (can3_RxMFD1DI[1] | ((can3_RxMFD2DI[0] &0xC0)>>6));		//取出高2位
		//			can1_DI_MC1[2] = ((can3_RxMFD2DI[0] &0x3E)<<2) | ((can3_RxMFD3DI[0] & 0xE0)>>5);
		//			can1_DI_MC1[3] = (can3_RxMFD3DI[0]  & 0x10)>>4;
		//		
		//			can2_DI_MC1[0] = can3_RxMFD1DI[0];
		//			can2_DI_MC1[1] = (can3_RxMFD1DI[1] | ((can3_RxMFD2DI[0] &0xC0)>>6));		//取出高2位
		//			can2_DI_MC1[2] = ((can3_RxMFD2DI[0] &0x3E)<<2) | ((can3_RxMFD3DI[0] & 0xE0)>>5);
	
	
//	can2_DI_MC1[3] = (can3_RxMFD3DI[0]  & 0x10)>>4;		
//	printf("aaa\r\n");
//	printf("indx=0x%x\r\n",unsused_indextable->index);
	
//	u8 i=0;
//  printf("di=0x%x\r\n",unsused_indextable->index);
//if(d== &MTDApp1CAN1Slave_Data)
//{

//}

//else
//{
//		printf("CAN1\r\n");
//	for(i=0;i<6;i++)
//	{
//		printf("0x%x ",can1_DI_MC1[i]);
//	}
//	printf("\r\n");
//	
//	printf("CAN2\r\n");
//	for(i=0;i<6;i++)
//	{
//		printf("0x%x ",can2_DI_MC1[i]);
//	}
//	printf("\r\n");
//	
//	printf("CAN3\r\n");
//	for(i=0;i<2;i++)
//	{
//		printf("0x%x ",can3_RxMFD2DI[i]);
//	}
//	printf("\r\n");
//	
//}
	
	switch(unsused_indextable->index)		//根据映射地址 来区分那个MFD板卡发来的DI数据
	{
		case 0x200B:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					can1_DI_MC1[0] = can3_RxMFD1DI[0];
					can1_DI_MC1[1] = can3_RxMFD1DI[1]; 
				
					can2_DI_MC1[0] = can3_RxMFD1DI[0];
					can2_DI_MC1[1] = can3_RxMFD1DI[1]; 
				
					DI_VVVFPowertx_B1_10 = (can3_RxMFD1DI[1]>>4) & 0x01;  //MC1车VVVF电源有效
				
//					 /* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[0].last_message.cob_id = 0;
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,0);		//pdo 0x183			
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,0);		//pdo号0
				break;
				
				case MC2_MTD_NODEID:
					can1_DI_MC2[0] = can3_RxMFD1DI[0];
					can1_DI_MC2[1] = can3_RxMFD1DI[1]; 
				
					can2_DI_MC2[0] = can3_RxMFD1DI[0];
					can2_DI_MC2[1] = can3_RxMFD1DI[1]; 
				
					DI_VVVFPowertx_B1_60 = (can3_RxMFD1DI[1]>>4) & 0x01;  //MC2车VVVF电源有效
				
//					 /* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[2].last_message.cob_id = 0;			
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,2);		//pdo 0x188				
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[2].last_message.cob_id = 0;	
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,2);		//pdo号2		

				break;
				
				case M_MTD_NODEID:
					/*数据赋值*/
					can1_DI_M = can3_RxMFD1DI[0];
				
					can2_DI_M = can3_RxMFD1DI[0];
		
//					 /* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[1].last_message.cob_id = 0;			
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,1);		//pdo 0x185
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[1].last_message.cob_id = 0;					
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,1);		//pdo号1				
				break;
			}
			break;
			
		case 0x200C:
			
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					can1_DI_MC1[2] = can3_RxMFD2DI[0];
				
					can2_DI_MC1[2] = can3_RxMFD2DI[0];
				
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[0].last_message.cob_id = 0;
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,0);		//pdo 0x183
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;						
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,0);		//pdo号0				
				break;
				
				case MC2_MTD_NODEID:
					can1_DI_MC2[2] = can3_RxMFD2DI[0];
				
					can2_DI_MC2[2] = can3_RxMFD2DI[0];	
				
//					 /* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[2].last_message.cob_id = 0;
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,2);		//pdo 0x188
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[2].last_message.cob_id = 0;		
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,2);		//pdo号2				
				break;
							
			}				
			break;
		case 0x200D:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:		
					can1_DI_MC1[3] = can3_RxMFD3DI[0];
				
					can2_DI_MC1[3] = can3_RxMFD3DI[0];
				
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[0].last_message.cob_id = 0;						
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,0);		//pdo号0	
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;						
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,0);		//pdo号0					
					break;
				
				case MC2_MTD_NODEID:				
					can1_DI_MC2[3] = can3_RxMFD3DI[0];
				
					can2_DI_MC2[3] = can3_RxMFD3DI[0];
				
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN1Slave_Data.PDO_status[2].last_message.cob_id = 0;				
//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,2);		//pdo 0x188				
//					/* force emission of PDO by artificially changing last emitted */
//					MTDApp1CAN2Slave_Data.PDO_status[2].last_message.cob_id = 0;	
//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,2);		//pdo号2			
					break;
			}
			break;
	}

	return 0;
}


/*
	can3 门接受到数据的回调函数
*/
UNS32 DoorCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case	0x200E:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_Door_MC1_Odd,can3_RxDoorSta1,8);		//1,3,5,7 门的数据
					memcpy(can2_Door_MC1_Odd,can3_RxDoorSta1,8);	
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[4].last_message.cob_id = 0;	
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,4);		//pdo 0x18C
					/* force emission of PDO by artificially changing last emitted */	
					MTDApp1CAN2Slave_Data.PDO_status[4].last_message.cob_id = 0;	
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,4);		//pdo号4
				
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_Door_M_Odd,can3_RxDoorSta1,8);		//1,3,5,7 门的数据
					memcpy(can2_Door_M_Odd,can3_RxDoorSta1,8);	
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[5].last_message.cob_id = 0;
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,5);		//pdo 0x18E
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[5].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,5);		//pdo号5
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_Door_MC2_Odd,can3_RxDoorSta1,8);		//1,3,5,7 门的数据
					memcpy(can2_Door_MC2_Odd,can3_RxDoorSta1,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[6].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,6);		//pdo 0x191
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[6].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,6);		//pdo号6
					break;
			}
		break;
			
		case 0x200F:	
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_Door_MC1_Even,can3_RxDoorSta2,8);		//2,4,6,8 门的数据
					memcpy(can2_Door_MC1_Even,can3_RxDoorSta2,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[67].last_message.cob_id = 0;	
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,67);		//pdo 0x1F2
					/* force emission of PDO by artificially changing last emitted */	
					MTDApp1CAN2Slave_Data.PDO_status[67].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,67);		//pdo号67
				
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_Door_M_Even,can3_RxDoorSta2,8);		//2,4,6,8  门的数据
					memcpy(can2_Door_M_Even,can3_RxDoorSta2,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[68].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,68);		//pdo 0x1F4
					/* force emission of PDO by artificially changing last emitted */	
					MTDApp1CAN2Slave_Data.PDO_status[68].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,68);		//pdo号68
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_Door_MC2_Even,can3_RxDoorSta2,8);		//2,4,6,8  门的数据
					memcpy(can2_Door_MC2_Even,can3_RxDoorSta2,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[69].last_message.cob_id = 0;						
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,69);		//pdo 0x1F7
					/* force emission of PDO by artificially changing last emitted */	
					MTDApp1CAN2Slave_Data.PDO_status[69].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,69);		//pdo号69
					break;
			}			
			break;
			
		default :
			printf("Door APP1 Call Board Err!\r\n");
			break;
	}
	return 0;
}


/*can3 接受空间数据的回调函数*/
UNS32 ACCCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case	0x2010:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ACC_MC1,can3_RxAccSta,8);		
					memcpy(can2_ACC_MC1,can3_RxAccSta,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[7].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,7);		//pdo 0x192
					/* force emission of PDO by artificially changing last emitted */	
					MTDApp1CAN2Slave_Data.PDO_status[7].last_message.cob_id = 0;						
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,7);		//pdo号7
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_ACC_M,can3_RxAccSta,8);		
					memcpy(can2_ACC_M,can3_RxAccSta,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[8].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,8);		//pdo 0x194
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[8].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,8);		//pdo号8
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ACC_MC2,can3_RxAccSta,8);		
					memcpy(can2_ACC_MC2,can3_RxAccSta,8);	

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[9].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,9);		//pdo 0x197
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[9].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,9);		//pdo号9
					break;
			
				default:
					printf("ACC APP1 Err!\r\n");
					break;
			}			
			break;
			
		case 0x2028:
				switch(ChooseBoard)
				{
					case MC1_MTD_NODEID:
						memcpy(&can1_ACC_MC1_2,can3_RxAccSta2,1);		
						memcpy(&can2_ACC_MC1_2,can3_RxAccSta2,1);	

						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[64].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,64);		//pdo 0x1E6
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[64].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,64);		//pdo号64
						break;
					
					case M_MTD_NODEID:
						memcpy(&can1_ACC_M_2,can3_RxAccSta2,1);		
						memcpy(&can2_ACC_M_2,can3_RxAccSta2,1);	

						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[65].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,65);		//pdo 0x1E8
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[65].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,65);		//pdo号65
						break;
					
					case MC2_MTD_NODEID:
						memcpy(&can1_ACC_MC2_2,can3_RxAccSta2,1);		
						memcpy(&can2_ACC_MC2_2,can3_RxAccSta2,1);	
					
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[66].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,66);		//pdo 0x1EB
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[66].last_message.cob_id = 0;				
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,66);		//pdo号66
						break;			

					default:
						printf("ACC APP1 Err!\r\n");
						break;
				}
				break;
				
		default:
				printf("ACC Err %#x 映射号!\r\n",unsused_indextable->index);
				break;
	}
	return 0;
}

/*can3 接受BCU数据的回调函数*/
UNS32 BCUCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case 0x2011:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_BCU_MC1_1,can3_RxBCUSta1,8);
					memcpy(can2_BCU_MC1_1,can3_RxBCUSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[10].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,10);		//pdo 0x198		11
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[10].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,10);		//pdo号10
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_BCU_M_1,can3_RxBCUSta1,8);
					memcpy(can2_BCU_M_1,can3_RxBCUSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[13].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,13);		//pdo 0x19E		14
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[13].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,13);		//pdo号13					
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_BCU_MC2_1,can3_RxBCUSta1,8);
					memcpy(can2_BCU_MC2_1,can3_RxBCUSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[16].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,16);		//pdo 0x1A7		17
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[16].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,16);		//pdo号16					
					break;
			}
			break;
		
		case 0x2012:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_BCU_MC1_2,can3_RxBCUSta2,8);
					memcpy(can2_BCU_MC1_2,can3_RxBCUSta2,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[11].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,11);		//pdo 0x199		12
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[11].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,11);		//pdo号11
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_BCU_M_2,can3_RxBCUSta2,8);
					memcpy(can2_BCU_M_2,can3_RxBCUSta2,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[14].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,14);		//pdo 0x19F		15
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[14].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,14);		//pdo号14					
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_BCU_MC2_2,can3_RxBCUSta2,8);
					memcpy(can2_BCU_MC2_2,can3_RxBCUSta2,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[17].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,17);		//pdo 0x1A8		18
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[17].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,17);		//pdo号17					
					break;
			}			
			break;
			
		case 0x2013:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_BCU_MC1_3,can3_RxBCUSta3,8);
					memcpy(can2_BCU_MC1_3,can3_RxBCUSta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[12].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,12);		//pdo 0x19A		13
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[12].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,12);		//pdo号12
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_BCU_M_3,can3_RxBCUSta3,8);
					memcpy(can2_BCU_M_3,can3_RxBCUSta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[15].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,15);		//pdo 0x1A0		16
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[15].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,15);		//pdo号15					
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_BCU_MC2_3,can3_RxBCUSta3,8);
					memcpy(can2_BCU_MC2_3,can3_RxBCUSta3,8);
		
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[18].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,18);		//pdo 0x1A9		19
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[18].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,18);		//pdo号18					
					break;
			}				
			break;
			
		default:
				printf("BCU APP1 映射Err!\r\n");
			break;
	}
	return 0;
}


/*can3 接受ATC数据的回调函数*/
UNS32 ATCCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case 0x2014:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ATC_MC1_1,can3_RxATCSta1,7);
					memcpy(can2_ATC_MC1_1,can3_RxATCSta1,7);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[19].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,19);		//pdo 0x1AA
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[19].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,19);		//pdo号19				
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ATC_MC2_1,can3_RxATCSta1,7);
					memcpy(can2_ATC_MC2_1,can3_RxATCSta1,7);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[23].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,23);		//pdo 0x1AE
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[23].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,23);		//pdo号23						
					break;
						
				default:
							printf("ATC APP1 Call 板卡错误!\r\n");
						break;		
			}
			break;

		case 0x2015:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ATC_MC1_2,can3_RxATCSta2,8);
					memcpy(can2_ATC_MC1_2,can3_RxATCSta2,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[20].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,20);		//pdo 0x1AB
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[20].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,20);		//pdo号20		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ATC_MC2_2,can3_RxATCSta2,8);
					memcpy(can2_ATC_MC2_2,can3_RxATCSta2,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[24].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,24);		//pdo 0x1AF
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[24].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,24);		//pdo号24							
					break;
						
				default:
							//printf("ATC APP1 Call 板卡错误!\r\n");
						break;		
			}
			break;

		case 0x2016:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ATC_MC1_3,can3_RxATCSta3,8);
					memcpy(can2_ATC_MC1_3,can3_RxATCSta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[21].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,21);		//pdo 0x1AC
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[21].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,21);		//pdo号21		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ATC_MC2_3,can3_RxATCSta3,8);
					memcpy(can2_ATC_MC2_3,can3_RxATCSta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[25].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,25);		//pdo 0x1B0
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[25].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,25);		//pdo号25						
					break;
						
				default:
							//printf("ATC APP1 Call 板卡错误!\r\n");
						break;		
			}
			break;

		case 0x2017:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ATC_MC1_4,can3_RxATCSta4,4);
					memcpy(can2_ATC_MC1_4,can3_RxATCSta4,4);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[22].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,22);		//pdo 0x1AD
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[22].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,22);		//pdo号22	
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ATC_MC2_4,can3_RxATCSta4,4);
					memcpy(can2_ATC_MC2_4,can3_RxATCSta4,4);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[26].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,26);		//pdo 0x1B0
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[26].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,26);		//pdo号26								
					break;
						
				default:
							//printf("ATC APP1 Call 板卡错误!\r\n");
						break;		
			}
			break;
			
		case 0x2029:		//ATC时间	
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_TxMC1ATO_Time,can3_RxATCTime,6);
					memcpy(can2_TxMC1ATO_Time,can3_RxATCTime,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[3].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,3);		//pdo 0x18A  3
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[3].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,3);		//pdo号26	
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_TxMC2ATO_Time,can3_RxATCTime,6);
					memcpy(can2_TxMC2ATO_Time,can3_RxATCTime,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[70].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,70);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[70].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,70);		//pdo号26					
					break;
						
				default:
							//printf("ATC APP1 Call 板卡错误!\r\n");
						break;				
			}
			break;
			
		default:
				printf("ATC Err 映射号!\r\n");
				break;
					
	}
	
	return 0;
}


/*ATC时间回调函数*/
UNS32 ATCTimeCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(ChooseBoard)
	{
		case MC1_MTD_NODEID:
			memcpy(can1_TxMC1ATO_Time,can3_RxATCTime,6);
			memcpy(can2_TxMC1ATO_Time,can3_RxATCTime,6);
		
			MTDApp1CAN1Slave_Data.PDO_status[3].last_message.cob_id = 0;
			sendOnePDOevent(&MTDApp1CAN1Slave_Data,3);		//pdo 0x18a
			MTDApp1CAN2Slave_Data.PDO_status[3].last_message.cob_id = 0;
			sendOnePDOevent(&MTDApp1CAN2Slave_Data,3);		//pdo 0x18a
		break;
		
		case MC2_MTD_NODEID:
			memcpy(can1_TxMC2ATO_Time,can3_RxATCTime,6);
			memcpy(can2_TxMC2ATO_Time,can3_RxATCTime,6);
		
			MTDApp1CAN1Slave_Data.PDO_status[70].last_message.cob_id = 0;
			sendOnePDOevent(&MTDApp1CAN1Slave_Data,70);		//pdo 0x1F8
			MTDApp1CAN2Slave_Data.PDO_status[70].last_message.cob_id = 0;
			sendOnePDOevent(&MTDApp1CAN2Slave_Data,70);		//pdo 0x1F8
		break;
		
		default :
			printf("ATCTime APP1 ChooseBoard Err!\r\n");
			break;
		
	}
	return 0;
}

/*PIS回调函数*/
UNS32 PISCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case 0x2018:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_PIS_MC1_1,can3_RxPISSta1,8);
					memcpy(can2_PIS_MC1_1,can3_RxPISSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[27].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,27);		//pdo 0x1B2  28
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[27].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,27);		//pdo号27	
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_PIS_MC2_1,can3_RxPISSta1,8);
					memcpy(can2_PIS_MC2_1,can3_RxPISSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[29].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,29);		//pdo 0x1B4  30
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[29].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,29);		//pdo号29					
					break;
						
				default:
							printf("PIS APP1 Call 板卡错误!\r\n");
						break;				
			}
			break;
		
		case 0x2019:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_PIS_MC1_2,can3_RxPISSta2,3);
					memcpy(can2_PIS_MC1_2,can3_RxPISSta2,3);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[28].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,28);		//pdo 0x1B3  29
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[28].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,28);		//pdo号28	
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_PIS_MC2_2,can3_RxPISSta2,3);
					memcpy(can2_PIS_MC2_2,can3_RxPISSta2,3);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[30].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,30);		//pdo 0x1B4  31
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[30].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,30);		//pdo号30				
					break;
						
				default:
							printf("PIS APP1 Call 板卡错误!\r\n");
						break;				
			}			
			break;
		
		default:
			printf("PIS APP1 映射Err!\r\n");
			break;
	}		
	return 0;
}

/*ACP 回调函数*/
UNS32 ACPCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case 0x202D:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ACP_MC1_1,can3_RxACPSta1,8);
					memcpy(can2_ACP_MC1_1,can3_RxACPSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[72].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,72);		//PDO 0x1FA   73
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[72].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,72);		//pdo号72
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ACP_MC2_1,can3_RxACPSta1,8);
					memcpy(can2_ACP_MC2_1,can3_RxACPSta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[74].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,74);		//PDO 0x1FC   75
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[74].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,74);		//pdo号74					
					break;
				
				default:
					break;
			}
			break;
		
		case 0x202E:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_ACP_MC1_2,can3_RxACPSta2,3);
					memcpy(can2_ACP_MC1_2,can3_RxACPSta2,3);
	
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[73].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,73);		//PDO 0x1FB   74
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[73].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,73);		//pdo号73
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_ACP_MC2_2,can3_RxACPSta2,3);
					memcpy(can2_ACP_MC2_2,can3_RxACPSta2,3);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[75].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,75);		//PDO 0x1FD   76
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[75].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,75);		//pdo号75					
					break;
				
				default:
					break;
			}			
			break;
		
		default:
			printf("ACP APP1 映射Err!\r\n");
			break;
	}
	return 0;
}

/*VVVF回调函数*/
UNS32 VVVF1CallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	//printf("VVVF call bcak =0x%x!\r\n",unsused_indextable->index);
	switch(unsused_indextable->index)
	{
		case 0x201A:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF1_MC1_1,can3_RxVVVF1Sta1,8);
					memcpy(can2_VVVF1_MC1_1,can3_RxVVVF1Sta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[31].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,31);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[31].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,31);		//pdo号26		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF1_M_1,can3_RxVVVF1Sta1,8);
					memcpy(can2_VVVF1_M_1,can3_RxVVVF1Sta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[41].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,41);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[41].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,41);		//pdo号26		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF1_MC2_1,can3_RxVVVF1Sta1,8);
					memcpy(can2_VVVF1_MC2_1,can3_RxVVVF1Sta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[51].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,51);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[51].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,51);		//pdo号26							
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}
			break;
		
		case 0x201B:
			//printf("VHb=%d\r\n",can3_RxVVVF1Sta2[0]);
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF1_MC1_2,can3_RxVVVF1Sta2,6);
					memcpy(can2_VVVF1_MC1_2,can3_RxVVVF1Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[32].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,32);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[32].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,32);		//pdo号26								
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF1_M_2,can3_RxVVVF1Sta2,6);
					memcpy(can2_VVVF1_M_2,can3_RxVVVF1Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[42].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,42);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[42].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,42);		//pdo号26								
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF1_MC2_2,can3_RxVVVF1Sta2,6);
					memcpy(can2_VVVF1_MC2_2,can3_RxVVVF1Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[52].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,52);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[52].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,52);		//pdo号26								
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}
			break;
		
		case 0x201C:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF1_MC1_3,can3_RxVVVF1Sta3,8);
					memcpy(can2_VVVF1_MC1_3,can3_RxVVVF1Sta3,8);
	
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[33].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,33);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[33].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,33);		//pdo号26		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF1_M_3,can3_RxVVVF1Sta3,8);
					memcpy(can2_VVVF1_M_3,can3_RxVVVF1Sta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[43].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,43);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[43].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,43);		//pdo号26		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF1_MC2_3,can3_RxVVVF1Sta3,8);
					memcpy(can2_VVVF1_MC2_3,can3_RxVVVF1Sta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[53].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,53);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[53].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,53);		//pdo号26							
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}			
			break;
		
		case 0x201D:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF1_MC1_4,can3_RxVVVF1Sta4,8);
					memcpy(can2_VVVF1_MC1_4,can3_RxVVVF1Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[34].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,34);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[34].last_message.cob_id = 0;
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,34);		//pdo号26		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF1_M_4,can3_RxVVVF1Sta4,8);
					memcpy(can2_VVVF1_M_4,can3_RxVVVF1Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[44].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,44);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[44].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,44);		//pdo号26		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF1_MC2_4,can3_RxVVVF1Sta4,8);
					memcpy(can2_VVVF1_MC2_4,can3_RxVVVF1Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[54].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,54);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[54].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,54);		//pdo号26							
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}			
			break;
		
		case 0x201E:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF1_MC1_5,can3_RxVVVF1Sta5,6);
					memcpy(can2_VVVF1_MC1_5,can3_RxVVVF1Sta5,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[35].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,35);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[35].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,35);		//pdo号26								
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF1_M_5,can3_RxVVVF1Sta5,6);
					memcpy(can2_VVVF1_M_5,can3_RxVVVF1Sta5,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[45].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,45);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[45].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,45);		//pdo号26								
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF1_MC2_5,can3_RxVVVF1Sta5,6);
					memcpy(can2_VVVF1_MC2_5,can3_RxVVVF1Sta5,6);
	
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[55].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,55);		//pdo号1F8  	70
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[55].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,55);		//pdo号26								
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}			
			break;
		
		default:
			printf("VVVF1 映射 Err！\r\n");
			break;
	}
	return 0;
}

/*VVVF2 回调函数*/
UNS32 VVVF2CallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	//printf("VVVF2 call bcak =0x%x!\r\n",unsused_indextable->index);
	switch(unsused_indextable->index)
	{
		case 0x201F:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF2_MC1_1,can3_RxVVVF2Sta1,8);
					memcpy(can2_VVVF2_MC1_1,can3_RxVVVF2Sta1,8);
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[36].last_message.cob_id = 0;
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,36);		//pdo号1BB  	37
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[36].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,36);		//pdo号36		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF2_M_1,can3_RxVVVF2Sta1,8);
					memcpy(can2_VVVF2_M_1,can3_RxVVVF2Sta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[46].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,46);		//pdo号1C5 	 47
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[46].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,46);		//pdo号46		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF2_MC2_1,can3_RxVVVF2Sta1,8);
					memcpy(can2_VVVF2_MC2_1,can3_RxVVVF2Sta1,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[56].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,56);		//pdo号1CF  	57
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[56].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,56);		//pdo号56							
					break;
				
				default:
					printf("VVVF2 Board Err!\r\n");
					break;
			}
			break;
		
		case 0x2020:
			//printf("VHb=%d\r\n",can3_RxVVVF2Sta2[0]);
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF2_MC1_2,can3_RxVVVF2Sta2,6);
					memcpy(can2_VVVF2_MC1_2,can3_RxVVVF2Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[37].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,37);		//pdo号1BC  	38
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[37].last_message.cob_id = 0;
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,37);		//pdo号37								
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF2_M_2,can3_RxVVVF2Sta2,6);
					memcpy(can2_VVVF2_M_2,can3_RxVVVF2Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[47].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,47);		//pdo号1C6  	48
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[47].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,47);		//pdo号47								
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF2_MC2_2,can3_RxVVVF2Sta2,6);
					memcpy(can2_VVVF2_MC2_2,can3_RxVVVF2Sta2,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[57].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,57);		//pdo号1D0  	58
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[57].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,57);		//pdo号57								
					break;
				
				default:
					printf("VVVF Board Err!\r\n");
					break;
			}
			break;
		
		case 0x2021:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF2_MC1_3,can3_RxVVVF2Sta3,8);
					memcpy(can2_VVVF2_MC1_3,can3_RxVVVF2Sta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[38].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,38);		//pdo号1BD  	39
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[38].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,38);		//pdo号38		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF2_M_3,can3_RxVVVF2Sta3,8);
					memcpy(can2_VVVF2_M_3,can3_RxVVVF2Sta3,8);
	
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[48].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,48);		//pdo号1C7  	49
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[48].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,48);		//pdo号48		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF2_MC2_3,can3_RxVVVF2Sta3,8);
					memcpy(can2_VVVF2_MC2_3,can3_RxVVVF2Sta3,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[58].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,58);		//pdo号1D1  	59
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[58].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,58);		//pdo号58						
					break;
				
				default:
					printf("VVVF2 Board Err!\r\n");
					break;
			}			
			break;
		
		case 0x2022:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF2_MC1_4,can3_RxVVVF2Sta4,8);
					memcpy(can2_VVVF2_MC1_4,can3_RxVVVF2Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[38].last_message.cob_id = 0;				
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,39);		//pdo号1BE  	40
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[39].last_message.cob_id = 0;						
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,39);		//pdo号39		
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF2_M_4,can3_RxVVVF2Sta4,8);
					memcpy(can2_VVVF2_M_4,can3_RxVVVF2Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[49].last_message.cob_id = 0;						
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,49);		//pdo号1C8  	50
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[49].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,49);		//pdo号49		
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF2_MC2_4,can3_RxVVVF2Sta4,8);
					memcpy(can2_VVVF2_MC2_4,can3_RxVVVF2Sta4,8);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[59].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,59);		//pdo号1D2  	60
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[59].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,59);		//pdo号59							
					break;
				
				default:
					printf("VVVF2 Board Err!\r\n");
					break;
			}			
			break;
		
		case 0x2023:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					memcpy(can1_VVVF2_MC1_5,can3_RxVVVF2Sta5,6);
					memcpy(can2_VVVF2_MC1_5,can3_RxVVVF2Sta5,6);
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[40].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,40);		//pdo号1BF  	41
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[40].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,40);		//pdo号40								
					break;
				
				case M_MTD_NODEID:
					memcpy(can1_VVVF2_M_5,can3_RxVVVF2Sta5,6);
					memcpy(can2_VVVF2_M_5,can3_RxVVVF2Sta5,6);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[50].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,50);		//pdo号1C9  	51
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[50].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,50);		//pdo号50								
					break;
				
				case MC2_MTD_NODEID:
					memcpy(can1_VVVF2_MC2_5,can3_RxVVVF2Sta5,6);
					memcpy(can2_VVVF2_MC2_5,can3_RxVVVF2Sta5,6);
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[60].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,60);		//pdo号1D3  	61
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[60].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,60);		//pdo号60								
					break;
				
				default:
					printf("VVVF2 Board Err!\r\n");
					break;
			}			
			break;
		
		default:
			printf("VVVF2 映射 Err！\r\n");
			break;
	}
	return 0;
}


/*子设备通信故障回调函数*/
UNS32 SubDevFaultCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	switch(unsused_indextable->index)
	{
		case 0x202A:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据,根据对象字典的定义，看Execl文档*/
					can1_MC1SubDevFault[0] = 0;
					can1_MC1SubDevFault[1] &= 0x0F;		// 清除前4位
					can2_MC1SubDevFault[0] = 0;
					can2_MC1SubDevFault[1] &= 0x0F;		//清除前4位

					can1_MC1SubDevFault[0] = can3_RxMFD1SubFault[0];	
					can1_MC1SubDevFault[1] |= (can3_RxMFD1SubFault[1]&0xF0);

					/*can2*/
					can2_MC1SubDevFault[0] = can3_RxMFD1SubFault[0];	//门
					can2_MC1SubDevFault[1] |= (can3_RxMFD1SubFault[1]&0xF0);
				
					MDCU_VVVF1ComFaultx_B1_10 = (can3_RxMFD1SubFault[1]>>5)&0x01; //VVVF1 通信状态
					MDCU_VVVF2ComFaultx_B1_10 = (can3_RxMFD1SubFault[1]>>4)&0x01; //VVVF2 通信状态
				
					MDCU_BCUComFaultrx_B1_10 = (can3_RxMFD1SubFault[1]>>6)&0x01; //BCU通信状态

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,61);		//pdo 0x1DA
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,61);		//pdo号61
					break;
				
				case M_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MSubDevFault[0] = 0;
					can1_MSubDevFault[1] &= 0x0F;		// 清除前4位
					can2_MSubDevFault[0] = 0;
					can2_MSubDevFault[1] &= 0x0F;		// 清除前4位
					
					can1_MSubDevFault[0] = can3_RxMFD1SubFault[0];	//门
					can1_MSubDevFault[1] |= (can3_RxMFD1SubFault[1]&0xF0);

					/*can2*/
					can2_MSubDevFault[0] = can3_RxMFD1SubFault[0];	//门
					can2_MSubDevFault[1] |= (can3_RxMFD1SubFault[1]&0xF0);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[62].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,62);		//pdo 0x1DC
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[62].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,62);		//pdo号62
					break;
				
				case MC2_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MC2SubDevFault[0] = 0;
					can1_MC2SubDevFault[1] &= 0x0F;
					can2_MC2SubDevFault[0] = 0;
					can2_MC2SubDevFault[1] &= 0x0F;
			
					can1_MC2SubDevFault[0] =can3_RxMFD1SubFault[0];	//门
					can1_MC2SubDevFault[1] |=(can3_RxMFD1SubFault[1]&0xF0);
				
					/*can2*/
					can2_MC2SubDevFault[0] = can3_RxMFD1SubFault[0];	//门
					can2_MC2SubDevFault[1] |=(can3_RxMFD1SubFault[1]&0xF0);

					MDCU_VVVF1ComFaultx_B1_60 = (can3_RxMFD1SubFault[1]>>5)&0x01; //VVVF1 通信状态
					MDCU_VVVF2ComFaultx_B1_60 = (can3_RxMFD1SubFault[1]>>4)&0x01; //VVVF2 通信状态
				
					MDCU_BCUComFaultrx_B1_60 = (can3_RxMFD1SubFault[1]>>6)&0x01; //BCU通信状态
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,63);		//pdo 0x1DF
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,63);		//pdo号63
					break;
				
				default:
					printf("Sub APP1 Call 板卡错误!\r\n");
					break;
			}
			break;
		
		case 0x202B:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MC1SubDevFault[1] &= 0xF5;		// 清除ATC  PIS
					can2_MC1SubDevFault[1] &= 0xF5;

					can1_MC1SubDevFault[1] |= (can3_RxMFD2SubFault[0]&0x0A);

					/*can2*/
					can2_MC1SubDevFault[1] |= (can3_RxMFD2SubFault[0]&0x0A);
				
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,61);		//pdo 0x1DA
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,61);		//pdo号61
					break;

				
				case MC2_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MC2SubDevFault[1] &= 0xF5;		// 清除ATC  PIS
					can2_MC2SubDevFault[1] &= 0xF5;

					can1_MC2SubDevFault[1] |= (can3_RxMFD2SubFault[0]&0x0A);

					/*can2*/
					can2_MC2SubDevFault[1] |= (can3_RxMFD2SubFault[0]&0x0A);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,63);		//pdo 0x1DF
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,63);		//pdo号63
					break;
				
				default:
					printf("Sub APP1 Call 板卡错误!\r\n");
					break;			
			}
			break;
		
		case 0x202C:
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MC1SubDevFault[1] &= 0xFB;		// 清除ACP
					can2_MC1SubDevFault[1] &= 0xFB;

					can1_MC1SubDevFault[1] |= (can3_RxMFD3SubFault[0]&0x04);

					/*can2*/
					can2_MC1SubDevFault[1] |= (can3_RxMFD3SubFault[0]&0x04);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,61);		//pdo 0x1DA
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[61].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,61);		//pdo号61
					break;

				
				case MC2_MTD_NODEID:
					/*can1*/
					/*清除之前故障数据*/
					can1_MC2SubDevFault[1] &= 0xFB;		// 清除ACP
					can2_MC2SubDevFault[1] &= 0xFB;

					can1_MC2SubDevFault[1] |= (can3_RxMFD3SubFault[0]&0x04);

					/*can2*/
					can2_MC2SubDevFault[1] |= (can3_RxMFD3SubFault[0]&0x04);

					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN1Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN1Slave_Data,63);		//pdo 0x1DF
					/* force emission of PDO by artificially changing last emitted */
					MTDApp1CAN2Slave_Data.PDO_status[63].last_message.cob_id = 0;					
					sendOnePDOevent(&MTDApp1CAN2Slave_Data,63);		//pdo号63
					break;
				
				default:
					printf("Sub APP1 Call 板卡错误!\r\n");
					break;						
			}
			break;
	}

	return 0;
}



s_BOARD CAN3MasterBoard = {"1", "500K"};
#include "iap.h"
/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(CAN3MasterBoard.baudrate, "none")){
		
		/*DI采集点的回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x200B,0x02,&DICallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x200C,0x01,&DICallBack);		//whh 回电函数错误，注意注意注意
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x200D,0x01,&DICallBack);

		/*子设备通信故障回调函数*/
			//RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2027,0x02,&SubDevFaultCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x202A,0x02,&SubDevFaultCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x202B,0x01,&SubDevFaultCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x202C,0x01,&SubDevFaultCallBack);
		
		/*门数据的回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x200E,0x08,&DoorCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x200F,0x08,&DoorCallBack);
		
		/*空调的回电函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2010,0x08,&ACCCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2028,0x01,&ACCCallBack);
		
		/*BCU的回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2011,0x08,&BCUCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2012,0x08,&BCUCallBack);			
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2013,0x08,&BCUCallBack);
			
		/*ATC回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2014,0x07,&ATCCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2015,0x08,&ATCCallBack);			
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2016,0x08,&ATCCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2017,0x04,&ATCCallBack);
			//RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2029,0x06,&ATCCallBack);
	
			/*ATC 时间回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2029,0x06,&ATCTimeCallBack);
						
			/*PIS 的回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2018,0x08,&PISCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2019,0x03,&PISCallBack);
			
			/*ACP 回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x202D,0x08,&ACPCallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x202E,0x03,&ACPCallBack);		
			
			/*VVVF1回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201A,0x08,&VVVF1CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201B,0x06,&VVVF1CallBack);			
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201C,0x08,&VVVF1CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201D,0x08,&VVVF1CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201E,0x06,&VVVF1CallBack);
			
			/*VVVF2回调函数*/
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x201F,0x08,&VVVF2CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2020,0x06,&VVVF2CallBack);			
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2021,0x08,&VVVF2CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2022,0x08,&VVVF2CallBack);
			RegisterSetODentryCallBack(&MTDCAN3Master_Data,0x2023,0x06,&VVVF2CallBack);			
			
		/* Defining the node Id */
		setNodeId(&MTDCAN3Master_Data, 0x08);			 	//设置自己的节点id为0x8
		/* init */
		setState(&MTDCAN3Master_Data, Initialisation);
		
		setState(&MTDCAN3Master_Data, Operational);  //can3将自己启动为操作状态
		IAP_Init(&MTDApp1CAN2Slave_Data,&MTDCAN3Master_Data,MTDCPU_CanAddVal(),0x2161207);
	}
}

static TimerCallback_t init_callback;

static void StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(NULL, 0, init_callback, 0, 0);
}

///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(CAN3MasterBoard.baudrate, "none")){
//		
//		masterSendNMTstateChange(&MTDCAN3Master_Data, 0x02, NMT_Reset_Node);    
//    
//   	//Stop master
//		setState(&MTDCAN3Master_Data, Stopped);
//	}
//}

/****************************************************************************/
/****************************  test_master  *********************************/
/****************************************************************************/
/*static*/ int test_can3master(void)
{
	if(strcmp(CAN3MasterBoard.baudrate, "none")){
		
		MTDCAN3Master_Data.canHandle = CAN3;			//W H H	,使用通道3
		
		MTDCAN3Master_Data.heartbeatError = CAN3Master_heartbeatError;
		MTDCAN3Master_Data.initialisation = CAN3Master_initialisation;
		MTDCAN3Master_Data.preOperational = CAN3Master_preOperational;
		MTDCAN3Master_Data.operational = CAN3Master_operational;
		MTDCAN3Master_Data.stopped = CAN3Master_stopped;
		MTDCAN3Master_Data.post_sync = CAN3Master_post_sync;
		MTDCAN3Master_Data.post_TPDO = CAN3Master_post_TPDO;
		MTDCAN3Master_Data.post_emcy = CAN3Master_post_emcy;
		MTDCAN3Master_Data.post_SlaveBootup=CAN3Master_post_SlaveBootup;
	}
	
	// Start timer thread
	StartTimerLoop(&InitNodes);

	return 0;
}
