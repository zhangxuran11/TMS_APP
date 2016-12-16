/****************************************Copyright (c)***************************************
// 固件库中带红色标志的，表示当前没有使用，若需要使用则需要加入到工程中
**-------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**-------------------------------------------------------------------------------------------
********************************************************************************************/
#include "bsp.h"


#define BUFFER_SIZE 512
	
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];


static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

/*任务创建定义变量的步骤*/

/*LED点阵显示任务*/
	//MainTask  任务优先级
#define Main_TASK_PRIO		6
	//任务堆栈大小	
#define Main_STK_SIZE 		512
	//任务控制块
static OS_TCB MainTaskTCB;
	//任务堆栈	
static CPU_STK Main_TASK_STK[Main_STK_SIZE];
	//任务函数
static void Main_task(void *p_arg);

////LED任务优先级
//#define LED_TASK_PRIO		5
////任务堆栈大小	
//#define LED_STK_SIZE 		256
////任务控制块
//static OS_TCB LedTaskTCB;
////任务堆栈	
//static CPU_STK LED_TASK_STK[LED_STK_SIZE];
////任务函数
//static void led_task(void *p_arg);

///*温度检测任务*/
//	//temper 温度检测任务优先级
//#define TemperCheck_TASK_PRIO		6
//	//任务堆栈大小	
//#define TemperCheck_STK_SIZE 		256
//	//任务控制块
//static OS_TCB TemperCheckTaskTCB;
//	//任务堆栈	
//static CPU_STK TemperCheck_TASK_STK[TemperCheck_STK_SIZE];
//	//任务函数
//static void TemperCheck_task(void *p_arg);

///*按键显示cpu,堆栈信息的任务*/
//	//KEY 按键任务优先级
//#define KEY_TASK_PRIO		7
//	//任务堆栈大小	
//#define KEY_STK_SIZE 		512
//	//任务控制块
//static OS_TCB KeyTaskTCB;
//	//任务堆栈	
//static CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//	//任务函数
//static void key_task(void *p_arg);


///*主站接受任务,堆栈信息的任务*/
//	//任务优先级
//#define MASTERRX_TASK_PRIO		9
//	//任务堆栈大小	
//#define MASTERRX_STK_SIZE 		512
//	//任务控制块
//static OS_TCB MasterRxTaskTCB;
//	//任务堆栈	
//static CPU_STK MASTERRX_TASK_STK[MASTERRX_STK_SIZE];
//	//任务函数
//extern void master_recv_thread(void *p_arg);



///*主站发送任务*/
//	//任务优先级
//#define MASTERTX_TASK_PRIO		10
//	//任务堆栈大小	
//#define MASTERTX_STK_SIZE 		512
//	//任务控制块
//static OS_TCB MasterTxTaskTCB;
//	//任务堆栈	
//static CPU_STK MASTERTX_TASK_STK[MASTERTX_STK_SIZE];
//	//任务函数
//extern void master_send_thread(void *p_arg);

	/*任务对象的创建*/
static  OS_SEM     	SEM_SYNCH;	   //用于同步


/*从站接受任务,堆栈信息的任务*/
	//任务优先级
#define SLAVERX_TASK_PRIO		7
	//任务堆栈大小	
#define SLAVERX_STK_SIZE 		512
	//任务控制块
static OS_TCB SlaveRxTaskTCB;
	//任务堆栈	
static CPU_STK SLAVERX_TASK_STK[SLAVERX_STK_SIZE];
	//任务函数
extern void slave_recv_thread(void *p_arg);



/*从站发送任务*/
	//任务优先级
#define SLAVETX_TASK_PRIO		8
	//任务堆栈大小	
#define SLAVETX_STK_SIZE 		512
	//任务控制块
static OS_TCB SlaveTxTaskTCB;
	//任务堆栈	
static CPU_STK SLAVETX_TASK_STK[SLAVETX_STK_SIZE];
	//任务函数
extern void slave_send_thread(void *p_arg);


/*从站启动管理任务*/
	//任务优先级
#define SLAVE_STARTUPMANAGER_TASK_PRIO		5
	//任务堆栈大小	
#define SLAVE_STARTUPMANAGER_STK_SIZE 		512
	//任务控制块
static OS_TCB SlaveStartupManagerTCB;
	//任务堆栈	
static CPU_STK SLAVE_STARTUPMANAGER_TASK_STK[SLAVE_STARTUPMANAGER_STK_SIZE];
	//任务函数
void Slave_StartupManager_Take(void *p_arg);


/*CCULED 管理其他设备的状态显示led任务*/
	//任务优先级
#define CCULED_DEVICE_STATUS_TASK_PRIO		4
	//任务堆栈大小	
#define CCULED_DEVICE_STATUS_STK_SIZE 		512
	//任务控制块
static OS_TCB CCULEDDeviceStatusTCB;
	//任务堆栈	
static CPU_STK CCULED_DEVICE_STATUS_TASK_STK[CCULED_DEVICE_STATUS_STK_SIZE];
	//任务函数
void CCULED_DeviceStatus_Take(void *p_arg);



	/*任务对象的创建*/
static  OS_SEM     	SEM_SYNCH;	   //用于同步
OS_SEM     	Slave_Start_Sem;			//从站启动管理的信号量

///*测试使用的全局变量的定义*/
////can
uint8_t DS2401P_ID[8]={0x00};			//必须定义在bsp，macid检测中使用了
uint8_t can2_send[]={0x11,0x22,0x33,0x44,0x11,0x66,0x78,0x99,};
u8 g_RxDataFlag=0;
u8 g_FrameErrorFlag=0;

/*canfestival 接受心跳报文的定义*/

IsNoReceHeart			 g_RxCAN1HeartFlag[15]={NoRxHeart};					//存放各个设备的心跳的标志，下标就是节点的ID号，故下标为0的数组没有使用。
IsNoReceHeart			 g_RxCAN2HeartFlag[15]={NoRxHeart};

CurReceHeartState  g_RxCAN1HeartStateFlag[15] = {InPowerInitState};	//存放当前接受心跳时，从设备所在的状态，是上电刚初始化的时候，还是建立的连接的时候		
CurReceHeartState  g_RxCAN2HeartStateFlag[15] = {InPowerInitState};

RxCanMessageState  g_RxCAN1MesStateFlag[15]={NoRxCanMesState};			//存放当前主站接到从站的can消息 所处的状态 
RxCanMessageState	 g_RxCAN2MesStateFlag[15]={NoRxCanMesState};

IsNoReceHeart  		 g_CCU1HeartFlag=NoRxHeart ;                //从站接受到主站的心跳存入全局标志中

//extern CO_Data TestSlave_Data;
//extern CO_Data TestMaster_Data;

extern uint8_t UART1_Rx_Cnt;

extern CO_Data Ccu1LedObjDict_Data;

//void Ican_Receive_ISR(void);		//ican的中断接受服务程序

	/*
		由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
		启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

		系统时钟缺省配置为168MHz，如果需要更改，可以修改 system_stm32f4xx.c 文件
	*/
	
int main(void)
{
		OS_ERR  err;

		CPU_SR_ALLOC();																	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000); //重新映射中断向量表
		SCB->VTOR = FLASH_BASE | 0x10000;	//设置中断向量表	
//		uart3_init();		//uart3的GPIO初始化 ,提前的初始化，防止中间有打印显示信息会无法输出
		Uart1_init();
	
		/* 初始化uC/OS-III 内核 */	
    OSInit(&err);                                               /* Init uC/OS-III.                                      */
		
		OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断

		/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,       /* Create the start task   *//* 任务控制块地址 */    
                 (CPU_CHAR     *)"App Task Start",			 /* 任务名 */	
                 (OS_TASK_PTR   )AppTaskStart, 					 /* 启动任务函数地址 */
                 (void         *)0,											 /* 传递给任务的参数 */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,/* 任务优先级 */
                 (CPU_STK      *)&AppTaskStartStk[0],		 /* 堆栈基地址 */
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10],	/* 堆栈监测区，这里表示后10%作为监测区 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,	/* 堆栈空间大小 */
                 (OS_MSG_QTY    )0,														/* 本任务支持接受的最大消息数 */
                 (OS_TICK       )0,													 /* 设置时间片 */
                 (void         *)0,													 /* 堆栈空间大小 */
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),
                 (OS_ERR       *)&err);
					 
									 /*  定义如下：
										OS_OPT_TASK_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
										OS_OPT_TASK_STK_CLR      在创建任务时，清零任务栈
										OS_OPT_TASK_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
									 */ 
								 
		OS_CRITICAL_EXIT();	//退出临界区

		/* 启动多任务系统，控制权交给uC/OS-III */
    OSStart(&err);                        /* Start multitasking (i.e. give control to uC/OS-III). */
    
    (void)&err;

		return(0);
}


		/*开始任务*/
static  void  AppTaskStart (void *p_arg)
{		

//	iCANID packet_id;
	
		OS_ERR      err;
   (void)p_arg;
    BSP_Init();                 /*所有的外设的初始化都在这里*/     /* Initialize BSP functions                             */
    CPU_Init();                                                	 /* Initialize the uC/CPU services                       */ 
	//  BSP_Tick_Init();                                            /* Start Tick Initialization                            */
	//  Mem_Init();                                                 /* Initialize Memory Management Module                  */
	//  Math_Init();                                                /* Initialize Mathematical Module                       */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

	/* 
	 1. 在OSInit里面默认每个任务的时间片是OSCfg_TickRate_Hz / 10u，	即 1000/10=100ms
	 2. 使用下面的函数可以设置全局时间片，也就是每个任务使用的时间片
	 3. 如果下面函数的第二个参数设置为0，那么表示所有任务默认时间片为OSCfg_TickRate_Hz / 10u
	 4. 当然，也可以在创建任务的时候单独设置时间片大小。 
	*/
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候	，每个时钟节拍为1ms
	 //使能时间片轮转调度功能,时间片长度为5个系统时钟节拍，既5*1=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,5,&err);  
#endif

		//对象的创建
  AppObjCreate();                                             /* Create Applicaiton kernel objects                    */

		//任务的创建
  AppTaskCreate();                                            /* Create Application tasks                             */

	/*打印任务信息*/
	delay_ms(1000);			//进行任务的调度，下面的函数是打印显示任务所有信息
	DispTaskInfo();			//打印完后，AppTaskStart任务就结束了，因为没有while();
	OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);		//挂起开始任务		
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	OS_ERR      err;
	CPU_SR_ALLOC();			//变量定义，用于保存SR状态.
	
	OS_CRITICAL_ENTER();	//进入临界区
	
		//创建MainTask任务
	OSTaskCreate((OS_TCB 		  *)&MainTaskTCB,			//任务控制块
								(CPU_CHAR	  *)"Main_task", 			//任务名字
                 (OS_TASK_PTR)Main_task, 				//任务函数
                 (void		  *)0,								//传递给任务函数的参数
                 (OS_PRIO	   )Main_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&Main_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)Main_TASK_STK[Main_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)Main_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值
								 
//	//创建LedTaskTCB任务
//	OSTaskCreate((OS_TCB 	 *)&LedTaskTCB,			//任务控制块
//							(CPU_CHAR	 *)"led_task", 			//任务名字
//							 (OS_TASK_PTR)led_task, 			//任务函数
//							 (void		  *)0,							//传递给任务函数的参数
//							 (OS_PRIO	   )LED_TASK_PRIO,   //任务优先级
//							 (CPU_STK   *)&LED_TASK_STK[0],//任务堆栈基地址
//							 (CPU_STK_SIZE)LED_TASK_STK[LED_STK_SIZE/10],//任务堆栈深度限位
//							 (CPU_STK_SIZE)LED_STK_SIZE,	 //任务堆栈大小
//							 (OS_MSG_QTY  )0,							 //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//							 (OS_TICK	    )0,							 //当使能时间片轮转时的时间片长度，为0时为默认长度，
//							 (void   	   *)0,							 //用户补充的存储区
//							 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),		//任务选项
//							 (OS_ERR 	   *)&err);					 //存放该函数错误时的返回值
								 
//	 //创建TemperCheckTask任务
//	OSTaskCreate((OS_TCB 		  *)&TemperCheckTaskTCB,			//任务控制块
//								(CPU_CHAR	  *)"TemperCheck_task", 			//任务名字
//                 (OS_TASK_PTR)TemperCheck_task, 			//任务函数
//                 (void		  *)0,											//传递给任务函数的参数
//                 (OS_PRIO	   )TemperCheck_TASK_PRIO,     	//任务优先级
//                 (CPU_STK   *)&TemperCheck_TASK_STK[0],	//任务堆栈基地址
//                 (CPU_STK_SIZE)TemperCheck_TASK_STK[TemperCheck_STK_SIZE/10],		//任务堆栈深度限位
//                 (CPU_STK_SIZE)TemperCheck_STK_SIZE,		//任务堆栈大小
//                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
//                 (void   	   *)0,					//用户补充的存储区
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP,		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值
//								 				 
//	//创建KEY任务
//	OSTaskCreate((OS_TCB 		* )&KeyTaskTCB,			//任务控制块
//								(CPU_CHAR	* )"key_task", 			//任务名字
//                 (OS_TASK_PTR)key_task, 			//任务函数
//                 (void		  *)0,					//传递给任务函数的参数
//                 (OS_PRIO	   )KEY_TASK_PRIO,     	//任务优先级
//                 (CPU_STK   *)&KEY_TASK_STK[0],	//任务堆栈基地址
//                 (CPU_STK_SIZE)KEY_TASK_STK[KEY_STK_SIZE/10],		//任务堆栈深度限位
//                 (CPU_STK_SIZE)KEY_STK_SIZE,		//任务堆栈大小
//                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
//                 (void   	   *)0,					//用户补充的存储区
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值
//								 
//		//创建主站接受任务
//	OSTaskCreate((OS_TCB 		* )&MasterRxTaskTCB,			//任务控制块
//								(CPU_CHAR	* )"MasterRx_task", 			//任务名字
//                 (OS_TASK_PTR)master_recv_thread, 	//任务函数
//                 (void		  *)0,										//传递给任务函数的参数
//                 (OS_PRIO	   )MASTERRX_TASK_PRIO,     	//任务优先级
//                 (CPU_STK   *)&MASTERRX_TASK_STK[0],	//任务堆栈基地址
//                 (CPU_STK_SIZE)MASTERRX_TASK_STK[MASTERRX_STK_SIZE/10],		//任务堆栈深度限位
//                 (CPU_STK_SIZE)MASTERRX_STK_SIZE,		//任务堆栈大小
//                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
//                 (void   	   *)0,					//用户补充的存储区
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				

//		//创建主站发送任务
//	OSTaskCreate((OS_TCB 		* )&MasterTxTaskTCB,			//任务控制块
//								(CPU_CHAR	* )"MasterTx_task", 			//任务名字
//                 (OS_TASK_PTR)master_send_thread, 	//任务函数
//                 (void		  *)0,										//传递给任务函数的参数
//                 (OS_PRIO	   )MASTERTX_TASK_PRIO,     	//任务优先级
//                 (CPU_STK   *)&MASTERTX_TASK_STK[0],	//任务堆栈基地址
//                 (CPU_STK_SIZE)MASTERTX_TASK_STK[MASTERTX_STK_SIZE/10],		//任务堆栈深度限位
//                 (CPU_STK_SIZE)MASTERTX_STK_SIZE,		//任务堆栈大小
//                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
//                 (void   	   *)0,					//用户补充的存储区
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			


/*Wh h*/
		//创建从站接受任务
	OSTaskCreate((OS_TCB 		* )&SlaveRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"SlaveRx_task", 			//任务名字
                 (OS_TASK_PTR)slave_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )SLAVERX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&SLAVERX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)SLAVERX_TASK_STK[SLAVERX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)SLAVERX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				

		//创建从站发送任务
	OSTaskCreate((OS_TCB 		* )&SlaveTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"SlaveTx_task", 			//任务名字
                 (OS_TASK_PTR)slave_send_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )SLAVETX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&SLAVETX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)SLAVETX_TASK_STK[SLAVETX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)SLAVETX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值										 
								 
								 
		//创建从站启动管理任务
	OSTaskCreate((OS_TCB 		* )&SlaveStartupManagerTCB,				//任务控制块
								(CPU_CHAR	* )"SlaveStartManager_task", 			//任务名字
                 (OS_TASK_PTR)Slave_StartupManager_Take, 		//任务函数
                 (void		  *)0,														//传递给任务函数的参数
                 (OS_PRIO	   )SLAVE_STARTUPMANAGER_TASK_PRIO,    	//任务优先级
                 (CPU_STK   *)&SLAVE_STARTUPMANAGER_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)SLAVE_STARTUPMANAGER_TASK_STK[SLAVE_STARTUPMANAGER_STK_SIZE/10],	//任务堆栈深度限位
                 (CPU_STK_SIZE)SLAVE_STARTUPMANAGER_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值						 

		//创建从站管理其他设备的状态显示led任务
	OSTaskCreate((OS_TCB 		* )&CCULEDDeviceStatusTCB,				//任务控制块
								(CPU_CHAR	* )"CCULEDDeviceStatus_Take", 			//任务名字
                 (OS_TASK_PTR)CCULED_DeviceStatus_Take, 		//任务函数
                 (void		  *)0,														//传递给任务函数的参数
                 (OS_PRIO	   )CCULED_DEVICE_STATUS_TASK_PRIO,    	//任务优先级
                 (CPU_STK   *)&CCULED_DEVICE_STATUS_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CCULED_DEVICE_STATUS_TASK_STK[CCULED_DEVICE_STATUS_STK_SIZE/10],	//任务堆栈深度限位
                 (CPU_STK_SIZE)CCULED_DEVICE_STATUS_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值								 
								 

	OS_CRITICAL_EXIT();	//退出临界区							 
}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{

	/* 创建同步信号量 */ 
		BSP_OS_SemCreate(&SEM_SYNCH, 0,(CPU_CHAR *)"SEM_SYNCH");

}


extern u8 User_CAN1Transmit(uint32_t id,uint8_t* val,uint8_t Len);
extern u8 User_CAN2Transmit(uint32_t id,uint8_t* val,uint8_t Len);
extern unsigned char canInit(CAN_TypeDef* CANx,unsigned int bitrate);

///*EEPROM 测试*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define sEE_WRITE_ADDRESS1        0xA0
#define sEE_READ_ADDRESS1         0xA0
#define BUFFER_SIZE1             (countof(aTxBuffer1)-1)
#define BUFFER_SIZE2             (countof(aTxBuffer2)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)
/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t aTxBuffer1[] =  "/* STM32F40xx I2C Firmware Library EEPROM driver example: \
                        buffer 1 transfer into address sEE_WRITE_ADDRESS1 */ \
                        Example Description \
                        This firmware provides a basic example of how to use the I2C firmware library and\
                        an associate I2C EEPROM driver to communicate with an I2C EEPROM device (here the\
                        example is interfacing with M24C64 EEPROM)\
                          \
                        I2C peripheral is configured in Master transmitter during write operation and in\
                        Master receiver during read operation from I2C EEPROM. \
                          \
                        The peripheral used is I2C1 but can be configured by modifying the defines values\
                        in stm324xg_eval.h file. The speed is set to 100kHz and can be configured by \
                        modifying the relative define in stm32_eval_i2c_ee.h file.\
                         \
                        For M24C64 devices all the memory is accessible through the two-bytes \
                        addressing mode and need to define block addresses. In this case, only the physical \
                        address has to be defined (according to the address pins (E0,E1 and E2) connection).\
                        This address is defined in i2c_ee.h (default is 0xA0: E0, E1 and E2 tied to ground).\
                        The EEPROM addresses where the program start the write and the read operations \
                        is defined in the main.c file. \
                         \
                        First, the content of Tx1_Buffer is written to the EEPROM_WriteAddress1 and the\
                        written data are read. The written and the read buffers data are then compared.\
                        Following the read operation, the program waits that the EEPROM reverts to its \
                        Standby state. A second write operation is, then, performed and this time, Tx2_Buffer\
                        is written to EEPROM_WriteAddress2, which represents the address just after the last \
                        written one in the first write. After completion of the second write operation, the \
                        written data are read. The contents of the written and the read buffers are compared.\
                         \
                        All transfers are managed in DMA mode (except when 1-byte read/write operation is\
                        required). Once sEE_ReadBuffer() or sEE_WriteBuffer() function is called, the \
                        use application may perform other tasks in parallel while Read/Write operation is\
                        managed by DMA.\
                          \
                        This example provides the possibility to use the STM324xG-EVAL LCD screen for\
                        messages display (transfer status: Ongoing, PASSED, FAILED).\
                        To enable this option uncomment the define ENABLE_LCD_MSG_DISPLAY in the main.c\
                        file.                                                                              ";
uint8_t aTxBuffer2[] =  "/* STM32F40xx I2C Firmware Library EEPROM driver example: \
                        buffer 2 transfer into address sEE_WRITE_ADDRESS2 */";
uint8_t aRxBuffer1[BUFFER_SIZE1];
uint8_t aRxBuffer2[BUFFER_SIZE2];

volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
//volatile uint16_t uhNumDataRead = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

//extern void can_recv_thread(void);
//extern	void can_slave_init(void);

//#include "MTD_CPU_CAN3_MCP2515.h"
//#include "MTD_REP_CAN4_MCP2515.h"
//#include "MTD_REP_CAN3_MCP2515.h"

//#include "stm324xg_eval_i2c_ee.h"  //whh 冲突屏蔽掉


void Main_task(void *p_arg)
{
	//CPU_SR_ALLOC();	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//	uint64_t  a;
//	OS_ERR err;
//	static u8 SlaveStartCount;
	
//	static u8 can1StartCount;
//	static u8 can2StartCount;
//	static u8 can1Flag;
//	static u8 can2Flag=1;

	/*iap UART1*/
	u16 UartOldCount=0;	//老的串口接收数据值
	//u16 AppLenth=0;	//接收到的app代码长度	
	
	/*spi can*/
//		Frame_TypeDef frame;

//		frame.SID=3;
//		frame.DLC=8;
//		frame.Type=CAN_STD;
//	
//	(void)p_arg;
//	debugprintf("进入到Main任务\r\n");
	
	/*初始化spi can*/
//	MTDCPU_MCP2515_Init();

	/*蜂鸣器*/
//	bsp_StartTimer(0,2000);
//	Buzzer_Func_On();
//	delay_ms(2000);

	/*	RUNLED */	
//	bsp_StartAutoTimer(1,500);
	
	/*can test*/
//	canInit(CAN1,CAN_BAUD_125K);
//	canInit(CAN2,CAN_BAUD_125K);
	
	/*MTD CPU mcp2515*/
	
//	 MCP2515_REST1_GpioInit();
//	 MTDCPU_MCP2515_Init();
//	 MCP2515_INT1_GpioInit();

///*can3 初始化*/
//{
//	u8 res;

//	
//		res = MTDREP_CAN3_MCP2515_Init(MCP2515_CAN_BAUD_125K);
////		if(res == MCP2515_OK)
////			return 0;
////		else
////			return CAN_FAILINIT;
//}


/*can4 初始化*/
//	MCP2515_REST2_GpioInit();
//	MTDREP_CAN4_MCP2515_Init();
//	MCP2515_INT2_GpioInit();


/*CCULED 初始化*/
//CCULED_LEDGpio_Init();


/*EEPROM_2*/
//{
//  /*------------- Initialize sEE_DevStructure -------------*/
//  sEE_DevStructure.sEEAddress = EEPROM_ADDRESS;
//  sEE_DevStructure.sEEPageSize = EEPROM_PAGE_SIZE;
//  sEE_DevStructure.sEEMemoryAddrMode = sEE_OPT_16BIT_REG;

//  /*-------------- Initialize sEE peripheral -------------*/
//  sEE_StructInit(&sEE_DevStructure);
//  sEE_Init(&sEE_DevStructure); 
//	
//	  /*---------------------  Phase1 : Write data from Tx1_Buffer
//  then Read data in Rx1_Buffer -----------------------------*/
//  
//  /* Write Data in EEPROM */
//  sEE_WriteBuffer(&sEE_DevStructure, (uint8_t*)aTxBuffer1, sEE_WRITE_ADDRESS1, BUFFER_SIZE1);
//  
//  /* Wail until communication is complete */
//  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
//        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
//  { 
//    /* Application may perform other tasks while CPAL write operation is ongoing */
//  }	 
//  
//  /* Read Data from EEPROM */
//  sEE_ReadBuffer(&sEE_DevStructure,aRxBuffer1 , sEE_READ_ADDRESS1, BUFFER_SIZE1);
//  
//  /* Wail until communication is complete */
//  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
//        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
//  {
//    /* Application may perform other tasks while CPAL read operation is ongoing */
//  }
//  
//  /* if Error Occurred reset sEE state */
//  if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
//  {
//    sEE_DevStructure.sEEState = sEE_STATE_IDLE;
//  }
//  
//		TransferStatus1 = Buffercmp(aTxBuffer1, aRxBuffer1, BUFFER_SIZE1);

//		if (TransferStatus1 == PASSED)
//		{
//			printf(" Transfer 1 PASSED \r\n ");
//		}
//		else
//		{
//			printf(" Transfer 1 FAILED  \r\n");
//		} 
//  /*---------------------  Phase2 : Write data from Tx2_Buffer
//  then Read data in Rx2_Buffer -----------------------------*/
//  
//  /* Write Data in EEPROM */
//  sEE_WriteBuffer(&sEE_DevStructure, aTxBuffer2, sEE_WRITE_ADDRESS2, BUFFER_SIZE2);
//  
//  /* Wail until communication is complete */
//  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
//        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
//  { 
//    /* Application may perform other tasks while CPAL write operation is ongoing */	   
//  }
//  
//  /* Read Data from EEPROM */
//  sEE_ReadBuffer(&sEE_DevStructure, aRxBuffer2, sEE_READ_ADDRESS2, BUFFER_SIZE2);
//  
//  /* Wail until communication is complete */
//  while((sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_IDLE) && 
//        (sEE_GetEepromState(&sEE_DevStructure) != sEE_STATE_ERROR))
//  {
//    /* Application may perform other tasks while CPAL read operation is ongoing */      
//  }
//  
//  /* if Error Occurred reset sEE state */
//  if (sEE_DevStructure.sEEState == sEE_STATE_ERROR)
//  {
//    sEE_DevStructure.sEEState = sEE_STATE_IDLE;
//  }
//  /*----------------- Test transfer phase 1 -----------------*/
//  
//  if (Buffercmp ((uint8_t*)aTxBuffer2,aRxBuffer2,BUFFER_SIZE2) == PASSED )
//  {
//    /* Turn on LED1 if Transfer1 Passed */
//   printf(" Transfer 2 PASSED \r\n ");
//  }
//  else
//  {
//    /* Turn on LED3 if Transfer1 Failed */
//    printf(" Transfer 2 FAILED  \r\n");
//  }
//  
//  /* Deinitialize peripheral */
//  //sEE_DeInit(&sEE_DevStructure);
//}



//		/* Initialize the I2C EEPROM driver ----------------------------------------*/
//		sEE_Init();  
//		delay_ms(10);
//				/*EPPROM*/
//				OS_CRITICAL_ENTER();
//	{
//		u8 i;
//		/*EEPROM 测试*/
//		printf("1_EEPROM\r\n");
//		/* First write in the memory followed by a read of the written data --------*/
//		/* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
//		sEE_WriteBuffer(aTxBuffer1, sEE_WRITE_ADDRESS1, BUFFER_SIZE1); 
//printf("2_EEPROM\r\n");
//		/* Wait for EEPROM standby state */
//		sEE_WaitEepromStandbyState();  
//		printf("3_EEPROM\r\n");
//		/* Set the Number of data to be read */
//		uhNumDataRead = BUFFER_SIZE1;
//		printf("4_EEPROM\r\n");
//		/* Read from I2C EEPROM from sEE_READ_ADDRESS1 */
//		sEE_ReadBuffer(aRxBuffer1, sEE_READ_ADDRESS1, (uint16_t *)(&uhNumDataRead)); 

//printf("5_EEPROM\r\n");
//		/* Starting from this point, if the requested number of data is higher than 1, 
//			 then only the DMA is managing the data transfer. Meanwhile, CPU is free to 
//			 perform other tasks:
//		
//			// Add your code here: 
//			//...
//			//...

//			 For simplicity reasons, this example is just waiting till the end of the 
//			 transfer. */
//	 
//		printf(" Transfer 1 Ongoing \r\n");
//		 /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
//			resets the variable holding the number of data to be read) */
//		 while (uhNumDataRead > 0)
//		{}  
//			
//						for(i=0;i<BUFFER_SIZE1;i++)
//			{
//				printf("r1=%c",aRxBuffer1[i]);
//			}
//			printf("\r\n");
//			
//		 /* Check if the data written to the memory is read correctly */
//		TransferStatus1 = Buffercmp(aTxBuffer1, aRxBuffer1, BUFFER_SIZE1);

//		if (TransferStatus1 == PASSED)
//		{
//			printf(" Transfer 1 PASSED \r\n ");
//		}
//		else
//		{
//			printf(" Transfer 1 FAILED  \r\n");
//		} 		
//	 
//		/* Second write in the memory followed by a read of the written data -------*/
//		/* Write on I2C EEPROM from sEE_WRITE_ADDRESS2 */
//		sEE_WriteBuffer(aTxBuffer2, sEE_WRITE_ADDRESS2, BUFFER_SIZE2); 

//		/* Wait for EEPROM standby state */
//		sEE_WaitEepromStandbyState();  
//		
//		/* Set the Number of data to be read */
//		uhNumDataRead = BUFFER_SIZE2;  
//		
//		/* Read from I2C EEPROM from sEE_READ_ADDRESS2 */
//		sEE_ReadBuffer(aRxBuffer2, sEE_READ_ADDRESS2, (uint16_t *)(&uhNumDataRead));

//		printf(" Transfer 2 Ongoing \r\n");
//		 /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
//			resets the variable holding the number of data to be read) */
//		 while (uhNumDataRead > 0)
//		{}  
//			for(i=0;i<BUFFER_SIZE1;i++)
//			{
//				printf("r2=%c",aRxBuffer2[i]);
//			}
//			printf("\r\n");
//		 /* Check if the data written to the memory is read correctly */
//		TransferStatus1 = Buffercmp(aTxBuffer1, aRxBuffer1, BUFFER_SIZE1);

//		if (TransferStatus1 == PASSED)
//		{
//			printf(" Transfer 2 PASSED \r\n ");
//		}
//		else
//		{
//			printf(" Transfer 2 FAILED  \r\n");
//		} 
//	}
//			OS_CRITICAL_EXIT();//退出临界区


//初始化旋钮
CCULED_CanAdd_Gpio_Init();

while(1)
{
		RunLed_Turn();
	
	if(UART1_Rx_Cnt)
	{
		if(UartOldCount==UART1_Rx_Cnt)//新周期内,没有收到任何数据,认为本次数据接收完成.
		{
			//AppLenth=UART1_Rx_Cnt;
			UartOldCount=0;
			UART1_Rx_Cnt=0;
			printf("接受数据完成!\r\n");
			
			if((Uart1_Rx_Buff[0]=='u')&&(Uart1_Rx_Buff[1]=='p')&&(Uart1_Rx_Buff[2]=='d')&&(Uart1_Rx_Buff[3]=='a')&&(Uart1_Rx_Buff[4]=='t')&&(Uart1_Rx_Buff[5]=='a'))  //表示更新 则使用软件复位
			{
				__set_FAULTMASK(1);		// 关闭所有中断
				NVIC_SystemReset();		// 复位
			}
		}
		else UartOldCount=UART1_Rx_Cnt;	
	}	
	
//	/*旋钮测试*/
//	{
//		u8 ChooseBoard;
//		ChooseBoard =  CCULED_CanAddVal();
//		printf("LED_BO=0x%x\r\n",ChooseBoard);
//	}
	
	
//	if(	bsp_CheckTimer(0))
//	{
////		Buzzer_Func_Off();
//	}
	
//	if(bsp_CheckTimer(1))
//	{
////		RunLed_Turn();
//	}	
	
	
//		CCULED_LED_Turn(25);
//		CCULED_LED_Turn(24);
	
	/*CCU LED 闪烁*/
//	for(a=0;a<26;a++)
////	{
//		CCULED_LED_Turn(a);
//	}
	
	
	//	spi mcp2515 test
	
	/*MTDCPU*/
//	SPICAN_SendData(can2_send,&frame);
	
//	{
//		
//		Message a;
//		a.cob_id=0x08;
//		a.len =8;
//		memcpy(a.data,can2_send,8);	
//		
//		canSend((void *)CAN3,&a);
//	}
	
	/*can4*/
//	MTDREP_CAN4_SPICAN_SendData(can2_send,&frame);
	
//	
//		if(can2Flag ==1 )
//		{
//			can1StartCount++;
//			printf("can1发送第%d组\r\n",can1StartCount);
//			User_CAN1Transmit(0x01,can2_send,8);
//					
//			if(can1StartCount>=10)
//			{
//				can2Flag=0;
//				can1Flag=1;
//				can1StartCount =0;
//			}
//			delay_ms(100);
//		}
//		
//		if(can1Flag==1)
//		{
//			can2StartCount++;
//			printf("can2发送第%d组\r\n",can2StartCount);
//			User_CAN2Transmit(0x02,can2_send,8);
//			
//			
//			if(can2StartCount>=10)
//			{
//				can2Flag=1;
//				can1Flag=0;
//				can2StartCount=0;
//			}
//			delay_ms(100);
//		}
//	
	
	BSP_OS_TimeDlyMs(500);
}	
	
	
	
	
//	{

//		debugprintf("开启定时器3\r\n");
//		debugprintf("创建一个信号量\r\n");
//		
//			//创建一个从站启动管理的信号量
//		OSSemCreate ((OS_SEM*	)&Slave_Start_Sem,
//                 (CPU_CHAR*	)"Slave_Start_Sem",
//                 (OS_SEM_CTR)0,		
//                 (OS_ERR*	)&err);
//		
//			TIM3_Int_Init(20000,8400-1);
//	}

//TIM4_start();
//	{
//		
//		Message m;
//		m.cob_id=1;
//		m.rtr=0;
//		m.len=1;
//		m.data[0]=0xaa;
//		canInit(CAN1,CAN_BAUD_1M);
//		canInit(CAN2,CAN_BAUD_1M);
//		
//		canSend(CAN1,&m);
//		
//		m.cob_id=2;
//		canSend(CAN2,&m);
//	}




//	while(1)
//	{

////		if(KEY_S2_UPDOWM() == Bit_RESET)
////		{
////			LED_Turn(2);
////			debugprintf("MainEnd!\r\n");
////			{
////				door_T_1 +=1;
////				door_T_2 +=1;
////				door_M_1 +=1;
////				door_M_2 +=1;
////				door_T1_1 +=1;
////				door_T1_2 +=1;
////				door_T2_1 +=1;
////				door_T2_2 +=1;
////				door_mc2_1 +=1;
////				door_mc2_2 +=1;
////				door_mc1_1 +=1;
////				door_mc1_2 +=1;
////				sendPDOevent(&TestSlave_Data);
//////				sendOnePDOevent(&TestSlave_Data,10);
////				a=0x12345678912345;
////				debugprintf("64bita=%llx\r\n",a);
////				debugprintf("boolsize=%d\r\n",sizeof(_Bool));
////				debugprintf("g_Recei=%d\r\n",sizeof(g_RxCAN1HeartFlag));
////				
//////				printf("Master: %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx \r\n",
//////				Masterdoor_mc1_1,Masterdoor_mc1_2,Masterdoor_T_1,Masterdoor_T_2,Masterdoor_M_1,Masterdoor_M_2,Masterdoor_T1_1,
//////				Masterdoor_T1_2,Masterdoor_T2_1,Masterdoor_T2_2,Masterdoor_mc2_1,Masterdoor_mc2_2);
////	
////			}
////		}
		
////				//WHＨ
////		{
////			if(g_RxCAN1HeartFlag[2] == NoRxHeart)
////			{
////				printf("master no rx slave herat!\r\n");
////			}
////			
////			if(g_RxCAN2HeartFlag[1] == NoRxHeart)
////			{
////				printf("slave no rx master herat!\r\n");
////			}
////		}
		
		
//			OSSemPend(&Slave_Start_Sem,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
//			debugprintf("请求到从站启动管理信号量\r\n");
//			{
//				e_nodeState slavestate;
//				slavestate = getState(&TestSlave_Data);
//				debugprintf("slavestate=0x%x\r\n",slavestate);
//				
//				if(SlaveStartCount >=5)
//				{
//					TIM_Cmd(TIM3,DISABLE);
//				}
//				
//				if(slavestate != 0x05)
//				{
//					debugprintf("slavestate=0x%x\r\n",slavestate);
//					SlaveStartCount++;
//					setNodeId(&TestSlave_Data, 0x02);
//					setState(&TestSlave_Data, Initialisation);
//				}
//				else
//				{
//					TIM_Cmd(TIM3,DISABLE);
//				}
//			}
		
		
		
//	
//	{
//		
//		Message m;
//		m.cob_id=1;
//		m.rtr=0;
//		m.len=1;
//		m.data[0]=0xaa;

//		
//		canSend(CAN1,&m);
//		
//		m.cob_id=2;
//		canSend(CAN2,&m);
//	}

//		BSP_OS_TimeDlyMs(2000);			//调用这个延时是将其进入调度函数，发生任务的调度。
//	}

}


//void key_task(void *p_arg)		 	//用来显示任务堆栈使用情况，已经cpu使用
//{
//	OS_ERR err;
//	uint8_t ucKeyCode;
//	int64_t runtime; 
//	ucKeyCode=0;
//	(void)p_arg;	               /* 避免编译器报警 */
//	printf("进入到key任务\r\n");
//	while(1)
//	{		
//		if(KEY_S1_UPDOWM() == Bit_RESET)
//		{
//			printf("进入到按键任务\r\n");
//			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);//延时1s	//包含有延时去抖
//			ucKeyCode	= KEY_Scan(0);	//再次确认按键状态
//			if(KEY_S1_UPDOWM() == Bit_RESET)	
//			{
//				ucKeyCode=1;
//			}
//		}
//		
//		if(ucKeyCode==1)
//		{
//			ucKeyCode=0;
//			DispTaskInfo();		//打印显示信息			
//			runtime=OSTimeGet(&err);
//			printf("runtime=%lld\r\n",runtime);
//			printf("KeyEnd!\r\n");
//		}
//		BSP_OS_TimeDlyMs(2000);//延时2s
//	}
//}

//void TemperCheck_task(void *p_arg)			//温度检测任务函数，用来检测驱动板的温度，并实施报警
//{

//	(void )p_arg;			//防止警告
//	LEDGpio_Init();
//	ADC1_CH16_Config();
//	printf("进入到Temper任务\r\n");	

//	while(1)
//	{
//		if(KEY_S1_UPDOWM() == Bit_RESET)
//		{
//			Get_Temprate();		//stm32内部的检测温度测试 ，利用ADC转换温度
//			LED_Turn (0);
//		}
//		BSP_OS_TimeDlyMs(3000);//延时2s	,并且进行任务间的切换
//	}
//}


void Slave_StartupManager_Take(void *p_arg)
{
	OS_ERR err;
	static u8 SlaveStateCount =0;
	debugprintf("进入到从站启动管理任务！\r\n");
	(void )p_arg;			//防止警告
	while(1)
	{
		debugprintf("slave manager\r\n");
		
		if(SlaveStateCount > 5)		//连续发送5次从节点到初始化状态，发送bootup
		{
			debugprintf("slave manager 5次到\r\n");
			//	SlaveStateCount = 0;
			OSTaskDel((OS_TCB*)&SlaveStartupManagerTCB,&err);	//任务从站管理任务函数执行5此后删除掉这个任务
		}						

		{
			e_nodeState slavestate;
			slavestate = getState(&Ccu1LedObjDict_Data);
			debugprintf("slavestate=0x%x\r\n",slavestate);
			if(slavestate != Operational/* 0x05*/)
			{
				debugprintf("slavestate=0x%x\r\n",slavestate);
				SlaveStateCount++;
				setNodeId(&Ccu1LedObjDict_Data, 0x01);
				setState(&Ccu1LedObjDict_Data, Initialisation);
			}
	
			else
			{
				OSTaskDel((OS_TCB*)&SlaveStartupManagerTCB,&err);	//任务从站状态位操作状态后删除掉这个任务
			}
		}
		BSP_OS_TimeDlyMs(2000);			//调用这个延时是将其进入调度函数，发生任务的调度。
		
	}

}

extern u32  CCULED_DataCombin(void);
void CCULED_DeviceStatus_Take(void *p_arg)
{
	u32 StateData;
	u8 i;
	(void )p_arg;			//防止警告
	
	debugprintf("cculed device take! \r\n");
	while(1)
	{	
		/*W h h*/
//		CCULED_LED_Turn(0);
//		CCULED_LED_Turn(1);
//		HeartMap_CAN1_EMR1 =1;
//		HeartMap_CAN2_EMR1 =1;
		
			debugprintf("cculed!\r\n");
		
		StateData = CCULED_DataCombin();
		
		for(i=0;i<CCULED_LED_NUMBER;i++)		//循环判断
		{
			if(StateData & 0x01)
			{
		//				CCULED_LED_Turn(i);
				CCULED_LED_On(i);//修改为常亮
			}
			else
			{
		//				CCULED_LED_Off(i);
				CCULED_LED_Off(i);//修改为常灭
			}
			StateData >>= 1;
		}
		
		// 清除接受到的数据
			HeartMap_CAN1_CANBC = 0;
			HeartMap_CAN1_CCU1 = 0;
			HeartMap_CAN1_CCU2 = 0;
			HeartMap_CAN1_MTD1 = 0;	//MC1_MTD
			HeartMap_CAN1_MTD2 = 0;	//T_MTD
			HeartMap_CAN1_MTD3 = 0;	//M_MTD
			HeartMap_CAN1_MTD4 = 0;	//T1_MTD
			HeartMap_CAN1_MTD5 = 0;	//T2_MTD
			HeartMap_CAN1_MTD6 = 0;	//MC2_MTD
			HeartMap_CAN1_DDU1 = 0;	//DDU1
			HeartMap_CAN1_DDU2 = 0;	//DDU2
			HeartMap_CAN1_EMR1 = 0;	//ERM1
			HeartMap_CAN1_EMR2 = 0;	//ERM2
		
			HeartMap_CAN2_CANBC = 0;
			HeartMap_CAN2_CCU1 = 0;
			HeartMap_CAN2_CCU2 = 0;
			HeartMap_CAN2_MTD1 = 0;	//MC1_MTD
			HeartMap_CAN2_MTD2 = 0;	//T_MTD
			HeartMap_CAN2_MTD3 = 0;	//M_MTD
			HeartMap_CAN2_MTD4 = 0;	//T1_MTD
			HeartMap_CAN2_MTD5 = 0;	//T2_MTD
			HeartMap_CAN2_MTD6 = 0;	//MC2_MTD
			HeartMap_CAN2_DDU1 = 0;	//DDU1
			HeartMap_CAN2_DDU2 = 0;	//DDU2
			HeartMap_CAN2_EMR1 = 0;	//ERM1
			HeartMap_CAN2_EMR2 = 0;	//ERM2
		
		BSP_OS_TimeDlyMs(500);//延时500ms	,并且进行任务间的切换,让led灯闪烁
	}
	
}

void assert_failed(u8* file, u32 line)
{
    debug("fail");
		while (1) ;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}



