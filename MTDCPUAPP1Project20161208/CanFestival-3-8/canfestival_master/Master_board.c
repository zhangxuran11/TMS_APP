#include "can_stm32.h"
#include "canfestival.h"
#include "Master.h"
#include "Slave.h"
#include "TestMasterSlave.h"

#include "objdictdef.h"			//W H H  这两个头文件必须添加，否则无法获取节点的id
#include "sysdep.h"

#include "os.h"

/**
  * @brief  Main program
  * @param  None
  * @retval : None
  */
 OS_SEM  	recv_sem;		
 OS_SEM 	tran_sem;

extern Message   	rx_msg_buf[RX_BUF_LEN];
extern uint32_t		rx_save, rx_read;
extern CanTxMsg 	tx_msg_buf[TX_BUF_LEN];
extern uint32_t 	tx_save, tx_read;

extern void TIM4_start(void);
static int  test_master(void);

void master_recv_thread(void* p_arg)
{
	OS_ERR err;
	printf("master test!\r\n");
	//创建一个接受信号量
	OSSemCreate ((OS_SEM*	)&recv_sem,
                 (CPU_CHAR*	)"recvsem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);

	printf("can1init\r\n");
	canInit(CAN1,CAN_BAUD_125K);			//W H H ,主设备用的can1

	rx_save = 0;
	rx_read = 0;

	tx_save = 0;
	tx_read = 0;
	

	printf("testmaster,start\r\n");
	test_master();		//调用主站测试函数
	printf("testmaster,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
		//请求信号量 ，等待信号量
		OSSemPend(&recv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		if(err)
		{
			printf("mas_err=%d\r\n",err);
		}
		{
			uint32_t next;
			Message *pmsg;
			
			next = rx_read;
			pmsg = &rx_msg_buf[next];
			
			/*W H H can1接受状态的改变*/
			{
				UNS8 nodeId = (UNS8) GET_NODE_ID((*pmsg));
				g_RxCAN1MesStateFlag[nodeId] = RxCanMesState;
			}
			
			//TIM_ITConfig(TIM4,TIM_IT_CC1,DISABLE);		//关定时器中断
    	/* Disable the Interrupt sources */
    	TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&TestMaster_Data, pmsg);
			/* Enable the Interrupt sources */
    	TIM4->DIER |= TIM_IT_CC1;
			//TIM_ITConfig(TIM4,TIM_IT_CC1,ENABLE);		//开定时器中断

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			rx_read = next;
//			printf("master,rece,end\r\n");
		}
  }
}

void master_send_thread(void *p_arg)
{
	OS_ERR err;
	
	//创建一个发送信号量
	OSSemCreate ((OS_SEM*	)&tran_sem,
                 (CPU_CHAR*	)"transem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	
	while(1)
	{
		//请求信号量 ，等待信号量
		OSSemPend(&tran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		{
	
			uint32_t next;
			uint8_t  mailbox_no;
			CanTxMsg *ptx_msg;
			
			next = tx_read;
			ptx_msg = &tx_msg_buf[next];

			mailbox_no = CAN_Transmit(CAN1, ptx_msg);
			
			if(mailbox_no != CAN_NO_MB)
			{
				next++;
				if(next >= TX_BUF_LEN) next = 0;
				tx_read = next;			
			}
			else
			{
				printf("can1 no mailbox !\r\n");
				//发送信号量
				OSSemPost(&tran_sem,OS_OPT_POST_1,&err);

			}
//			printf("masetr send ok,end!\r\n");			
		}
	}		
}



static TimerCallback_t init_callback;

UNS32 OnMasterMap1Update(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	
	UNS32 ConsumerHeartTime = 0x0002012C;
	UNS32 size = sizeof(UNS32); 
	
//	eprintf("OnMasterMap1Update:%lld\r\n",Masterdoor_mc1_1);
		eprintf("Master: %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx \r\n",
	Masterdoor_mc1_1,Masterdoor_mc1_2,Masterdoor_T_1,Masterdoor_T_2,Masterdoor_M_1,Masterdoor_M_2,Masterdoor_T1_1,
	Masterdoor_T1_2,Masterdoor_T2_1,Masterdoor_T2_2,Masterdoor_mc2_1,Masterdoor_mc2_2);
	
//			writeLocalDict( &TestMaster_Data, /*CO_Data* d*/
//			0x1016, /*UNS16 index*/
//			0x01, /*UNS8 subind*/ 
//			&ConsumerHeartTime, /*void * pSourceData,*/ 
//			&size, /* UNS8 * pExpectedSize*/
//			RW);  /* UNS8 checkAccess */
	return 0;
}

s_BOARD MasterBoard = {"1", "1M"};

void help(void)
{
  eprintf("**************************************************************\n");
  eprintf("*  TestMasterSlave                                           *\n");
  eprintf("*                                                            *\n");
  eprintf("*  A simple example for PC. It does implement 2 CanOpen      *\n");
  eprintf("*  nodes in the same process. A master and a slave. Both     *\n");
  eprintf("*  communicate together, exchanging periodically NMT, SYNC,  *\n");
  eprintf("*  SDO and PDO. Master configure heartbeat producer time     *\n");
  eprintf("*  at 1000 ms for slave node-id 0x02 by concise DCF.         *\n");                                  
  eprintf("*                                                            *\n");
  eprintf("*   Usage:                                                   *\n");
  eprintf("*   ./TestMasterSlave  [OPTIONS]                             *\n");
  eprintf("*                                                            *\n");
  eprintf("*   OPTIONS:                                                 *\n");
  eprintf("*     -l : Can library [\"libcanfestival_can_virtual.so\"]     *\n");
  eprintf("*                                                            *\n");
  eprintf("*    Slave:                                                  *\n");
  eprintf("*     -s : bus name [\"0\"]                                    *\n");
  eprintf("*     -S : 1M,500K,250K,125K,100K,50K,20K,10K,none(disable)  *\n");
  eprintf("*                                                            *\n");
  eprintf("*    Master:                                                 *\n");
  eprintf("*     -m : bus name [\"1\"]                                    *\n");
  eprintf("*     -M : 1M,500K,250K,125K,100K,50K,20K,10K,none(disable)  *\n");
  eprintf("*                                                            *\n");
  eprintf("**************************************************************\n");
}

UNS32  OnMasterGetHerat(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
		eprintf("接收到了心跳!\r\n");
		return 0;
}


/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION MASTER *******************************/
	if(strcmp(MasterBoard.baudrate, "none")){
 		RegisterSetODentryCallBack(&TestMaster_Data, 0x2000, 0, &OnMasterMap1Update);
 		RegisterSetODentryCallBack(&TestMaster_Data, 0x1016, 0x01, &OnMasterGetHerat);
		
		/* Defining the node Id */
		setNodeId(&TestMaster_Data, 0x01);

		/* init */
		setState(&TestMaster_Data, Initialisation);
	}
}

static void StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(NULL, 0, init_callback, 0, 0);
}

/***************************  EXIT  *****************************************/
void Exit(CO_Data* d, UNS32 id)
{
	if(strcmp(MasterBoard.baudrate, "none")){
		
		masterSendNMTstateChange(&TestMaster_Data, 0x02, NMT_Reset_Node);    
    
   	//Stop master
		setState(&TestMaster_Data, Stopped);
	}
}

/****************************************************************************/
/****************************  test_master  *********************************/
/****************************************************************************/
static int test_master(void)
{
	if(strcmp(MasterBoard.baudrate, "none")){
		
		TestMaster_Data.canHandle = CAN1;		//W H H
		TestMaster_Data.heartbeatError = TestMaster_heartbeatError;
		TestMaster_Data.initialisation = TestMaster_initialisation;
		TestMaster_Data.preOperational = TestMaster_preOperational;
		TestMaster_Data.operational = TestMaster_operational;
		TestMaster_Data.stopped = TestMaster_stopped;
		TestMaster_Data.post_sync = TestMaster_post_sync;
		TestMaster_Data.post_TPDO = TestMaster_post_TPDO;
		TestMaster_Data.post_emcy = TestMaster_post_emcy;
		TestMaster_Data.post_SlaveBootup=TestMaster_post_SlaveBootup;
	}
	
	// Start timer thread
	StartTimerLoop(&InitNodes);

	return 0;
}
