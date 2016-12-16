#include "can_stm32.h"
#include "canfestival.h"
#include "CcuLedSlave.h"
#include "CcuLedMasterSlave.h"

#include "objdictdef.h"		//W H H  这两个头文件必须添加，否则无法获取节点的id
#include "sysdep.h"

#include "bsp_os.h"
#include "os.h"
#include "delay.h"
//#include "Tim.h"
#include "CCU_LED_LedControl.h"

// OS_SEM  	slaverecv_sem;		
// OS_SEM 	slavetran_sem;

Message   slaverx_msg_buf[RX_BUF_LEN];
uint32_t	slaverx_save, slaverx_read;
CanTxMsg 	slavetx_msg_buf[TX_BUF_LEN];
uint32_t 	slavetx_save, slavetx_read;

extern void TIM4_start(void);
extern void TIM4_stop(void);
static int  test_slave(void);

/*将所有表示心跳的数据拼接起来*/
u32  CCULED_DataCombin(void);


void slave_recv_thread(void* p_arg)			//CCULED 用的CAN2 发送接受数据
{
//	OS_ERR err;
	
	printf("slave test!\r\n");
//	//创建一个接受信号量
//	OSSemCreate ((OS_SEM*	)&slaverecv_sem,
//                 (CPU_CHAR*	)"slaverecvsem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
//	
//	//创建一个发送信号量
//	OSSemCreate ((OS_SEM*	)&slavetran_sem,
//                 (CPU_CHAR*	)"slavetransem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
	
	printf("CAN2int\r\n");
	canInit(CAN2,CAN_BAUD_500K);			//W H H ,从设备用的can2

	/* transmit queue init */
	slavetx_save = 0;
	slavetx_read = 0;
	/* receive  queue init */
	slaverx_save = 0;
	slaverx_read = 0;
	
	printf("testslave,start\r\n");
	delay_ms(500);			//防止从节点比主节点先启动
	test_slave();
	printf("testslave,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
//		//请求信号量 ，等待信号量
//		OSSemPend(&slaverecv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
//		
//		if(err)
//		{
//			printf("sla_err=%d\r\n",err);
//		}
		
		//W H H  ，使用全局变量来代替信号量
		if(g_can2RxCount>0)		
		{
			
			uint32_t next;
			Message *pmsg;
			
			next = slaverx_read;
			pmsg = &slaverx_msg_buf[next];

//			/*W H H can2接受状态的改变,cculed 里面不需要存储心跳*/
//			{
//				UNS8 nodeId = (UNS8) GET_NODE_ID((*pmsg));
//				g_RxCAN2MesStateFlag[nodeId] = RxCanMesState;
//			}
			
    	/* Disable the Interrupt sources */
    	TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&Ccu1LedObjDict_Data, pmsg);		//W H H 
			/* Enable the Interrupt sources */
    	TIM4->DIER |= TIM_IT_CC1;

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			slaverx_read = next;
			
			//W H H  ，使用全局变量来代替信号量
			g_can2RxCount--;
			//printf("slave,rece,edn\r\n");
		}
		BSP_OS_TimeDlyMs(2);	//W H H
  }
}

void slave_send_thread(void *p_arg)
{
//	OS_ERR err;
//	
//	//创建一个发送信号量
//	OSSemCreate ((OS_SEM*	)&slavetran_sem,
//                 (CPU_CHAR*	)"slavetransem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
	
	while(1)
	{
//		//请求信号量 ，等待信号量
//		OSSemPend(&slavetran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		
		//W H H  ，使用全局变量来代替信号量
		if(g_can2TxCount>0)
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
			
				//W H H  ，使用全局变量来代替信号量
				g_can2TxCount--;
			}
			else
			{
					printf("can2 no mailbox !\r\n");
//				//发送信号量
//				OSSemPost(&slavetran_sem,OS_OPT_POST_1,&err);
				
			} 
//			printf("slave send ok,end!\r\n");
		}
		BSP_OS_TimeDlyMs(2);	//W H H
	}		
}


static TimerCallback_t init_callback;

s_BOARD SlaveBoard  = {"0", "1M"};


UNS32 OnCcuLedCAN1NBCUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	//UNS8  i;
//	for(i=0;i<CCULED_LED_NUMBER;i++)
//	{
//		CCULED_LED_Off(i);
//	}
	
	printf("can1cnb=%d\r\n",HeartMap_CAN1_CANBC);
//	printf("can2cnb=%d\r\n",HeartMap_CAN2_CANBC);
//	printf("can1ccu1=%d\r\n",HeartMap_CAN1_CCU1);
	return 0;
}
UNS32 OnCcuLedCAN2NBCUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{

	return 0;
}
UNS32 OnCcuLedCAN1CCU1NBCUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{

	return 0;
}
#include "iap.h"
#include "CCU_LED_CanAddres.h"
/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION SLAVE *******************************/
	if(strcmp(SlaveBoard.baudrate, "none")) {
		
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x01, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x02, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x03, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x04, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x05, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x06, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x07, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x08, &OnCcuLedCAN1NBCUpdate);		
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x09, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0a, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0b, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0c, &OnCcuLedCAN1NBCUpdate);		
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0d, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0e, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x0f, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x10, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x11, &OnCcuLedCAN1NBCUpdate);		
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x12, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x13, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x14, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x15, &OnCcuLedCAN1NBCUpdate);		
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x16, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x17, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x18, &OnCcuLedCAN1NBCUpdate);
//			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x19, &OnCcuLedCAN1NBCUpdate);
			RegisterSetODentryCallBack(&Ccu1LedObjDict_Data, 0x2000, 0x1A, &OnCcuLedCAN1NBCUpdate);			
			
		setNodeId(&Ccu1LedObjDict_Data, 0x01);		//设置cculed从站的节点id为0x01    旋钮值0x11

		/* init */
		setState(&Ccu1LedObjDict_Data, Initialisation);
		
		setState(&Ccu1LedObjDict_Data, Operational);  //cculed将自己启动为操作状态
		IAP_Init(&Ccu1LedObjDict_Data,NULL,CCULED_CanAddVal(),0x161212);
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

///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(SlaveBoard.baudrate, "none")){
//		 
//   //Stop slave
//		setState(&Ccu1LedObjDict_Data, Stopped);
//	}
//}


/****************************************************************************/
/*****************************  test_salve  *********************************/
/****************************************************************************/
static int test_slave(void)
{
	if(strcmp(SlaveBoard.baudrate, "none")){
		
		Ccu1LedObjDict_Data.canHandle = CAN2;		//W H H
		
		Ccu1LedObjDict_Data.heartbeatError = Ccu1LedSlave_heartbeatError;
		Ccu1LedObjDict_Data.initialisation = Ccu1LedSlave_initialisation;
		Ccu1LedObjDict_Data.preOperational = Ccu1LedSlave_preOperational;
		Ccu1LedObjDict_Data.operational = Ccu1LedSlave_operational;
		Ccu1LedObjDict_Data.stopped   = Ccu1LedSlave_stopped;
		Ccu1LedObjDict_Data.post_sync = Ccu1LedSlave_post_sync;
		Ccu1LedObjDict_Data.post_TPDO = Ccu1LedSlave_post_TPDO;
		Ccu1LedObjDict_Data.storeODSubIndex = Ccu1LedSlave_storeODSubIndex;
		Ccu1LedObjDict_Data.post_emcy = Ccu1LedSlave_post_emcy;
	}
	// Start timer thread
	StartTimerLoop(&InitNodes);

	/*开启定时器3检测从站的当前状态，如果没有进入操作状态，则重新启动节点*/
//	TIM3_Int_Init(20000-1,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1000ms
		
	return 0;
}


u32  CCULED_DataCombin(void)
{
		u32 DataCombin;
		DataCombin =  ((u32)(HeartMap_CAN2_CANBC & 0x01) <<25)|((u32)( HeartMap_CAN1_CANBC & 0x01)<<24)  | ((u32)(HeartMap_CAN2_CCU1 & 0x01) <<23) | \
								 ((u32)(HeartMap_CAN1_CCU1 & 0x01) <<22) | ((u32)(HeartMap_CAN2_CCU2 & 0x01) <<21) | ((u32)(HeartMap_CAN1_CCU2 & 0x01) <<20) | \
								 ((u32)(HeartMap_CAN2_DDU1 & 0x01) <<19) | ((u32)(HeartMap_CAN1_DDU1 & 0x01) <<18)| ((u32)(HeartMap_CAN2_DDU2 & 0x01) <<17) | \
								 ((u32)(HeartMap_CAN1_DDU2 & 0x01) <<16) | ((u32)(HeartMap_CAN2_EMR1 & 0x01) <<15) |((u32)(HeartMap_CAN1_EMR1 & 0x01) <<14) | \
								 ((u32)(HeartMap_CAN2_EMR2 & 0x01) <<13) | ((u32)(HeartMap_CAN1_EMR2 & 0x01) <<12) | ((u32)(HeartMap_CAN2_MTD1 & 0x01) <<11)| \
								 ((u32)(HeartMap_CAN1_MTD1 & 0x01) <<10) | ((u32)(HeartMap_CAN2_MTD2 & 0x01) <<9) |((u32)(HeartMap_CAN1_MTD2 & 0x01) <<8) |\
								 ((u32)(HeartMap_CAN2_MTD3 & 0x01) <<7) | ((u32)(HeartMap_CAN1_MTD3 & 0x01) <<6) | ((u32)(HeartMap_CAN2_MTD4 & 0x01) <<5) | \
								 ((u32)(HeartMap_CAN1_MTD4 & 0x01) <<4) | ((u32)(HeartMap_CAN2_MTD5 & 0x01) <<3) | ((u32)(HeartMap_CAN1_MTD5 & 0x01) <<2) |\
								 ((u32)(HeartMap_CAN2_MTD6 & 0x01) <<1) | ((u32)(HeartMap_CAN1_MTD6 & 0x01) <<0);
	return DataCombin;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

