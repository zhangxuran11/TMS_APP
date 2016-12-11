#include "can_stm32.h"
#include "canfestival.h"
#include "CAN3Master.h"
#include "CAN2Master.h"
#include "CAN3MasterSlave.h"
#include "MTD_CCU_CAN3_MCP2515.h"
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
	
	printf("can3init\r\n");
	canInit(CAN3,MCP2515_CAN_BAUD_500K);			//W H H ,主设备用的can3
	
	can3rx_save = 0;
	can3rx_read = 0;

	can3tx_save = 0;
	can3tx_read = 0;
	
	printf("test can3 master,start\r\n");
	test_can3master();		//调用主站测试函数
	printf("test can3 master,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
			//请求信号量 ，等待信号量
		OSSemPend(&can3recv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		if(err)
		{
			printf("can3Rx_err=0x%x\r\n",err);
		}
			else
				//if(g_can3RxCount>0 )
			{
				uint32_t next;
				Message *pmsg;

				//printf("sem3=%d\r\n",g_can3RxCount);
				
				//whh 9-13	 接受数据放入到缓冲区				
				{
					CanRxMsg RxMessage;
					Message *pmsg;
					uint32_t i, next;

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
						//					#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
						//						OSIntExit();    	//退出中断
						//					#endif
						//					return;
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
					}
					
					OSMutexPost(&CAN3_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量
				}
				
				
				
				next = can3rx_read;
				pmsg = &can3rx_msg_buf[next];
			
				/*W H H can1接受状态的改变*/  //can3,不需要监控其他设置的心跳
	//			{
	//				UNS8 nodeId = (UNS8) GET_NODE_ID((*pmsg));
	//				g_RxCAN1MesStateFlag[nodeId] = RxCanMesState;
	//			}
				
				
				//TIM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);		//关定时器中断
				/* Disable the Interrupt sources */
				TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
					canDispatch(&CCUCAN3Master_Data, pmsg);
				/* Enable the Interrupt sources */
				TIM4->DIER |= TIM_IT_CC1;
				//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);		//开定时器中断

				next++;
				if(next >= RX_BUF_LEN) next = 0;
				can3rx_read = next;
				
				//g_can3RxCount--;
				//printf("can3,rece,end\r\n");

			}
		//	 OSSched();		//W H  H 进行任务调度,会产生错误，调度太快，低优先级的任务无法执行到。无法使任务切换到低有限的任务
		//BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
  }
}

void can3master_send_thread(void *p_arg)
{
	OS_ERR err;
//	
//	//创建一个发送信号量
//	OSSemCreate ((OS_SEM*	)&can3tran_sem,
//                 (CPU_CHAR*	)"can3transem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
	
	while(1)
	{	
//		printf("can3 sendthread\r\n");
		//请求信号量 ，等待信号量
//		OSSemPend(&can3tran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
//		if(err)
//		{
//			printf("can3Tx_err=%d\r\n",err);
//		}
	
		//W H H  ，使用全局变量来代替信号量
			if(g_can3TxCount >0)
			{
				uint32_t next;
				uint8_t  mailbox_no,TxBufNum;
				CanTxMsg *ptx_msg;
				
				u8 timeout=0;
				next = can3tx_read;
				ptx_msg = &can3tx_msg_buf[next];
			
				//	debugprintf("can3 whh11\r\n");
				OSMutexPend (&CAN3_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
				do{
							mailbox_no = MTDCCU_CAN3_MCP2515_Get_Free_TxBuf(&TxBufNum); // info = addr.
							timeout++;
							if (timeout == 0xFF)
							{		
//								printf(" 3 no mailbox!\r\n");//ZXR
								break;
							}
				} while (mailbox_no == MCP2515_ALLTXBUSY);
				
				if (timeout != 0xFF)			//获得邮箱成功,可以发送数据
				{	
					//debugprintf("can3 whh22\r\n");
					MTDCCU_CAN3_MCP2515_Write_CAN_Message(TxBufNum, ptx_msg);
					//debugprintf("can3 whh33\r\n");
					next++;
					if(next >= TX_BUF_LEN) next = 0;
					can3tx_read = next;	
					
					//W H H  ，使用全局变量来代替信号量
					g_can3TxCount--;
					//debugprintf("can3,ok!\r\n");
				}
				else					//表示还没有可以发送的邮箱
				{
					 //printf("can3 no mailbox !\r\n");//ZXR
					//发送信号量
					//OSSemPost(&can3tran_sem,OS_OPT_POST_1,&err);
				}
			
				OSMutexPost(&CAN3_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量
			}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}


s_BOARD CAN3MasterBoard = {"1", "500K"};
/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(CAN3MasterBoard.baudrate, "none")){
// 		RegisterSetODentryCallBack(&CCUCAN3Master_Data, 0x2000, 0, &OnMasterMap1Update);
// 		RegisterSetODentryCallBack(&CCUCAN3Master_Data, 0x1016, 0x01, &OnMasterGetHerat);
		
		/* Defining the node Id */
		setNodeId(&CCUCAN3Master_Data, 0x08);
		/* init */
		setState(&CCUCAN3Master_Data, Initialisation);
		
		setState(&CCUCAN3Master_Data, Operational);  //can3将自己启动为操作状态
		IAP_Init(&CCUCAN2Master_Data,&CCUCAN3Master_Data,MTDCCU_CanAddVal(),0x161207);
	}
}

static TimerCallback_t init_callback;

static void StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(&CCUCAN3Master_Data, 0, init_callback, 0, 0);
}

///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(CAN3MasterBoard.baudrate, "none")){
//		
//		masterSendNMTstateChange(&CCUCAN3Master_Data, 0x02, NMT_Reset_Node);    
//    
//   	//Stop master
//		setState(&CCUCAN3Master_Data, Stopped);
//	}
//}

/****************************************************************************/
/****************************  test_master  *********************************/
/****************************************************************************/
/*static*/ int test_can3master(void)
{
	if(strcmp(CAN3MasterBoard.baudrate, "none")){
		
		CCUCAN3Master_Data.canHandle = CAN3;			//W H H	,使用通道3
		
		CCUCAN3Master_Data.heartbeatError = CAN3Master_heartbeatError;
		CCUCAN3Master_Data.initialisation = CAN3Master_initialisation;
		CCUCAN3Master_Data.preOperational = CAN3Master_preOperational;
		CCUCAN3Master_Data.operational = CAN3Master_operational;
		CCUCAN3Master_Data.stopped = CAN3Master_stopped;
		CCUCAN3Master_Data.post_sync = CAN3Master_post_sync;
		CCUCAN3Master_Data.post_TPDO = CAN3Master_post_TPDO;
		CCUCAN3Master_Data.post_emcy = CAN3Master_post_emcy;
		CCUCAN3Master_Data.post_SlaveBootup=CAN3Master_post_SlaveBootup;
	}
	
	// Start timer thread
	StartTimerLoop(&InitNodes);

	return 0;
}
