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

#define CAN_SYNC_NETWORK_STA_MAP	0x5FFE

static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

/*任务创建定义变量的步骤*/

/*LED点阵显示任务*/
	//MainTask  任务优先级
#define Main_TASK_PRIO		4
	//任务堆栈大小	
#define Main_STK_SIZE 		256
	//任务控制块
static OS_TCB MainTaskTCB;
	//任务堆栈	
static CPU_STK Main_TASK_STK[Main_STK_SIZE];
	//任务函数
static void Main_task(void *p_arg);


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


/*按键显示cpu,堆栈信息的任务*/
	//KEY 按键任务优先级
#define KEY_TASK_PRIO		20
	//任务堆栈大小	
#define KEY_STK_SIZE 		128
	//任务控制块
static OS_TCB KeyTaskTCB;
	//任务堆栈	
static CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
	//任务函数
static void key_task(void *p_arg);

/*CAN3主站发送其他设备的心跳任务*/
	//任务优先级
#define CAN3_CHANGE_HEART_TASK_PRIO		5
	//任务堆栈大小	
#define CAN3_CHANGE_HEART_STK_SIZE 		512
	//任务控制块
static OS_TCB Can3ChangeHeartTCB;
	//任务堆栈	
static CPU_STK  CAN3_CHANGE_HEART_TASK_STK[CAN3_CHANGE_HEART_STK_SIZE];
	//任务函数
void Can3ChangeHeart_Take(void *p_arg);

/*主站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN1MASTERRX_TASK_PRIO		8
	//任务堆栈大小	
#define CAN1MASTERRX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN1MasterRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN1MASTERRX_TASK_STK[CAN1MASTERRX_STK_SIZE];
	//任务函数
extern void can1master_recv_thread(void *p_arg);

/*主站发送任务*/
	//任务优先级
#define CAN1MASTERTX_TASK_PRIO		9
	//任务堆栈大小	
#define CAN1MASTERTX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN1MasterTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN1MASTERTX_TASK_STK[CAN1MASTERTX_STK_SIZE];
	//任务函数
extern void can1master_send_thread(void *p_arg);


/*主站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN2MASTERRX_TASK_PRIO		10
	//任务堆栈大小	
#define CAN2MASTERRX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN2MasterRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2MASTERRX_TASK_STK[CAN2MASTERRX_STK_SIZE];
	//任务函数
extern void can2master_recv_thread(void *p_arg);

/*主站发送任务*/
	//任务优先级
#define CAN2MASTERTX_TASK_PRIO		11
	//任务堆栈大小	
#define CAN2MASTERTX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN2MasterTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2MASTERTX_TASK_STK[CAN2MASTERTX_STK_SIZE];
	//任务函数
extern void can2master_send_thread(void *p_arg);


/*主站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN3MASTERRX_TASK_PRIO		6
	//任务堆栈大小	
#define CAN3MASTERRX_STK_SIZE 		512
	//任务控制块
OS_TCB CAN3MasterRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERRX_TASK_STK[CAN3MASTERRX_STK_SIZE];
	//任务函数
extern void can3master_recv_thread(void *p_arg);

/*主站发送任务*/
	//任务优先级
#define CAN3MASTERTX_TASK_PRIO		 7
	//任务堆栈大小	
#define CAN3MASTERTX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN3MasterTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERTX_TASK_STK[CAN3MASTERTX_STK_SIZE];
	//任务函数
extern void can3master_send_thread(void *p_arg);


/*can1 网络监控任务*/
	//任务优先级
#define CAN1NETWORKMONITOR_TASK_PRIO		14
	//任务堆栈大小	
#define CAN1NETWORKMONITOR_STK_SIZE 		512
	//任务控制块
OS_TCB CAN1NetWorkMonitorTaskTCB;
	//任务堆栈	
static CPU_STK CAN1NetWorkMonitor_TASK_STK[CAN1NETWORKMONITOR_STK_SIZE];
	//任务函数
void CAN1NetWorkMonitorTask(void *p_arg);

/*can2 网络监控任务*/
	//任务优先级
#define CAN2NETWORKMONITOR_TASK_PRIO		15
	//任务堆栈大小	
#define CAN2NETWORKMONITOR_STK_SIZE 		512
	//任务控制块
OS_TCB CAN2NetWorkMonitorTaskTCB;
	//任务堆栈	
static CPU_STK CAN2NetWorkMonitor_TASK_STK[CAN2NETWORKMONITOR_STK_SIZE];
	//任务函数
void CAN2NetWorkMonitorTask(void *p_arg);

/*can 查询can1，can2同时坏，突然另一条有好的触发发送任务		*/
	//任务优先级
#define CANQUERY_TASK_PRIO		16
	//任务堆栈大小	
#define CANQUERY_STK_SIZE 		256
	//任务控制块
OS_TCB CANQueryTaskTCB;
	//任务堆栈	
static CPU_STK CANQuery_TASK_STK[CANQUERY_STK_SIZE];
	//任务函数
void CANQueryTask(void *p_arg);


/*任务对象的创建*/
static  OS_SEM     	SEM_SYNCH;	   	//用于同步
OS_SEM     	Slave_Start_Sem;				//从站启动管理的信号量
OS_MUTEX	CAN3_MUTEX;			 					//定义一个互斥信号量,用于can3发送接受任务的互斥

/*定时器的定义*/
OS_TMR Can3ErrResetTmr;	//can3错误中断复位定时器
extern void Can3ErrResetTmrTrmCallBack(void *p_tmr, void *p_arg);

/*canfestival 接受心跳报文的定义*/
IsNoReceHeart			 g_RxCAN1HeartFlag[15]={NoRxHeart};					//存放各个设备的心跳的标志，下标就是节点的ID号，故下标为0的数组没有使用。
IsNoReceHeart			 g_RxCAN2HeartFlag[15]={NoRxHeart};

//CurReceHeartState  g_RxCAN1HeartStateFlag[15] = {0};		//存放当前接受心跳时，从设备所在的状态，是上电刚初始化的时候，还是建立的连接的时候		
//CurReceHeartState  g_RxCAN2HeartStateFlag[15] = {0};

RxCanMessageState  g_RxCAN1MesStateFlag[15]={NoRxCanMesState};			//存放当前主站接到从站的can消息 所处的状态 
RxCanMessageState	 g_RxCAN2MesStateFlag[15]={NoRxCanMesState};

CCUCANState 	CCU1SwitchState;			//CCU1 工作还是CCU2工作
CCUCANState   CCU2SwitchState;			//ccu1与ccu2的状态一定要互斥

CurrentUserState CurrentUseCAN1 = UseState;			/*表示当前使用can1总线，还是can2总线，默认使用can1网络*/
CurrentUserState CurrentUseCAN2 = NoUseState;	

u8 CAN1_Heart;			/*表示ccu1 或者 的can1接收到其他设备的心跳，表示自己能正常通信*/
u8 CAN2_Heart;			

u8 ChooseBoard=0;			//这个全局变量用来接受旋钮的值，从而选择板子
extern uint8_t UART1_Rx_Cnt;
//u32 g_SlaveTxCount=0;  //can2的发送标志


/*
	由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
	启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

	系统时钟缺省配置为168MHz，如果需要更改，可以修改 system_stm32f4xx.c 文件
*/
int main(void)
{
		OS_ERR  err;

		CPU_SR_ALLOC();																	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
	
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000); //重新映射中断向量表
		SCB->VTOR = FLASH_BASE | 0x10000;	//设置中断向量表	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
		
		//uart1的GPIO初始化 ,提前的初始化，防止中间有打印显示信息会无法输出
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
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
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

#include "iap.h"
		/*开始任务*/
static  void  AppTaskStart (void *p_arg)
{			
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

	__disable_irq();   // 关闭总中断
	//任务的创建
  AppTaskCreate();                                            /* Create Application tasks                             */
	__enable_irq();    // 开启总中断

	/*打印任务信息*/
	delay_ms(1000);			//进行任务的调度，下面的函数是打印显示任务所有信息
	DispTaskInfo();			//打印完后，AppTaskStart任务就结束了，因为没有while();
	IAP_Init(&CCUCAN2Master_Data,&CCUCAN3Master_Data,MTDCCU_CanAddVal(),0x161213);
	
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
                 (OS_TASK_PTR)Main_task, 			//任务函数
                 (void		  *)0,					//传递给任务函数的参数
                 (OS_PRIO	   )Main_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&Main_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)Main_TASK_STK[Main_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)Main_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/,		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值
								 
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
								 				 
	//创建KEY任务
	OSTaskCreate((OS_TCB 		* )&KeyTaskTCB,			//任务控制块
								(CPU_CHAR	* )"key_task", 			//任务名字
                 (OS_TASK_PTR)key_task, 			//任务函数
                 (void		  *)0,					//传递给任务函数的参数
                 (OS_PRIO	   )KEY_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&KEY_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)KEY_TASK_STK[KEY_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)KEY_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值
							 								 
		//创建CAN3主站发送其他设备的心跳任务
	OSTaskCreate((OS_TCB 		* )&Can3ChangeHeartTCB,				//任务控制块
								(CPU_CHAR	* )"Can3ChangeHeart_task", 			//任务名字
                 (OS_TASK_PTR)Can3ChangeHeart_Take, 		//任务函数
                 (void		  *)0,														//传递给任务函数的参数
                 (OS_PRIO	   )CAN3_CHANGE_HEART_TASK_PRIO,    	//任务优先级
                 (CPU_STK   *)&CAN3_CHANGE_HEART_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3_CHANGE_HEART_TASK_STK[CAN3_CHANGE_HEART_STK_SIZE/10],	//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3_CHANGE_HEART_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
								 
								 
		//创建can1主站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN1MasterRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN1MasterRx_task", 			//任务名字
                 (OS_TASK_PTR)can1master_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN1MASTERRX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN1MASTERRX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1MASTERRX_TASK_STK[CAN1MASTERRX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1MASTERRX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				

		//创建主站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN1MasterTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN1MasterTx_task", 			//任务名字
                 (OS_TASK_PTR)can1master_send_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN1MASTERTX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN1MASTERTX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1MASTERTX_TASK_STK[CAN1MASTERTX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1MASTERTX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			


		//创建can2主站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN2MasterRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN2MasterRx_task", 			//任务名字
                 (OS_TASK_PTR)can2master_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN2MASTERRX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN2MASTERRX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2MASTERRX_TASK_STK[CAN2MASTERRX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2MASTERRX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				

		//创建主站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN2MasterTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN2MasterTx_task", 			//任务名字
                 (OS_TASK_PTR)can2master_send_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN2MASTERTX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN2MASTERTX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2MASTERTX_TASK_STK[CAN2MASTERTX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2MASTERTX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			


		//创建can3主站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN3MasterRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN3MasterRx_task", 			//任务名字
                 (OS_TASK_PTR)can3master_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN3MASTERRX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN3MASTERRX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3MASTERRX_TASK_STK[CAN3MASTERRX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3MASTERRX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				

		//创建主站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN3MasterTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN3MasterTx_task", 			//任务名字
                 (OS_TASK_PTR)can3master_send_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN3MASTERTX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN3MASTERTX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3MASTERTX_TASK_STK[CAN3MASTERTX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3MASTERTX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			

  /*can1 网络监控任务*/
	OSTaskCreate((OS_TCB 		* )&CAN1NetWorkMonitorTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN1NetWorkMonitor_Task", 			//任务名字
                 (OS_TASK_PTR)CAN1NetWorkMonitorTask, 				//任务函数
                 (void		  *)0,															//传递给任务函数的参数
                 (OS_PRIO	   )CAN1NETWORKMONITOR_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CAN1NetWorkMonitor_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1NetWorkMonitor_TASK_STK[CAN1NETWORKMONITOR_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1NETWORKMONITOR_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值										 

  /*can2 网络监控任务*/
	OSTaskCreate((OS_TCB 		* )&CAN2NetWorkMonitorTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN2NetWorkMonitor_Task", 			//任务名字
                 (OS_TASK_PTR)CAN2NetWorkMonitorTask, 				//任务函数
                 (void		  *)0,															//传递给任务函数的参数
                 (OS_PRIO	   )CAN2NETWORKMONITOR_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CAN2NetWorkMonitor_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2NetWorkMonitor_TASK_STK[CAN2NETWORKMONITOR_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2NETWORKMONITOR_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			

	/*
	 can 查询can1，can2同时坏，突然另一条有好的触发发送任务										
	*/
	OSTaskCreate((OS_TCB 		* )&CANQueryTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CANQuery_Task", 			//任务名字
                 (OS_TASK_PTR)CANQueryTask, 				//任务函数
                 (void		  *)0,															//传递给任务函数的参数
                 (OS_PRIO	   )CANQUERY_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CANQuery_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CANQuery_TASK_STK[CANQUERY_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CANQUERY_STK_SIZE,		//任务堆栈大小
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
	OS_ERR err;
	
	/* 创建同步信号量 */ 
	BSP_OS_SemCreate(&SEM_SYNCH, 0,(CPU_CHAR *)"SEM_SYNCH");
	
	//创建一个互斥信号量
	OSMutexCreate((OS_MUTEX*	)&CAN3_MUTEX,
								(CPU_CHAR*	)"CAN3_MUTEX",
                (OS_ERR*		)&err);
	
	//创建一个接受信号量
	OSSemCreate ((OS_SEM*	)&can3recv_sem,
                 (CPU_CHAR*	)"can3recvsem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);

		//创建定时器 can3错误中断复位定时器
		OSTmrCreate((OS_TMR		*)&Can3ErrResetTmr,		//定时器1
								(CPU_CHAR	*)"Can3ErrResetTmr",	//定时器名字
								(OS_TICK	 )0,									//初始化定时器的延时值
								(OS_TICK	 )1,   							  //1*100=100ms  重复周期
								(OS_OPT		 )OS_OPT_TMR_ONE_SHOT,	 		//周期模式
								(OS_TMR_CALLBACK_PTR)Can3ErrResetTmrTrmCallBack,	//定时器1回调函数
								(void	    *)0,					//回调函数参数为0
								(OS_ERR	  *)&err);			//返回的错误码				

}


static  void CCUSyncStatus_SDO_Callback(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;
	UNS32 data=0;
	UNS32 size=sizeof(UNS8);
	
	//定义CCU工作状态的变量
	UNS8  CCUMSStatus = 0;
	UNS8  CCUCanWorkSta = 0;

	UNS32 ret = getReadResultNetworkDict(d, nodeId,&data, &size, &abortCode);
	if( ret != SDO_FINISHED) //通讯异常
	{
		printf("Result : Failed in getting information for slave %2.2x, AbortCode :%4.4x \r\n", nodeId, (unsigned int)abortCode);
	}
	else
	{
		printf("Result : %#lx\r\n",data);
		CCUMSStatus = (data>>4) &0x0F;
		CCUCanWorkSta = data&0x0F;
		
		if(ChooseBoard == CCU1_NODEID)   //CCU1
		{
			if/*(CCU1SwitchState == ChooseOperate) &&*/ (CCUMSStatus == 2)  //如CCU2为为主时，判为CCU2抢到主 .
			{
				CCU1SwitchState = ChooseNotOperate;
				CCU2SwitchState = ChooseOperate;
				
				if(CCUCanWorkSta == 1)   //当前使用can1
				{
					CurrentUseCAN1 = UseState;
					CurrentUseCAN2 = NoUseState;
				}	
				else if(CCUCanWorkSta == 2)
				{
					CurrentUseCAN1 = NoUseState;
					CurrentUseCAN2 = UseState;	
				}
				else
				{
					printf("CCU1 can work status Err!\r\n");
				}
			}
		}
		else if(ChooseBoard == CCU2_NODEID)  //CCU2
		{
			if((CCUMSStatus == 1) &&(CCU2SwitchState == ChooseNotOperate)) //如果CCU1为主，并且CCU2自己为从
			{
				CCU1SwitchState = ChooseOperate;
				CCU2SwitchState = ChooseNotOperate;
							
				if(CCUCanWorkSta == 1)   //当前使用can1
				{
					CurrentUseCAN1 = UseState;
					CurrentUseCAN2 = NoUseState;
				}	
				else if(CCUCanWorkSta == 2)
				{
					CurrentUseCAN1 = NoUseState;
					CurrentUseCAN2 = UseState;	
				}
				else
				{
					printf("CCU2 can work status Err!\r\n");
				}
			}
		}
	}
	
	/* Finalize last SDO transfer with this node */
	closeSDOtransfer(d, nodeId, SDO_CLIENT);
}	


void Main_task(void *p_arg)
{
//	OS_ERR err;
	
	/*iap UART1*/
	u16 UartOldCount=0;	//老的串口接收数据值
	//u16 AppLenth=0;	//接收到的app代码长度
		
	/*用于ccu判断是否接收到所有心跳计数*/
	uint8_t	can1Count=0;
	uint8_t can2Count=0;
	static	uint8_t CCUdelayCount=0;
	uint8_t i;
	
	/*用于将计算主从状态，can1，can2工作状态，记录到映射对象字典0x5FFE中*/
	u8 CCUMasterSlaveSta = 0;   //1:表示CCU1为主, 2：表示CCU2为主
	u8 CCUCanWorkSta = 0;       //1：表示can1工作中，2：表示can2工作中。
	
	/*用于SDO上电2秒后传输1次*/
	static u8 SDO2sCount = 0;
	
	(void)p_arg;
	debugprintf("Enter Main Task!\r\n");
		
while(1)
{
	/*1.运行灯每500ms翻转一次*/
	RunLed_Turn();
	
	/*2.IAP串口接受数据*/
	if(UART1_Rx_Cnt)
	{
		if(UartOldCount==UART1_Rx_Cnt)//新周期内,没有收到任何数据,认为本次数据接收完成.
		{
			UartOldCount=0;
			UART1_Rx_Cnt=0;
			//printf("Rx Uart1 Data End!\r\n");
			
			if((Uart1_Rx_Buff[0]=='u')&&(Uart1_Rx_Buff[1]=='p')&&(Uart1_Rx_Buff[2]=='d')&&(Uart1_Rx_Buff[3]=='a')&&(Uart1_Rx_Buff[4]=='t')&&(Uart1_Rx_Buff[5]=='a'))  //表示更新 则使用软件复位
			{
				__set_FAULTMASK(1);		// 关闭所有中断
				NVIC_SystemReset();		// 复位
			}
		}
		else UartOldCount=UART1_Rx_Cnt;	
	}
	
	
	/*3.ccu主设备，从设备 指示灯。自己判断两路数据都接受不到将自己置为从设备*/
	CCUdelayCount++;
	if(CCUdelayCount >=6) //每次延时500ms，则总共延时了3s
	{
		for(i=3;i<10/*12*/;i++)	//计算是否能接受到其他设备的心跳
		{
			if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) 	
			{
				can1Count++;
			}
			
			if(g_RxCAN2HeartFlag[i] == NoRxHeart) 
			{
				can2Count++;
			}		
		}

		if((7==can1Count) && (7==can2Count))		//can1,can2 线收不到所有设备的心跳
		{
			if(CCU1 == CCU1_NODEID)		//CCU1 设备
			{
				if(CCU1SwitchState == ChooseOperate)	//如果ccu1当前为主控
				{
					CCU1SwitchState = ChooseNotOperate;
					CCU2SwitchState = ChooseOperate;
				}
			}
			else if(CCU1 == CCU2_NODEID )
			{
				if(CCU2SwitchState == ChooseOperate)		//如果ccu2为主控
				{
					CCU1SwitchState = ChooseOperate;
					CCU2SwitchState = ChooseNotOperate;
				}
			}
		}

			
		CCUdelayCount =0;
		can1Count =0;
		can2Count =0;
	}
	
	if(CCU1 == CCU1_NODEID)		//CCU1 设备
	{
		if(CCU1SwitchState == ChooseOperate)		//CCU1为主控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_ON);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_OFF);
			printf("C1 Device CCU1 Master!\r\n");
		}
		else			//CCU1 为副控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_OFF);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_ON);			
			printf("C1 Device CCU2 Master!\r\n");
		}
	}
	
	else		//表示为CCU2设备
	{
		if(CCU2SwitchState == ChooseOperate)		//CCU2为主控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_ON);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_OFF);
			printf("C2 Device CCU2 Master!\r\n");
		}
		else			//CCU1 为副控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_OFF);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_ON);	
			printf("C2 Device CCU1 Master \r\n");
		}		
	}
	
	
	/*4. 将CUU主从状态,以及can1，can2网络工作状态写入到0x5FFE 映射对象字典中*/
	/*4.1.判断can网络设备通信状态，主从状态，can网络工作状态*/
		/*（1设备通信状态）*/
	for(i=1;i<=12;i++)	//当前屏蔽事件记录仪
	{
		if((ChooseBoard == CCU1_NODEID) && (i==1))    //CCU1 自己判断自己的心跳
		{
			if((CAN1_Heart == RxHeart) || (CAN2_Heart == RxHeart)) 
			{
				can1_CCUTxStatu_SubStatu1 |=(1<<(7-(i-1)));
				can2_CCUTxStatu_SubStatu1 |=(1<<(7-(i-1)));
				
				continue;
			}
		}
		else if((ChooseBoard == CCU2_NODEID ) && (i==2))  //CCU2 自己判断自己的心跳
		{
		 		can1_CCUTxStatu_SubStatu1 |=(1<<(7-(i-1)));
				can2_CCUTxStatu_SubStatu1 |=(1<<(7-(i-1)));
			
				continue;
		}
		else
		{
			can1_CCUTxStatu_SubStatu1 &= (~(1<<(7-(i-1)))); 
			can2_CCUTxStatu_SubStatu1 &= (~(1<<(7-(i-1))));	
		}
		
		
		/*判断其他id*/
		if((g_RxCAN1HeartFlag[i]	==  RxHeart) || (g_RxCAN2HeartFlag[i] == RxHeart))   //表示设备通信正常
		{
			//置1
			if(i<9)   //SubStatu1
			{
			  can1_CCUTxStatu_SubStatu1 |= (1<<(7-(i-1))); 
				can2_CCUTxStatu_SubStatu1 |= (1<<(7-(i-1)));
			}
			else      //SubStatu2
			{
			  can1_CCUTxStatu_SubStatu2 |= (1<<(15-(i-1))); 
				can2_CCUTxStatu_SubStatu2 |= (1<<(15-(i-1))); 
			}
		}
		else  //通信异常
		{
			//清0
			if(i<9)   //SubStatu1
			{
			  can1_CCUTxStatu_SubStatu1 &= (~(1<<(7-(i-1)))); 
				can2_CCUTxStatu_SubStatu1 &= (~(1<<(7-(i-1)))); 
			}
			else      //SubStatu2
			{
			  can1_CCUTxStatu_SubStatu2 &= (~(1<<(15-(i-1)))); 
				can2_CCUTxStatu_SubStatu2 &= (~(1<<(7-(i-1)))); 
			}			
		}
	}	
		
	if((CCU1SwitchState == ChooseOperate) && (CCU2SwitchState == ChooseNotOperate))  //CCU1主控
	{
	  CCUMasterSlaveSta = 1;
		
		can1_CCUTxStatu_CCU1MSStatu = 1;
		can2_CCUTxStatu_CCU1MSStatu = 1;
		
		can1_CCUTxStatu_CCU2MSStatu = 2;
		can2_CCUTxStatu_CCU2MSStatu = 2;			
	}
	else if ((CCU1SwitchState == ChooseNotOperate) && (CCU2SwitchState == ChooseOperate))  //CCU2主控
	{
	  CCUMasterSlaveSta = 2;
		
		can1_CCUTxStatu_CCU1MSStatu = 2;
		can2_CCUTxStatu_CCU1MSStatu = 2;	
		
		can1_CCUTxStatu_CCU2MSStatu = 1;
		can2_CCUTxStatu_CCU2MSStatu = 1;				
	}
	else       //bug  //两个从，两个主 都是为故障
	{
	  CCUMasterSlaveSta = 0xF;  

		can1_CCUTxStatu_CCU2MSStatu = 0;
		can2_CCUTxStatu_CCU2MSStatu = 0;
																	
		can1_CCUTxStatu_CCU1MSStatu = 0;
		can2_CCUTxStatu_CCU1MSStatu = 0;	
		
		printf("CCU Master Slave Sta Err！\r\n");
	}
	
	if((CurrentUseCAN1 == UseState) && (CurrentUseCAN2 == NoUseState))  //当前使用can1
	{
		CCUCanWorkSta = 1;
		
		can1_CCUTxStatu_CANLineWork = 1;
		can2_CCUTxStatu_CANLineWork = 1;		
	}
	else if((CurrentUseCAN1 == NoUseState) && (CurrentUseCAN2 == UseState))   //当前使用can2
	{
		CCUCanWorkSta = 2;
		
		can1_CCUTxStatu_CANLineWork = 2;
		can2_CCUTxStatu_CANLineWork = 2;		
	}
	else       //bug
	{
	  CCUCanWorkSta = 0xF;  

		can1_CCUTxStatu_CANLineWork = 0;
		can2_CCUTxStatu_CANLineWork = 0;		
		printf("CCU Master Slave Sta Err！\r\n");
	}	
	
	/*赋值到0x5FFE映射的对象字典中*/
	Can1_CCUNetworkSta = (CCUMasterSlaveSta <<4) |  CCUCanWorkSta;
	
	Can2_CCUNetworkSta = (CCUMasterSlaveSta <<4) |  CCUCanWorkSta;
	
   /*主控发送PDO到can网络上*/
	if((ChooseBoard == CCU1_NODEID) && (CCUMasterSlaveSta ==1 ))    //当前设备是CCU1 并且是主控 发送PDO
	{
		CCUCAN1Master_Data.PDO_status[0].last_message.cob_id = 0;			
		sendOnePDOevent(&CCUCAN1Master_Data,0);		//pdo 0x1E0		
		
		CCUCAN2Master_Data.PDO_status[0].last_message.cob_id = 0;			
		sendOnePDOevent(&CCUCAN2Master_Data,0);		//pdo 0x1E0			
	}
	else if ((ChooseBoard == CCU2_NODEID) && (CCUMasterSlaveSta ==2 ))    //当前设备是CCU2 并且是主控 发送PDO
	{
		CCUCAN1Master_Data.PDO_status[0].last_message.cob_id = 0;			
		sendOnePDOevent(&CCUCAN1Master_Data,0);		//pdo 0x1E0		
		
		CCUCAN2Master_Data.PDO_status[0].last_message.cob_id = 0;			
		sendOnePDOevent(&CCUCAN2Master_Data,0);		//pdo 0x1E0	
	}		
	
	
	/*5.上电初始化2s 之后开始发送一帧SDO数据去检测 另一个CCU的can网络的通信状态*/

	if(SDO2sCount ==4)
	{
		UNS32 val = 0;
		UNS32 size = 0; 
		
		SDO2sCount = 0xFF;  //只传输一次SDO
		
		if(ChooseBoard == CCU1_NODEID)
		{
			//can1 开始传输SDO
			val = 0x600 | CCU2_NODEID ;
			size = sizeof(UNS32); 
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x01,&val,&size,RW);   //修改客户端TSDO node id
			val = 0x580 | CCU2_NODEID;
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x02,&val,&size,RW);   //修改客户端RSDO node id
			val = CCU2_NODEID;
			size = sizeof(UNS8);
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x03,&val,&size,RW);   //修改客户端的服务器 node id

			readNetworkDictCallback(&CCUCAN1Master_Data,CCU2_NODEID,CAN_SYNC_NETWORK_STA_MAP,0x00,0,&CCUSyncStatus_SDO_Callback,0);   //读取0x5FFE，0x00 中的数据
			
			//can2 开始传输SDO
			val = 0x600 | CCU2_NODEID ;
			size = sizeof(UNS32); 
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x01,&val,&size,RW);   //修改客户端TSDO node id
			val = 0x580 | CCU2_NODEID;
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x02,&val,&size,RW);   //修改客户端RSDO node id
			val = CCU2_NODEID;
			size = sizeof(UNS8);
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x03,&val,&size,RW);   //修改客户端的服务器 node id

			readNetworkDictCallback(&CCUCAN2Master_Data,CCU2_NODEID,CAN_SYNC_NETWORK_STA_MAP,0x00,0,&CCUSyncStatus_SDO_Callback,0);   //读取0x5FFE，0x00 中的数据			
		}
		else if(ChooseBoard == CCU2_NODEID)
		{
			//can1 开始传输SDO
			val = 0x600 | CCU1_NODEID ;
			size = sizeof(UNS32); 
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x01,&val,&size,RW);   //修改客户端TSDO node id
			val = 0x580 | CCU1_NODEID;
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x02,&val,&size,RW);   //修改客户端RSDO node id
			val = CCU1_NODEID;
			size = sizeof(UNS8);
			writeLocalDict(&CCUCAN1Master_Data,0x1280,0x03,&val,&size,RW);   //修改客户端的服务器 node id

			readNetworkDictCallback(&CCUCAN1Master_Data,CCU1_NODEID,CAN_SYNC_NETWORK_STA_MAP,0x00,0,&CCUSyncStatus_SDO_Callback,0);   //读取0x5FFE，0x00 中的数据
			
			//can2 开始传输SDO
			val = 0x600 | CCU1_NODEID ;
			size = sizeof(UNS32); 
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x01,&val,&size,RW);   //修改客户端TSDO node id
			val = 0x580 | CCU1_NODEID;
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x02,&val,&size,RW);   //修改客户端RSDO node id
			val = CCU1_NODEID;
			size = sizeof(UNS8);
			writeLocalDict(&CCUCAN2Master_Data,0x1280,0x03,&val,&size,RW);   //修改客户端的服务器 node id

			readNetworkDictCallback(&CCUCAN2Master_Data,CCU1_NODEID,CAN_SYNC_NETWORK_STA_MAP,0x00,0,&CCUSyncStatus_SDO_Callback,0);   //读取0x5FFE，0x00 中的数据			
		}

	}
	
	if(SDO2sCount <5)
	{
		SDO2sCount++;	
	}

	BSP_OS_TimeDlyMs(500);
	}	
}

void key_task(void *p_arg)		 	//用来显示任务堆栈使用情况，已经cpu使用
{

	(void)p_arg;	               /* 避免编译器报警 */
	printf("Enter key Task!\r\n");
	while(1)
	{		
		if(MTDCCU_CanAddVal() == 0xFF )
		{
			DispTaskInfo();		//打印显示信息	
		}
		BSP_OS_TimeDlyMs(2000);//延时2s
	}
}

//void TemperCheck_task(void *p_arg)			//温度检测任务函数，用来检测驱动板的温度，并实施报警
//{

//	(void )p_arg;			//防止警告
//	LEDGpio_Init();
//	ADC1_CH16_Config();
//	printf("Enter Temper Task!\r\n");	

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



void Can3ChangeHeart_Take(void *p_arg)
{
		(void )p_arg;			//防止警告
	
	while(1)
	{
	 if(ChooseBoard == CCU1_NODEID)		//表示此设备为CCU1设备
	 {
		if(CurrentUseCAN1 == UseState )		//can1总线在使用
		{
			HeartMap_CAN1_CANBC = CurrentUseCAN1;
			HeartMap_CAN1_CCU1 =  CAN1_Heart;
			HeartMap_CAN1_CCU2 = g_RxCAN1HeartFlag[2];
			HeartMap_CAN1_MTD1 = g_RxCAN1HeartFlag[3];	//MC1_MTD
			HeartMap_CAN1_MTD2 = g_RxCAN1HeartFlag[4];	//T_MTD
			HeartMap_CAN1_MTD3 = g_RxCAN1HeartFlag[5];	//M_MTD
			HeartMap_CAN1_MTD4 = g_RxCAN1HeartFlag[6];	//T1_MTD
			HeartMap_CAN1_MTD5 = g_RxCAN1HeartFlag[7];	//T2_MTD
			HeartMap_CAN1_MTD6 = g_RxCAN1HeartFlag[8];	//MC2_MTD
			HeartMap_CAN1_DDU1 = g_RxCAN1HeartFlag[9];	//DDU1
			HeartMap_CAN1_DDU2 = g_RxCAN1HeartFlag[10];	//DDU2
			HeartMap_CAN1_EMR1 = g_RxCAN1HeartFlag[11];	//ERM1
			HeartMap_CAN1_EMR2 = g_RxCAN1HeartFlag[12];	//ERM2
			
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
		}
		else if(CurrentUseCAN2 == UseState )		//can2总线在使用
		{
			HeartMap_CAN2_CANBC = CurrentUseCAN2;
			HeartMap_CAN2_CCU1 =  CAN2_Heart;
			HeartMap_CAN2_CCU2 = g_RxCAN2HeartFlag[2];
			HeartMap_CAN2_MTD1 = g_RxCAN2HeartFlag[3];	//MC1_MTD
			HeartMap_CAN2_MTD2 = g_RxCAN2HeartFlag[4];	//T_MTD
			HeartMap_CAN2_MTD3 = g_RxCAN2HeartFlag[5];	//M_MTD
			HeartMap_CAN2_MTD4 = g_RxCAN2HeartFlag[6];	//T1_MTD
			HeartMap_CAN2_MTD5 = g_RxCAN2HeartFlag[7];	//T2_MTD
			HeartMap_CAN2_MTD6 = g_RxCAN2HeartFlag[8];	//MC2_MTD
			HeartMap_CAN2_DDU1 = g_RxCAN2HeartFlag[9];	//DDU1
			HeartMap_CAN2_DDU2 = g_RxCAN2HeartFlag[10];	//DDU2
			HeartMap_CAN2_EMR1 = g_RxCAN2HeartFlag[11];	//ERM1
			HeartMap_CAN2_EMR2 = g_RxCAN2HeartFlag[12];	//ERM2
			
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
		
		}
	}
	else if(ChooseBoard == CCU2_NODEID)					//表示使用CCU2设备
	{
		if(CurrentUseCAN1 == UseState )
		{
			HeartMap_CAN1_CANBC = CurrentUseCAN1;
			HeartMap_CAN1_CCU1 = g_RxCAN1HeartFlag[1];
			HeartMap_CAN1_CCU2 = CAN1_Heart;
			HeartMap_CAN1_MTD1 = g_RxCAN1HeartFlag[3];	//MC1_MTD
			HeartMap_CAN1_MTD2 = g_RxCAN1HeartFlag[4];	//T_MTD
			HeartMap_CAN1_MTD3 = g_RxCAN1HeartFlag[5];	//M_MTD
			HeartMap_CAN1_MTD4 = g_RxCAN1HeartFlag[6];	//T1_MTD
			HeartMap_CAN1_MTD5 = g_RxCAN1HeartFlag[7];	//T2_MTD
			HeartMap_CAN1_MTD6 = g_RxCAN1HeartFlag[8];	//MC2_MTD
			HeartMap_CAN1_DDU1 = g_RxCAN1HeartFlag[9];	//DDU1
			HeartMap_CAN1_DDU2 = g_RxCAN1HeartFlag[10];	//DDU2
			HeartMap_CAN1_EMR1 = g_RxCAN1HeartFlag[11];	//ERM1
			HeartMap_CAN1_EMR2 = g_RxCAN1HeartFlag[12];	//ERM2
			
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
		}
		else if(CurrentUseCAN2 == UseState )
		{
			HeartMap_CAN2_CANBC = CurrentUseCAN2;
			HeartMap_CAN2_CCU1 = g_RxCAN2HeartFlag[1];
			HeartMap_CAN2_CCU2 = CAN2_Heart;
			HeartMap_CAN2_MTD1 = g_RxCAN2HeartFlag[3];	//MC1_MTD
			HeartMap_CAN2_MTD2 = g_RxCAN2HeartFlag[4];	//T_MTD
			HeartMap_CAN2_MTD3 = g_RxCAN2HeartFlag[5];	//M_MTD
			HeartMap_CAN2_MTD4 = g_RxCAN2HeartFlag[6];	//T1_MTD
			HeartMap_CAN2_MTD5 = g_RxCAN2HeartFlag[7];	//T2_MTD
			HeartMap_CAN2_MTD6 = g_RxCAN2HeartFlag[8];	//MC2_MTD
			HeartMap_CAN2_DDU1 = g_RxCAN2HeartFlag[9];	//DDU1
			HeartMap_CAN2_DDU2 = g_RxCAN2HeartFlag[10];	//DDU2
			HeartMap_CAN2_EMR1 = g_RxCAN2HeartFlag[11];	//ERM1
			HeartMap_CAN2_EMR2 = g_RxCAN2HeartFlag[12];	//ERM2
			
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
		
		}

	}	
		BSP_OS_TimeDlyMs(50);			//延时50ms,改变一次心跳映射的值。调用这个延时是将其进入调度函数，发生任务的调度。		
	}
}


/*whh 2016-12-12 增加两个任务处理can网络切换，（将can1Masterc. can2Master.c定时器中断中的处理放入到任务中处理）*/
extern UNS8 sendEMCY(CO_Data* d, UNS16 errCode, UNS8 errRegister, const void *Specific, UNS8 SpecificLength);
extern void Switch_LED_Display(void);  //切换led显示

/*can1 网络监控任务*/
void CAN1NetWorkMonitorTask(void *p_arg)
{
	OS_ERR err;
	EmcyMsgState CAN1EmcyState = NoEmcy;		//使用局部变量，不需要使用全局变量来标识
	UNS8	i=0, j=0, k=0;									 //i 表示循环变量，j用于存储can1有几个从设备没有心跳，k用于表示can2有几个从设备没有心跳
	UNS16 RecordErrorCoode = 0;
	UNS8  heartbeatID = 0;
	
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);  //阻塞等待任务级信号量
		BSP_OS_TimeDlyMs(50);  //延时50ms等待can2线心跳数据由定时器中断判断完毕
		
		for(i=1;i<=10;i++)   /*CCU1_NODEID 至  DDU2_NODEID*/
		{
		  if(ChooseBoard == CCU1_NODEID) //CCU1 从节点2开始监控
			{
				if(i==1)
				{
					continue;
				}
				else
				{
					if(	g_RxCAN1HeartFlag[i] == NoRxHeart)
					{
					  heartbeatID = i;  //没有接受到心跳的id
						break;  //跳出循环
					}
				}
			}
			
			else if(ChooseBoard == CCU2_NODEID) //CCU2 从节点1开始监控，如果为节点2，跳过
			{
				if(i==2)
				{
					continue;
				}
				else
				{
					if(	g_RxCAN1HeartFlag[i] == NoRxHeart)
					{
					  heartbeatID = i;  //没有接受到心跳的id
						break;  //跳出循环
					}
				}
			}
		}
		
		switch(heartbeatID)
		{
			case CCU1_NODEID:
					if(ChooseBoard == CCU2_NODEID)
					{
						CAN1EmcyState	= MainControlFaultSwitch;
						printf("CCU2 CAN1Master_heartbeatError %d\r\n", heartbeatID);
					}
				break;
		
			case CCU2_NODEID:
					if(ChooseBoard == CCU1_NODEID)
					{
						CAN1EmcyState	= MainControlFaultSwitch;	//主控故障可切换
						printf("CCU1 CAN1Master_heartbeatError %d\r\n", heartbeatID);
					}
				break;
			
			case MC1_MTD_NODEID:

			case T_MTD_NODEID:

			case M_MTD_NODEID:

			case T1_MTD_NODEID:
			
			case T2_MTD_NODEID:

			case MC2_MTD_NODEID:

			case DDU1_NODEID:

			case DDU2_NODEID:
		
			//case ERM1_NODEID:

			//case ERM2_NODEID:
					CAN1EmcyState	= OnePointFaultSwitch;		//单点故障可切换，
					printf("CAN1 Error %d\r\n", heartbeatID);
				break;
			
			default :
					CAN1EmcyState =NoEmcy;
				break;
				
		}		
		
		/*can1 网络 切换处理流程*/
		if(CCU1 == 1)			//表示当前设备为ccu1
		{
			if(CCU1SwitchState == ChooseOperate)	//判断ccu1是否为主控
			{
				/*并且can2口设备进入到操作状态之后接受的心跳*/
				if (CurrentUseCAN1 == UseState)	//判断can1线是否为主线
				{
					/*1.如果主控故障,*/
					if(CAN1EmcyState == MainControlFaultSwitch) 	//说明接受的为副控心跳故障
					{
							//errCode=0x11xx 单点故障，可切换		(	// can1收不到此设备的心跳，但是can2接受到了心跳,并且当前主控设备为ccu1。)
							if(g_RxCAN2HeartFlag[heartbeatID] == RxHeart)		
							{
								k=0;		//记录can2口的无心跳设备的个数	
								for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
								{
									if(g_RxCAN2HeartFlag[i] == NoRxHeart)
									{
										k++;
									}								
								}
								if(0==k)	//表示can2线上其他设备都正常通信。则发送单点故障可切换，切换到can2线上 
								{
									#ifdef DEBUG_PRINTF
										debugprintf("ccu1 can2 Tx errCode=0x11xx Single point of failure, switchable!\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN1EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x1100  | heartbeatID);
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送									
								}
								
								continue ;				//直接返回
							}
														
							//表示can2也没有接收到副控心跳，就发送副控坏不可切换，这样总线还能使用
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{
								#ifdef DEBUG_PRINTF	
									debugprintf("ccu1 can2 Tx errCode=0x40xx  Slave2 Bad，Not Switch!a \r\n");
								#endif 
							
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x4000 |	heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送	

								continue ;				//直接返回
							}
					}
				
					 /*2.如果单点故障，或者主干线故障*/
					if(CAN1EmcyState == OnePointFaultSwitch) 
					{
							j=0;		//记录can1口的无心跳设备的个数
							k=0;		//记录can2口的无心跳设备的个数
								
							for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
							{
								if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) 
								{
									j++;  
								}
								if(g_RxCAN2HeartFlag[i] == NoRxHeart)
								{
									k++;
								}
							}
							
							//errCode=0x11xx 单点故障，可切换
							if((j==1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 Tx errCode=0x11xx Single point of failure, switchable!\r\n");
								#endif 
								
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x1100 | heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								continue ;				//直接返回
							}
							
							//errCode=0x10xx  单点故障，不可切换 
							else if((j==1) && (k>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 Tx errCode=0x10xx  Single point of failure, Not switchable\r\n");
								#endif 
								
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x1000 |	heartbeatID;                                                //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
								continue ;				//直接返回					
							}
							
							//errCode=0x21xx 主干线断故障，可切换
							else if((j>1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 Tx errCode=0x21xx Trunk line fault, switch!\r\n");
								#endif
			
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x2100 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								continue ;				//直接返回						
							}
							
							//errCode=0x20xx 主干线断故障，不可切换
							else if((j>1) && (k>0) )
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 Tx 0x20xx Trunk line fault,Not switch!\r\n");
								#endif
								
								CAN1EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x2000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
								continue ;				//直接返回					
							}
						
						}
				}
			
			}
			
			
			else		//ccu1不是主控，去监控ccu2 设备的心跳
			{
				printf("CCU1 can1 node2 Err, can2_node2=%d\r\n",g_RxCAN2HeartFlag[heartbeatID]);
				//1.如果主控故障,这里有ccu1 监控ccu2的心跳
				//当前故障为主控故障，并且can2线也接受不到主控ccu2的心跳
				if((CAN1EmcyState == MainControlFaultSwitch) && (g_RxCAN2HeartFlag[heartbeatID] == NoRxHeart))
				{
					j=0;		//记录can1口的无心跳设备的个数
					k=0;		//记录can2口的无心跳设备的个数
					for(i=3;i<10/*12*/;i++)
					{
						if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) //计算是否能接受到其他设备的心跳
						{
							j++;  
						}
						
						if(g_RxCAN2HeartFlag[i] == NoRxHeart)
						{
							k++;
						}
					}
					
					//errCode=0x30xx 主控2坏，不可切换
					if((j!=0 )&&(k!=0))
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x60xx Master2 Bad，Not Switch!\r\n");
						#endif 
						
						CAN1EmcyState = 	MainControlFaultNotSwitch;
						RecordErrorCoode = 0x6000 |heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3000 |	heartbeatID;					
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回
					}
					
					//errCode=0x30xx主控2坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x61xx Master2 Bad， Switch!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x31xx Master2 Bad， Switch!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						continue ;				//直接返回 ;
					}
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x61xx Master2 Bad， Switch!\r\n");
						#endif
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
					
						//	RecordErrorCoode = 0x3100 |	heartbeatID;
						//	sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回
					}
				}
			}
			
		}
		else		//表示使用的ccu2
		{
			if(CCU2SwitchState == ChooseOperate)		//ccu2 主控
			{
				/*并且can2口设备进入到操作状态之后接受的心跳*/
				if (CurrentUseCAN1 == UseState)
				{
						/*1.如果主控故障,*/
					if(CAN1EmcyState == MainControlFaultSwitch)
					{
							//errCode=0x11xx 单点故障，可切换		(	// can1收不到此设备的心跳，但是can2接受到了心跳,并且当前主控设备为ccu1。)
							if(g_RxCAN2HeartFlag[heartbeatID] == RxHeart)		
							{							
								k=0;		//记录can2口的无心跳设备的个数	
								for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
								{
									if(g_RxCAN2HeartFlag[i] == NoRxHeart)
									{
										k++;
									}								
								}
								if(0==k)	//表示can2线上其他设备都正常通信。则发送单点故障可切换，切换到can2线上 
								{
									#ifdef DEBUG_PRINTF
										debugprintf("ccu2 can2 Tx errCode=0x11xx Single point of failure, switchable!\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN1EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x1100  | heartbeatID);
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送									
								}
								continue ;				//直接返回 ;				//直接返回
							}
														
							//表示can2也没有接收到副控心跳，就发送副控坏不可切换，这样总线还能使用
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{							
									#ifdef DEBUG_PRINTF	
										debugprintf("ccu1 can2 Tx errCode=0x10xx Slave2 Bad，Not Switch! \r\n");
									#endif 
								
									CAN1EmcyState = 	OnePointFaultNotSwitch;
									RecordErrorCoode = 0x1000 |	heartbeatID;                                                    	//不可切换，有谁发送都可以
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送								
									
									continue ;				//直接返回 ;
							}
					}
			
					 /*2.如果单点故障，或者主干线故障*/
					if(CAN1EmcyState == OnePointFaultSwitch) 
					{
							j=0;		//记录can1口的无心跳设备的个数
							k=0;		//记录can2口的无心跳设备的个数
							
							for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
							{
								if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) 
								{
									j++;  
								}
								if(g_RxCAN2HeartFlag[i] == NoRxHeart) 
								{
									k++;
								}
							}
							
							//errCode=0x11xx 单点故障，可切换
							if((j==1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 Tx 0x11xx Switch\r\n");
								#endif
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x1100 | heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								continue ;				//直接返回 ;
							}
							//errCode=0x10xx  单点故障，不可切换 
							else if((j==1) && (k>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 Tx 0x10xx Not Switch!\r\n");
								#endif
								
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x1000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
								continue ;				//直接返回 ;					
							}
							
							//errCode=0x21xx 主干线断故障，可切换
							else if((j>1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 Tx 0x21xx Switch!\r\n");
								#endif 
								
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x2100 |	heartbeatID;                                                       
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								continue ;				//直接返回 ;						
							}
							//errCode=0x20xx 主干线断故障，不可切换
							else if((j>1) && (k>0) )
							{	
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 Tx 0x20xx Switch!\r\n");
								#endif
								
								CAN1EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x2000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
								continue ;				//直接返回 ;						
							}
				
					}
				}
			
			}
			
			
			else		//ccu2不是主控，去监1 设备的心跳
			{
				//printf("whh ccu2 is not master!\r\n");
				//1.如果主控故障,这里有ccu1 监控ccu2的心跳
				//当前故障为主控故障，并且can2线也接受不到主控ccu2的心跳
				if((CAN1EmcyState == MainControlFaultSwitch) && (g_RxCAN2HeartFlag[heartbeatID] == NoRxHeart) )
				{
					j=0;		//记录can1口的无心跳设备的个数
					k=0;		//记录can2口的无心跳设备的个数
					for(i=3;i<10/*12*/;i++)
					{
						if(g_RxCAN1HeartFlag[i]	==  NoRxHeart )	//计算是否能接受到其他设备的心跳
						{
							j++;  
						}
						
						if(g_RxCAN2HeartFlag[i] == NoRxHeart)
						{
							k++;
						}
					}
					
					//errCode=0x31xx 主控1坏，不可切换
					if((j!=0 )&&(k!=0))
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x60xx Master1 Not Switch!\r\n");
						#endif 
						
						CAN1EmcyState = 	MainControlFaultNotSwitch;
						RecordErrorCoode = 0x6000 |heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送						
						
						//RecordErrorCoode = 0x3000 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送						                                                                                                                 
						continue ;				//直接返回 ;
					}
					
					//errCode=0x30xx主控1坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x61xx Master1  Switch!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x31xx Master1  Switch!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						continue ;				//直接返回 ;
					}
					//errCode=0x30xx主控1坏，可切换
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x61xx Master1  Switch!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送					
						continue ;				//直接返回 ;
					}
				}
			}
		}
		
		BSP_OS_TimeDlyMs(50);
	}
}


/*can2 网络监控任务*/
void CAN2NetWorkMonitorTask(void *p_arg)
{
	OS_ERR err;
	EmcyMsgState CAN2EmcyState;			//使用局部变量，不需要使用全局变量来标识
	UNS8	i=0, j=0 , k=0;									//i 表示循环变量，j用于存储can1有几个从设备没有心跳，k用于表示can2有几个从设备没有心跳
	UNS16 RecordErrorCoode =0;
	UNS8  heartbeatID = 0;
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,0,&err);  //阻塞等待任务级信号量		
		BSP_OS_TimeDlyMs(50);  //延时50ms等待can2线心跳数据由定时器中断判断完毕
		
		for(i=1;i<=10;i++)   /*CCU1_NODEID 至  DDU2_NODEID*/
		{
		  if(ChooseBoard == CCU1_NODEID) //CCU1 从节点2开始监控
			{
				if(i==1)
				{
					continue;
				}
				else
				{
					if(	g_RxCAN2HeartFlag[i] == NoRxHeart)
					{
					  heartbeatID = i;  //没有接受到心跳的id
						break;  //跳出for循环
					}
				}
			}
			
			else	if(ChooseBoard == CCU2_NODEID) //CCU2 从节点1开始监控，如果为节点2，跳过
			{
				if(i==2)
				{
					continue;
				}
				else
				{
					if(	g_RxCAN2HeartFlag[i] == NoRxHeart)
					{
					  heartbeatID = i;  //没有接受到心跳的id
						break;  //跳出for循环
					}
				}
			}
		}
				
		switch(heartbeatID)
		{
			case CCU1_NODEID:
					if(ChooseBoard == CCU2_NODEID)
					{
						CAN2EmcyState	= MainControlFaultSwitch;
						printf("CCU2 CAN2Master_heartbeatError %d\r\n", heartbeatID);
					}
				break;
		
			case CCU2_NODEID:
					if(ChooseBoard == CCU1_NODEID)
					{
						CAN2EmcyState	= MainControlFaultSwitch;	//主控故障可切换					
						printf("CCU1 CAN2Master_heartbeatError %d\r\n", heartbeatID);
					}
				break;
			
			case MC1_MTD_NODEID:
				
			case T_MTD_NODEID:
				
			case M_MTD_NODEID:
			
			case T1_MTD_NODEID:
			
			case T2_MTD_NODEID:
			
			case MC2_MTD_NODEID:
				
			case DDU1_NODEID:
			
			case DDU2_NODEID:
			
			//case ERM1_NODEID:

			//case ERM2_NODEID:
					CAN2EmcyState	= OnePointFaultSwitch;		//单点故障可切换，
					printf("CAN2 Error %d\r\n", heartbeatID);
					break;
		default :
				CAN2EmcyState =NoEmcy;
			break;
		}
	
	
		/*can2 网络 切换处理流程*/	
		if(CCU1 == 1)			//表示当前设备为ccu1
		{
			if(CCU1SwitchState == ChooseOperate)	//判断ccu1是否为主控
			{
				/*并且can2口设备进入到操作状态之后接受的心跳*/
				if(CurrentUseCAN2 == UseState) 
				{
						/*1.如果主控故障,*/
					if(CAN2EmcyState == MainControlFaultSwitch)
					{
							//errCode=0x41xx 单点故障，可切换		(	// can1收不到此设备的心跳，但是can2接受到了心跳,并且当前主控设备为ccu1。)
							if(g_RxCAN1HeartFlag[heartbeatID] == RxHeart)		
							{	
								j=0;  		//记录can1口的无心跳设备的个数	
								for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
								{
									if(g_RxCAN1HeartFlag[i] == NoRxHeart)
									{
										j++;
									}								
								}
								
								if(0 == j)		//表示can1线上其他设备都正常通信。则发送单点故障可切换，切换到can1线上 
								{
									#ifdef DEBUG_PRINTF
										debugprintf("ccu1 can1 Tx errCode=0x41xx Single point of failure, Switchable!\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN2EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x4100  | heartbeatID);
									sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);			//这里必须通过can1口发送	
								}
								continue ;				//直接返回 ;				//直接返回
							}
														
							//表示can1也没有接收到副控心跳，就发送副控坏不可切换，这样总线还能使用
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{
									//errCode=0x61xx 主控1坏，不可切换 	
									#ifdef DEBUG_PRINTF
										debugprintf("ccu1 can1 Tx errCode=0x10xx  Slave 2 bad, Not switchable \r\n");
									#endif
								
									CAN2EmcyState = 	OnePointFaultNotSwitch;
									RecordErrorCoode = 0x1000 |	heartbeatID;                                                      //不可切换，有谁发送都可以
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
									continue ;				//直接返回 ;
							}
					}
			
					 /*2.如果单点故障，或者主干线故障*/
					if(CAN2EmcyState == OnePointFaultSwitch) 
					{
							j=0;		//记录can1口的无心跳设备的个数
							k=0;		//记录can2口的无心跳设备的个数
			
							for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
							{
								if(g_RxCAN1HeartFlag[i]	==  NoRxHeart )
								{
									j++; 
								}
								if(g_RxCAN2HeartFlag[i] == NoRxHeart)
								{
									k++;
								}
							}
							
							//errCode=0x41xx 单点故障，可切换
							if((k==1) && (j==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can1 Tx 0x41xx Switchable!\r\n");
								#endif 
								
								Switch_LED_Display();  //切换led显示
								CAN2EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x4100 | heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送
								continue ;				//直接返回 ;
							}
							//errCode=0x40xx  单点故障，不可切换 
							else if((k==1) && (j>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can1 Tx 0x40xx Not Switchable!\r\n");
								#endif 
								
								CAN2EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x4000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
								continue ;				//直接返回 ;					
							}
							
							//errCode=0x51xx 主干线断故障，可切换
							else if((k>1) && (j==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can1 Tx 0x51xx Switchable!\r\n");
								#endif
								
								Switch_LED_Display();  //切换led显示
								CAN2EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x5100 |	heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送
								continue ;				//直接返回 ;						
							}
							//errCode=0x50xx 主干线断故障，不可切换
							else if((k>1) && (j>0) )
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can1 Tx 0x50xx Not Switchable!\r\n");
								#endif
								
								CAN2EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x5000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
								continue ;				//直接返回 ;						
							}
						
						}
				}
			
			}
			
			
			else		//ccu1不是主控，去监控ccu2 设备的心跳
			{
				printf("CCU1 can2 node2 Err, can1_node2=%d\r\n",g_RxCAN1HeartFlag[heartbeatID]);
				//1.如果主控故障,这里有ccu1 监控ccu2的心跳
				//当前故障为主控故障，并且can1线也接受不到主控ccu2的心跳
				if((CAN2EmcyState == MainControlFaultSwitch) && (g_RxCAN1HeartFlag[heartbeatID] == NoRxHeart))
				{
					j=0;		//记录can1口的无心跳设备的个数
					k=0;		//记录can2口的无心跳设备的个数
					for(i=3;i<10/*12*/;i++)
					{
						if(g_RxCAN1HeartFlag[i]	==  NoRxHeart )//计算是否能接受到其他设备的心跳
						{
							j++; 
						}
						
						if(g_RxCAN2HeartFlag[i] == NoRxHeart) 
						{
							k++;
						}
					}

					//errCode=0x60xx 主控1坏，不可切换
					if((j!=0 )&&(k!=0))
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x30xx Master2 Bad Not switchable!\r\n");
						#endif 
						
						CAN2EmcyState = 	MainControlFaultNotSwitch;
						//RecordErrorCoode = 0x6000 |heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						RecordErrorCoode = 0x3000 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送						
						continue ;				//直接返回 ;
					}

					//errCode=0x31xx主控1坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x61xx Master2 Bad Switchable!\r\n");
						#endif 
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),RecordErrorCoode,NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode 0x31xx Master2 Bad Switchable!\r\n");
						#endif 
					
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						continue ;				//直接返回 ;
					}
					
					//errCode=0x61xx主控1坏，可切换
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode  0x61xx Master2 Bad Switchable!\r\n");
						#endif
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
				}
			}
			
		}
		else		//表示使用的ccu2
		{
			if(CCU2SwitchState == ChooseOperate)		//ccu2 主控
			{
				/*并且can2口设备进入到操作状态之后接受的心跳*/
				if(CurrentUseCAN2 == UseState)
				{
						/*1.如果主控故障,*/
					if(CAN2EmcyState == MainControlFaultSwitch)
					{
							//errCode=0x41xx 单点故障，可切换		(	// can1收不到此设备的心跳，但是can2接受到了心跳,并且当前主控设备为ccu1。)
							if(g_RxCAN1HeartFlag[heartbeatID] == RxHeart)		
							{							
								j=0;  		//记录can1口的无心跳设备的个数	
								for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
								{
									if(g_RxCAN1HeartFlag[i] == NoRxHeart)
									{
										j++;
									}								
								}
								
								if(0 == j)		//表示can1线上其他设备都正常通信。则发送单点故障可切换，切换到can1线上 
								{
									#ifdef DEBUG_PRINTF
										debugprintf("ccu2 can1 Tx errCode=0x41xx Single point of failure, switchable!\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN2EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x4100  | heartbeatID);
									sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);			//这里必须通过can1口发送	
								}
								
								continue ;				//直接返回 ;				//直接返回
							}
														
							//表示can2也没有心跳
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{
								//errCode=0x61xx 主控1坏，不可切换 	
									#ifdef DEBUG_PRINTF
										debugprintf("ccu2 can2 Tx errCode=0x10xx  Slave2  Not switch! \r\n");
									#endif
								
									CAN2EmcyState = 	OnePointFaultNotSwitch;
									RecordErrorCoode = 0x1000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送								
									continue ;				//直接返回 ;
							}
					}
				
					 /*2.如果单点故障，或者主干线故障*/
					if(CAN2EmcyState == OnePointFaultSwitch) 
					{
							j=0;		//记录can1口的无心跳设备的个数
							k=0;		//记录can2口的无心跳设备的个数					
							for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
							{
								if(g_RxCAN1HeartFlag[i]	==  NoRxHeart )
								{
									j++; 
								}
								if(g_RxCAN2HeartFlag[i] == NoRxHeart)
								{
									k++;
								}
							}
							
							//errCode=0x41xx 单点故障，可切换
							if((k==1) && (j==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can1 Tx 0x41xx switch!\r\n");
								#endif 
								Switch_LED_Display();  //切换led显示
								CAN2EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x4100 | heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送
								continue ;				//直接返回 ;
							}
							//errCode=0x40xx  单点故障，不可切换 
							else if((k==1) && (j>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can1 Tx 0x40xx Not switch!\r\n");
								#endif 
								
								CAN2EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x4000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
								continue ;				//直接返回 ;					
							}
							
							//errCode=0x51xx 主干线断故障，可切换
							else if((k>1) && (j==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can1 Tx 0x51xx switch!\r\n");
								#endif
								
								Switch_LED_Display();  //切换led显示
								CAN2EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x5100 |	heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送
								continue ;				//直接返回 ;						
							}
							//errCode=0x50xx 主干线断故障，不可切换
							else if((k>1) && (j>0) )
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can1 Tx  0x50xx Not switch!\r\n");
								#endif 
								
								CAN2EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x5000 |	heartbeatID;                                                       //不可切换，有谁发送都可以
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can2口发送
								continue ;				//直接返回 ;						
							}
						
						}
				}
			}
			
			
			else		//ccu2不是主控，去监1 设备的心跳
			{
				//1.如果主控故障,这里有ccu1 监控ccu2的心跳
				if((CAN2EmcyState == MainControlFaultSwitch)&& (g_RxCAN1HeartFlag[heartbeatID] == NoRxHeart))
				{
					j=0;		//记录can1口的无心跳设备的个数
					k=0;		//记录can2口的无心跳设备的个数
					for(i=3;i<10/*12*/;i++)
					{
						if(g_RxCAN1HeartFlag[i]	==  NoRxHeart )		//计算是否能接受到其他设备的心跳
						{
							j++; 
						}
						
						if(g_RxCAN2HeartFlag[i] == NoRxHeart)
						{
							k++;
						}
					}
					
					//errCode=0x60xx 主控1坏，不可切换
					if((j!=0 )&&(k!=0))
					{
						#ifdef DEBUG_PRINTF
						debugprintf("Tx ErrorCode :0x30xx Master1 Not switch!\r\n");
						#endif 
						
						CAN2EmcyState = 	MainControlFaultNotSwitch;
						//RecordErrorCoode = 0x6000 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						RecordErrorCoode = 0x3000 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
					
					//errCode=0x31xx主控1坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode :0x61xx Master1  switch!\r\n\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
	
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						continue ;				//直接返回 ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode :0x31xx Master1  switch!\r\n\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						continue ;				//直接返回 ;
					}
					
					//errCode=0x60xx主控1坏，可切换
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("Tx ErrorCode :0x61xx Master1 switch!\r\n\r\n");
						#endif          
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN2EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
	
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送					
						continue ;				//直接返回 ;
					}

				}
			}
		}
		
		BSP_OS_TimeDlyMs(50);
	}
}


/*
	 can 查询can1，can2同时坏，突然另一条有好的触发发送任务							
*/
								 	//任务函数
void CANQueryTask(void *p_arg)
{
  OS_ERR err;
	UNS8	i=0, j=0, k=0;									 //i 表示循环变量，j用于存储can1有几个从设备没有心跳，k用于表示can2有几个从设备没有心跳
	while(1)
	{
		/*1.当前使用can网络故障，查另一条是否正常的查询*/
		if(CurrentUseCAN1 == UseState)  //表示当前使用can1网络
		{
			/*(1),当can1网络故障的时候，查询can2网络是否正常，正常，则发送can1网络故障的任务级信号量*/
			j=0;		//记录can1口的无心跳设备的个数
			k=0;		//记录can2口的无心跳设备的个数
								
			for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
			{
				if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) 
				{
					j++;  
				}
				if(g_RxCAN2HeartFlag[i] == NoRxHeart)
				{
					k++;
				}
			}
			if((j!=0) && (k==0))    //can1故障，can2正常
			{
			 OSTaskSemPost(&CAN1NetWorkMonitorTaskTCB,OS_OPT_POST_NONE,&err);   //释放can1网络监控任务级信号量
			}	
		}
		else if (CurrentUseCAN2 == UseState) //表示当前使用can2网络 
		{
			/*(1),当can1网络故障的时候，查询can2网络是否正常，正常，则发送can1网络故障的任务级信号量*/
			j=0;		//记录can1口的无心跳设备的个数
			k=0;		//记录can2口的无心跳设备的个数
								
			for(i=3;i<=10/*12*/;i++)	//当前屏蔽事件记录仪
			{
				if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) 
				{
					j++;  
				}
				if(g_RxCAN2HeartFlag[i] == NoRxHeart)
				{
					k++;
				}
			}
			if((k!=0) && (j==0))    //can2故障，can1正常
			{
			 OSTaskSemPost(&CAN2NetWorkMonitorTaskTCB,OS_OPT_POST_NONE,&err);  //释放can2网络监控任务级信号量
			}			
		}
		else
		{
			printf("can NetWork Choose Error！\r\n");
		}
		
		BSP_OS_TimeDlyMs(400);     //以心跳频率为基准
	}
}

void assert_failed(u8* file, u32 line)
{
    debug("fail");
		while (1) ;
}




