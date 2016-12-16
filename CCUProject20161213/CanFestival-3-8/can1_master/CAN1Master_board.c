#include "can_stm32.h"
#include "canfestival.h"
#include "CAN1Master.h"
#include "CAN1MasterSlave.h"

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
static int  test_master(void);

void can1master_recv_thread(void* p_arg)
{
	//	OS_ERR err;
	//	printf("can1 test!\r\n");
	//	//创建一个接受信号量
	//	OSSemCreate ((OS_SEM*	)&can1recv_sem,
	//                 (CPU_CHAR*	)"can1recvsem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);

	printf("can1init\r\n");
	canInit(CAN1,CAN_BAUD_125K);			//W H H ,主设备用的can1

	can1rx_save = 0;
	can1rx_read = 0;

	can1tx_save = 0;
	can1tx_read = 0;
	
	/*设置 默认 使用ccu1设备,通道1开启管理功能，否则不开启管理功能 */

	CCU1SwitchState = ChooseOperate;			//ccu1使用
	CCU2SwitchState = ChooseNotOperate;
	CurrentUseCAN1 = UseState;					 //使用can1总线显示状态
	CurrentUseCAN2 = NoUseState;				//不使用ccu2显示总线状态

	
	printf("tes can1tmaster,start\r\n");
	test_master();		//调用主站测试函数
	printf("test can1 master,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
	//			//请求信号量 ，等待信号量
	//		OSSemPend(&can1recv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
	//		if(err != OS_ERR_NONE)
	//		{
	//			printf("can1Rx_err=%d\r\n",err);
	//		}
	
		if(g_can1RxCount>0)
		{
			uint32_t next;
			Message *pmsg;
			
			next = can1rx_read;
			pmsg = &can1rx_msg_buf[next];
			
			/*W H H can1接受状态的改变*/
			{
				UNS8 nodeId = (UNS8) GET_NODE_ID((*pmsg));
				g_RxCAN1MesStateFlag[nodeId] = RxCanMesState;
				
				CAN1_Heart = RxHeart; //表示can1口能接受到其他设备的消息，则表示can1口正常，有心跳
			}

			//TIM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);		//关定时器中断
			/* Disable the Interrupt sources */
			TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&CCUCAN1Master_Data, pmsg);
			/* Enable the Interrupt sources */
			TIM4->DIER |= TIM_IT_CC1;
			//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);		//开定时器中断

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			can1rx_read = next;
			
			//W H H  ，使用全局变量来代替信号量
			g_can1RxCount--;
			//printf("can1,rece,end\r\n");
		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。		
  }
}

void can1master_send_thread(void *p_arg)
{
	//	OS_ERR err;
	//	
	//	//创建一个发送信号量
	//	OSSemCreate ((OS_SEM*	)&can1tran_sem,
	//                 (CPU_CHAR*	)"can1transem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);

	while(1)
	{
			//请求信号量 ，等待信号量
			//		OSSemPend(&can1tran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
			//		if(err != OS_ERR_NONE)
			//		{
			//			printf("can1Tx_err=%d\r\n",err);
			//		}
		
			//	printf("can1 send\r\n");

			if(g_can1TxCount>0)
			{
				uint32_t next;
				uint8_t  mailbox_no;
				CanTxMsg *ptx_msg;
				
				next = can1tx_read;
				ptx_msg = &can1tx_msg_buf[next];

				mailbox_no = CAN_Transmit(CAN1, ptx_msg);
				
				if(mailbox_no != CAN_NO_MB)
				{
					next++;
					if(next >= TX_BUF_LEN) next = 0;
					can1tx_read = next;			
					
					//W H H  ，使用全局变量来代替信号量
					g_can1TxCount--;
					//printf("can1,ok!\r\n");					
				}
				else
				{
					//printf("can1 no mailbox !\r\n");
					//发送信号量
					//	OSSemPost(&can1tran_sem,OS_OPT_POST_1,&err);
				}	
			}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}



s_BOARD CAN1MasterBoard = {"1", "1M"};

/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(CAN1MasterBoard.baudrate, "none")){
	//		RegisterSetODentryCallBack(&CCUCAN1Master_Data, 0x2001, 0, &can1MasterGetHerat);
	// 		RegisterSetODentryCallBack(&CCUCAN1Master_Data, 0x1016, 0x01, &OnMasterGetHerat);

		/* Defining the node Id */	
		if( CCU1 == 1)		//CCU1 设备
			setNodeId(&CCUCAN1Master_Data, 0x01);
		else						//CCU2设备
			setNodeId(&CCUCAN1Master_Data, 0x02);
		
		/* init */
		setState(&CCUCAN1Master_Data, Initialisation);
		
			
		setState(d, Operational);  //ccu1,can2将自己启动为操作状态	
	}
}

static TimerCallback_t init_callback;

static void StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(&CCUCAN1Master_Data, 0, init_callback, 0, 0);
}

///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(CAN1MasterBoard.baudrate, "none")){
//		
//		masterSendNMTstateChange(&CCUCAN1Master_Data, 0x00, NMT_Reset_Node);    	//将所有的节点复位
//		
//   	//Stop master
//		setState(&CCUCAN1Master_Data, Stopped);
//	}
//}

/****************************************************************************/
/****************************  test_master  *********************************/
/****************************************************************************/
static int test_master(void)
{
	if(strcmp(CAN1MasterBoard.baudrate, "none")){
		
		CCUCAN1Master_Data.canHandle = CAN1;		//W H H
		
		CCUCAN1Master_Data.heartbeatError = CAN1Master_heartbeatError;
		CCUCAN1Master_Data.initialisation = CAN1Master_initialisation;
		CCUCAN1Master_Data.preOperational = CAN1Master_preOperational;
		CCUCAN1Master_Data.operational = CAN1Master_operational;
		CCUCAN1Master_Data.stopped = CAN1Master_stopped;
		CCUCAN1Master_Data.post_sync = CAN1Master_post_sync;
		CCUCAN1Master_Data.post_TPDO = CAN1Master_post_TPDO;
		CCUCAN1Master_Data.post_emcy = CAN1Master_post_emcy;
		CCUCAN1Master_Data.post_SlaveBootup=CAN1Master_post_SlaveBootup;
	}

	// Start timer thread
	StartTimerLoop(&InitNodes);

	return 0;
}
