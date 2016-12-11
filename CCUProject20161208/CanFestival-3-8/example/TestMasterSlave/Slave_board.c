#include "can_stm32.h"
#include "canfestival.h"
#include "Master.h"
#include "Slave.h"
#include "TestMasterSlave.h"

#include "os.h"
#include "delay.h"

 OS_SEM  	slaverecv_sem;		
 OS_SEM 	slavetran_sem;

Message   slaverx_msg_buf[RX_BUF_LEN];
uint32_t	slaverx_save, slaverx_read;
CanTxMsg 	slavetx_msg_buf[TX_BUF_LEN];
uint32_t 	slavetx_save, slavetx_read;

extern void TIM4_start(void);
extern void TIM4_stop(void);
static int  test_slave(void);


void slave_recv_thread(void* p_arg)
{
	OS_ERR err;
	
	printf("slave test!\r\n");
	//创建一个接受信号量
	OSSemCreate ((OS_SEM*	)&slaverecv_sem,
                 (CPU_CHAR*	)"slaverecvsem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);

	printf("CAN2int\r\n");
	canInit(CAN2,CAN_BAUD_1M);			//W H H ,从设备用的can2

	/* transmit queue init */
	slavetx_save = 0;
	slavetx_read = 0;
	/* receive  queue init */
	slaverx_save = 0;
	slaverx_read = 0;
	
	printf("testslave,start\r\n");
	delay_ms(20);			//防止从节点比主节点先启动
	test_slave();
	printf("testslave,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
		//请求信号量 ，等待信号量
		OSSemPend(&slaverecv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		{
			
			uint32_t next;
			Message *pmsg;
			
			next = slaverx_read;
			pmsg = &slaverx_msg_buf[next];

    	/* Disable the Interrupt sources */
    	TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&TestSlave_Data, pmsg);		//W H H 
			/* Enable the Interrupt sources */
    	TIM4->DIER |= TIM_IT_CC1;

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			slaverx_read = next;
			printf("slave,rece,edn\r\n");
		}
  }
}

void slave_send_thread(void *p_arg)
{
	OS_ERR err;
	
	//创建一个发送信号量
	OSSemCreate ((OS_SEM*	)&slavetran_sem,
                 (CPU_CHAR*	)"slavetransem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	
	while(1)
	{
		//请求信号量 ，等待信号量
		OSSemPend(&slavetran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		{
			uint32_t next;
			uint8_t  mailbox_no;
			CanTxMsg *ptx_msg;
			
			next = slavetx_read;
			ptx_msg = &slavetx_msg_buf[next];

			mailbox_no = CAN_Transmit(CAN2, ptx_msg);
			if(mailbox_no != CAN_NO_MB)
			{
				next++;
				if(next >= TX_BUF_LEN) next = 0;
					slavetx_read = next;			
			}
			else
			{
				//发送信号量
				OSSemPost(&slavetran_sem,OS_OPT_POST_1,&err);
			} 
			printf("slave send ok,end!\r\n");
		}
	}		
}


static TimerCallback_t init_callback;

s_BOARD SlaveBoard  = {"0", "1M"};

void slavehelp(void)
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

/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION SLAVE *******************************/
	if(strcmp(SlaveBoard.baudrate, "none")) {
		setNodeId(&TestSlave_Data, 0x02);

		/* init */
		setState(&TestSlave_Data, Initialisation);
	}
}

static void  StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(NULL, 0, init_callback, 0, 0);	
}

void StopTimerLoop(TimerCallback_t exitfunction)
{
	TIM4_stop();
}

/***************************  EXIT  *****************************************/
static void Exit(CO_Data* d, UNS32 id)
{
	if(strcmp(SlaveBoard.baudrate, "none")){
		 
   //Stop slave
		setState(&TestSlave_Data, Stopped);
	}
}


#include "Tim.h"
/****************************************************************************/
/*****************************  test_salve  *********************************/
/****************************************************************************/
static int test_slave(void)
{
	if(strcmp(SlaveBoard.baudrate, "none")){
		
		TestSlave_Data.canHandle = CAN2;		//W H H
		
		TestSlave_Data.heartbeatError = TestSlave_heartbeatError;
		TestSlave_Data.initialisation = TestSlave_initialisation;
		TestSlave_Data.preOperational = TestSlave_preOperational;
		TestSlave_Data.operational = TestSlave_operational;
		TestSlave_Data.stopped   = TestSlave_stopped;
		TestSlave_Data.post_sync = TestSlave_post_sync;
		TestSlave_Data.post_TPDO = TestSlave_post_TPDO;
		TestSlave_Data.storeODSubIndex = TestSlave_storeODSubIndex;
		TestSlave_Data.post_emcy = TestSlave_post_emcy;
	}

	/*开启定时器3检测从站的当前状态，如果没有进入操作状态，则重新启动节点*/
//	TIM3_Int_Init(20000-1,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1000ms
	
	// Start timer thread
	StartTimerLoop(&InitNodes);
	
	return 0;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

