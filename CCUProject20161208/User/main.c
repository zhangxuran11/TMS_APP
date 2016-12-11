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
#define Main_TASK_PRIO		4
	//任务堆栈大小	
#define Main_STK_SIZE 		128
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
#define KEY_TASK_PRIO		12
	//任务堆栈大小	
#define KEY_STK_SIZE 		512
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
#define CAN1MASTERRX_TASK_PRIO		6
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
#define CAN1MASTERTX_TASK_PRIO		7
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
#define CAN2MASTERRX_TASK_PRIO		8
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
#define CAN2MASTERTX_TASK_PRIO		9
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
#define CAN3MASTERRX_TASK_PRIO		10
	//任务堆栈大小	
#define CAN3MASTERRX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN3MasterRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERRX_TASK_STK[CAN3MASTERRX_STK_SIZE];
	//任务函数
extern void can3master_recv_thread(void *p_arg);

/*主站发送任务*/
	//任务优先级
#define CAN3MASTERTX_TASK_PRIO		11
	//任务堆栈大小	
#define CAN3MASTERTX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN3MasterTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERTX_TASK_STK[CAN3MASTERTX_STK_SIZE];
	//任务函数
extern void can3master_send_thread(void *p_arg);


	/*任务对象的创建*/
static  OS_SEM     	SEM_SYNCH;	   //用于同步
OS_SEM     	Slave_Start_Sem;			//从站启动管理的信号量

OS_MUTEX	CAN3_MUTEX;		//定义一个互斥信号量,用于can3发送接受任务的互斥

///*测试使用的全局变量的定义*/
////can
uint8_t DS2401P_ID[8]={0x00};			//必须定义在bsp，macid检测中使用了
uint8_t can2_send[]={0x11,0x22,0x33,0x44,0x11,0x66,0x78,0x99,0x23,0xac,0x89,0xaa,0xbb,0xcc,0xdd,0xaa,0xbb,0x00,0x33};
u8 g_RxDataFlag=0;
u8 g_FrameErrorFlag=0;

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

//EmcyMsgState 	CAN1EmcyState;
//EmcyMsgState 	CAN2EmcyState;

extern int  test_can3master(void);

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
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),		//任务选项
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

}

extern u8 User_CAN1Transmit(uint32_t id,uint8_t* val,uint8_t Len);
extern u8 User_CAN2Transmit(uint32_t id,uint8_t* val,uint8_t Len);
extern unsigned char canInit(CAN_TypeDef* CANx,unsigned int bitrate);

extern CO_Data CCUCAN1Master_Data;
extern CO_Data CCUCAN2Master_Data;
extern u8 CCUcan1TxFlag;
extern u8 CCUcan2TxFlag;

u8 aaa;  
#define a1 aaa
#define a2 aaa


void Main_task(void *p_arg)
{
//	uint64_t  a;
//	OS_ERR err;
//	static u8 SlaveStartCount;
//	
//	static u8 can1StartCount;
//	static u8 can2StartCount;
//	static u8 can1Flag;
//	static u8 can2Flag=1;

	/*iap UART1*/
	u16 UartOldCount=0;	//老的串口接收数据值
	//u16 AppLenth=0;	//接收到的app代码长度
	
	/*spi can*/
//		Frame_TypeDef frame;
//		frame.SID=10;
//		frame.DLC=8;
//		frame.Type=CAN_STD;
	
/*ccu1  ccu2 状态的判断*/
//		e_nodeState MasterState;
		
	/*用于ccu判断是否接收到所有心跳计数*/
	uint8_t	can1Count=0;
	uint8_t can2Count=0;
	static	uint8_t CCUdelayCount=0;
	uint8_t i;
	
	u8 k=0;
	
	(void)p_arg;
	debugprintf("进入到Main任务\r\n");
	
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
	
	/*mcp2515*/
//	 MCP2515_REST1_GpioInit();
//	 MTDCPU_MCP2515_Init();
//	 MCP2515_INT1_GpioInit();


//			/*18.can3板子的初始化*/
//		MTDCCU_CAN3_MCP2515_Init(MCP2515_CAN_BAUD_125K);
//		canInit(CAN3,MCP2515_CAN_BAUD_125K);			//W H H ,主设备用的can3
//		test_can3master();
		
//	/*can3 初始化*/
//{
//	u8 res;

//	
//		res = MTDCCU_CAN3_MCP2515_Init(MCP2515_CAN_BAUD_125K);
////		if(res == MCP2515_OK)
////			return 0;
////		else
////			return CAN_FAILINIT;
//}

/*can adder*/
//MTDCCU_CanAdd_Gpio_Init();
	
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
	
	
	/*ccu主设备，从设备 指示灯*/
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
			printf("C1设备 CCU1 主控!\r\n");
		}
		else			//CCU1 为副控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_OFF);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_ON);			
			printf("C1设备 CCU2 主控!\r\n");
		}
	}
	
	else		//表示为CCU2设备
	{
		if(CCU2SwitchState == ChooseOperate)		//CCU2为主控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_ON);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_OFF);
			printf("C2设备 CCU2 主控!\r\n");
		}
		else			//CCU1 为副控
		{
			MASTER_LED3_ONOFF(MASTER_SLAVE_LED_OFF);
			SLAVE_LED4_ONOFF(MASTER_SLAVE_LED_ON);	
			printf("C2设备 CCU1 主控 \r\n");
		}		
	}
	
	
	
	
	/*不用发送pdo来管理ccu1或者ccu2的心跳*/
//	if(CCU1 == CCU1_NODEID)		//CCU1
//	{
//			MasterState= getState(&CCUCAN1Master_Data);
//			if(MasterState == Operational)
//			{
//				CCUcan1TxFlag =1;
//				sendPDOevent(&CCUCAN1Master_Data);
//			}
//	}
//		
//	else					//CCU2
//	{			
//			MasterState= getState(&CCUCAN2Master_Data);
//			if(MasterState == Operational)
//			{
//				CCUcan2TxFlag =1;
//				sendPDOevent(&CCUCAN2Master_Data);
//			}
//	}
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
	
	
		BSP_OS_TimeDlyMs(500);
	}	
}

void key_task(void *p_arg)		 	//用来显示任务堆栈使用情况，已经cpu使用
{

	(void)p_arg;	               /* 避免编译器报警 */
	printf("进入到key任务\r\n");
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


void assert_failed(u8* file, u32 line)
{
    debug("fail");
		while (1) ;
}




