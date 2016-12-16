#include "can_stm32.h"
#include "canfestival.h"
#include "can2Slave.h"
#include "can2MasterSlave.h"
#include "objdictdef.h"		//W H H  这两个头文件必须添加，否则无法获取节点的id
#include "sysdep.h"
#include "os.h"
#include "delay.h"
#include "MTD_MFD_CANLED.h"
#include "stdbool.h"
#include "bsp.h"
#include "main.h"

extern OS_FLAG_GRP CAN2LedEventFlag;

extern void TIM4_start(void);
extern void TIM4_stop(void);
int  can2test_slave(void);

void can2slave_recv_thread(void* p_arg)
{
	OS_ERR err;
	
	printf("CAN2int\r\n");
	canInit(CAN2,CAN_BAUD_500K);			//W H H ,从设备用的can2

	/* transmit queue init */
	can2slavetx_save = 0;
	can2slavetx_read = 0;
	/* receive  queue init */
	can2slaverx_save = 0;
	can2slaverx_read = 0;
	
		
	printf("can2slave,start\r\n");
	can2test_slave();
	printf("can2slave,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
//		//请求信号量 ，等待信号量
//		OSSemPend(&can2slaverecv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
//		if(err)
//		{
//			printf("can2slarecv_err=%d\r\n",err);
//		}
		
		if(g_can2RxCount>0)
		//else
		{
			uint32_t next;
			Message *pmsg;
			
			next = can2slaverx_read;
			pmsg = &can2slaverx_msg_buf[next];

			/* Disable the Interrupt sources */
			TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&MFDCAN2Slave_Data, pmsg);		//W H H 
			/* Enable the Interrupt sources */
			TIM4->DIER |= TIM_IT_CC1;

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			can2slaverx_read = next;
		
			g_can2RxCount--;
			//printf("can2,rece,end\r\n");				

			//向事件标志组CAN2LedEventFlag发送标志
			OSFlagPost((OS_FLAG_GRP*)&CAN2LedEventFlag,
								 (OS_FLAGS	  )CAN2RxLED_FLAG,
								 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
								 (OS_ERR*	    )&err);
		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
  }
}

void can2slave_send_thread(void *p_arg)
{
	OS_ERR err;

	while(1)
	{
	//		//请求信号量 ，等待信号量
	//		OSSemPend(&can2slavetran_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
	//		if(err)
	//		{
	//			printf("slasend_err=%d\r\n",err);
	//		}
		
		if(g_can2TxCount>0)
		//else
		{
				uint32_t next;
				uint8_t  mailbox_no;
				CanTxMsg *ptx_msg;
				
				next = can2slavetx_read;
				ptx_msg = &can2slavetx_msg_buf[next];

				mailbox_no = CAN_Transmit(CAN2, ptx_msg);		//can2发送
				if(mailbox_no != CAN_NO_MB)
				{
					next++;
					if(next >= TX_BUF_LEN) next = 0;
						can2slavetx_read = next;		

						g_can2TxCount--;
						//printf("can2,ok!\r\n");			
						//printf("MFD id=0x%x",ptx_msg->StdId);
					
					//向事件标志组CAN2LedEventFlag发送标志
					OSFlagPost((OS_FLAG_GRP*)&CAN2LedEventFlag,
										 (OS_FLAGS	  )CAN2TxLED_FLAG,
										 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
										 (OS_ERR*	    )&err);		
					
         /*WHH 打印调式 2016-11-29*/
					//						if(ptx_msg->StdId == 0x18D)
					//						{
					//							printf("MFD1_DI=%#x\r\n",ptx_msg->Data[0]);
					//						}
				}
				else
				{
					printf("can2 no mailbox !\r\n");
					//发送信号量
					//OSSemPost(&can2slavetran_sem,OS_OPT_POST_1,&err);	
				} 
		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}

///*MFD can2 接口回调函数的定义*/
//UNS32 SetAccSdrCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
//{
//	/*在任务中已经操作 ，不需要回调函数了。*/
//	return 0;
//}

/*设置BCU命令的回调函数*/
UNS32 SetBCUCmdCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
	if(((can2_RxBCUCmd[1]>>1) & 0x01) ==1)  //表示时间设置有效
	{
		BCUSetTimeFlag = true;
	}
	
	if((can2_RxBCUCmd[1]&0x01 ) == 1) //表示 清除状态数据有效
	{
		BCUClearStatDataFlag = true;
	}
	
	return 0;
}

s_BOARD CAN2SlaveBoard  = {"0", "1M"};
#include"iap.h"
/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	u8 SetNodeId;
	/****************************** INITIALISATION SLAVE *******************************/
	if(strcmp(CAN2SlaveBoard.baudrate, "none")) {

		/*注册回调函数*/
		//RegisterSetODentryCallBack(&MFDCAN2Slave_Data, 0x2003, 2, &SetAccSdrCallBack);
    RegisterSetODentryCallBack(&MFDCAN2Slave_Data, 0x2004, 2, &SetBCUCmdCallBack);
		
		SetNodeId = ChooseBoard &0x0F;

		/* init */
		setNodeId(&MFDCAN2Slave_Data,SetNodeId);
		setState(&MFDCAN2Slave_Data, Initialisation);
		IAP_Init(&MFDCAN2Slave_Data,NULL,MTDMFD_CanAddVal(),0x161214);
	}
}

static TimerCallback_t init_callback;

static void  StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;
	TIM4_start();
	SetAlarm(NULL, 0, init_callback, 0, 0);	
}


///***************************  EXIT  *****************************************/
//static void Exit(CO_Data* d, UNS32 id)
//{
//	if(strcmp(CAN2SlaveBoard.baudrate, "none")){
//		 
//   //Stop slave
//		setState(&MFDCAN2Slave_Data, Stopped);
//	}
//}


//#include "Tim.h"
/****************************************************************************/
/*****************************  test_salve  *********************************/
/****************************************************************************/
 int can2test_slave(void)
{
	if(strcmp(CAN2SlaveBoard.baudrate, "none")){
		
		MFDCAN2Slave_Data.canHandle = CAN2;		//W H H
		
		MFDCAN2Slave_Data.heartbeatError = CAN2Slave_heartbeatError;
		MFDCAN2Slave_Data.initialisation = CAN2Slave_initialisation;
		MFDCAN2Slave_Data.preOperational = CAN2Slave_preOperational;
		MFDCAN2Slave_Data.operational = CAN2Slave_operational;
		MFDCAN2Slave_Data.stopped   = CAN2Slave_stopped;
		MFDCAN2Slave_Data.post_sync = CAN2Slave_post_sync;
		MFDCAN2Slave_Data.post_TPDO = CAN2Slave_post_TPDO;
		MFDCAN2Slave_Data.storeODSubIndex = CAN2Slave_storeODSubIndex;
		MFDCAN2Slave_Data.post_emcy = CAN2Slave_post_emcy;
	}
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

