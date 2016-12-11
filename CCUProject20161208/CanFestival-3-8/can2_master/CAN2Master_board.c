#include "can_stm32.h"
#include "canfestival.h"
#include "CAN2Master.h"
#include "CAN2MasterSlave.h"

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

extern void TIM4_start(void);
static int  test_can2master(void);
extern u32 g_SlaveTxCount;

void can2master_recv_thread(void* p_arg)
{
	//	OS_ERR err;
	//	//printf("master test!\r\n");
	//	//创建一个接受信号量
	//	OSSemCreate ((OS_SEM*	)&can2recv_sem,
	//                 (CPU_CHAR*	)"can2recvsem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);

	printf("can2init\r\n");
	canInit(CAN2,CAN_BAUD_125K);			//W H H ,主设备用的can1

	can2rx_save = 0;
	can2rx_read = 0;

	can2tx_save = 0;
	can2tx_read = 0;
	
	
	/*设置 默认 使用ccu1设备,通道1开启管理功能，否则不开启管理功能 */
//	if(CCU1 == 1)
//	{

		CCU1SwitchState = ChooseOperate;			//ccu1使用
		CCU2SwitchState = ChooseNotOperate;
		CurrentUseCAN1 = UseState;					 //使用can1总线显示状态
		CurrentUseCAN2 = NoUseState;				//不使用ccu2显示总线状态
		
//	}
//	else
//	{
//		CCU2SwitchState = ChooseNotOperate;			//ccu1使用
//		CurrentUseCAN2 =  NoUseState;						//不使用can2总线显示状态
//	}
//	

	printf("testmaster,start\r\n");
	test_can2master();		//调用主站测试函数
	printf("testmaster,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
			//		//请求信号量 ，等待信号量
			//		OSSemPend(&can2recv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
			//		if(err)
			//		{
			//			printf("can2Rx_err=%d\r\n",err);
			//		}
		
			if(g_can2RxCount>0)
			{
				uint32_t next;
				Message *pmsg;
				
				next = can2rx_read;
				pmsg = &can2rx_msg_buf[next];
				
				/*W H H can1接受状态的改变*/
				{
					UNS8 nodeId = (UNS8) GET_NODE_ID((*pmsg));
					g_RxCAN2MesStateFlag[nodeId] = RxCanMesState;
					
					CAN2_Heart = RxHeart; //表示can2口能接受到其他设备的消息，则表示can2口正常，有心跳
				}
				
				//TIM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);		//关定时器中断
				/* Disable the Interrupt sources */
				TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
					canDispatch(&CCUCAN2Master_Data, pmsg);
				/* Enable the Interrupt sources */
				TIM4->DIER |= TIM_IT_CC1;
				//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);		//开定时器中断

				next++;
				if(next >= RX_BUF_LEN) next = 0;
				can2rx_read = next;
				
				g_can2RxCount--;
				//				printf("can2,rece,end\r\n");
			}
		//	 OSSched();		//W H  H 进行任务调度,会产生错误，调度太快，低优先级的任务无法执行到。无法使任务切换到低有限的任务
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
  }
}

void can2master_send_thread(void *p_arg)
{
	//	OS_ERR err;
	//	
	//	//创建一个发送信号量
	//	OSSemCreate ((OS_SEM*	)&can2tran_sem,
	//                 (CPU_CHAR*	)"can2transem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);
	
	while(1)
	{
		//请求信号量 ，等待信号量
		//		OSSemPend(&can2tran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		//		if(err)
		//		{
		//			printf("can2Tx_err=%d\r\n",err);
		//		}		
		//		printf("can2 send\r\n");

			if(g_can2TxCount>0)
			{
				uint32_t next;
				uint8_t  mailbox_no;
				CanTxMsg *ptx_msg;
				
				next = can2tx_read;
				ptx_msg = &can2tx_msg_buf[next];

				mailbox_no = CAN_Transmit(CAN2, ptx_msg);
				
				if(mailbox_no != CAN_NO_MB)
				{
					next++;
					if(next >= TX_BUF_LEN) next = 0;
					can2tx_read = next;		

					g_can2TxCount--;
					//					printf("can2,ok!\r\n");
				}
				else
				{
						printf("can2 no mailbox !\r\n");
					//发送信号量
   				//	OSSemPost(&can2tran_sem,OS_OPT_POST_1,&err);
				}
			}

		//		OSSched();		//W H  H 进行任务调度
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}





//UNS32 OnMasterMap1Update(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
//{
//	
//	UNS32 ConsumerHeartTime = 0x0002012C;
//	UNS32 size = sizeof(UNS32); 
//	
////	eprintf("OnMasterMap1Update:%lld\r\n",Masterdoor_mc1_1);
//		eprintf("Master: %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx \r\n",
//	Masterdoor_mc1_1,Masterdoor_mc1_2,Masterdoor_T_1,Masterdoor_T_2,Masterdoor_M_1,Masterdoor_M_2,Masterdoor_T1_1,
//	Masterdoor_T1_2,Masterdoor_T2_1,Masterdoor_T2_2,Masterdoor_mc2_1,Masterdoor_mc2_2);
//	
////			writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
////			0x1016, /*UNS16 index*/
////			0x01, /*UNS8 subind*/ 
////			&ConsumerHeartTime, /*void * pSourceData,*/ 
////			&size, /* UNS8 * pExpectedSize*/
////			RW);  /* UNS8 checkAccess */
//	return 0;
//}



UNS32  can2MasterGetHerat(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
//		if(CCUcan2RxFlag == 1)
//		{
//			if(CCU1 == CCU1_NODEID)
//			{
//				g_RxCAN2HeartStateFlag[2] = InOperatState;
//			}
//			
//			else
//			{
//				g_RxCAN2HeartStateFlag[1]= InOperatState;
//			}
//			
//		}
	return 0;
}

s_BOARD CAN2MasterBoard = {"1", "1M"};
/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(CAN2MasterBoard.baudrate, "none")){
// 		RegisterSetODentryCallBack(&CCUCAN2Master_Data, 0x2001, 0, &can2MasterGetHerat);
// 		RegisterSetODentryCallBack(&TestMaster_Data, 0x1016, 0x01, &OnMasterGetHerat);
//		
		if(CCU1 == 1)
		/* Defining the node Id */
			setNodeId(&CCUCAN2Master_Data, 0x01);
		else		//表示CCU2
			setNodeId(&CCUCAN2Master_Data, 0x02);	

		/* init */
		setState(&CCUCAN2Master_Data, Initialisation);
		
		setState(d, Operational);  //ccu1,can2将自己启动为操作状态	
	}
}

static TimerCallback_t init_callback;

static void StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(&CCUCAN2Master_Data, 0, init_callback, 0, 0);
}

///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(CAN2MasterBoard.baudrate, "none")){
//		
//		masterSendNMTstateChange(&CCUCAN2Master_Data, 0x00, NMT_Reset_Node);    
//    
//   	//Stop master
//		setState(&CCUCAN2Master_Data, Stopped);
//	}
//}

/****************************************************************************/
/****************************  test_master  *********************************/
/****************************************************************************/
static int test_can2master(void)
{
	if(strcmp(CAN2MasterBoard.baudrate, "none")){
		
		CCUCAN2Master_Data.canHandle = CAN2;		//W H H		选择通道2
		
		CCUCAN2Master_Data.heartbeatError = CAN2Master_heartbeatError;
		CCUCAN2Master_Data.initialisation = CAN2Master_initialisation;
		CCUCAN2Master_Data.preOperational = CAN2Master_preOperational;
		CCUCAN2Master_Data.operational = CAN2Master_operational;
		CCUCAN2Master_Data.stopped = CAN2Master_stopped;
		CCUCAN2Master_Data.post_sync = CAN2Master_post_sync;
		CCUCAN2Master_Data.post_TPDO = CAN2Master_post_TPDO;
		CCUCAN2Master_Data.post_emcy = CAN2Master_post_emcy;
		CCUCAN2Master_Data.post_SlaveBootup=CAN2Master_post_SlaveBootup;
	}
	
	// Start timer thread
	StartTimerLoop(&InitNodes);

	return 0;
}
