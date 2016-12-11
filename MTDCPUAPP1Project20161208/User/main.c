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
#include "main.h"

//#define BUFFER_SIZE 512
	
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
//#define TemperCheck_STK_SIZE 		128
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
//#define KEY_STK_SIZE 		128
//	//任务控制块
//static OS_TCB KeyTaskTCB;
//	//任务堆栈	
//static CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//	//任务函数
//static void key_task(void *p_arg);


/*从站启动管理任务*/
	//任务优先级
#define CAN1SLAVE_STARTUPMANAGER_TASK_PRIO		5
	//任务堆栈大小	
#define CAN1SLAVE_STARTUPMANAGER_STK_SIZE 		96
	//任务控制块
static OS_TCB CAN1SlaveStartupManagerTCB;
	//任务堆栈	
static CPU_STK CAN1SLAVE_STARTUPMANAGER_TASK_STK[CAN1SLAVE_STARTUPMANAGER_STK_SIZE];
	//任务函数
void CAN1Slave_StartupManager_Take(void *p_arg);


///*从站启动管理任务*/
//	//任务优先级
//#define CAN2SLAVE_STARTUPMANAGER_TASK_PRIO		6
//	//任务堆栈大小	
//#define CAN2SLAVE_STARTUPMANAGER_STK_SIZE 		64
//	//任务控制块
//static OS_TCB CAN2SlaveStartupManagerTCB;
//	//任务堆栈	
//static CPU_STK CAN2SLAVE_STARTUPMANAGER_TASK_STK[CAN2SLAVE_STARTUPMANAGER_STK_SIZE];
//	//任务函数
//void CAN2Slave_StartupManager_Take(void *p_arg);


/*从站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN1SLAVERX_TASK_PRIO		10//7
	//任务堆栈大小	
#define CAN1SLAVERX_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN1SlaveRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN1SLAVERX_TASK_STK[CAN1SLAVERX_STK_SIZE];
	//任务函数
extern void can1slave_recv_thread(void *p_arg);


/*从站发送任务*/
	//任务优先级
#define CAN1SLAVETX_TASK_PRIO		9//8
	//任务堆栈大小	
#define CAN1SLAVETX_STK_SIZE 		128
	//任务控制块
/*static*/ OS_TCB CAN1SlaveTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN1SLAVETX_TASK_STK[CAN1SLAVETX_STK_SIZE];
	//任务函数
extern void can1slave_send_thread(void *p_arg);


/*从站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN2SLAVERX_TASK_PRIO		13//9
	//任务堆栈大小	
#define CAN2SLAVERX_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN2SlaveRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2SLAVERX_TASK_STK[CAN2SLAVERX_STK_SIZE];
	//任务函数
extern void can2slave_recv_thread(void *p_arg);

/*从站发送任务*/
	//任务优先级
#define CAN2SLAVETX_TASK_PRIO		12//10
	//任务堆栈大小	
#define CAN2SLAVETX_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN2SlaveTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2SLAVETX_TASK_STK[CAN2SLAVETX_STK_SIZE];
	//任务函数
extern void can2slave_send_thread(void *p_arg);



/*主站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN3MASTERRX_TASK_PRIO		6
	//任务堆栈大小	
#define CAN3MASTERRX_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN3MasterRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERRX_TASK_STK[CAN3MASTERRX_STK_SIZE];
	//任务函数
extern void can3master_recv_thread(void *p_arg);

/*主站发送任务*/
	//任务优先级
#define CAN3MASTERTX_TASK_PRIO		8//12
	//任务堆栈大小	
#define CAN3MASTERTX_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN3MasterTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN3MASTERTX_TASK_STK[CAN3MASTERTX_STK_SIZE];
	//任务函数
extern void can3master_send_thread(void *p_arg);


/*can3接受处理任务*/
	//任务优先级
#define CAN3RXPROC_TASK_PRIO			7//13
	//任务堆栈大小	
#define CAN3RXPROC_STK_SIZE 	  	128
	//任务控制块
static OS_TCB CAN3RxProcTaskTCB;
	//任务堆栈	
static CPU_STK CAN3RxProc_TASK_STK[CAN3RXPROC_STK_SIZE];
	//任务函数
extern void can3RxProcthread(void *p_arg);


/*OLED Key任务*/
	//任务优先级
#define OLEDKEY_TASK_PRIO		14
	//任务堆栈大小	
#define OLEDKEY_STK_SIZE 		128
	//任务控制块
static OS_TCB OledKeyTaskTCB;
	//任务堆栈	
static CPU_STK OLEDKEY_TASK_STK[OLEDKEY_STK_SIZE];
	//任务函数
void Oled_Key_thread(void *p_arg);


/*can1 接受发送LED灯闪烁任务*/
	//任务优先级
#define CAN1LED_TASK_PRIO		20
	//任务堆栈大小	
#define CAN1LED_STK_SIZE 		64
	//任务控制块
static OS_TCB  CAN1LED_TCB;
	//任务堆栈	
static CPU_STK  CAN1LED_TASK_STK[CAN1LED_STK_SIZE];
	//任务函数
void CAN1LEDTake(void *p_arg);

/*can2 接受发送LED灯闪烁任务*/
	//任务优先级
#define CAN2LED_TASK_PRIO		21
	//任务堆栈大小	
#define CAN2LED_STK_SIZE 		64
	//任务控制块
static OS_TCB  	CAN2LED_TCB;
	//任务堆栈	
static CPU_STK  CAN2LED_TASK_STK[CAN2LED_STK_SIZE];
	//任务函数
void CAN2LEDTake(void *p_arg);


/*can1 2 定时发送PDO数据 */
	//任务优先级
#define TimTxPDO_TASK_PRIO		19
	//任务堆栈大小	
#define TimTxPDO_STK_SIZE 		256
	//任务控制块
static OS_TCB  	TimTxPDO_TCB;
	//任务堆栈	
static CPU_STK  TimTxPDO_TASK_STK[TimTxPDO_STK_SIZE];
	//任务函数
void TimTxPDOTake(void *p_arg);

/*任务对象的创建*/
static  OS_SEM     	SEM_SYNCH;	   //用于同步
//OS_SEM     	Slave_Start_Sem;			//从站启动管理的信号量

OS_MUTEX	CAN3_MUTEX;		//定义一个互斥信号量,用于can3发送接受任务的互斥


/*创建事件标志组*/
OS_FLAG_GRP	CAN1LedEventFlag;		//定义can1led闪烁时间标志组
OS_FLAG_GRP	CAN2LedEventFlag;		//定义can2led闪烁事件标志组

/*任务定时器的创建*/
OS_TMR CAN3TimeTmr;		//can3网络发送公共时间给MFD卡 定时器
void CAN3TimeTrmCallBack(void *p_tmr, void *p_arg);

/*定时发送PDO的定时器*/
OS_TMR TxPDOTmr;
void TxPDOTmrCallBack(void *p_tmr, void *p_arg);

///*测试使用的全局变量的定义*/

u8 g_RxDataFlag=0;
u8 g_FrameErrorFlag=0;

/*canfestival 接受心跳报文的定义*/

//IsNoReceHeart			 g_RxCAN1HeartFlag[15]={0};					//存放各个设备的心跳的标志，下标就是节点的ID号，故下标为0的数组没有使用。
//IsNoReceHeart			 g_RxCAN2HeartFlag[15]={0};

//CurReceHeartState  g_RxCAN1HeartStateFlag[15] = {0};		//存放当前接受心跳时，从设备所在的状态，是上电刚初始化的时候，还是建立的连接的时候		
//CurReceHeartState  g_RxCAN2HeartStateFlag[15] = {0};

//RxCanMessageState  g_RxCAN1MesStateFlag[15]={0};						//存放当前主站接到从站的can消息 所处的状态 
//RxCanMessageState	 g_RxCAN2MesStateFlag[15]={0};

CurrentUserState CurrentUseCAN1 = UseState;			/*表示当前使用can1总线，还是can2总线，默认使用can1网络*/
CurrentUserState CurrentUseCAN2 = NoUseState;	

extern CO_Data MTDApp1CAN1Slave_Data;
extern CO_Data MTDApp1CAN2Slave_Data;

u8 can1CarNumberFlag = false;
u8 can2CarNumberFlag = false;
u8 can1UpdateTimeFlag = false;
u8 can2UpdateTimeFlag = false;

extern __IO uint16_t PWM2DutyCycle;
extern __IO uint32_t PWM2Frequency ;
extern __IO uint16_t PWM1DutyCycle;
extern __IO uint32_t PWM1Frequency;

extern uint8_t UART1_Rx_Cnt;
u8 ChooseBoard=0;			//这个全局变量用来接受旋钮的值，从而选择板子


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
		Uart1_init();//uart1的GPIO初始化 ,提前的初始化，防止中间有打印显示信息会无法输出
	
		printf("MTDCPU_APP1 板卡启动!\r\n");
			
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

	/*Whh 2016-11-9 修改在创建内核对象 以及 任务的时候 防止中断打断，关闭所有中断。*/
	CPU_IntDis(); //关闭全部中断  
	//对象的创建
	AppObjCreate();                                          /* Create Applicaiton kernel objects                    */

	//任务的创建
  AppTaskCreate();                                            /* Create Application tasks                             */

	CPU_IntEn(); //使能全部中断
	
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
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/,		//任务选项
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
//                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/,		//任务选项
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
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值


/*W H H*/
		//创建从站启动管理任务
	OSTaskCreate((OS_TCB 		* )&CAN1SlaveStartupManagerTCB,				//任务控制块
								(CPU_CHAR	* )"CAN1SlaveStartManager_task", 			//任务名字
                 (OS_TASK_PTR)CAN1Slave_StartupManager_Take, 		//任务函数
                 (void		  *)0,														//传递给任务函数的参数
                 (OS_PRIO	   )CAN1SLAVE_STARTUPMANAGER_TASK_PRIO,    	//任务优先级
                 (CPU_STK   *)&CAN1SLAVE_STARTUPMANAGER_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1SLAVE_STARTUPMANAGER_TASK_STK[CAN1SLAVE_STARTUPMANAGER_STK_SIZE/10],	//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1SLAVE_STARTUPMANAGER_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
								 
//		//创建从站启动管理任务
//	OSTaskCreate((OS_TCB 		* )&CAN2SlaveStartupManagerTCB,				//任务控制块
//								(CPU_CHAR	* )"CAN2SlaveStartManager_task", 			//任务名字
//                 (OS_TASK_PTR)CAN2Slave_StartupManager_Take, 		//任务函数
//                 (void		  *)0,														//传递给任务函数的参数
//                 (OS_PRIO	   )CAN2SLAVE_STARTUPMANAGER_TASK_PRIO,    	//任务优先级
//                 (CPU_STK   *)&CAN2SLAVE_STARTUPMANAGER_TASK_STK[0],	//任务堆栈基地址
//                 (CPU_STK_SIZE)CAN2SLAVE_STARTUPMANAGER_TASK_STK[CAN2SLAVE_STARTUPMANAGER_STK_SIZE/10],	//任务堆栈深度限位
//                 (CPU_STK_SIZE)CAN2SLAVE_STARTUPMANAGER_STK_SIZE,			//任务堆栈大小
//                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
//                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
//                 (void   	   *)0,					//用户补充的存储区
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
//                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
				 

		//创建can1从站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN1SlaveRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN1SlaveRx_task", 			//任务名字
                 (OS_TASK_PTR)can1slave_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN1SLAVERX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN1SLAVERX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1SLAVERX_TASK_STK[CAN1SLAVERX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1SLAVERX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				


		//创建can1从站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN1SlaveTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN1SlaveTx_task", 			//任务名字
                 (OS_TASK_PTR)can1slave_send_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN1SLAVETX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN1SLAVETX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1SLAVETX_TASK_STK[CAN1SLAVETX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1SLAVETX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		


		//创建can2从站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN2SlaveRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN2SlaveRx_task", 			//任务名字
                 (OS_TASK_PTR)can2slave_recv_thread, 	//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN2SLAVERX_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&CAN2SLAVERX_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2SLAVERX_TASK_STK[CAN2SLAVERX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2SLAVERX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值				


		//创建can2从站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN2SlaveTxTaskTCB,			 //任务控制块
								(CPU_CHAR	* )"CAN2SlaveTx_task", 			 //任务名字
                 (OS_TASK_PTR)can2slave_send_thread, 	 //任务函数
                 (void		  *)0,											 //传递给任务函数的参数
                 (OS_PRIO	   )CAN2SLAVETX_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CAN2SLAVETX_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2SLAVETX_TASK_STK[CAN2SLAVETX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2SLAVETX_STK_SIZE,	//任务堆栈大小
                 (OS_MSG_QTY  )0,											//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,											//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,											//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		

		//创建can3主站接受任务
	OSTaskCreate((OS_TCB 		* )&CAN3MasterRxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN3MasterRx_task", 			//任务名字
                 (OS_TASK_PTR)can3master_recv_thread, 	//任务函数
                 (void		  *)0,												//传递给任务函数的参数
                 (OS_PRIO	   )CAN3MASTERRX_TASK_PRIO,  	//任务优先级
                 (CPU_STK   *)&CAN3MASTERRX_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3MASTERRX_TASK_STK[CAN3MASTERRX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3MASTERRX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,												//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,												//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,												//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);										//存放该函数错误时的返回值				

		//创建can3主站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN3MasterTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN3MasterTx_task", 			//任务名字
                 (OS_TASK_PTR)can3master_send_thread, 	//任务函数
                 (void		  *)0,												//传递给任务函数的参数
                 (OS_PRIO	   )CAN3MASTERTX_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CAN3MASTERTX_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3MASTERTX_TASK_STK[CAN3MASTERTX_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3MASTERTX_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,												//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,												//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,												//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);										//存放该函数错误时的返回值			
								 								 
	//创建can3主站接受处理任务
	OSTaskCreate((OS_TCB 		* )&CAN3RxProcTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN3RxProc_task", 			//任务名字
                 (OS_TASK_PTR)can3RxProcthread, 			//任务函数
                 (void		  *)0,											//传递给任务函数的参数
                 (OS_PRIO	   )CAN3RXPROC_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&CAN3RxProc_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN3RxProc_TASK_STK[CAN3RXPROC_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN3RXPROC_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,											//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,											//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,											//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);									//存放该函数错误时的返回值		
									 
		///*OLE显示以及按键任务*/
	OSTaskCreate((OS_TCB 		* )&OledKeyTaskTCB,					//任务控制块
								(CPU_CHAR	* )"OledKey_task", 					//任务名字
                 (OS_TASK_PTR)Oled_Key_thread, 				//任务函数
                 (void		  *)0,											//传递给任务函数的参数
                 (OS_PRIO	   )OLEDKEY_TASK_PRIO,     	//任务优先级
                 (CPU_STK   *)&OLEDKEY_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)OLEDKEY_TASK_STK[OLEDKEY_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)OLEDKEY_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,					//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			

		//创建can1 led 发送任务
	OSTaskCreate((OS_TCB 		* )&CAN1LED_TCB,					//任务控制块
								(CPU_CHAR	* )"CAN1LEDTask", 				//任务名字
                 (OS_TASK_PTR)CAN1LEDTake, 					//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN1LED_TASK_PRIO,    //任务优先级
                 (CPU_STK   *)&CAN1LED_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1LED_TASK_STK[CAN1LED_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1LED_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,										//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);								//存放该函数错误时的返回值		

		//创建can2 led 发送任务
	OSTaskCreate((OS_TCB 		* )&CAN2LED_TCB,					//任务控制块
								(CPU_CHAR	* )"CAN2LEDTask", 				//任务名字
                 (OS_TASK_PTR)CAN2LEDTake, 					//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )CAN2LED_TASK_PRIO,    //任务优先级
                 (CPU_STK   *)&CAN2LED_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2LED_TASK_STK[CAN2LED_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2LED_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,										//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);								//存放该函数错误时的返回值				
						 
		//创建can 定时发送PDO 发送任务
	OSTaskCreate((OS_TCB 		* )&TimTxPDO_TCB,						//任务控制块
								(CPU_CHAR	* )"TimTxPDOTask", 					//任务名字
                 (OS_TASK_PTR)TimTxPDOTake, 					//任务函数
                 (void		  *)0,											//传递给任务函数的参数
                 (OS_PRIO	   )TimTxPDO_TASK_PRIO,   	//任务优先级
                 (CPU_STK   *)&TimTxPDO_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)TimTxPDO_TASK_STK[TimTxPDO_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)TimTxPDO_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,										//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);								//存放该函数错误时的返回值											 
								 
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
	CPU_SR_ALLOC();			//变量定义，用于保存SR状态.
	OS_CRITICAL_ENTER();	//进入临界区	
	
	/* 创建同步信号量 */ 
	BSP_OS_SemCreate(&SEM_SYNCH, 0,(CPU_CHAR *)"SEM_SYNCH");

		//创建一个互斥信号量
	OSMutexCreate((OS_MUTEX*	)&CAN3_MUTEX,
								(CPU_CHAR*	)"CAN3_MUTEX",
                (OS_ERR*		)&err);
	
	//创建一个can3接受信号量
	OSSemCreate ((OS_SEM*	)&can3recv_sem,
                 (CPU_CHAR*	)"can3recvsem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);	
	
	//创建一个can3接受处理函数信号量
	OSSemCreate ((OS_SEM*	)&can3RxProc_sem,
                 (CPU_CHAR*	)"can3RxProSem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);	
	
	//	//创建一个can3发送信号量
	//	OSSemCreate ((OS_SEM*	)&can3tran_sem,
	//                 (CPU_CHAR*	)"can3transem",
	//                 (OS_SEM_CTR)0,		
	//                 (OS_ERR*	)&err);
	

	/*创建事件标志组*/
	OSFlagCreate((OS_FLAG_GRP*)&CAN1LedEventFlag,				//指向事件标志组
                 (CPU_CHAR*	  )"CAN1 Led Event Flag",	//名字
                 (OS_FLAGS	  )0,											//事件标志组初始值
                 (OS_ERR*  	  )&err);									//错误码
								 
	OSFlagCreate((OS_FLAG_GRP*)&CAN2LedEventFlag,				//指向事件标志组
                 (CPU_CHAR*	  )"CAN2 Led Event Flag",	//名字
                 (OS_FLAGS	  )0,											//事件标志组初始值
                 (OS_ERR*  	  )&err);									//错误码	
	
	//创建can3功能时间
	OSTmrCreate((OS_TMR		*)&CAN3TimeTmr,		//定时器1
							(CPU_CHAR	*)"CAN3TimeTmr",	//定时器名字
							(OS_TICK	 )0,							//初始化定时器的延时值
							(OS_TICK	 )100,   		  		//100*10(频率)=1000ms						//1000*1=1000ms  重复周期
							(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
							(OS_TMR_CALLBACK_PTR)CAN3TimeTrmCallBack,	//定时器1回调函数
							(void	    *)0,							//回调函数参数为0
							(OS_ERR	  *)&err);					//返回的错误码		
	
	//创建can3功能时间
	OSTmrCreate((OS_TMR		*)&TxPDOTmr,			//定时器1
							(CPU_CHAR	*)"TxPDOTmr",			//定时器名字
							(OS_TICK	 )0,							//初始化定时器的延时值
							(OS_TICK	 )1,   		  			//1*10(频率)=10ms							//1000*1=1000ms  重复周期
							(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
							(OS_TMR_CALLBACK_PTR)TxPDOTmrCallBack,	//定时器1回调函数
							(void	    *)0,							//回调函数参数为0
							(OS_ERR	  *)&err);					//返回的错误码		

	OS_CRITICAL_EXIT();	//退出临界区		

}


/////*EEPROM 测试*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
//#define sEE_WRITE_ADDRESS1        0xB0
//#define sEE_READ_ADDRESS1         0xB0
//#define BUFFER_SIZE1             (countof(aTxBuffer1)-1)
//#define BUFFER_SIZE2             (countof(aTxBuffer2)-1)
//#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
//#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)
///* Private macro -------------------------------------------------------------*/
//#define countof(a) (sizeof(a) / sizeof(*(a)))

///* Private variables ---------------------------------------------------------*/
//uint8_t aTxBuffer1[] =   "/* STM32F40xx I2C Firmware Library EEPROM driver example: \
//                        buffer 1 transfer into address sEE_WRITE_ADDRESS1 */ \
//                        Example Description \
//                        This firmware provides a basic example of how to use the I2C firmware library and\
//                        an associate I2C EEPROM driver to communicate with an I2C EEPROM device (here the\
//                        example is interfacing with M24C64 EEPROM)\
//                          \
//                        I2C peripheral is configured in Master transmitter during write operation and in\
//                        Master receiver during read operation from I2C EEPROM. \
//                          \
//                        The peripheral used is I2C1 but can be configured by modifying the defines values\
//                        in stm324xg_eval.h file. The speed is set to 100kHz and can be configured by \
//                        modifying the relative define in stm32_eval_i2c_ee.h file.\
//                         \
//                        For M24C64 devices all the memory is accessible through the two-bytes \
//                        addressing mode and need to define block addresses. In this case, only the physical \
//                        address has to be defined (according to the address pins (E0,E1 and E2) connection).\
//                        This address is defined in i2c_ee.h (default is 0xA0: E0, E1 and E2 tied to ground).\
//                        The EEPROM addresses where the program start the write and the read operations \
//                        is defined in the main.c file. \
//                         \
//                        First, the content of Tx1_Buffer is written to the EEPROM_WriteAddress1 and the\
//                        written data are read. The written and the read buffers data are then compared.\
//                        Following the read operation, the program waits that the EEPROM reverts to its \
//                        Standby state. A second write operation is, then, performed and this time, Tx2_Buffer\
//                        is written to EEPROM_WriteAddress2, which represents the address just after the last \
//                        written one in the first write. After completion of the second write operation, the \
//                        written data are read. The contents of the written and the read buffers are compared.\
//                         \
//                        All transfers are managed in DMA mode (except when 1-byte read/write operation is\
//                        required). Once sEE_ReadBuffer() or sEE_WriteBuffer() function is called, the \
//                        use application may perform other tasks in parallel while Read/Write operation is\
//                        managed by DMA.\
//                          \
//                        This example provides the possibility to use the STM324xG-EVAL LCD screen for\
//                        messages display (transfer status: Ongoing, PASSED, FAILED).\
//                        To enable this option uncomment the define ENABLE_LCD_MSG_DISPLAY in the main.c\
//                        file.                                                                              ";
//uint8_t aTxBuffer2[] =  "/* STM32F40xx I2C Firmware Library EEPROM driver example: \
//                        buffer 2 transfer into address sEE_WRITE_ADDRESS2 */";
//uint8_t aRxBuffer1[BUFFER_SIZE1];
//uint8_t aRxBuffer2[BUFFER_SIZE2];

//volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
//volatile uint16_t uhNumDataRead = 0;

///* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

//extern void can_recv_thread(void);
//extern	void can_slave_init(void);



//#define OLED_BASE     	 ((uint32_t)(0x6C000000))
//#define OLED_CMD		*(__IO uint16_t *)(OLED_BASE)								// D/C=0: Command
//#define OLED_DATA		*(__IO uint16_t *)(OLED_BASE + (1 << 18))		/* FSMC_A18接D/C ,D/C=1: Data，采用的8bit模式*/


//#define MC1_CAR_TYPE	"MC1(1)"
//#define T_CAR_TYPE		"T(4)"
//#define M_CAR_TYPE		"M(3)"
//#define T1_CAR_TYPE		"T1(5)"
//#define T2_CAR_TYPE		"T2(6)"
//#define MC2_CAR_TYPE	"MC2(2)"



///*W H H*/
//u8 oled_read_Date(u8 *pBuffer)
//{
//	*pBuffer =  OLED_DATA;
//}

	/*车厢号*/
	u16		CarNumber=4031;
	u8    CarNo=1;
	char cartype[10];

void Main_task(void *p_arg)
{
//	CPU_SR_ALLOC();	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//	uint64_t  a;
	OS_ERR err;
//	static u8 SlaveStartCount;
//	static u8 can1StartCount;
//	static u8 can2StartCount;
//	static u8 can1Flag;
//	static u8 can2Flag=1;
//		RTC_TIME time;

	/*iap UART1*/
	static u16 UartOldCount=0;	//老的串口接收数据值
	//u16 AppLenth=0;	//接收到的app代码长度
	
	/*spi can*/
//		Frame_TypeDef frame;
//		frame.SID=10;
//		frame.DLC=8;
//		frame.Type=CAN_STD;
//	
	(void)p_arg;
	debugprintf("进入到Main任务\r\n");
	
	
	/*pwm*/
//	MTDCPU_PWM2IN_TIM5_CH1_Init(0xFFFF,84-1);		//pwm输入初始化以1M的频率捕捉
//	MTDCPU_PWM1IN_TIM2_CH2_Init(0xFFFF,84-1);
	
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
	 
	 /*RTC*/
//	 time._year = 16;
//	 time._month=7;
//	 time._day=23;
//	 time._hour=18;
//	 time._min=54;
//	 time._sec=20;
//	 time._week=6;
//	 
//	 ISL1208_Init();
//	 Auto_Time_Set();
//	Manual_Set_Time(&time);
	
	/*can3 初始化*/
//{
//	u8 res;

//	
//		res = MTDCPU_CAN3_MCP2515_Init(MCP2515_CAN_BAUD_125K);
////		if(res == MCP2515_OK)
////			return 0;
////		else
////			return CAN_FAILINIT;
//}

	/*pwm输出*/
//	bsp_SetTIMOutPWM(BUZZER_GPIO,BUZZER_GPIO_PIN,TIM1,1,1000,9000);  //蜂鸣器

//	MTDCPU_PWM2IN_TIM5_CH1_Init(0xFFFF,84-1);		//pwm输入初始化以1M的频率捕捉
//	MTDCPU_PWM1IN_TIM2_CH2_Init(0xFFFF,84-1);
	
	
//Buzzer_Func_On();


///*EEPROM_2*/
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



///*EEPROM*/
//		/* Initialize the I2C EEPROM driver ----------------------------------------*/
//		sEE_Init();  
//		delay_ms(10);
//			
//	
//		/*EPPROM*/
//	OS_CRITICAL_ENTER();
//	{
//		u8 i;
//		/*EEPROM 测试*/
//		printf("1_EEPROM\r\n");
//		/* First write in the memory followed by a read of the written data --------*/
//		/* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
//		sEE_WriteBuffer(aTxBuffer1, sEE_WRITE_ADDRESS1, BUFFER_SIZE1); 
//			printf("2_EEPROM\r\n");
//		/* Wait for EEPROM standby state */
//		sEE_WaitEepromStandbyState();  
//		printf("3_EEPROM\r\n");
//		/* Set the Number of data to be read */
//		uhNumDataRead = BUFFER_SIZE1;
//		printf("4_EEPROM\r\n");
//		delay_ms(10);
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
////			for(i=0;i<BUFFER_SIZE1;i++)
////			{
////				printf("r1=%c",aRxBuffer1[i]);
////			}
////			printf("\r\n");
//			printf("Size1=%d\r\n",BUFFER_SIZE1);
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

//		/* Second write in the memory followed by a read of the written data -------*/
//		/* Write on I2C EEPROM from sEE_WRITE_ADDRESS2 */
//		sEE_WriteBuffer(aTxBuffer2, sEE_WRITE_ADDRESS2, BUFFER_SIZE2); 

//		/* Wait for EEPROM standby state */
//		sEE_WaitEepromStandbyState();  
//		
//		/* Set the Number of data to be read */
//		uhNumDataRead = BUFFER_SIZE2;  
//		
//		delay_ms(10);
//		/* Read from I2C EEPROM from sEE_READ_ADDRESS2 */
//		sEE_ReadBuffer(aRxBuffer2, sEE_READ_ADDRESS2, (uint16_t *)(&uhNumDataRead));

//		printf(" Transfer 2 Ongoing \r\n");
//		 /* Wait till DMA transfer is complete (Transfer complete interrupt handler 
//			resets the variable holding the number of data to be read) */
//		 while (uhNumDataRead > 0)
//		{}  
////			for(i=0;i<BUFFER_SIZE1;i++)
////			{
////				printf("r2=%c",aRxBuffer2[i]);
////			}
////			printf("\r\n");
//				 printf("Size2=%d\r\n",BUFFER_SIZE2);
//		 /* Check if the data written to the memory is read correctly */
//		TransferStatus1 = Buffercmp(aTxBuffer2, aRxBuffer2, BUFFER_SIZE1);

//		if (TransferStatus1 == PASSED)
//		{
//			printf(" Transfer 2 PASSED \r\n ");
//		}
//		else
//		{
//			printf(" Transfer 2 FAILED  \r\n");
//		} 
//		  /* Free all used resources */
//	}
//			OS_CRITICAL_EXIT();//退出临界区
//		
		
	
/*main函数中 初始化一些参数*/
/*OLED*/

		switch(ChooseBoard)
		{
			case MC1_MTD_NODEID:
				memcpy(cartype,MC1_CAR_TYPE,sizeof(MC1_CAR_TYPE));
				CarNumber = 4011;
				CarNo=1;
	
				can1_MC1TxCarNo[0] =  CarNumber/100;
				can1_MC1TxCarNo[1] =  CarNumber%100;
			
				can2_MC1TxCarNo[0] =  CarNumber/100; 
				can2_MC1TxCarNo[1] =  CarNumber%100;					
				break;
			case T_MTD_NODEID:
				memcpy(cartype,T_CAR_TYPE,sizeof(T_CAR_TYPE));
				CarNumber = 4012;
				CarNo=2;	
				break;
			case M_MTD_NODEID:
				memcpy(cartype,M_CAR_TYPE,sizeof(M_CAR_TYPE));
				CarNumber = 4013;
				CarNo=3;
			
				can1_MTxCarNo[0] =  CarNumber/100;
				can1_MTxCarNo[1] =  CarNumber%100;	

				can2_MTxCarNo[0] =  CarNumber/100;
				can2_MTxCarNo[1] =  CarNumber%100;			
				break;
			case T1_MTD_NODEID:
				memcpy(cartype,T1_CAR_TYPE,sizeof(T1_CAR_TYPE));
				CarNumber = 4014;
				CarNo=4;
				break;
			case T2_MTD_NODEID:
				memcpy(cartype,T2_CAR_TYPE,sizeof(T2_CAR_TYPE));
				CarNumber = 4015;
				CarNo=5;	
				break;
			case MC2_MTD_NODEID:
				memcpy(cartype,MC2_CAR_TYPE,sizeof(MC2_CAR_TYPE));
				CarNumber = 4016;
				CarNo=6;
			
				can1_MC2TxCarNo[0] =  CarNumber/100;
				can1_MC2TxCarNo[1] =  CarNumber%100;			
				
				can2_MC2TxCarNo[0] =  CarNumber/100;
				can2_MC2TxCarNo[1] =  CarNumber%100;				
				break;
			default :
				break;
		}

		/*在while中已经显示了*/
//	ISL1208_GetTime(&time);		//获取时间
//	
//	sprintf(pbuf,"%d%s%d%s%d%s%d%s%d",time._year+2000,"-",time._month,"-",time._day,"  ",time._hour,":",time._min);
////OLED屏幕不能出界，出界就会有闪动
//{
////	Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
//	
//	Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);                 /* 绘制Agreement标题 */
//	Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  16,  0,   0, 0, 0, (unsigned char *)"车辆编号:  4031");    /* 绘制两行文本 */
//	Draw_Component(COMPONENT_LABEL_ENABLED,  3, 32, 0,  16,  0,   0, 0, 0, (unsigned char *)"车    号:  1 ");
//	Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)"车型编号:  MC1(1)");                     /* 绘制确认按钮 */

////	Change_Index(1);                                                                                    /* 将光标下移一位，库会自动选中Button */
//	Update_Component(4);                                                                                /* 更新组件状态 */
//	Next_Frame(); 

//}
		
//			Auto_Time_Set();
		
//			time._year=16;
//			time._month=10;
//			time._day=8;
//			time._sec=50;
//			time._min=42;
//			time._hour=10;		
//		//OS_CRITICAL_ENTER();
//		Manual_Set_Time(&time);	//W H H 测试
//		//OS_CRITICAL_EXIT();//退出临界区


/*开启软件定时器*/
	OSTmrStart(&CAN3TimeTmr,&err);	//开启定时器can3Time
	OSTmrStart(&TxPDOTmr,&err);

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

	
//	/*测试按键*/
//	a=MTDCPU_KEY_Scan(0);
//	switch(a)
//	{
//		case 1:
//			printf("按键1按下！\r\n");
//			break;
//		case 2:
//			printf("按键2按下！\r\n");		
//		break;
//		case 3:
//			printf("按键3按下！\r\n");
//		break;
//		case 4:
//			printf("按键4按下！\r\n");
//		break;
//	}
	
//	if(can1CarNumberFlag == true )
//	{
//		debugprintf("can1car !\r\n");
//		CarNo = can1_Car_Number_1;
//		CarNumber = can1_Car_Number_2;		//车辆编号
//		can1CarNumberFlag = false;
//	}
//	
//	if(can2CarNumberFlag == true )
//	{
//		debugprintf("can2car !\r\n");
//		CarNo = can2_Car_Number_1;
//		CarNumber = can2_Car_Number_2;		//车辆编号
//		can2CarNumberFlag =false;
//	}
//	
//	if(can1UpdateTimeFlag == true)
//	{
//		debugprintf("can1Data !\r\n");
//		time._year = can1_Car_DateTime[1];
//		time._month = can1_Car_DateTime[2];
//		time._day = can1_Car_DateTime[3];
//		time._hour = can1_Car_DateTime[4];
//		time._min = can1_Car_DateTime[5];
//		time._sec = can1_Car_DateTime[6];
//		Manual_Set_Time(&time);
//		can1UpdateTimeFlag = false;
//	}
//	
//	if(can2UpdateTimeFlag == true)
//	{
//		debugprintf("can2Data !\r\n");
//		time._year = can1_Car_DateTime[1];
//		time._month = can1_Car_DateTime[2];
//		time._day = can1_Car_DateTime[3];
//		time._hour = can1_Car_DateTime[4];
//		time._min = can1_Car_DateTime[5];
//		time._sec = can1_Car_DateTime[6];
//		Manual_Set_Time(&time);
//		can2UpdateTimeFlag =false;
//	}
//	
//	//进入临界区 代码应该写在这里
//	OS_CRITICAL_ENTER();
//		ISL1208_GetTime(&time);		//获取时间
//	OS_CRITICAL_EXIT();//退出临界区
//	
//	Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
//	
//	sprintf(pbuf,"%d%s%02d%s%02d%02d%02d%02d",time._year+2000,"-",time._month,"-",time._day,time._hour,time._min,time._sec);
//		
//	Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);     /* 绘制文本 */

//	memset(pbuf,0,sizeof(pbuf));
//	sprintf(pbuf,"%s%d","车辆编号: ",CarNumber);	
//	Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
//	
//	memset(pbuf,0,sizeof(pbuf));
//	sprintf(pbuf,"%s%d","车    号: ",CarNo);		
//	Draw_Component(COMPONENT_LABEL_ENABLED,  3, 32, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	 	/* 绘制文本 */
//		
//	memset(pbuf,0,sizeof(pbuf));
//	sprintf(pbuf,"%s%s", "车型编号: ",cartype);	
//	Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */               
//	
//	Update_Component(0);                                                                              /* 更新组件状态 */

//	Draw_Reverse(32,0,48,128,0);//反色
//	Next_Frame(); 
//	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	/*PWM输入打印*/
//	{
//		printf("PWM1占空比%d%%\r\n",PWM1DutyCycle);
//		printf("PWM1频率%dus\r\n",PWM1Frequency);
//		printf("PWM2占空比%d%%\r\n",PWM2DutyCycle);
//		printf("PWM2频率%dus\r\n",PWM2Frequency);
////		printf("PA0=%d\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0));
////		printf("PA1=%d\r\n",GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1));
//	}
	
	
	
	
	
	
	
	
	
	
	
	
//	if(	bsp_CheckTimer(0))
//	{
//		Buzzer_Func_Off();
//	}
	
//	if(bsp_CheckTimer(1))
//	{
//		RunLed_Turn();
//	}	
	
	/*MTDCPU*/
//	SPICAN_SendData(can2_send,&frame);
//	
//	{
//		
//		Message a;
//		a.cob_id=0x08;
//		a.len =8;
//		memcpy(a.data,can2_send,8);	
//		
//		canSend((void *)CAN3,&a);
//	}
//	

		/*OLED*/

//	Fill_RAM_12864(1);
//	Checkerboard_12864();
//	Grayscale_12864();
//	Show_Frame_12864();
//Draw_Rectangle_12864(10,20,10,20,0xff);
//Show_Pattern_12864((unsigned char *)szASC16,0,10,0,10,0);
//{
//	unsigned char  Name[]={53,78,73,86,73,83,73,79,78,0};
//Show_String_12864(1,Name,0,0);
	
	
//}

//OLED_Draw_16x16String(0,0,0,6);

//{
//		OLED_DATA = 0x08;
//		a=	OLED_DATA;
//		printf("a=%lld\r\n",a);
//	}
//{
//	FONT_T a;
//	a.FontCode=FC_ST_16;
//	a.FrontColor=0;
//	a.BackColor=0;
//	a.Space=0;
////	OLED_DispStr(0,0,"车辆编号",&a);
//		OLED_DispStr(0,0,"ABCD",&a);
//	
//	a.FontCode =FC_ST_12;
//	OLED_DispStr(0,16,"车辆编号",&a);
////	
////	
////	Show_String_12864(1,"ABCDEFG",0,32);
////	Show_String_12864(1,"2",0,40);
////	
////	Draw_String(0,50,1,"2015-05-07  15:24",0xff,0x00);
////	Next_Frame(); 
//}


///*绘画组件*/
//{
//Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  6,  0,   0, 0, 0, "Agreement");                 /* 绘制Agreement标题 */
//Draw_Component(COMPONENT_LABEL_ENABLED,  2, 10, 0,  4,  0,   0, 0, 0, "COPYRIGHT RIJN, 2014. ");    /* 绘制两行文本 */
//Draw_Component(COMPONENT_LABEL_ENABLED,  3, 16, 0,  4,  0,   0, 0, 0, "PIXELNFINITE.COM ");
//Draw_Component(COMPONENT_BUTTON_ENABLED, 4, 50, 80, 62, 127, 0, 0, 0, "Agree");                     /* 绘制确认按钮 */

////Change_Index(1);                                                                                    /* 将光标下移一位，库会自动选中Button */
//Update_Component(4);                                                                                /* 更新组件状态 */
//Next_Frame();   
//}



////OLED屏幕不能出界，出界就会有闪动
//{
////	Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
//	
//	Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  16,  0,   0, 0, 0, pbuf);                 /* 绘制Agreement标题 */
//	Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  16,  0,   0, 0, 0, "车辆编号: 4031");    /* 绘制两行文本 */
//	Draw_Component(COMPONENT_LABEL_ENABLED,  3, 32, 0,  16,  0,   0, 0, 0, "车    号: 1 ");
//	Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, "车型编号: MC1(1)");                     /* 绘制确认按钮 */
////	
////	Change_Index(1);                                                                                    /* 将光标下移一位，库会自动选中Button */
//	Update_Component(4);                                                                                /* 更新组件状态 */
//	Next_Frame(); 

//}


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
		
//		{
//			/*RTC*/
//			ISL1208_GetTime(&time);
//			printf("_yera=%d\r\n",time._year);
//			printf("_month=%d\r\n",time._month);
//			printf("_day=%d\r\n",time._day);
//			printf("_hour=%d\r\n",time._hour);
//			printf("_min=%d\r\n",time._min);
//			printf("_sec=%d\r\n",time._sec);
//			printf("_week=%d\r\n",time._week);
//			
//		}
	
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
////			LED_Turn (0);
//		}
//		BSP_OS_TimeDlyMs(3000);//延时2s	,并且进行任务间的切换
//	}
//}


//extern int  can1test_slave(void);
//extern int  can2test_slave(void);
void CAN1Slave_StartupManager_Take(void *p_arg)
{
	OS_ERR err;
	static u8 SlaveStateCount =0;

	debugprintf("进入到从站启动管理任务！\r\n");

	while(1)
	{
		debugprintf("can1 slave manager\r\n");
		
		if(SlaveStateCount > 5 )		//连续发送5次从节点到初始化状态，发送bootup
		{
			debugprintf("can1 slave manager 5次到\r\n");
			//	SlaveStateCount = 0;
			OSTaskDel((OS_TCB*)&CAN1SlaveStartupManagerTCB,&err);	//任务从站管理任务函数执行5此后删除掉这个任务
		}						
		{
			e_nodeState slavestate;
			slavestate = getState(&MTDApp1CAN1Slave_Data);
			debugprintf("can1slavestate=0x%x\r\n",slavestate);
			if(slavestate != Operational/* 0x05*/)
			{	
					/*发送一个启动信号 cansend*/
					
					//					u8 BootMsg =0x00;
					//					uint32_t id= (0x700 | ChooseBoard);
					//					SlaveStateCount++;
					//					User_CAN1Transmit(id,&BootMsg,1);
					
					Message BootMessage;
					BootMessage.cob_id = (0x700 | ChooseBoard);
					BootMessage.data[0] = 0x00;
					BootMessage.len =1;
					BootMessage.rtr = CAN_RTR_DATA;
					canSend(CAN1,&BootMessage);
				
					canSend(CAN2,&BootMessage);			//WHH  修改can1 can2 同时发送启动报文
					
					SlaveStateCount++;					
			}
			else
			{
				OSTaskDel((OS_TCB*)&CAN1SlaveStartupManagerTCB,&err);	//任务从站状态位操作状态后删除掉这个任务
			}
		}
		BSP_OS_TimeDlyMs(2000);			//调用这个延时是将其进入调度函数，发生任务的调度。
	}
}


//void CAN2Slave_StartupManager_Take(void *p_arg)
//{
//	OS_ERR err;
//	static u8 SlaveStateCount =0;
//	debugprintf("进入到从站启动管理任务！\r\n");
//	
//	while(1)
//	{
//		debugprintf("slave manager\r\n");
//		
//		if(SlaveStateCount > 5)		//连续发送5次从节点到初始化状态，发送bootup
//		{
//			debugprintf("can2slave manager 5次到\r\n");
//			//	SlaveStateCount = 0;
//			OSTaskDel((OS_TCB*)&CAN2SlaveStartupManagerTCB,&err);	//任务从站管理任务函数执行5此后删除掉这个任务
//		}						
//		{
//			e_nodeState slavestate;
//			slavestate = getState(&MTDApp1CAN2Slave_Data);
//			debugprintf("can2slavestate=0x%x\r\n",slavestate);
//			if(slavestate != Operational/* 0x05*/)
//			{
//					//					/*发送一个启动信号 cansend*/
//					//					u8 BootMsg =0x00;
//					//					uint32_t id= (0x700 | ChooseBoard);
//					//					SlaveStateCount++;
//					//					User_CAN2Transmit(id,&BootMsg,1);
//					Message BootMessage;
//					BootMessage.cob_id = (0x700 | ChooseBoard);
//					BootMessage.data[0] = 0x00;
//					BootMessage.len =1;
//					BootMessage.rtr = CAN_RTR_DATA;
//					canSend(CAN2,&BootMessage);
//					
//					SlaveStateCount++;				
//			}
//			else
//			{
//				OSTaskDel((OS_TCB*)&CAN2SlaveStartupManagerTCB,&err);	//任务从站状态位操作状态后删除掉这个任务
//			}
//		}
//		BSP_OS_TimeDlyMs(2000);			//调用这个延时是将其进入调度函数，发生任务的调度。
//	}
//}

/*OLED Key任务*/
extern void Oled_Key_thread(void *p_arg)
{
	(void)p_arg;/* 避免编译器报警 */
	
	while(1)
	{
		KeySetMenu();
		BSP_OS_TimeDlyMs(50);		//whh经测试50ms按键的速度可以很快。
	}
}
					


/*
can1LED闪烁任务，利用事件标志组来判断任务
*/
void CAN1LEDTake(void *p_arg)
{
	OS_ERR err;

	while(1)
	{	
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&CAN1LedEventFlag,
							 (OS_FLAGS	  )CAN1TxLED_FLAG,		//发送标志位
							 (OS_TICK     )20,									//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_CPU_CANLED_Off(CAN1_TX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_CPU_CANLED_On(CAN1_TX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_CPU_CANLED_Off(CAN1_TX_LED);
		}
		
				//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&CAN1LedEventFlag,
							 (OS_FLAGS	  )CAN1RxLED_FLAG,		//发送标志位
							 (OS_TICK     )20,									//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_CPU_CANLED_Off(CAN1_RX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_CPU_CANLED_On(CAN1_RX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_CPU_CANLED_Off(CAN1_RX_LED);
		}
		//BSP_OS_TimeDlyMs(10);
	}
}


/*
can2LED闪烁任务，利用事件标志组来判断任务
*/
void CAN2LEDTake(void *p_arg)
{
	OS_ERR err;

	while(1)
	{	
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&CAN2LedEventFlag,
							 (OS_FLAGS	  )CAN2TxLED_FLAG,		//发送标志位
							 (OS_TICK     )20,								//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_CPU_CANLED_Off(CAN2_TX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_CPU_CANLED_On(CAN2_TX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_CPU_CANLED_Off(CAN2_TX_LED);
		}
		
				//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&CAN2LedEventFlag,
							 (OS_FLAGS	  )CAN2RxLED_FLAG,		//发送标志位
							 (OS_TICK     )20,								//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_CPU_CANLED_Off(CAN2_RX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_CPU_CANLED_On(CAN2_RX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_CPU_CANLED_Off(CAN2_RX_LED);
		}
		//BSP_OS_TimeDlyMs(10);
	}
}

								 
/*
	can1 2 定时发送PDO数据 
	调用的频率是10ms，即每10ms调用一次这个函数
*/
void TimTxPDOTake(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	//u8 i;
	static u8 Tim100msCount=0;
	static u8 Tim200msCount=0;
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);		//阻塞等待任务级信号量
		if(err !=OS_ERR_NONE)
		{
			printf("TimTxPDO Timer Err!\r\n");
		}
		
		else
		{
			
			if(Tim100msCount>10)       //调用5次就是50ms
			{
				Tim100msCount = 0; //清零
				
				switch(ChooseBoard)
				{
					case MC1_MTD_NODEID:
						/*MC1*/
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[0].last_message.cob_id = 0;
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,0);		//pdo 0x183			
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,0);		//pdo号0
					break;
					
					case M_MTD_NODEID:
						/*M*/
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[1].last_message.cob_id = 0;			
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,1);		//pdo 0x185
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[1].last_message.cob_id = 0;					
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,1);		//pdo号1	
					break;

					case MC2_MTD_NODEID:
						/*MC2*/
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[2].last_message.cob_id = 0;			
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,2);		//pdo 0x188				
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[2].last_message.cob_id = 0;	
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,2);		//pdo号		
					break;
					
					default:
						break;
				}				
				
			}
			Tim100msCount++;	
			
			if(Tim200msCount>20)
			{
				Tim200msCount =0;
				
				switch(ChooseBoard)
				{
					case MC1_MTD_NODEID:
						/*MC1 ATC_Sdr*/
						MTDCPUProATCSdrLogicalData();
					
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[80].last_message.cob_id = 0;	
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,80);		//pdo号0x20B , 80			

						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[80].last_message.cob_id = 0;	
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,80);		//pdo号0x20B , 80							
					
						/*can3 发送ATC sdr1*/
						MTDCAN3Master_Data.PDO_status[7].last_message.cob_id = 0;
					  sendOnePDOevent(&MTDCAN3Master_Data,7);		//pdo号0x189 , 7		
						break;
					
					case MC2_MTD_NODEID:
						/*MC2 ATC_Sdr*/
						MTDCPUProATCSdrLogicalData();
					
						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN1Slave_Data.PDO_status[81].last_message.cob_id = 0;	
						sendOnePDOevent(&MTDApp1CAN1Slave_Data,81);		//pdo号0x20B , 81			

						/* force emission of PDO by artificially changing last emitted */
						MTDApp1CAN2Slave_Data.PDO_status[81].last_message.cob_id = 0;	
						sendOnePDOevent(&MTDApp1CAN2Slave_Data,81);		//pdo号0x20B , 81							
					
						/*can3 发送ATC sdr1*/
						MTDCAN3Master_Data.PDO_status[7].last_message.cob_id = 0;
					  sendOnePDOevent(&MTDCAN3Master_Data,7);		//pdo号0x189 ,					
						break;
					
					default :
						break;
				}
			}
			Tim200msCount++;
			
			
			//		for(i=0;i<80;i++)
			//		{
			//			/*0x209*/
			//			MTDApp1CAN1Slave_Data.PDO_status[/*79*/i].last_message.cob_id = 0;	
			//			sendOnePDOevent(&MTDApp1CAN1Slave_Data,i/*79*/);		//pdo 0x209
			//			MTDApp1CAN2Slave_Data.PDO_status[i/*79*/].last_message.cob_id = 0;
			//			sendOnePDOevent(&MTDApp1CAN2Slave_Data,i/*79*/);		//pdo号0x209	
			//		}
		
		}
		//BSP_OS_TimeDlyMs(10);
	}
}

/*
	任务定时器：
		回调函数被执行时会锁调度器，所以你必须让回调函数尽可能地短
*/
/*定时器回调函数*/
void CAN3TimeTrmCallBack(void *p_tmr, void *p_arg)
{
	/*获取本地时间 ,并把时间复制给can3对象字典*/
	static 	RTC_TIME time;			
	
	ISL1208_GetTime(&time);		//获取时间
	
	can3_TxTime[0] = time._year;
	can3_TxTime[1] = time._month;
	can3_TxTime[2] = time._day;
	can3_TxTime[3] = time._hour;
	can3_TxTime[4] = time._min;
	can3_TxTime[5] = time._sec;
}


/*
	定时发送PDO的定时器回调函数
*/
void TxPDOTmrCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	
	OSTaskSemPost(&TimTxPDO_TCB,OS_OPT_POST_NONE,&err);//发送can1任务及信号量			
}



/*1.VVVF动作判断 所需要的全局变量*/
/*VVVF电源 */
bool DI_VVVFPowertx_B1_10 = 0;	 			//VVVF电源  MC1
bool DI_VVVFPowertx_B1_30 = 0;	 			//VVVF电源  M
bool DI_VVVFPowertx_B1_60 = 0;	 			//VVVF电源  MC2

/*VVVF 电制有效位CRD*/
bool TCU1_CRDtx_SD_B1_10 = 0; 				//VVVF1协议中的电制有效位 （char No.7 Bit5）
bool TCU2_CRDtx_SD_B1_10 = 0; 				//VVVF2协议中的电制有效位 （char No.7 Bit5） 

bool TCU1_CRDtx_SD_B1_30 = 0; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
bool TCU2_CRDtx_SD_B1_30 = 0; 				//VVVF2协议中的电制有效位 （char No.7 Bit5） 

bool TCU1_CRDtx_SD_B1_60 = 0; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
bool TCU2_CRDtx_SD_B1_60 = 0; 				//VVVF2协议中的电制有效位 （char No.7 Bit5） 

/*VVVF“群开放UCORM*/
bool TCU1_UCORtx_SD_B1_10 = 0;    		//VVVF1协议中的（char No.17 Bit1）
bool TCU2_UCORtx_SD_B1_10 = 0;		 		//VVVF2协议中的（char No.17 Bit1）

bool TCU1_UCORtx_SD_B1_30 = 0;    		//VVVF1协议中的（char No.17 Bit1）
bool TCU2_UCORtx_SD_B1_30 = 0;		 		//VVVF2协议中的（char No.17 Bit1）

bool TCU1_UCORtx_SD_B1_60 = 0;    		//VVVF1协议中的（char No.17 Bit1）
bool TCU2_UCORtx_SD_B1_60 = 0;		 		//VVVF2协议中的（char No.17 Bit1）

/*VVVF BSLB_F大滑行有效位*/
bool TCU1_BSLBFtx_SD_B1_10 = 0; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
bool TCU2_BSLBFtx_SD_B1_10 = 0; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）

bool TCU1_BSLBFtx_SD_B1_30 = 0; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
bool TCU2_BSLBFtx_SD_B1_30 = 0; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）

bool TCU1_BSLBFtx_SD_B1_60 = 0; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
bool TCU2_BSLBFtx_SD_B1_60 = 0; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）

/*VVVF通信状态*/
bool MDCU_VVVF1ComFaultx_B1_10 = 0;   //VVVF1通信状态
bool MDCU_VVVF2ComFaultx_B1_10 = 0;   //VVVF2通信状态

bool MDCU_VVVF1ComFaultx_B1_30 = 0;   //VVVF1通信状态
bool MDCU_VVVF2ComFaultx_B1_30 = 0;   //VVVF2通信状态

bool MDCU_VVVF1ComFaultx_B1_60 = 0;   //VVVF1通信状态
bool MDCU_VVVF2ComFaultx_B1_60 = 0;   //VVVF2通信状态

/*BCU 通信状态*/
bool MDCU_BCUComFaultrx_B1_10 = 0;  	//MC1 BCU通信状态 
bool MDCU_BCUComFaultrx_B1_20 = 0;  	//T BCU通信状态 
bool MDCU_BCUComFaultrx_B1_30 = 0;  	//M BCU通信状态 
bool MDCU_BCUComFaultrx_B1_40 = 0;  	//T1 BCU通信状态 
bool MDCU_BCUComFaultrx_B1_50 = 0;  	//T2 BCU通信状态 
bool MDCU_BCUComFaultrx_B1_60 = 0;  	//MC2 BCU通信状态 

/*BCU 滑行标志位*/
bool BCU_Axle1Skidtx_SD_B1_10 = 0 ;   //MC1 BCU滑行
bool BCU_Axle2Skidtx_SD_B1_10 = 0 ;
bool BCU_Axle3Skidtx_SD_B1_10 = 0 ;
bool BCU_Axle4Skidtx_SD_B1_10 = 0 ;

bool BCU_Axle1Skidtx_SD_B1_60 = 0 ;   //MC2 BCU滑行
bool BCU_Axle2Skidtx_SD_B1_60 = 0 ;
bool BCU_Axle3Skidtx_SD_B1_60 = 0 ;
bool BCU_Axle4Skidtx_SD_B1_60 = 0 ;

/*监控终端对ATC—Sdr逻辑协议文档中 一些数据 的判断函数*/
void  MTDCPUProATCSdrLogicalData(void)
{
	u8 MC1SlideSta=0;		//局部变量
	u8 MC2SlideSta=0;
	
	switch(ChooseBoard)
	{
		case MC1_MTD_NODEID:		 //MC1车	
			
/*1.VVVF动作判断*/
			/*VVVF单元通信正常，VVVF控制电源有效，且VVVF单元 “群开放UCORM”信号有效*/		
			/*VVVF1 与 VVVF2 只要一个动作认为就是动作中*/
			
			/*1号车VVVF动作*/
			if((DI_VVVFPowertx_B1_10 && TCU1_UCORtx_SD_B1_10 && (!MDCU_VVVF1ComFaultx_B1_10))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_10 && TCU2_UCORtx_SD_B1_10 && (!MDCU_VVVF2ComFaultx_B1_10)))       /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x80;  //1车VVVF动作    
				can2_TxMC1ATCSdr[0] |=0x80;

				can3_TxATCSdr1[0] |=0x80;
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0x7F;  //1车VVVF动作 无效    
				can2_TxMC1ATCSdr[0] &=0x7F;
			
				can3_TxATCSdr1[0] &=0x7F;				
			}
			
			/*3号车VVVF动作*/
			if((DI_VVVFPowertx_B1_30 && TCU1_UCORtx_SD_B1_30 && (!MDCU_VVVF1ComFaultx_B1_30))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_30 && TCU2_UCORtx_SD_B1_30 && (!MDCU_VVVF2ComFaultx_B1_30)))       /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x40;  //3车VVVF动作    
				can2_TxMC1ATCSdr[0] |=0x40;		
				
				can3_TxATCSdr1[0] |=0x40;				     
			}		
			else
			{
				can1_TxMC1ATCSdr[0] &=0xBF;  //3车VVVF动作  无效   
				can2_TxMC1ATCSdr[0] &=0xBF;
				
				can3_TxATCSdr1[0]  &=0xBF;					
			}
			
			/*6号车VVVF动作*/
			if((DI_VVVFPowertx_B1_60 && TCU1_UCORtx_SD_B1_60 && (!MDCU_VVVF1ComFaultx_B1_60))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_60 && TCU2_UCORtx_SD_B1_60 && (!MDCU_VVVF2ComFaultx_B1_60)))       /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x20;  //6车VVVF动作    
				can2_TxMC1ATCSdr[0] |=0x20;
				
				can3_TxATCSdr1[0] |=0x20;						     
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0xDF;  //6车VVVF动作 无效   
				can2_TxMC1ATCSdr[0] &=0xDF;;
				
				can3_TxATCSdr1[0] &=0xDF;					
			}
			
/*2.车电制有效判断*/
			/*VVVF 通信正常，且控制电源有效，由VVVF 通过传输接收的CRD(电制有效)有效。*/
			if((DI_VVVFPowertx_B1_10 && TCU1_CRDtx_SD_B1_10 && (!MDCU_VVVF1ComFaultx_B1_10)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_10 && TCU2_CRDtx_SD_B1_10 && (!MDCU_VVVF2ComFaultx_B1_10)))     	 /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x10;  //1车电制有效   
				can2_TxMC1ATCSdr[0] |=0x10;

				can3_TxATCSdr1[0] |=0x10;				
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0xEF;  //1车电制有效  无效    
				can2_TxMC1ATCSdr[0] &=0xEF;
			
				can3_TxATCSdr1[0] &=0xEF;				
			}
			
			if((DI_VVVFPowertx_B1_30 && TCU1_CRDtx_SD_B1_30 && (!MDCU_VVVF1ComFaultx_B1_30)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_30 && TCU2_CRDtx_SD_B1_30 && (!MDCU_VVVF2ComFaultx_B1_30)))     	 /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x08;  //3车电制有效   
				can2_TxMC1ATCSdr[0] |=0x08;

				can3_TxATCSdr1[0] |=0x08;				
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0xF7;  //3车电制有效  无效    
				can2_TxMC1ATCSdr[0] &=0xF7;
			
				can3_TxATCSdr1[0] &=0xF7;				
			}		

			if((DI_VVVFPowertx_B1_60 && TCU1_CRDtx_SD_B1_60 && (!MDCU_VVVF1ComFaultx_B1_60)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_60 && TCU2_CRDtx_SD_B1_60 && (!MDCU_VVVF2ComFaultx_B1_60)))     	 /*VVVF2*/
			{
				can1_TxMC1ATCSdr[0] |=0x04;  //6车电制有效   
				can2_TxMC1ATCSdr[0] |=0x04;

				can3_TxATCSdr1[0] |=0x04;				
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0xFB;  //1车电制有效  无效    
				can2_TxMC1ATCSdr[0] &=0xFB;
			
				can3_TxATCSdr1[0] &=0xFB;				
			}	

/*3.BCU动作*/
			can1_TxMC1ATCSdr[1] =  (MDCU_BCUComFaultrx_B1_10<<7) |(MDCU_BCUComFaultrx_B1_20<<6) |(MDCU_BCUComFaultrx_B1_30<<5) |
														 (MDCU_BCUComFaultrx_B1_40<<4) |(MDCU_BCUComFaultrx_B1_50<<3) |(MDCU_BCUComFaultrx_B1_60<<2);
			can2_TxMC1ATCSdr[1] = can1_TxMC1ATCSdr[1];
			
			can3_TxATCSdr1[1] = can1_TxMC1ATCSdr[1];
			
/*4.滑行的判断,4的判断必须在5前面，MC1SlideSta 在5中使用*/		
		if((!MDCU_BCUComFaultrx_B1_10) &&(BCU_Axle1Skidtx_SD_B1_10 || BCU_Axle2Skidtx_SD_B1_10 || BCU_Axle3Skidtx_SD_B1_10 ||BCU_Axle4Skidtx_SD_B1_10) )
		{
			can1_TxMC1ATCSdr[2] |=0x80;  //滑行 
			can2_TxMC1ATCSdr[2] |=0x80;  //滑行
			MC1SlideSta =1;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  |=0x80;
		}
		else
		{
			can1_TxMC1ATCSdr[2] &=0x7F;  //滑行     无效
			can2_TxMC1ATCSdr[2] &=0x7F;  //滑行     无效
			MC1SlideSta =0;             //局部变量 用于滑行状态的判断 无效
			
			can3_TxATCSdr1[2]  &=0x7F;		
		}
		
		if((!MDCU_BCUComFaultrx_B1_60) &&(BCU_Axle1Skidtx_SD_B1_60 || BCU_Axle2Skidtx_SD_B1_60 || BCU_Axle3Skidtx_SD_B1_60 ||BCU_Axle4Skidtx_SD_B1_60) )
		{
			can1_TxMC1ATCSdr[2] |=0x40;  //滑行 
			can2_TxMC1ATCSdr[2] |=0x40;  //滑行
			MC2SlideSta =1;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  |=0x40;
		}
		else
		{
			can1_TxMC1ATCSdr[2] &=0xBF;  //滑行    无效
			can2_TxMC1ATCSdr[2] &=0xBF;  //滑行    无效
			MC2SlideSta =0;             //局部变量 用于滑行状态的判断   无效
			
			can3_TxATCSdr1[2]  &=0xBF;		
		}		

/*5.滑行状态的判断*/		
		if((MC1SlideSta ||((!MDCU_VVVF1ComFaultx_B1_10) && TCU1_BSLBFtx_SD_B1_10)) || (MC1SlideSta || ((!MDCU_VVVF2ComFaultx_B1_10) && TCU2_BSLBFtx_SD_B1_10)))     //1车滑行状态
		{
			can1_TxMC1ATCSdr[2] |=0x20;  //滑行状态 
			can2_TxMC1ATCSdr[2] |=0x20;  //滑行状态 
			
			can3_TxATCSdr1[2]  |=0x20;
		}
		else
		{
			can1_TxMC1ATCSdr[2] &=0xDF;  //滑行状态  无效 
			can2_TxMC1ATCSdr[2] &=0xDF;  //滑行状态  无效 
			
			can3_TxATCSdr1[2]  &=0xDF;			
		}
		
		if((MC2SlideSta ||((!MDCU_VVVF1ComFaultx_B1_60) && TCU1_BSLBFtx_SD_B1_60)) || (MC2SlideSta || ((!MDCU_VVVF2ComFaultx_B1_60) && TCU2_BSLBFtx_SD_B1_60)))     //6车滑行状态
		{
			can1_TxMC1ATCSdr[2] |=0x10;  //滑行状态 
			can2_TxMC1ATCSdr[2] |=0x10;  //滑行状态
			
			can3_TxATCSdr1[2]  |=0x10;		
		}
		else
		{
			can1_TxMC1ATCSdr[2] &=0xEF;  //滑行状态    无效
			can2_TxMC1ATCSdr[2] &=0xEF;  //滑行状态    无效

			can3_TxATCSdr1[2]  &=0xEF;				
		}

/*6. 列车编号的计算，放在can3ATCSdr2中*/
			can1_TxMC1ATCSdr[3] = can3_TxATCSdr2[2];
			can2_TxMC1ATCSdr[3] = can3_TxATCSdr2[2];
		
			can3_TxATCSdr1[3] = can3_TxATCSdr2[2];

/*7.牵引制动力*/		
		can1_TxMC1ATCSdr[4] = can1_DI_MC1[4]; //1车牵引制动力
		can2_TxMC1ATCSdr[4] = can1_DI_MC1[4];
		
		can1_TxMC1ATCSdr[5] = can1_DI_MC2[4]; //6车牵引制动力
		can2_TxMC1ATCSdr[5] = can1_DI_MC2[4];
		
		can3_TxATCSdr1[4] = can1_DI_MC1[4];
		can3_TxATCSdr1[5] = can1_DI_MC2[4];
		break;

		
		case MC2_MTD_NODEID:  	 //MC2车
/*1.VVVF动作判断*/
			/*VVVF单元通信正常，VVVF控制电源有效，且VVVF单元 “群开放UCORM”信号有效*/		
			/*VVVF1 与 VVVF2 只要一个动作认为就是动作中*/
			
			/*1号车VVVF动作*/
			if((DI_VVVFPowertx_B1_10 && TCU1_UCORtx_SD_B1_10 && (!MDCU_VVVF1ComFaultx_B1_10))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_10 && TCU2_UCORtx_SD_B1_10 && (!MDCU_VVVF2ComFaultx_B1_10)))       /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x80;  //1车VVVF动作    
				can2_TxMC2ATCSdr[0] |=0x80;

				can3_TxATCSdr1[0] |=0x80;
			}
			else
			{
				can1_TxMC2ATCSdr[0] &=0x7F;  //1车VVVF动作 无效    
				can2_TxMC2ATCSdr[0] &=0x7F;
			
				can3_TxATCSdr1[0] &=0x7F;				
			}
			
			/*3号车VVVF动作*/
			if((DI_VVVFPowertx_B1_30 && TCU1_UCORtx_SD_B1_30 && (!MDCU_VVVF1ComFaultx_B1_30))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_30 && TCU2_UCORtx_SD_B1_30 && (!MDCU_VVVF2ComFaultx_B1_30)))       /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x40;  //3车VVVF动作    
				can2_TxMC2ATCSdr[0] |=0x40;		
				
				can3_TxATCSdr1[0] |=0x40;				     
			}		
			else
			{
				can1_TxMC2ATCSdr[0] &=0xBF;  //3车VVVF动作  无效   
				can2_TxMC2ATCSdr[0] &=0xBF;
				
				can3_TxATCSdr1[0]  &=0xBF;					
			}
			
			/*6号车VVVF动作*/
			if((DI_VVVFPowertx_B1_60 && TCU1_UCORtx_SD_B1_60 && (!MDCU_VVVF1ComFaultx_B1_60))  ||    /*VVVF1*/
				 (DI_VVVFPowertx_B1_60 && TCU2_UCORtx_SD_B1_60 && (!MDCU_VVVF2ComFaultx_B1_60)))       /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x20;  //6车VVVF动作    
				can2_TxMC2ATCSdr[0] |=0x20;
				
				can3_TxATCSdr1[0] |=0x20;						     
			}
			else
			{
				can1_TxMC2ATCSdr[0] &=0xDF;  //6车VVVF动作 无效   
				can2_TxMC2ATCSdr[0] &=0xDF;;
				
				can3_TxATCSdr1[0] &=0xDF;					
			}	

/*2.车电制有效判断*/
			/*VVVF 通信正常，且控制电源有效，由VVVF 通过传输接收的CRD(电制有效)有效。*/
			if((DI_VVVFPowertx_B1_10 && TCU1_CRDtx_SD_B1_10 && (!MDCU_VVVF1ComFaultx_B1_10)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_10 && TCU2_CRDtx_SD_B1_10 && (!MDCU_VVVF2ComFaultx_B1_10)))     	 /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x10;  //1车电制有效   
				can2_TxMC2ATCSdr[0] |=0x10;

				can3_TxATCSdr1[0] |=0x10;				
			}
			else
			{
				can1_TxMC2ATCSdr[0] &=0xEF;  //1车电制有效  无效    
				can2_TxMC2ATCSdr[0] &=0xEF;
			
				can3_TxATCSdr1[0] &=0xEF;				
			}
			
			if((DI_VVVFPowertx_B1_30 && TCU1_CRDtx_SD_B1_30 && (!MDCU_VVVF1ComFaultx_B1_30)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_30 && TCU2_CRDtx_SD_B1_30 && (!MDCU_VVVF2ComFaultx_B1_30)))     	 /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x08;  //3车电制有效   
				can2_TxMC2ATCSdr[0] |=0x08;

				can3_TxATCSdr1[0] |=0x08;				
			}
			else
			{
				can1_TxMC1ATCSdr[0] &=0xF7;  //3车电制有效  无效    
				can2_TxMC1ATCSdr[0] &=0xF7;
			
				can3_TxATCSdr1[0] &=0xF7;				
			}		

			if((DI_VVVFPowertx_B1_60 && TCU1_CRDtx_SD_B1_60 && (!MDCU_VVVF1ComFaultx_B1_60)) ||   	 /*VVVF1*/
				 (DI_VVVFPowertx_B1_60 && TCU2_CRDtx_SD_B1_60 && (!MDCU_VVVF2ComFaultx_B1_60)))     	 /*VVVF2*/
			{
				can1_TxMC2ATCSdr[0] |=0x04;  //6车电制有效   
				can2_TxMC2ATCSdr[0] |=0x04;

				can3_TxATCSdr1[0] |=0x04;				
			}
			else
			{
				can1_TxMC2ATCSdr[0] &=0xFB;  //1车电制有效  无效    
				can2_TxMC2ATCSdr[0] &=0xFB;
			
				can3_TxATCSdr1[0] &=0xFB;				
			}	

/*3.BCU动作*/
			can1_TxMC2ATCSdr[1] =  (MDCU_BCUComFaultrx_B1_10<<7) |(MDCU_BCUComFaultrx_B1_20<<6) |(MDCU_BCUComFaultrx_B1_30<<5) |
														 (MDCU_BCUComFaultrx_B1_40<<4) |(MDCU_BCUComFaultrx_B1_50<<3) |(MDCU_BCUComFaultrx_B1_60<<2);
			can2_TxMC2ATCSdr[1] = can1_TxMC2ATCSdr[1];
			
			can3_TxATCSdr1[1] = can1_TxMC2ATCSdr[1];		
			
/*4.滑行的判断,4的判断必须在5前面，MC1SlideSta 在5中使用*/		
		if((!MDCU_BCUComFaultrx_B1_10) &&(BCU_Axle1Skidtx_SD_B1_10 || BCU_Axle2Skidtx_SD_B1_10 || BCU_Axle3Skidtx_SD_B1_10 ||BCU_Axle4Skidtx_SD_B1_10) )
		{
			can1_TxMC2ATCSdr[2] |=0x80;  //滑行 
			can2_TxMC2ATCSdr[2] |=0x80;  //滑行
			MC1SlideSta =1;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  |=0x80;
		}
		else
		{
			can1_TxMC2ATCSdr[2] &=0x7F;  //滑行 
			can2_TxMC2ATCSdr[2] &=0x7F;  //滑行
			MC1SlideSta =0;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  &=0x7F;		
		}
		
		if((!MDCU_BCUComFaultrx_B1_60) &&(BCU_Axle1Skidtx_SD_B1_60 || BCU_Axle2Skidtx_SD_B1_60 || BCU_Axle3Skidtx_SD_B1_60 ||BCU_Axle4Skidtx_SD_B1_60) )
		{
			can1_TxMC2ATCSdr[2] |=0x40;  //滑行 
			can2_TxMC2ATCSdr[2] |=0x40;  //滑行
			MC2SlideSta =1;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  |=0x40;
		}
		else
		{
			can1_TxMC2ATCSdr[2] &=0xBF;  //滑行 
			can2_TxMC2ATCSdr[2] &=0xBF;  //滑行
			MC2SlideSta =0;             //局部变量 用于滑行状态的判断
			
			can3_TxATCSdr1[2]  &=0xBF;		
		}	
		
/*5.滑行状态的判断*/		
		if((MC1SlideSta ||((!MDCU_VVVF1ComFaultx_B1_10) && TCU1_BSLBFtx_SD_B1_10)) || (MC1SlideSta || ((!MDCU_VVVF2ComFaultx_B1_10) && TCU2_BSLBFtx_SD_B1_10)))     //1车滑行状态
		{
			can1_TxMC2ATCSdr[2] |=0x20;  //滑行状态 
			can2_TxMC2ATCSdr[2] |=0x20;  //滑行状态 
			
			can3_TxATCSdr1[2]  |=0x20;
		}
		else
		{
			can1_TxMC2ATCSdr[2] &=0xDF;  //滑行状态  无效 
			can2_TxMC2ATCSdr[2] &=0xDF;  //滑行状态  无效 
			
			can3_TxATCSdr1[2]  &=0xDF;			
		}
		
		if((MC2SlideSta ||((!MDCU_VVVF1ComFaultx_B1_60) && TCU1_BSLBFtx_SD_B1_60)) || (MC2SlideSta || ((!MDCU_VVVF2ComFaultx_B1_60) && TCU2_BSLBFtx_SD_B1_60)))     //6车滑行状态
		{
			can1_TxMC2ATCSdr[2] |=0x10;  //滑行状态 
			can2_TxMC2ATCSdr[2] |=0x10;  //滑行状态
			
			can3_TxATCSdr1[2]  |=0x10;		
		}
		else
		{
			can1_TxMC2ATCSdr[2] &=0xEF;  //滑行状态    无效
			can2_TxMC2ATCSdr[2] &=0xEF;  //滑行状态    无效

			can3_TxATCSdr1[2]  &=0xEF;				
		}

/*6. 列车编号的计算，放在can3ATCSdr2中*/
			can1_TxMC2ATCSdr[3] = can3_TxATCSdr2[2];
			can2_TxMC2ATCSdr[3] = can3_TxATCSdr2[2];
		
			can3_TxATCSdr1[3] = can3_TxATCSdr2[2];
		
/*7.牵引制动力*/		
		can1_TxMC2ATCSdr[4] = can1_DI_MC1[4]; //1车牵引制动力
		can2_TxMC2ATCSdr[4] = can1_DI_MC1[4];
		
		can1_TxMC2ATCSdr[5] = can1_DI_MC2[4]; //6车牵引制动力
		can2_TxMC2ATCSdr[5] = can1_DI_MC2[4];
		
		can3_TxATCSdr1[4] = can1_DI_MC1[4];
		can3_TxATCSdr1[5] = can1_DI_MC2[4];		
		
		break;
		
		default:
			break;
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





