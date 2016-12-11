#include "can_stm32.h"
#include "canfestival.h"
#include "can1Slave.h"
#include "can1MasterSlave.h"
#include "objdictdef.h"		//W H H  这两个头文件必须添加，否则无法获取节点的id
#include "sysdep.h"
#include "os.h"
#include "delay.h"
#include "MTD_CPU_CANLED.h"
#include "stdbool.h"
#include "bsp.h"
#include "can12CallBack.h"

/*can1 led灯事件标志组*/
extern OS_FLAG_GRP CAN1LedEventFlag;

extern void TIM4_start(void);
extern void TIM4_stop(void);
int  can1test_slave(void);

extern u8 can1CarNumberFlag;
extern u8 can2CarNumberFlag;
extern u8 can1UpdateTimeFlag ;
extern u8 can2UpdataTimeFlag;

void can1slave_recv_thread(void* p_arg)
{
	OS_ERR err;
	
	uint32_t next;
	Message *pmsg;
	
//	
//	printf("slave test!\r\n");
//	//创建一个接受信号量
//	OSSemCreate ((OS_SEM*	)&can1slaverecv_sem,
//                 (CPU_CHAR*	)"can1slaverecvsem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);

	printf("CAN1int\r\n");
	canInit(CAN1,CAN_BAUD_125K);			//W H H ,从设备用的can2

	/* transmit queue init */
	can1slavetx_save = 0;
	can1slavetx_read = 0;
	/* receive  queue init */
	can1slaverx_save = 0;
	can1slaverx_read = 0;
	

		CurrentUseCAN1 = UseState;					 //使用can1总线显示状态
		CurrentUseCAN2 = NoUseState;				//不使用ccu2显示总线状态
		
	
	printf("testslave,start\r\n");
//	delay_ms(100);			//防止从节点比主节点先启动
	can1test_slave();
	printf("testslave,end\r\n");

  /* Infinite loop*/
  while(1)
  {	
		//请求信号量 ，等待信号量
//		OSSemPend(&can1slaverecv_sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
//		
//		if(err)
//		{
//			printf("sla_err=%d\r\n",err);
//		}
		
		if(g_can1RxCount>0)
		{
			next = can1slaverx_read;
			pmsg = &can1slaverx_msg_buf[next];

			/* Disable the Interrupt sources */
			TIM4->DIER &= (uint16_t)~TIM_IT_CC1;
				canDispatch(&MTDApp1CAN1Slave_Data, pmsg);		//W H H 
			/* Enable the Interrupt sources */
			TIM4->DIER |= TIM_IT_CC1;

			next++;
			if(next >= RX_BUF_LEN) next = 0;
			can1slaverx_read = next;

			//W H H  ，使用全局变量来代替信号量
			g_can1RxCount--;
			//printf("can1,rece,end\r\n");

			//MTD_CPU_CANLED_Turn(CAN1_RX_LED);			//can1接受led指示灯
			
			//向事件标志组CAN2LedEventFlag发送标志
			OSFlagPost((OS_FLAG_GRP*)&CAN1LedEventFlag,
								 (OS_FLAGS	  )CAN1RxLED_FLAG,
								 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
								 (OS_ERR*	    )&err);
		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。		
  }
}

void can1slave_send_thread(void *p_arg)
{
	OS_ERR err;
	//CPU_TS ts;
	
	uint32_t next;
	uint8_t  mailbox_no;
	CanTxMsg *ptx_msg;

	//	//创建一个发送信号量
	//	OSSemCreate ((OS_SEM*	)&can1slavetran_sem,
	//                 (CPU_CHAR*	)"can1slavetransem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);
	
	while(1)
	{
		//请求信号量 ，等待信号量
		//		OSSemPend(&can1slavetran_sem,0,OS_OPT_PEND_BLOCKING,0,&err);
		//OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);
		
		if(g_can1TxCount>0)
		{				
			next = can1slavetx_read;
			ptx_msg = &can1slavetx_msg_buf[next];

			mailbox_no = CAN_Transmit(CAN1, ptx_msg);		//can1发送
			next++;
				if(next >= TX_BUF_LEN) next = 0;
				can1slavetx_read = next;			
				
				//W H H  ，使用全局变量来代替信号量
				g_can1TxCount--;
				//	printf("can1,ok!\r\n");		
				//	printf("MC can1 Tx id=0x%x\r\n",ptx_msg->StdId);
				
						
			if(mailbox_no != CAN_NO_MB)
			{
						//向事件标志组CAN1LedEventFlag发送标志
				OSFlagPost((OS_FLAG_GRP*)&CAN1LedEventFlag,
									 (OS_FLAGS	  )CAN1TxLED_FLAG,
									 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
									 (OS_ERR*	    )&err);	
			}
			else
			{
					printf("can1 no mailbox !\r\n");
				
					//发送信号量
					//OSSemPost(&can1slavetran_sem,OS_OPT_POST_1,&err);
			} 

		}
		BSP_OS_TimeDlyMs(2);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}		
}



/*回调函数定义*/
UNS32 can1SlaveNumberUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
//	printf("aaaaaaaaaaaaaaaaaaaaaaa%d   %d\r\n",can1_Car_Number_1,can1_Car_Number_2);
//	if(CurrentUseCAN1 == UseState)
//	{
//		switch(can1_Car_Number_1)
//		{
//			case 0x01:
//				if(ChooseBoard == MC1_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;
//			case 0x02:
//				if(ChooseBoard == T_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;		
//			case 0x03:
//				if(ChooseBoard == M_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;	
//			case 0x04:
//				if(ChooseBoard == T1_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;	
//			case 0x05:
//				if(ChooseBoard == T2_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;	
//			case 0x06:
//				if(ChooseBoard == MC2_MTD_NODEID)
//					can1CarNumberFlag=true;
//				break;				
//		}
//		printf("can1_Car_Number_1=%d,can1_Car_Number_2=%d\r\n",can1_Car_Number_1,can1_Car_Number_2);
//	}
	return 0;
}


UNS32 can1SlaveDataUpdate(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex)
{
//		printf("cccccccccccccccccccccc%d %d %d %d %d   %d\r\n",can1_Car_DateTime[0],can1_Car_DateTime[1],can1_Car_DateTime[2],can1_Car_DateTime[3],can1_Car_DateTime[4],can1_Car_DateTime[5]);
//	if((CurrentUseCAN1 == UseState) && (can1_Car_DateTime[0] !=0))
//	{
//			can1UpdateTimeFlag=true;
//	}
	return 0;
}

s_BOARD CAN1SlaveBoard  = {"0", "1M"};

/***************************  INIT  *****************************************/
static void InitNodes(CO_Data* d, UNS32 id)
{
	/****************************** INITIALISATION SLAVE *******************************/
	if(strcmp(CAN1SlaveBoard.baudrate, "none")) {
		
		/*注册回调函数*/

		/*设置本地时钟*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2008, 7, &SetMTDTimeCallBack);

		/*接受空调温度*/
		//RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2060, 1, &SetAccTempCallBack);

		/*接受HMI发送的参数设置*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x205E, 4, &SetCarNoCallBack);		//HMI设置车辆编号
		
		/*接受HMI发送的ATO模式 等参数，PDO号0x189*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2006, 8, &Can3SnedProDataCallBack);
		
		/*接受HMI发送的PDO 0x1E3,映射0x2060 空调温度设置*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2060, 1, &Can3SnedACCSdrCallBack);
		
		/*接受HMI发送删除记录指令，PDO 0x1ED,映射0x206A */
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x206A, 3, &Can3SnedDelCmdCallBack);
		
		/*接受HMI发送的单条故障记录指令*/

		/*接受HIM发送PIS的Sdr协议数据，PDO 0x1FE,0x1FF,映射0x207D,0x207E */
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x207D, 2, &Can3SendPISSdrCallBack);
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x207E, 8, &Can3SendPISSdrCallBack);


		/*接受MCDU 发送的DI硬线数据*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2000, 2, &MDCURxATCDICallBack);
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2002, 2, &MDCURxATCDICallBack);
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2005, 2, &MDCURxATCDICallBack);

		/*接受MDCU 发送的通信状态数据*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2057, 2, &MDCURxATCSbuFaultCallBack);  
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2058, 2, &MDCURxATCSbuFaultCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2059, 2, &MDCURxATCSbuFaultCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x205A, 2, &MDCURxATCSbuFaultCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x205B, 2, &MDCURxATCSbuFaultCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x205C, 2, &MDCURxATCSbuFaultCallBack); 
		
		/*接受MDCU 发送的VVVF协议数据*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2033, 2, &MDCURxATCVVVFProCallBack);   //MC1 VVVF1
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2034, 5, &MDCURxATCVVVFProCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2038, 2, &MDCURxATCVVVFProCallBack);   //MC1 VVVF2
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2039, 5, &MDCURxATCVVVFProCallBack); 		
		
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x203D, 2, &MDCURxATCVVVFProCallBack);   //M VVVF1
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x203E, 5, &MDCURxATCVVVFProCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2042, 2, &MDCURxATCVVVFProCallBack);   //M VVVF2
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2043, 5, &MDCURxATCVVVFProCallBack); 	

		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2047, 2, &MDCURxATCVVVFProCallBack);   //MC2 VVVF1
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2048, 5, &MDCURxATCVVVFProCallBack); 
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x204C, 2, &MDCURxATCVVVFProCallBack);   //MC2 VVVF2
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x204D, 5, &MDCURxATCVVVFProCallBack); 	

		/*接受MDCU 发送的BCU协议数据*/
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2015, 3, &MDCURxATCBCUProCallBack);   	//MC1 BCU
		RegisterSetODentryCallBack(&MTDApp1CAN1Slave_Data, 0x2024, 3, &MDCURxATCBCUProCallBack); 		//MC2 BCU	

		switch(ChooseBoard)
		{
			case MC1_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,MC1_MTD_NODEID);
				break;
			case T_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,T_MTD_NODEID);
				break;
			case M_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,M_MTD_NODEID);
				break;
			case T1_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,T1_MTD_NODEID);
				break;
			case T2_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,T2_MTD_NODEID);
				break;
			case MC2_MTD_NODEID:
				setNodeId(&MTDApp1CAN1Slave_Data,MC2_MTD_NODEID);
				break;
			default :
				break;
		}
		/* init */
		setState(&MTDApp1CAN1Slave_Data, Initialisation);
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
//	if(strcmp(CAN1SlaveBoard.baudrate, "none")){
//		 
//   //Stop slave
//		setState(&MTDApp1CAN1Slave_Data, Stopped);
//	}
//}


//#include "Tim.h"
/****************************************************************************/
/*****************************  test_salve  *********************************/
/****************************************************************************/
int can1test_slave(void)
{
	if(strcmp(CAN1SlaveBoard.baudrate, "none")){
		
		MTDApp1CAN1Slave_Data.canHandle = CAN1;		//W H H  can1口的从站
		
		MTDApp1CAN1Slave_Data.heartbeatError = CAN1Slave_heartbeatError;
		
		MTDApp1CAN1Slave_Data.initialisation = CAN1Slave_initialisation;
		MTDApp1CAN1Slave_Data.preOperational = CAN1Slave_preOperational;
		MTDApp1CAN1Slave_Data.operational = CAN1Slave_operational;
		MTDApp1CAN1Slave_Data.stopped   = CAN1Slave_stopped;
		MTDApp1CAN1Slave_Data.post_sync = CAN1Slave_post_sync;
		MTDApp1CAN1Slave_Data.post_TPDO = CAN1Slave_post_TPDO;
		MTDApp1CAN1Slave_Data.storeODSubIndex = CAN1Slave_storeODSubIndex;
		MTDApp1CAN1Slave_Data.post_emcy = CAN1Slave_post_emcy;
	}
	// Start timer thread
	StartTimerLoop(&InitNodes);

	/*开启定时器3检测从站的当前状态，如果没有进入操作状态，则重新启动节点*/
//	TIM3_Int_Init(20000-1,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数10000次为1000ms
		
	return 0;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

