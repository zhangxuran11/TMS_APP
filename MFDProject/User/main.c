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

u8 szASC16[200]={	0};

#define BUFFER_SIZE 512

static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  void  AppTaskStart          (void     *p_arg);
static  void  AppTaskCreate         (void);
static  void  AppObjCreate          (void);

/*任务创建定义变量的步骤*/

	//MainTask  任务优先级
#define Main_TASK_PRIO		4
	//任务堆栈大小	
#define Main_STK_SIZE 		512		//512*4 = 2k的空间
	//任务控制块
static OS_TCB MainTaskTCB;
	//任务堆栈	
static CPU_STK Main_TASK_STK[Main_STK_SIZE];
	//任务函数
static void Main_task(void *p_arg);


///*温度检测任务*/
//	//temper 温度检测任务优先级
//#define TemperCheck_TASK_PRIO		10
//	//任务堆栈大小	
//#define TemperCheck_STK_SIZE 		256
//	//任务控制块
//static OS_TCB TemperCheckTaskTCB;
//	//任务堆栈	
//static CPU_STK TemperCheck_TASK_STK[TemperCheck_STK_SIZE];
//	//任务函数
//static void TemperCheck_task(void *p_arg);


/*从站启动管理任务*/
	//任务优先级
#define CAN2SLAVE_STARTUPMANAGER_TASK_PRIO		5
	//任务堆栈大小	
#define CAN2SLAVE_STARTUPMANAGER_STK_SIZE 		128
	//任务控制块
static OS_TCB CAN2SlaveStartupManagerTCB;
	//任务堆栈	
static CPU_STK CAN2SLAVE_STARTUPMANAGER_TASK_STK[CAN2SLAVE_STARTUPMANAGER_STK_SIZE];
	//任务函数
void CAN2Slave_StartupManager_Take(void *p_arg);



/*从站接受任务,堆栈信息的任务*/
	//任务优先级
#define CAN2SLAVERX_TASK_PRIO		7
	//任务堆栈大小	
#define CAN2SLAVERX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN2SlaveRxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2SLAVERX_TASK_STK[CAN2SLAVERX_STK_SIZE];
	//任务函数
extern void can2slave_recv_thread(void *p_arg);

/*从站发送任务*/
	//任务优先级
#define CAN2SLAVETX_TASK_PRIO		6
	//任务堆栈大小	
#define CAN2SLAVETX_STK_SIZE 		512
	//任务控制块
static OS_TCB CAN2SlaveTxTaskTCB;
	//任务堆栈	
static CPU_STK CAN2SLAVETX_TASK_STK[CAN2SLAVETX_STK_SIZE];
	//任务函数
extern void can2slave_send_thread(void *p_arg);


/*DI采集查询任务*/
	//任务优先级
#define	DI_CHECK_TASK_PRIO					5//12			//DI任务的优先级应该放到最高的
	//任务堆栈大小	
#define DI_CHECK_STK_SIZE 					256
	//任务控制块
static OS_TCB DICheckTCB; 
	//任务堆栈	
static CPU_STK DI_CHECK_TASK_STK[DI_CHECK_STK_SIZE];
	//任务函数
void DICheck_Take(void *p_arg);


/*车门状态查询任务*/
	//任务优先级
#define DOOR_STATE_QUERY_TASK_PRIO		8
	//任务堆栈大小	
#define DOOR_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB DoorStateQueryTCB; 
	//任务堆栈	
static CPU_STK DOOR_STATE_QUERY_TASK_STK[DOOR_STATE_QUERY_STK_SIZE];
	//任务函数
void DoorStateQuery_Take(void *p_arg);


/*空调状态查询任务*/
	//任务优先级
#define ACC_STATE_QUERY_TASK_PRIO		10
	//任务堆栈大小	
#define ACC_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB AccStateQueryTCB; 
	//任务堆栈	
static CPU_STK ACC_STATE_QUERY_TASK_STK[ACC_STATE_QUERY_STK_SIZE];
	//任务函数
void AccStateQuery_Take(void *p_arg);

/*制动状态查询任务*/
	//任务优先级
#define BCU_STATE_QUERY_TASK_PRIO		9//14
	//任务堆栈大小	
#define BCU_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB BcuStateQueryTCB; 
	//任务堆栈	
static CPU_STK BCU_STATE_QUERY_TASK_STK[BCU_STATE_QUERY_STK_SIZE];
	//任务函数
void BcuStateQuery_Take(void *p_arg);

/*空压机状态查询任务*/
	//任务优先级
#define ACP_STATE_QUERY_TASK_PRIO		16
	//任务堆栈大小	
#define ACP_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB AcpStateQueryTCB; 
	//任务堆栈	
static CPU_STK ACP_STATE_QUERY_TASK_STK[ACP_STATE_QUERY_STK_SIZE];
	//任务函数
void AcpStateQuery_Take(void *p_arg);


/*ATC（信号）状态查询任务*/
	//任务优先级
#define ATC_STATE_QUERY_TASK_PRIO		17
	//任务堆栈大小	
#define ATC_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB ATCStateQueryTCB; 
	//任务堆栈	
static CPU_STK ATC_STATE_QUERY_TASK_STK[ATC_STATE_QUERY_STK_SIZE];
	//任务函数
void ATCStateQuery_Take(void *p_arg);

/*PIS（广播）状态查询任务*/
	//任务优先级
#define PIS_STATE_QUERY_TASK_PRIO		18
	//任务堆栈大小	
#define PIS_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB PISStateQueryTCB; 
	//任务堆栈	
static CPU_STK PIS_STATE_QUERY_TASK_STK[PIS_STATE_QUERY_STK_SIZE];
	//任务函数
void PISStateQuery_Take(void *p_arg);

/*VVVF1状态查询任务*/
	//任务优先级
#define VVVF1_STATE_QUERY_TASK_PRIO		12
	//任务堆栈大小	
#define VVVF1_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB VVVF1StateQueryTCB; 
	//任务堆栈	
static CPU_STK VVVF1_STATE_QUERY_TASK_STK[VVVF1_STATE_QUERY_STK_SIZE];
	//任务函数
void VVVF1StateQuery_Take(void *p_arg);

/*VVVF2状态查询任务*/
	//任务优先级
#define VVVF2_STATE_QUERY_TASK_PRIO		13
	//任务堆栈大小	
#define VVVF2_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB VVVF2StateQueryTCB; 
	//任务堆栈	
static CPU_STK VVVF2_STATE_QUERY_TASK_STK[VVVF2_STATE_QUERY_STK_SIZE];
	//任务函数
void VVVF2StateQuery_Take(void *p_arg);


/*SIV状态查询任务*/
	//任务优先级
#define SIV_STATE_QUERY_TASK_PRIO		12
	//任务堆栈大小	
#define SIV_STATE_QUERY_STK_SIZE 		512
	//任务控制块
static OS_TCB SIVStateQueryTCB; 
	//任务堆栈	
static CPU_STK SIV_STATE_QUERY_TASK_STK[SIV_STATE_QUERY_STK_SIZE];
	//任务函数
void SIVStateQuery_Take(void *p_arg);



///*UART3 接受处理任务*/
//	//任务优先级
//#define USART3Rx_TASK_PRIO		19
//	//任务堆栈大小	
//#define USART3Rx_STK_SIZE 		512
//	//任务控制块
//OS_TCB  USART3Rx_TCB;
//	//任务堆栈	
//static CPU_STK  USART3Rx_TASK_STK[USART3Rx_STK_SIZE];
//	//任务函数
//void USART3RxTake(void *p_arg);


/*can2 接受发送LED灯闪烁任务*/
	//任务优先级
#define CAN2LED_TASK_PRIO		20
	//任务堆栈大小	
#define CAN2LED_STK_SIZE 		128
	//任务控制块
static OS_TCB  CAN2LED_TCB;
	//任务堆栈	
static CPU_STK  CAN2LED_TASK_STK[CAN2LED_STK_SIZE];
	//任务函数
void CAN2LEDTake(void *p_arg);


/*任务对象的创建*/
//static  OS_SEM     	SEM_SYNCH;	   	//用于同步
OS_SEM  Slave_Start_Sem;						//从站启动管理的信号量
OS_SEM	DI_Check_Sem;								//定义DI采集信号量

/*创建事件标志组*/
OS_FLAG_GRP	CAN2LedEventFlag;		//定义can2led闪烁事件标志组

OS_FLAG_GRP	DoorEventFlags;			//定义子系统协议中 （DOOR）门传输数据 事件标志组
OS_FLAG_GRP ACCEventFlags;			//空调事件标志组
OS_FLAG_GRP BCUEventFlags;			//BCU制动 事件标志组
OS_FLAG_GRP ACPEventFlags;			//ACP空压机 事件标志组
OS_FLAG_GRP SIVEventFlags;			//SIV辅助电源 事件标志组
OS_FLAG_GRP VVVF1EventFlags;		//VVVF1牵引1 事件标志组
OS_FLAG_GRP VVVF2EventFlags;		//VVVF2牵引2 事件标志组
OS_FLAG_GRP ATCEventFlags;			//ATC信号 事件标志组
OS_FLAG_GRP PISEventFlags;			//PIS广播 事件标志组

//定时器1		，测试使用，主要让led闪烁
//OS_TMR 	tmr1;		
//void tmr1_callback(void *p_tmr, void *p_arg); 	//定时器1回调函数


/*子系统协议中需要的定时器定义*/
OS_TMR DoorTmr;	//门定时器
void DoorTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR ACCTmr;	//空调定时器
void ACCTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR BCUTmr;	//制动定时器
void BCUTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR ACPTmr;	//空压机定时器
void ACPTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR PISTmr;	//广播定时器
void PISTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR ATCTmr;	//信号定时器
void ATCTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR SIVTmr;	//辅助电源定时器
void SIVTrmCallBack(void *p_tmr, void *p_arg);

OS_TMR VVVF1Tmr;	//VVVF1定时器
void VVVF1TrmCallBack(void *p_tmr, void *p_arg);

OS_TMR VVVF2Tmr;	//VVVF2定时器
void VVVF2TrmCallBack(void *p_tmr, void *p_arg);

OS_TMR DITmr;			//DI采集定时器
void DITrmCallBack(void *p_tmr, void *p_arg);



/*485口使用的全局变量的定义*/
u8 g_Uart2RxStartFlag=0;
u8 g_Uart2RxDataFlag=0;
u8 g_Uart2FrameErrorFlag=0;

u8 g_Uart3RxStartFlag=0;
u8 g_Uart3RxDataFlag=0;
u8 g_Uart3FrameErrorFlag=0;

u8 g_Uart4RxStartFlag=0;
u8 g_Uart4RxDataFlag=0;
u8 g_Uart4FrameErrorFlag=0;
u8 g_BCURxSdOrTdFlag=0;
u8 g_Uart4NackFlag=0;

//这个全局变量用来接受旋钮的值，从而选择板子
u8 ChooseBoard=0;		

extern  uint8_t UART1_Rx_Cnt;

//	extern u8 HSE_AND_HSI;

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
	
		Uart1_init(); /* uart1 初始化串口,提前的初始化，防止中间有打印显示信息会无法输出 */
		
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
  AppObjCreate();                                             /* Create Applicaiton kernel objects                    */
	
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
                 (OS_TASK_PTR)Main_task, 				//任务函数
                 (void		  *)0,								//传递给任务函数的参数
                 (OS_PRIO	   )Main_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&Main_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)Main_TASK_STK[Main_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)Main_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,								//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,								//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,								//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/,		//任务选项
                 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值

	//创建从站启动管理任务
	OSTaskCreate((OS_TCB 		* )&CAN2SlaveStartupManagerTCB,				//任务控制块
								(CPU_CHAR	* )"CAN2SlaveStartManager_task", 			//任务名字
								 (OS_TASK_PTR)CAN2Slave_StartupManager_Take, 		//任务函数
								 (void		  *)0,														//传递给任务函数的参数
								 (OS_PRIO	   )CAN2SLAVE_STARTUPMANAGER_TASK_PRIO,    	//任务优先级
								 (CPU_STK   *)&CAN2SLAVE_STARTUPMANAGER_TASK_STK[0],	//任务堆栈基地址
								 (CPU_STK_SIZE)CAN2SLAVE_STARTUPMANAGER_TASK_STK[CAN2SLAVE_STARTUPMANAGER_STK_SIZE/10],	//任务堆栈深度限位
								 (CPU_STK_SIZE)CAN2SLAVE_STARTUPMANAGER_STK_SIZE,			//任务堆栈大小
								 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
								 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
								 (void   	   *)0,					//用户补充的存储区
								 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
								 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		 
								 
		//创建从站接受任务
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


		//创建从站发送任务
	OSTaskCreate((OS_TCB 		* )&CAN2SlaveTxTaskTCB,			//任务控制块
								(CPU_CHAR	* )"CAN2SlaveTx_task", 			//任务名字
								 (OS_TASK_PTR)can2slave_send_thread, 	//任务函数
								 (void		  *)0,										//传递给任务函数的参数
								 (OS_PRIO	   )CAN2SLAVETX_TASK_PRIO,     	//任务优先级
								 (CPU_STK   *)&CAN2SLAVETX_TASK_STK[0],	//任务堆栈基地址
								 (CPU_STK_SIZE)CAN2SLAVETX_TASK_STK[CAN2SLAVETX_STK_SIZE/10],		//任务堆栈深度限位
								 (CPU_STK_SIZE)CAN2SLAVETX_STK_SIZE,		//任务堆栈大小
								 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
								 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
								 (void   	   *)0,					//用户补充的存储区
								 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
								 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值										 

		//创建从站发送任务
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
								 
	//	/*UART3 接受处理任务*/
	//	OSTaskCreate((OS_TCB 		* )&USART3Rx_TCB,					//任务控制块
	//								(CPU_CHAR	* )"Usart3RxTask", 				//任务名字
	//                 (OS_TASK_PTR)USART3RxTake, 				//任务函数
	//                 (void		  *)0,										//传递给任务函数的参数
	//                 (OS_PRIO	   )USART3Rx_TASK_PRIO,   //任务优先级
	//                 (CPU_STK   *)&USART3Rx_TASK_STK[0],//任务堆栈基地址
	//                 (CPU_STK_SIZE)USART3Rx_TASK_STK[USART3Rx_STK_SIZE/10],		//任务堆栈深度限位
	//                 (CPU_STK_SIZE)USART3Rx_STK_SIZE,		//任务堆栈大小
	//                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
	//                 (OS_TICK	    )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
	//                 (void   	   *)0,										//用户补充的存储区
	//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
	//                 (OS_ERR 	   *)&err);								//存放该函数错误时的返回值		
	
								 
	/*WHH*/							 
									
	/*DI采集查询任务*/
	OSTaskCreate((OS_TCB 		* )&DICheckTCB,						//任务控制块
								(CPU_CHAR	* )"DICheckTask", 				//任务名字
                 (OS_TASK_PTR)DICheck_Take, 				//任务函数
                 (void		  *)0,										//传递给任务函数的参数
                 (OS_PRIO	   )DI_CHECK_TASK_PRIO,   //任务优先级
                 (CPU_STK   *)&DI_CHECK_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)DI_CHECK_TASK_STK[DI_CHECK_STK_SIZE/10],		//任务堆栈深度限位
                 (CPU_STK_SIZE)DI_CHECK_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	    )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	   *)0,										//用户补充的存储区
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR/* | OS_OPT_TASK_SAVE_FP*/),		//任务选项
                 (OS_ERR 	   *)&err);								//存放该函数错误时的返回值											 

	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case T_MFD1_NODEID:	
		case M_MFD1_NODEID:
		case T1_MFD1_NODEID:
		case T2_MFD1_NODEID:
		case MC2_MFD1_NODEID:		
								 
		/*车门状态查询任务*/
		OSTaskCreate((OS_TCB 		* )&DoorStateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"DoorStateQuery_task", 				//任务名字
									 (OS_TASK_PTR)DoorStateQuery_Take, 					//任务函数
									 (void		  *)0,														//传递给任务函数的参数
									 (OS_PRIO	   )DOOR_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&DOOR_STATE_QUERY_TASK_STK[0],	//任务堆栈基地址
									 (CPU_STK_SIZE)DOOR_STATE_QUERY_TASK_STK[DOOR_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)DOOR_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
							 							 
		/*空调状态查询任务*/						 
			OSTaskCreate((OS_TCB 		* )&AccStateQueryTCB,						//任务控制块
								(CPU_CHAR	* )"AccStateQueryTask", 						//任务名字
								 (OS_TASK_PTR)AccStateQuery_Take, 						//任务函数
								 (void		  *)0,															//传递给任务函数的参数
								 (OS_PRIO	   )ACC_STATE_QUERY_TASK_PRIO,    	//任务优先级
								 (CPU_STK   *)&ACC_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
								 (CPU_STK_SIZE)ACC_STATE_QUERY_TASK_STK[ACC_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
								 (CPU_STK_SIZE)ACC_STATE_QUERY_STK_SIZE,			//任务堆栈大小
								 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
								 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
								 (void   	   *)0,					//用户补充的存储区
								 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
								 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值								 									 
							 
		/*制动状态查询任务*/				 
			OSTaskCreate((OS_TCB 		* )&BcuStateQueryTCB,						//任务控制块
								(CPU_CHAR	* )"BcuStateQueryTask", 						//任务名字
								 (OS_TASK_PTR)BcuStateQuery_Take, 						//任务函数
								 (void		  *)0,															//传递给任务函数的参数
								 (OS_PRIO	   )BCU_STATE_QUERY_TASK_PRIO,    	//任务优先级
								 (CPU_STK   *)&BCU_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
								 (CPU_STK_SIZE)BCU_STATE_QUERY_TASK_STK[BCU_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
								 (CPU_STK_SIZE)BCU_STATE_QUERY_STK_SIZE,			//任务堆栈大小
								 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
								 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
								 (void   	   *)0,					//用户补充的存储区
								 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
								 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值			
	
			break;
		default:
			printf("MFD Board Choose Err！\r\n");
			break;
	}
	
	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case MC2_MFD1_NODEID:	
		case M_MFD1_NODEID:	
				/*VVVF1,VVVF2*/
				/*VVVF1状态查询任务*/						 
				OSTaskCreate((OS_TCB 		* )&VVVF1StateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"VVVF1StateQueryTask", 						//任务名字
									 (OS_TASK_PTR)VVVF1StateQuery_Take, 						//任务函数
									 (void		  *)0,																//传递给任务函数的参数
									 (OS_PRIO	   )VVVF1_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&VVVF1_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
									 (CPU_STK_SIZE)VVVF1_STATE_QUERY_TASK_STK[VVVF1_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)VVVF1_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值	
				if(err == OS_ERR_NONE)
				{
					printf("VVVF1Task创建成功!\r\n");
				}
				else
				{
					printf("VVVF1Task创建失败!\r\n");
				}			
				
				/*VVVF1状态查询任务*/						 
				OSTaskCreate((OS_TCB 		* )&VVVF2StateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"VVVF2StateQueryTask", 						//任务名字
									 (OS_TASK_PTR)VVVF2StateQuery_Take, 						//任务函数
									 (void		  *)0,																//传递给任务函数的参数
									 (OS_PRIO	   )VVVF2_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&VVVF2_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
									 (CPU_STK_SIZE)VVVF2_STATE_QUERY_TASK_STK[VVVF2_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)VVVF2_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值	
				if(err == OS_ERR_NONE)
				{
					printf("VVVF2Task创建成功!\r\n");
				}
				else
				{
					printf("VVVF2Task创建失败!\r\n");
				}						
		
			break;
		
		case T_MFD1_NODEID:	
		case T1_MFD1_NODEID:	
			/*SIV*/
			/*SIV状态查询任务*/					 
			OSTaskCreate((OS_TCB 		* )&SIVStateQueryTCB,						//任务控制块
								(CPU_CHAR	* )"SIVStateQueryTask", 						//任务名字
								 (OS_TASK_PTR)SIVStateQuery_Take, 						//任务函数
								 (void		  *)0,															//传递给任务函数的参数
								 (OS_PRIO	   )SIV_STATE_QUERY_TASK_PRIO,    	//任务优先级
								 (CPU_STK   *)&SIV_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
								 (CPU_STK_SIZE)SIV_STATE_QUERY_TASK_STK[SIV_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
								 (CPU_STK_SIZE)SIV_STATE_QUERY_STK_SIZE,			//任务堆栈大小
								 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
								 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
								 (void   	   *)0,					//用户补充的存储区
								 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
								 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值	
			if(err == OS_ERR_NONE)
			{
				printf("SIVTask创建成功!\r\n");
			}
			else
			{
				printf("SIVTask创建失败!\r\n");
			}				
			break;
		
		case MC1_MFD2_NODEID:
		case MC2_MFD2_NODEID:	
			/*ATC ,PIS*/
			/*ATC（信号）状态查询任务*/
		OSTaskCreate((OS_TCB 		* )&ATCStateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"ATCStateQuery_task", 				//任务名字
									 (OS_TASK_PTR)ATCStateQuery_Take, 					//任务函数
									 (void		  *)0,														//传递给任务函数的参数
									 (OS_PRIO	   )ATC_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&ATC_STATE_QUERY_TASK_STK[0],	//任务堆栈基地址
									 (CPU_STK_SIZE)ATC_STATE_QUERY_TASK_STK[ATC_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)ATC_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
									 
		/*PIS（广播）状态查询任务*/							 
		OSTaskCreate((OS_TCB 		* )&PISStateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"PISStateQuery_task", 				//任务名字
									 (OS_TASK_PTR)PISStateQuery_Take, 					//任务函数
									 (void		  *)0,														//传递给任务函数的参数
									 (OS_PRIO	   )PIS_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&PIS_STATE_QUERY_TASK_STK[0],	//任务堆栈基地址
									 (CPU_STK_SIZE)PIS_STATE_QUERY_TASK_STK[PIS_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)PIS_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值						
			break;
		
		case MC1_MFD3_NODEID:
		case MC2_MFD3_NODEID:
			/*ACP*/
			/*空压机状态查询任务*/						 
				OSTaskCreate((OS_TCB 		* )&AcpStateQueryTCB,						//任务控制块
									(CPU_CHAR	* )"AcpStateQueryTask", 						//任务名字
									 (OS_TASK_PTR)AcpStateQuery_Take, 						//任务函数
									 (void		  *)0,															//传递给任务函数的参数
									 (OS_PRIO	   )ACP_STATE_QUERY_TASK_PRIO,    	//任务优先级
									 (CPU_STK   *)&ACP_STATE_QUERY_TASK_STK[0],		//任务堆栈基地址
									 (CPU_STK_SIZE)ACP_STATE_QUERY_TASK_STK[ACP_STATE_QUERY_STK_SIZE/10],	//任务堆栈深度限位
									 (CPU_STK_SIZE)ACP_STATE_QUERY_STK_SIZE,			//任务堆栈大小
									 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
									 (OS_TICK	    )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
									 (void   	   *)0,					//用户补充的存储区
									 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR /*| OS_OPT_TASK_SAVE_FP*/),		//任务选项
									 (OS_ERR 	   *)&err);				//存放该函数错误时的返回值		
			break;
									 
		default :
			break;
	}
	
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
	//BSP_OS_SemCreate(&SEM_SYNCH, 0,(CPU_CHAR *)"SEM_SYNCH");


	//创建一个can2接受信号量
	OSSemCreate ((OS_SEM*	)&can2slaverecv_sem,
                 (CPU_CHAR*	)"can2slaverecvsem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);	
	if(err == OS_ERR_NONE)
	{
		printf("Creat can2 Rx Sem Success!\r\n");
	}
	else
	{
		printf("Creat can2 Rx Sem False!\r\n");
	}

	//创建一个can2发送信号量
	OSSemCreate ((OS_SEM*	)&can2slavetran_sem,
                 (CPU_CHAR*	)"can2slavetransem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);	
	if(err == OS_ERR_NONE)
	{
		printf("Creat can2 Tx Sem Success!\r\n");
	}
	else
	{
		printf("Creat can2 Tx Sem False!\r\n");
	}

	/*创建DI采集信号量	*/
	OSSemCreate ((OS_SEM*	)&DI_Check_Sem,
                 (CPU_CHAR*	)"DiCheckSem",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);	
	if(err == OS_ERR_NONE)
	{
		printf("Creat DI Sem Success!\r\n");
	}
	else
	{
		printf("Creat DI Sem False!\r\n");
	}
	
	/*创建事件标志组*/
	OSFlagCreate((OS_FLAG_GRP*)&CAN2LedEventFlag,				//指向事件标志组
                 (CPU_CHAR*	  )"CAN2 Led Event Flag",	//名字
                 (OS_FLAGS	  )0,											//事件标志组初始值
                 (OS_ERR*  	  )&err);									//错误码	
	if(err == OS_ERR_NONE)
	{
		printf("Creat can2Led EvenFlag Success!\r\n");
	}
	else
	{
		printf("Creat can2Led EvenFlag False!\r\n");
	}	

	//	//创建定时器1
	//	OSTmrCreate((OS_TMR		*)&tmr1,		//定时器1
	//                (CPU_CHAR	*)"tmr1",	//定时器名字
	//                (OS_TICK	 )0,			//初始化定时器的延时值
	//                (OS_TICK	 )100,    //100*1=100ms  重复周期
	//                (OS_OPT		 )OS_OPT_TMR_PERIODIC,	 //周期模式
	//                (OS_TMR_CALLBACK_PTR)tmr1_callback,//定时器1回调函数
	//                (void	    *)0,			//回电函数参数为0
	//                (OS_ERR	  *)&err);	//返回的错误码			
	//	if(err == OS_ERR_NONE)
	//	{
	//		printf("创建Trm1成功!\r\n");
	//	}
	//	else
	//	{
	//		printf("创建Trm1失败!\r\n");
	//	}			
								
	/*6个车厢的MFD1 多功能板卡都有 门，空调 ，制动,硬线采集*/							
	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case T_MFD1_NODEID:	
		case M_MFD1_NODEID:
		case T1_MFD1_NODEID:
		case T2_MFD1_NODEID:
		case MC2_MFD1_NODEID:
			
			//创建定时器 门(Door)
			OSTmrCreate((OS_TMR		*)&DoorTmr,		//定时器1
									(CPU_CHAR	*)"DoorTmr",	//定时器名字
									(OS_TICK	 )10,					//初始化定时器的延时值
									(OS_TICK	 )25,   		  //25*1=25ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)DoorTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码			
			if(err == OS_ERR_NONE)
			{
				printf("创建DoorTrm成功!\r\n");
			}
			else
			{
				printf("创建DoorTrm失败!\r\n");
			}	
									
			//创建定时器 空调(ACC)
			OSTmrCreate((OS_TMR		*)&ACCTmr,		//定时器1
									(CPU_CHAR	*)"ACCTmr",		//定时器名字
									(OS_TICK	 )20,					//初始化定时器的延时值
									(OS_TICK	 )50,   		  //50*1=50ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)ACCTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码	
			if(err == OS_ERR_NONE)
			{
				printf("创建ACCTrm成功!\r\n");
			}
			else
			{
				printf("创建ACCTrm失败!\r\n");
			}										
			//创建定时器 制动(BCU)
			OSTmrCreate((OS_TMR		*)&BCUTmr,		//定时器1
									(CPU_CHAR	*)"BCUTmr",		//定时器名字
									(OS_TICK	 )30,					//初始化定时器的延时值
									(OS_TICK	 )200,   		  //200*1=200ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)BCUTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码		
			if(err == OS_ERR_NONE)
			{
				printf("创建BCUTrm成功!\r\n");
			}
			else
			{
				printf("创建BCUTrm失败!\r\n");
			}	
														
			//创建定时器 硬线(DI)
			OSTmrCreate((OS_TMR		*)&DITmr,			//定时器1
									(CPU_CHAR	*)"DITmr",		//定时器名字
									(OS_TICK	 )0,					//初始化定时器的延时值
									(OS_TICK	 )20,   		  //20*1=20ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 	//周期模式
									(OS_TMR_CALLBACK_PTR)DITrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码		
			if(err == OS_ERR_NONE)
			{
				printf("创建DITrm成功!\r\n");
			}
			else
			{
				printf("创建DITrm失败!\r\n");
			}	
														
			/*创建子系统协议中的事件标志组*/
			/*Door*/
			OSFlagCreate((OS_FLAG_GRP*)&DoorEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"Door Event Flags",			//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码	
			if(err == OS_ERR_NONE)
			{
				printf("创建DoorEvenFlag成功!\r\n");
			}
			else
			{
				printf("创建DoorEvenFlag失败!\r\n");
			}	
			
			/*ACC*/							
			OSFlagCreate((OS_FLAG_GRP*)&ACCEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"ACC Event Flags",				//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码							 
				if(err == OS_ERR_NONE)
			{
				printf("创建ACCEvenFlag成功!\r\n");
			}
			else
			{
				printf("创建ACCEvenFlag失败!\r\n");
			}	
			
			/*BCU*/
			OSFlagCreate((OS_FLAG_GRP*)&BCUEventFlags,					//指向事件标志组
								 (CPU_CHAR*	  )"BCU Event Flags",				//名字
								 (OS_FLAGS	  )0,												//事件标志组初始值
								 (OS_ERR*  	  )&err);										//错误码							 
			if(err == OS_ERR_NONE)
			{
				printf("创建BCUEvenFlag成功!\r\n");
			}
			else
			{
				printf("创建BCUEvenFlag失败!\r\n");
			}	
			
			break;
		default :
			break;
	}								
								
	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case MC2_MFD1_NODEID:	
		case M_MFD1_NODEID:	
			//创建定时器 牵引 （VVVF1）/*100ms 发一次sdr，100ms发一次tdr，总的额时间周期为200ms，在回调函数中静态变量计数*/
			OSTmrCreate((OS_TMR		*)&VVVF1Tmr,		//定时器1
									(CPU_CHAR	*)"VVVF1Tmr",		//定时器名字
									(OS_TICK	 )2000,					//初始化定时器的延时值
									(OS_TICK	 )100,   		  //100*1=100ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)VVVF1TrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码	
			if(err == OS_ERR_NONE)
			{
				printf("创建VVVF1Trm成功!\r\n");
			}
			else
			{
				printf("创建VVVF1Trm失败!\r\n");
			}	
			
			//创建定时器 牵引 （VVVF2）
			OSTmrCreate((OS_TMR		*)&VVVF2Tmr,		//定时器1
									(CPU_CHAR	*)"VVVF2Tmr",		//定时器名字
									(OS_TICK	 )2020,					//初始化定时器的延时值
									(OS_TICK	 )100,   		  //100*1=100ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)VVVF2TrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码			
			if(err == OS_ERR_NONE)
			{
				printf("创建VVVF2Trm成功!\r\n");
			}
			else
			{
				printf("创建VVVF2Trm失败!\r\n");
			}	
			/*VVVF1*/							
			OSFlagCreate((OS_FLAG_GRP*)&VVVF1EventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"VVVF1 Event Flags",				//名字
									 (OS_FLAGS	  )0,													//事件标志组初始值
									 (OS_ERR*  	  )&err);											//错误码		
			if(err == OS_ERR_NONE)
			{
				printf("创建VVVF1EvenFlag成功!\r\n");
			}
			else
			{
				printf("创建VVVF1EvenFlag失败!\r\n");
			}	
			
			/*VVVF2*/							
			OSFlagCreate((OS_FLAG_GRP*)&VVVF2EventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"VVVF2 Event Flags",				//名字
									 (OS_FLAGS	  )0,													//事件标志组初始值
									 (OS_ERR*  	  )&err);											//错误码			
			if(err == OS_ERR_NONE)
			{
				printf("创建VVVF2EvenFlag成功!\r\n");
			}
			else
			{
				printf("创建VVVF2EvenFlag失败!\r\n");
			}	
			break;
									
		case T_MFD1_NODEID:	
		case T1_MFD1_NODEID:	
			//创建定时器 辅助电源(SIV)
			OSTmrCreate((OS_TMR		*)&SIVTmr,		//定时器1
									(CPU_CHAR	*)"SIVTmr",		//定时器名字
									(OS_TICK	 )5000,				//初始化定时器的延时值
									(OS_TICK	 )200,   		  //200*1=200ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)SIVTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码		
			if(err == OS_ERR_NONE)
			{
				printf("创建SIVTrm成功!\r\n");
			}
			else
			{
				printf("创建SIVTrm失败!\r\n");
			}		
			
			/*SIV*/							
			OSFlagCreate((OS_FLAG_GRP*)&SIVEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"SIV Event Flags",				//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码		
			if(err == OS_ERR_NONE)
			{
				printf("创建SIVEvenFlag成功!\r\n");
			}
			else
			{
				printf("创建SIVEvenFlag失败!\r\n");
			}			
			
			break;
												
		case MC1_MFD2_NODEID:
		case MC2_MFD2_NODEID:	
			//创建定时器 信号(ATC)
			OSTmrCreate((OS_TMR		*)&ATCTmr,		//定时器1
									(CPU_CHAR	*)"ATCTmr",		//定时器名字
									(OS_TICK	 )20,					//初始化定时器的延时值
									(OS_TICK	 )200,   		  //200*1=200ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)ATCTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码	
									
			//创建定时器 广播(PIS)
			OSTmrCreate((OS_TMR		*)&PISTmr,		//定时器1
									(CPU_CHAR	*)"PISTmr",		//定时器名字
									(OS_TICK	 )40,					//初始化定时器的延时值
									(OS_TICK	 )200,   		  //200*1=200ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)PISTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码		
									
			//创建定时器 硬线(DI)
			OSTmrCreate((OS_TMR		*)&DITmr,		//定时器1
									(CPU_CHAR	*)"DITmr",		//定时器名字
									(OS_TICK	 )0,					//初始化定时器的延时值
									(OS_TICK	 )20,   		  //20*1=20ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)DITrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码					

			/*ATC*/							
			OSFlagCreate((OS_FLAG_GRP*)&ATCEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"ATC Event Flags",				//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码			
			/*PIS*/							
			OSFlagCreate((OS_FLAG_GRP*)&PISEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"PIS Event Flags",				//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码																						
			break;
									
		case MC1_MFD3_NODEID:
		case MC2_MFD3_NODEID:
			//创建定时器 空压机(ACP)
			OSTmrCreate((OS_TMR		*)&ACPTmr,		//定时器1
									(CPU_CHAR	*)"ACPTmr",		//定时器名字
									(OS_TICK	 )0,					//初始化定时器的延时值
									(OS_TICK	 )1000,   		//1000*1=1000ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)ACPTrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码		
									
			//创建定时器 硬线(DI)
			OSTmrCreate((OS_TMR		*)&DITmr,		//定时器1
									(CPU_CHAR	*)"DITmr",		//定时器名字
									(OS_TICK	 )0,					//初始化定时器的延时值
									(OS_TICK	 )20,   		  //20*1=20ms  重复周期
									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
									(OS_TMR_CALLBACK_PTR)DITrmCallBack,	//定时器1回调函数
									(void	    *)0,					//回调函数参数为0
									(OS_ERR	  *)&err);			//返回的错误码				

			/*ACP事件标志组*/							
			OSFlagCreate((OS_FLAG_GRP*)&ACPEventFlags,					//指向事件标志组
									 (CPU_CHAR*	  )"ACP Event Flags",				//名字
									 (OS_FLAGS	  )0,												//事件标志组初始值
									 (OS_ERR*  	  )&err);										//错误码							 				
			break;
									
		//		case T_MFD2_NODEID:
		//		case M_MFD2_NODEID:
		//		case T1_MFD2_NODEID:
		//		case T2_MFD2_NODEID:
		//		case MC1_MFD4_NODEID:
		//		case MC2_MFD4_NODEID:	
		//						//创建定时器 硬线(DI)
		//			OSTmrCreate((OS_TMR		*)&DITmr,			//定时器1
		//									(CPU_CHAR	*)"DITmr",		//定时器名字
		//									(OS_TICK	 )0,					//初始化定时器的延时值
		//									(OS_TICK	 )20,   		  //20*1=20ms  重复周期
		//									(OS_OPT		 )OS_OPT_TMR_PERIODIC,	 		//周期模式
		//									(OS_TMR_CALLBACK_PTR)DITrmCallBack,	//定时器1回调函数
		//									(void	    *)0,					//回调函数参数为0
		//									(OS_ERR	  *)&err);			//返回的错误码					
		//			break;

	}								
							
	OS_CRITICAL_EXIT();	//退出临界区									
}


	//extern u8 User_CAN1Transmit(uint32_t id,uint8_t* val,uint8_t Len);
	//extern u8 User_CAN2Transmit(uint32_t id,uint8_t* val,uint8_t Len);
	//extern unsigned char canInit(CAN_TypeDef* CANx,unsigned int bitrate);

/*EEPROM 测试*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define sEE_WRITE_ADDRESS1        0x50
#define sEE_READ_ADDRESS1         0x50
#define BUFFER_SIZE1            (countof(aTxBuffer1)-1)
#define BUFFER_SIZE2            (countof(aTxBuffer2)-1)
#define sEE_WRITE_ADDRESS2       (sEE_WRITE_ADDRESS1 + BUFFER_SIZE1)
#define sEE_READ_ADDRESS2        (sEE_READ_ADDRESS1 + BUFFER_SIZE1)
/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t aTxBuffer1[] = "/* STM32F40xx I2C Firmware Library EEPROM driver example: \
															";
uint8_t aTxBuffer2[] = "/* STM32F40xx I2C Firmware  */";
uint8_t aRxBuffer1[BUFFER_SIZE1];
uint8_t aRxBuffer2[BUFFER_SIZE2];

volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
volatile uint16_t uhNumDataRead = 0;

/* Private functions ---------------------------------------------------------*/
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);


//extern void can_recv_thread(void);
//extern	void can_slave_init(void);


void Main_task(void *p_arg)
{
	OS_ERR err;

	/*iap UART1*/
	u16 UartOldCount=0;	//老的串口接收数据值
	//u16 AppLenth=0;	//接收到的app代码长度
	
	//	u8 CLTxBuf[10]={9,8,7,6,5,4,3,2,1,0};
	
	/*HDLC*/
		u8 ATxBuf[200]={0,1,2,3,4,5,6,7,8,9,};
		u8	BTxBuf[200]={9,8,7,6,5,4,3,2,1,0};
	//		u8 Rxlen;
		u8 i;
		u8 aa=0;
					
		for(i=0;i<128;i++)
		{
			szASC16[i]=i;
		}
		memcpy(ATxBuf,szASC16,sizeof(ATxBuf));
		memcpy(BTxBuf,szASC16,sizeof(BTxBuf));
		
	(void)p_arg;
	printf("进入到Main任务\r\n");
	


	/*110 24*/
	//Vol_110VGpio_Init();
	//Vol_24VGpio_Init();

	RunLedGpio_Init();			//初始化运行灯
	
	SAF82525_Init(SAF_BAUD_RATE_125000);//HDLC初始化

	//	SAF82525_INT_GPIO_Init();
	//	SAF82525_Rest_CPIO_Init();
	//	SAF82525_GPIO_Init();
	//	
	//	//step1: SAF82525芯片复位
	//	SAF82525_REST_ON();		//开启复位
	//	delay_ms(10);
	//	SAF82525_REST_OFF();  //关闭复位

		/*时钟频率测试*/
	//	System_Clk_Output_Init();
	//	HSE_Clk_Output_Init();
		
		/*CL*/
	//	MTDMFD_CL_Send_Data(&CLTxBuf[0],10);
	
	
	
	

			//		/* Initialize the I2C EEPROM driver ----------------------------------------*/
			//		sEE_Init();  

			//	/*EPPROM*/
			//	i=0;
			////	for(i=0;i<64;i++)
			//	{
			//		printf("w1=%d\r\n",sEE_WRITE_ADDRESS1+i*BUFFER_SIZE1);	
			//		printf("w2=%d\r\n",sEE_WRITE_ADDRESS2+(i*BUFFER_SIZE1+1));
			//		
			//		/*EEPROM 测试*/
			//		//printf("1_EEPROM\r\n");
			//		/* First write in the memory followed by a read of the written data --------*/
			//		/* Write on I2C EEPROM from sEE_WRITE_ADDRESS1 */
			//		sEE_WriteBuffer(aTxBuffer1, sEE_WRITE_ADDRESS1/*+i*BUFFER_SIZE1*/, BUFFER_SIZE1); 
			//		//printf("2_EEPROM\r\n");
			//		/* Wait for EEPROM standby state */
			//		sEE_WaitEepromStandbyState();  
			//		//printf("3_EEPROM\r\n");
			//		/* Set the Number of data to be read */
			//		uhNumDataRead = BUFFER_SIZE1;
			//		//printf("4_EEPROM\r\n");
			//		/* Read from I2C EEPROM from sEE_READ_ADDRESS1 */
			//		sEE_ReadBuffer(aRxBuffer1, sEE_READ_ADDRESS1/*+i*BUFFER_SIZE1*/, (uint16_t *)(&uhNumDataRead)); 

			////printf("5_EEPROM\r\n");
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
			//			//printf("a1=%s\r\n",aTxBuffer1);
			//			//printf("a2=%s\r\n",aRxBuffer1);
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
			//		uhNumDataRead =10;
			//	 	sEE_ReadBuffer(aRxBuffer1, sEE_READ_ADDRESS1+10,(uint16_t *)(&uhNumDataRead)); 

			//			printf("test=%s\r\n",aRxBuffer1);
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
			////			printf("a1=%s\r\n",aTxBuffer2);
			////			printf("a2=%s\r\n",aRxBuffer2);			
			//		 /* Check if the data written to the memory is read correctly */
			//		TransferStatus1 = Buffercmp(aTxBuffer2, aRxBuffer2, BUFFER_SIZE2);

			//		if (TransferStatus1 == PASSED)
			//		{
			//			printf(" Transfer 2 PASSED \r\n ");
			//		}
			//		else
			//		{
			//			printf(" Transfer 2 FAILED  \r\n");
			//		} 
			//		
			//		uhNumDataRead = 10;
			//	 	sEE_ReadBuffer(aRxBuffer2, sEE_READ_ADDRESS2+10,(uint16_t *)(&uhNumDataRead)); 

			//			printf("test=%s\r\n",aRxBuffer1);

			//	}		



	
	//OSTmrStart(&tmr1,&err);	//开启定时器1
	//			printf("开启定时器1\r\n");
				
	/*6个车厢的MFD1 多功能板卡都有 门，空调 ，制动,硬线采集*/							
	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case T_MFD1_NODEID:	
		case M_MFD1_NODEID:
		case T1_MFD1_NODEID:
		case T2_MFD1_NODEID:
		case MC2_MFD1_NODEID:			
			OSTmrStart(&DoorTmr,&err);	//开启定时器Door
						printf("开启定时器Door\r\n");			
			OSTmrStart(&ACCTmr,&err);	//开启定时器ACC
						printf("开启定时器ACC\r\n");					
			OSTmrStart(&BCUTmr,&err);	//开启定时器BCU
						printf("开启定时器BCU\r\n");					
			OSTmrStart(&DITmr,&err);	//开启定时器DI
						printf("开启定时器DI\r\n");
			break;
	}	
		
	switch(ChooseBoard)
	{
		case MC1_MFD1_NODEID:
		case MC2_MFD1_NODEID:	
		case M_MFD1_NODEID:	
			OSTmrStart(&VVVF1Tmr,&err);	//开启定时器VVVF1
						printf("开启定时器VVVF1\r\n");					
			OSTmrStart(&VVVF2Tmr,&err);	//开启定时器VVVF2
						printf("开启定时器VVVF2\r\n");
			break;
		
		case T_MFD1_NODEID:	
		case T1_MFD1_NODEID:	
			OSTmrStart(&SIVTmr,&err);	//开启定时器SIV
						printf("开启定时器SIV\r\n");			
			break;
		
		case MC1_MFD2_NODEID:
		case MC2_MFD2_NODEID:
			OSTmrStart(&ATCTmr,&err);	//开启定时器ATC
						printf("开启定时器ATC\r\n");					
			OSTmrStart(&PISTmr,&err);	//开启定时器PIS
						printf("开启定时器PIS\r\n");					
			OSTmrStart(&DITmr,&err);	//开启定时器DI
						printf("开启定时器DI\r\n");
			break;
		
		case MC1_MFD3_NODEID:
		case MC2_MFD3_NODEID:
			OSTmrStart(&ACPTmr,&err);	//开启定时器ACP
						printf("开启定时器ACP\r\n");					
			OSTmrStart(&DITmr,&err);	//开启定时器DI
						printf("开启定时器DI\r\n");	
			break;
		
		//		case T_MTD2_MFD2_NODEID:
		//		case M_MTD3_MFD2_NODEID:
		//		case T1_MTD4_MFD2_NODEID:
		//		case T2_MTD5_MFD2_NODEID:
		//		case MC1_MTD1_MFD4_NODEID:
		//		case MC2_MTD6_MFD4_NODEID:	
		//					OSTmrStart(&DITmr,&err);	//开启定时器DI
		//						printf("开启定时器DI\r\n");
	}
		
				
				
	while(1)
	{
		//i++;
		RunLed_Turn();

//		/*旋钮检测*/
//		 aa=	MTDMFD_CanAddVal();
//			printf("MFD旋钮值0x%x\r\n",aa);

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
		BSP_OS_TimeDlyMs(500);
	}	
	
}



//		void key_task(void *p_arg)		 	//用来显示任务堆栈使用情况，已经cpu使用
//		{
//		//	OS_ERR err;
//		//	uint8_t ucKeyCode;
//		//	int64_t runtime; 
//		//	ucKeyCode=0;
//			(void)p_arg;	               /* 避免编译器报警 */
//			printf("进入到key任务\r\n");
//			while(1)
//			{		
//		//		if(KEY_S1_UPDOWM() == Bit_RESET)
//		//		{
//		//			printf("进入到按键任务\r\n");
//		//			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err);//延时1s	//包含有延时去抖
//		//			ucKeyCode	= KEY_Scan(0);	//再次确认按键状态
//		//			if(KEY_S1_UPDOWM() == Bit_RESET)	
//		//			{
//		//				ucKeyCode=1;
//		//			}
//		//		}
//		//		
//		//		if(ucKeyCode==1)
//		//		{
//		//			ucKeyCode=0;
//		//			DispTaskInfo();		//打印显示信息			
//		//			runtime=OSTimeGet(&err);
//		//			printf("runtime=%lld\r\n",runtime);
//		//			printf("KeyEnd!\r\n");
//		//		}
//				
//				if(MTDMFD_CanAddVal() == 0xFF )
//				{
//					DispTaskInfo();		//打印显示信息	
//				}
//				
//				BSP_OS_TimeDlyMs(2000);//延时2s
//			}
//		}

//		void TemperCheck_task(void *p_arg)			//温度检测任务函数，用来检测驱动板的温度，并实施报警
//		{

//			(void )p_arg;			//防止警告
//		//	LEDGpio_Init();
//			ADC1_CH16_Config();
//			printf("进入到Temper任务\r\n");	

//			while(1)
//			{
//		//		if(KEY_S1_UPDOWM() == Bit_RESET)
//		//		{
//					Get_Temprate();		//stm32内部的检测温度测试 ，利用ADC转换温度
//		//			LED_Turn (0);
//		//		}
//				BSP_OS_TimeDlyMs(3000);//延时2s	,并且进行任务间的切换
//			}
//		}



void CAN2Slave_StartupManager_Take(void *p_arg)
{
	OS_ERR err;
	static u8 SlaveStateCount =0;
	debugprintf("进入到从站启动管理任务！\r\n");
	
	while(1)
	{
		debugprintf("slave manager\r\n");
		
		if(SlaveStateCount > 5)		//连续发送5次从节点到初始化状态，发送bootup
		{
			debugprintf("can2slave manager 5次到\r\n");
			//SlaveStateCount = 0;
			OSTaskDel((OS_TCB*)&CAN2SlaveStartupManagerTCB,&err);	//任务从站管理任务函数执行5此后删除掉这个任务
		}						

		{
			e_nodeState slavestate;
			slavestate = getState(&MFDCAN2Slave_Data);
			debugprintf("can2slavestate=0x%x\r\n",slavestate);
			if(slavestate != Operational/* 0x05*/)
			{
					/*发送一个启动信号 cansend*/
				
					Message BootMessage;
					BootMessage.cob_id = (0x700 | (ChooseBoard & 0x0F));
					BootMessage.data[0] = 0x00;
					BootMessage.len =1;
					BootMessage.rtr = CAN_RTR_DATA;
					canSend(CAN2,&BootMessage);
					
					SlaveStateCount++;
			}
	
			else
			{
				OSTaskDel((OS_TCB*)&CAN2SlaveStartupManagerTCB,&err);	//任务从站状态位操作状态后删除掉这个任务
			}
		}
		BSP_OS_TimeDlyMs(2000);			//调用这个延时是将其进入调度函数，发生任务的调度。	
	}
}

/*车门状态发送任务*/
void DoorStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	/*1.发送门SDR数据 变量定义*/
	static u8 DoorAdd=0;
	DoorSdrFrame DoorSDRFrame;
	static u8 TimeFlagCount=0;
	
	/*2.接受SD数据变量定义*/
	DoorSdFrame SdFrmaeData;
	//static u8 TxErrFlagCount=0;
	static u8 DoorErrFlagCount[8]={0};		//8个门传输错误计数标志
	s8 resvalue;
	
	memset(DoorErrFlagCount,0,8);
	
	while(1)
	{
		/*等待发送SDR事件标志*/
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&DoorEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		
		if(err == OS_ERR_NONE)
		{
			if(can2_RxDoorSpeedTimBit[0] == 1)		//表示设置时间
			{	
				TimeFlagCount++;
				if(TimeFlagCount >5)	//表示5次
				{
					TimeFlagCount = 0;
					can2_RxDoorSpeedTimBit[0] = 0;
				}
			}
			
			DoorSDRFrame.DoorSdrData.Address =0x00;
			DoorSDRFrame.DoorSdrData.Sub_Address =0x00;
			DoorSDRFrame.DoorSdrData.Command = 0x20;
			DoorSDRFrame.DoorSdrData.SetClock = can2_RxDoorSpeedTimBit[0];
			DoorSDRFrame.DoorSdrData.Year = can2_RxTime[0];
			DoorSDRFrame.DoorSdrData.Month = can2_RxTime[1];
			DoorSDRFrame.DoorSdrData.Day = can2_RxTime[2];
			DoorSDRFrame.DoorSdrData.Hour = can2_RxTime[3];
			DoorSDRFrame.DoorSdrData.Minute = can2_RxTime[4];
			DoorSDRFrame.DoorSdrData.Second = can2_RxTime[5];
			DoorSDRFrame.DoorSdrData.Speed = can2_RxDoorSpeedTimBit[1];
			DoorSDRFrame.DoorSdrData.Dummy2 = 0x00;	

			/*变更门的地址*/
			DoorAdd = DoorAdd+1;
			if(DoorAdd>8)
			{
				DoorAdd =1;
			}
			DoorSDRFrame.DoorSdrData.Address = DoorAdd;
			
			/*发送Sdr*/
			SendDoorSdr(&DoorSDRFrame.DoorSdrData);	//将数据放入到串口3的发送缓冲区并发送

			
			
			/*2.接受SD数据*/
			{
				resvalue = GetDoorSd(USART3_RS485_RxBuffer,&SdFrmaeData);
				
				if(resvalue !=0)	//返回值有错误
				{
						//					TxErrFlagCount++;
						//					
						//					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
						//					{
						//						can2_TxSubDevFault[0] = 0xFF;
						//						
						//						/* force emission of PDO by artificially changing last emitted */
						//						MFDCAN2Slave_Data.PDO_status[TPDO_1A9_SubDecFault].last_message.cob_id = 0;
						//						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1A9_SubDecFault);		//发送PDO
						//						//debugprintf("Door2传输错误,va=%d!\r\n",resvalue);
						//						//printf("door2接受完整数据超时!\r\n");
						//						TxErrFlagCount =0;	//清除
						//						//can2_TxSubDevFault_Door =0 ;//清除门故障
						//					}					

							DoorErrFlagCount[DoorAdd-1]++;		//对应的错误计数++
						
							if(DoorErrFlagCount[DoorAdd-1] >=5)//赋值给对象字典 子系统错误表示位
							{
								can2_TxMFD1SubFault[0] |= (1<<(7-(DoorAdd-1)));	  //门对应的位 置1  （1,2,3,4,5,6,7,8 门分别对应的位置从高到低）
								
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
								
								DoorErrFlagCount[DoorAdd-1] = 0;
								
								printf("Door传输异常,%d!\r\n",resvalue);
							}
				}
				
				else		//将数据copy给对象字典，并发送到can网络上
				{
					debugprintf("door ok!\r\n");
					
					/*发送 门通信正常*/
					DoorErrFlagCount[DoorAdd-1] = 0;
					can2_TxMFD1SubFault[0] &= (~(1<<(7-(DoorAdd-1))));	  //门对应的位 清0
					
					//MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0; //whh 2016-11-23
					//sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
					
					switch(SdFrmaeData.DoorSdData.Address)		//分析门的地址放到不同的位置
					{
						case 0x01:		//子地址0x1
							memcpy(&can2_TxDoorSta1[0],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3		//WHH ,注意需要强转为(U8 *)的指针加的地址一次才加1
							memcpy(&can2_TxDoorSta1[1],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
							//printf("st1=0x%02x,st2=0x%02x\r\n",can2_TxDoorSta1[0],can2_TxDoorSta1[1]);
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_190_DoorSta1);		//发送PDO,地址等于8时在发送到can网络上
							break;
						
						case 0x02:
							memcpy(&can2_TxDoorSta2[0],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta2[1],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
							//printf("st1=0x%02x,st2=0x%02x\r\n",can2_TxDoorSta2[0],can2_TxDoorSta2[1]);
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_191_DoorSta2);	//发送PDO
							break;
						
						case 0x03:
							memcpy(&can2_TxDoorSta1[2],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta1[3],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
						  //sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_190_DoorSta1);	//发送PDO
							break;
						
						case 0x04:
							memcpy(&can2_TxDoorSta2[2],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta2[3],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_191_DoorSta2);	//发送PDO
							break;
						
						case 0x05:
							memcpy(&can2_TxDoorSta1[4],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta1[5],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_190_DoorSta1);	//发送PDO
							break;
						case 0x06:
							memcpy(&can2_TxDoorSta2[4],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta2[5],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4
						
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_191_DoorSta2);	//发送PDO
							break;
						
						case 0x07:
							memcpy(&can2_TxDoorSta1[6],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta1[7],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4								
						
							//sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_190_DoorSta1);	//发送PDO
							break;
						
						case 0x08:
							memcpy(&can2_TxDoorSta2[6],(u8*)&SdFrmaeData.DoorSdData+3,1);	//地址+3
							memcpy(&can2_TxDoorSta2[7],(u8*)&SdFrmaeData.DoorSdData+4,1);	//首地址+4								

							break;
					}
					
					if(SdFrmaeData.DoorSdData.Address == 0x08)    //8个门都通信了在发送
					{
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_190_DoorSta1].last_message.cob_id = 0;	
						sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_190_DoorSta1);		//发送PDO				
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TPDO_191_DoorSta2].last_message.cob_id = 0;						
						sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_191_DoorSta2);

						MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0; //whh 2016-11-23
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO						
						printf("send DOOR DATA!\r\n");						
					}
				}
			}			
		}
	}
}


/*空调状态查询任务*/
void AccStateQuery_Take(void *p_arg)			//空调协议SDR中增加两个预留字节没有加入  WHH
{
	OS_ERR err;
	 	
	/*1. 发送空调的SDR数据 变量定义*/
	static uint8_t TimeFlagCount=0;			//设置时间 次数计数。
	ACCSdrText	Sdrtext;
	
	/*2.空调接受SD数据 变量定义*/
	ACCSdText SdTextData;
	static u8 TxErrFlagCount=0;
	s8 resvalue; //接受函数返回值
	
	while(1)
	{
		/*等待发送SDR事件标志*/
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&ACCEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			if(can2_RxAccTempTimBit[0] &0x01)	//表示设置时间
			{	
				TimeFlagCount++;
				if(TimeFlagCount >5)	//表示5次
				{
					TimeFlagCount = 0;	//清0
					can2_RxAccTempTimBit[0] &= 0xFE;	//最后一位清0
				}
			}			
			
			memset(&Sdrtext,0,sizeof(ACCSdrText));	//清0
			
			Sdrtext.Address = ACC_ADDRES;
			Sdrtext.Sub_Address = ACC_SUB_ADDRES;
			Sdrtext.Command = ACC_COMMAND;
			
			Sdrtext.SetClock = can2_RxAccTempTimBit[0]&0x01;		//设置时钟
			Sdrtext.TargetTempVaild = (can2_RxAccTempTimBit[0]>>1)&0x01;  //目标温度有效
			
			Sdrtext.Year = can2_RxTime[0];
			Sdrtext.Month = can2_RxTime[1];
			Sdrtext.Day = can2_RxTime[2];
			Sdrtext.Hour = can2_RxTime[3];
			Sdrtext.Minute = can2_RxTime[4];
			Sdrtext.Second =can2_RxTime[5];

			Sdrtext.NetTargetTemp = can2_RxAccTempTimBit[1]; //网络目标温度为10进制
			
			SendACCSdr(&Sdrtext);	//将数据放入到串口2的发送缓冲区并发送
			
			
			/*2.接受空调状态数据*/
			{	
				resvalue = GetACCSd(USART2_RS485_RxBuffer,&SdTextData);
				
				if(resvalue !=0)	//返回值有错误
				{
					TxErrFlagCount++;
					
					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
					{
						can2_TxMFD1SubFault[1] |= 0x80;		//空调故障位置1
						
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
						
						debugprintf("ACC	Err!,va=%d!\r\n",resvalue);
						TxErrFlagCount =0;	//清除
					}					
				}
				
				else		//将数据copy给对象字典，并发送到can网络上
				{
					debugprintf("Acc ok!\r\n");
					
					/*调试接受到的数据用*/
					//					{
					//						int i;
					//							printf("u2=%d\r\n",USART2_RS485_RxLength);
					//						printf("原始数据：");		
					//						for ( i = 0; i < USART2_RS485_RxLength; i++)
					//						{	
					//							printf("%02x",USART2_RS485_RxBuffer[i]);
					//						}
					//					}					
										
					TxErrFlagCount =0;	//清除			

					can2_TxMFD1SubFault[1] &= 0x7F;		//空调故障位 清0
					/* force emission of PDO by artificially changing last emitted */
					MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
					
					memcpy(&can2_TxAccSta,(u8*)&SdTextData+3,8);		//地址+3
					memcpy(&can2_TxAccSta2,(u8*)&SdTextData+11,1);	//首地址+11							

					/* force emission of PDO by artificially changing last emitted */
					MFDCAN2Slave_Data.PDO_status[TPDO_192_ACCSta1].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_192_ACCSta1);		//发送PDO
					/* force emission of PDO by artificially changing last emitted */
					MFDCAN2Slave_Data.PDO_status[TPDO_1AA_ACCSta2].last_message.cob_id = 0;				
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_1AA_ACCSta2);
				}
			}
				
		}
		
		else
		{
			printf("ACC EventFlags Err!\r\n");
		}
	}		
}


/*
定义一个全部标号，或者在对象字典中   来表示当前是否需要发送TDR
*/
bool 	BCUTxTdrFlag = false; //是否需要发送TDR 0,不需要， 1 需要
bool 	BCUClearStatDataFlag	= false;
bool	BCUSetTimeFlag = false;
	
/*制动状态查询任务*/
void BcuStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	static u8  TimeFlagCount=0;			//时间设置标志计数
	static u8  ClearStaFlagCount=0;	//清除状态数据标志 计数位
	//static u8  CallCount = 0;			//调用计数值
	static bool	SendSdrOrTdrFlag =true;	//发送SDR 或者 TDR 标志位
	//u8 CallOddEvenCount=0; 				//函数调用的奇偶次数 (偶数的时候一直发送SDR,只有奇数才会判断是否发送TDR，TCR，或退出)
	
	BcuSdrText 	SdrText ;
	BcuTdrText 	TdrText ;
	static u8 BlockNo=0;
	
	//定义静态变量，保存要重发一次的数据
	//查看流程图 应该为当接受到NACK时，立马发送一次Sdr 或则Tdr
	/*static*/ 	BcuSdrText 	OldSdrText;
	/*static*/ 	BcuTdrText 	OldTdrText;
	
	/*接受数据变量*/
	static u8 TxErrFlagCount=0;
	//static bool GetNackFlag = false; 	//false 表示没有获得Nack，Ture 表示获得Nack，下次继续发送上次的数据（仅限发送一次）
	s8 resvalue;
	u8 IsNoACK = 0;
	
	BcuSdFrame SdDataFrame;	//定义sd变量	
	BcuTdFrame TdDataFrame;	//定义Td变量	
	
	memset(&SdrText,0,sizeof(BcuSdrText));	
	memset(&TdrText,0,sizeof(BcuTdrText));
	
	while(1)
	{	
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&BCUEventFlags,
							 (OS_FLAGS	  )TxSDROrTDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,												//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			if(BCUTxTdrFlag == true)		//表示需要发送Tdr
			{
				if(SendSdrOrTdrFlag ==true)		//先发送SDR
				{
					/*1. 发送BCU的SDR数据*/
					//memset(&SdrText,0,sizeof(BcuSdrText));
					
					//SendSdrOrTdrFlag = false;		//下次将发送TDR
					
					SdrText.Command = BCU_SDR_COMMAND;
					
					/*BCD码*/
					SdrText.Year = DecToBcd(can2_RxTime[0]);
					SdrText.Month = DecToBcd(can2_RxTime[1]);
					SdrText.Day = DecToBcd(can2_RxTime[2]);
					SdrText.Hour = DecToBcd(can2_RxTime[3]);
					SdrText.Minute = DecToBcd(can2_RxTime[4]);
					SdrText.Second = DecToBcd(can2_RxTime[5]);
					
					SdrText.BrakeRise = can2_RxBCUCmd[0];		//制动力上升指令，0xFF有效
					SdrText.Reserve2 = 0;
					SdrText.Reserve3 = 0;
					
					//清除数据位
					if(BCUClearStatDataFlag == true)
					{
						ClearStaFlagCount++;			//清除状态 标志加1
						SdrText.BCU_TraceDataClea = 1;	
						
						if(ClearStaFlagCount>=5)
						{
							ClearStaFlagCount = 0;  //清零
							BCUClearStatDataFlag = false ; 	//不在发送清除数据位
						}
					}
					else
					{
						SdrText.BCU_TraceDataClea = 0;
					}
					
					//时间有效位
					if(BCUSetTimeFlag == true)
					{
						TimeFlagCount++;		//时间设置计算标志位
						SdrText.BCU_TimeSet = 1;	
						
						if(TimeFlagCount>=5)		
						{
							TimeFlagCount = 0;	//时间设置标志计算清零
							BCUSetTimeFlag = false;
						}
					}
					else
					{
						SdrText.BCU_TimeSet = 0;
					}
					
					//1.首先将数据copy到静态变量里
					memcpy(&OldSdrText,&SdrText,sizeof(BcuSdrText));
					
					SendBcuSdrOrTdr(&SdrText,SEND_SDR);				//发送SDR
				}
				
				else //在发送TDR
				{
					//memset(&TdrText,0,sizeof(BcuTdrText));
					//SendSdrOrTdrFlag =true;  //下次将发送SDR
					
					TdrText.Command = BCU_TDR_COMMAND;
					TdrText.BCU_TraceDataNoRq = can2_RxBCUCmd[3];
					TdrText.BCU_TraceDataBlackNoRq = BlockNo;
					BlockNo ++;
					
					//1.首先将数据copy到静态变量里
					memcpy(&OldTdrText,&TdrText,sizeof(BcuTdrText));
					
					SendBcuSdrOrTdr(&TdrText,SEND_TDR);		//发送TDR
					
					//					if(BlockNo >=40)
					//					{
					//						BlockNo = 0;		//清除
					//						BCUTxTdrFlag = false;  //清除 ，不在发送TDR
					//					}
				}
			}
			
			else		//发送SDR
			{
					/*1. 发送BCU的SDR数据*/
					//memset(&SdrText,0,sizeof(BcuSdrText));
					
					SendSdrOrTdrFlag = true;		//当遇到需要发送TDR的时候，则先发送SDR
				
					SdrText.Command = BCU_SDR_COMMAND;
					
					/*BCD码*/
					SdrText.Year = DecToBcd(can2_RxTime[0]);
					SdrText.Month = DecToBcd(can2_RxTime[1]);
					SdrText.Day = DecToBcd(can2_RxTime[2]);
					SdrText.Hour = DecToBcd(can2_RxTime[3]);
					SdrText.Minute = DecToBcd(can2_RxTime[4]);
					SdrText.Second = DecToBcd(can2_RxTime[5]);
					
					SdrText.BrakeRise = can2_RxBCUCmd[0];		//制动力上升指令，0xFF有效
	        SdrText.Reserve2 = 0;
					SdrText.Reserve3 = 0;
				
					//清除数据位
					if(BCUClearStatDataFlag == true)
					{
						ClearStaFlagCount++;			//清除状态 标志加1
						SdrText.BCU_TraceDataClea = 1;	
						
						if(ClearStaFlagCount>=5)
						{
							ClearStaFlagCount = 0;  //清零
							BCUClearStatDataFlag = false ; 	//不在发送清除数据位
						}
					}
					else
					{
						SdrText.BCU_TraceDataClea = 0;
					}
					
					//时间有效位
					if(BCUSetTimeFlag == true)
					{
						TimeFlagCount++;		//时间设置计算标志位
						SdrText.BCU_TimeSet = 1;	
						
						if(TimeFlagCount>=5)		
						{
							TimeFlagCount = 0;	//时间设置标志计算清零
							BCUSetTimeFlag = false;
						}
					}
					else
					{
						SdrText.BCU_TimeSet = 0;
					}
					
					//1.首先将数据copy到静态变量里
					memcpy(&OldSdrText,&SdrText,sizeof(BcuSdrText));
					
					SendBcuSdrOrTdr(&SdrText,SEND_SDR);				//发送SDR
					
					printf("buc 11\r\n");
			}
	
			
			/*2.开始接受数据*/		//WHH 2016-10-27 有问题需要修改，请查看BCU协议手册修改
			{
				if(BCUTxTdrFlag == true)		//表示需要接受Td
				{
					if(SendSdrOrTdrFlag ==true)		//先接受Sd
					{
						SendSdrOrTdrFlag  = false; //下次将发送Tdr
						
						resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM1);			//第一次获取数据
			
						if(resvalue !=0)	//表示接受数据错误
						{
							TxErrFlagCount++;
							
							//表示连续5次都通信异常 ，则发送BCU通信异常	
							if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
							{
								can2_TxMFD1SubFault[1] |= 0x40;  //置1表示故障 
								
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
								
								debugprintf("BCU传输错误,va=%d!\r\n",resvalue);
								TxErrFlagCount =0;	//清除
							}	

							/*WHH 2016-11-3 疑问？？？*/
							if(resvalue == BCU_NACKError)	//表示获得NACK错误，则重新发送一次数据
							{
								SendBcuSdrOrTdr(&OldSdrText,SEND_SDR);				//重新发送SDR
								
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM1);			//第一次获取数据
								
								if(resvalue !=0)	//表示有错误发生
								{
									resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM2);  //第二次获取数据
									if(resvalue !=0)	//不处理
									{
									
									}
									else
									{
										//0.清除通信异常计数标志
										TxErrFlagCount = 0 ; 	
										
										can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
	
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
										sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
										
										//1.发送ACK,表示接受Bcu数据成功
										IsNoACK = BCU_ACK;
										MTDMFD_CL_Send_Data(&IsNoACK,1);

										//2.将数据copy到对象字典中 发送到can网络
										memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
										memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
										memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
										
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;	
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;											
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);									
									}
								}
								
								else	//表示没有错误
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);

									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;																		
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}
									
							}
							
							else if( (resvalue == BCU_FrameError) ||(resvalue ==BCU_BCCError))		//协议图5条.表示接受信息出错
							{
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM2);  //第二次获取数据
								
								if(resvalue!=0)		//表示有错误发生
								{
									//有错误继续下一次数据
								}
								else		//表示接受消息正确
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);

									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}
							}

							else		//表示其他错误 ，则直接返回，执行下次的发送数据
							{
								
							}
						}
						
						else		//表示接受数据正确
						{
							//0.清除通信异常计数标志
							TxErrFlagCount = 0 ; 	
							
							can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
							
							//1.发送ACK,表示接受Bcu数据成功
							IsNoACK = BCU_ACK;
							MTDMFD_CL_Send_Data(&IsNoACK,1);

							//2.将数据copy到对象字典中 发送到can网络
							memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
							memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
							memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);			
						}

						
					}
					
					else		//接受Td
					{
						SendSdrOrTdrFlag  = true; //下次将发送Sdr	

						if(BlockNo >=40)
						{
							BlockNo = 0;		//清除
							BCUTxTdrFlag = false;  //清除 ，不在发送TDR
						}						
						
						resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&TdDataFrame,GET_TD,GET_NUM1);			//第一次获取数据
			
						if(resvalue !=0)	//表示接受数据错误
						{
							TxErrFlagCount++;
							
							//表示连续5次都通信异常 ，则发送BCU通信异常	
							if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
							{
								can2_TxMFD1SubFault[1] |= 0x40;  //置1表示故障 
								
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
								
								debugprintf("BCU传输错误,va=%d!\r\n",resvalue);
								TxErrFlagCount =0;	//清除
							}	

							if(resvalue == BCU_NACKError)	//表示获得NACK错误
							{
								//GetNackFlag = true;
								SendBcuSdrOrTdr(&OldTdrText,SEND_TDR);				//发送TDR
								
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&TdDataFrame,GET_TD,GET_NUM1);			//第一次获取数据
								
								if(resvalue !=0)	//表示有错误发生
								{
									resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&TdDataFrame,GET_TD,GET_NUM2);  //第二次获取数据
									if(resvalue !=0)	//不处理
									{
									
									}
									else
									{
										//0.清除通信异常计数标志
										TxErrFlagCount = 0 ; 	
										
										can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
				
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
										sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
										
										//1.发送ACK,表示接受Bcu数据成功
										IsNoACK = BCU_ACK;
										MTDMFD_CL_Send_Data(&IsNoACK,1);

										//2.将数据copy到对象字典中 发送到can网络
										memcpy(can2_TxBCUSta1,(u8*)&TdDataFrame.BcuTdData+1,8);		//复制8字节数据
										memcpy(can2_TxBCUSta2,(u8*)&TdDataFrame.BcuTdData+9,8);
										memcpy(can2_TxBCUSta3,(u8*)&TdDataFrame.BcuTdData+17,8);
										
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);							
									}
								}
								
								else	//表示没有错误
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
			
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&TdDataFrame.BcuTdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&TdDataFrame.BcuTdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&TdDataFrame.BcuTdData+17,8);
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}								
							}
							
							else if( (resvalue == BCU_FrameError) ||(resvalue ==BCU_BCCError))		//协议图5条.表示接受信息出错
							{
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&TdDataFrame,GET_TD,GET_NUM2);  //第二次获取数据
								
								if(resvalue!=0)		//表示有错误发生
								{
									//有错误继续下一次数据
								}
								else		//表示接受消息正确
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&TdDataFrame.BcuTdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&TdDataFrame.BcuTdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&TdDataFrame.BcuTdData+17,8);
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}
							}

							else		//表示其他错误 ，则直接返回，执行下次的发送数据
							{
								
							}
						}
						
						else		//表示接受数据正确
						{
							//0.清除通信异常计数标志
							TxErrFlagCount = 0 ; 	
							
							can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
							
							//1.发送ACK,表示接受Bcu数据成功
							IsNoACK = BCU_ACK;
							MTDMFD_CL_Send_Data(&IsNoACK,1);

							//2.将数据copy到对象字典中 发送到can网络
							memcpy(can2_TxBCUSta1,(u8*)&TdDataFrame.BcuTdData+1,8);		//复制8字节数据
							memcpy(can2_TxBCUSta2,(u8*)&TdDataFrame.BcuTdData+9,8);
							memcpy(can2_TxBCUSta3,(u8*)&TdDataFrame.BcuTdData+17,8);
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);		
						}

					}
				}
				
				
				else	//表示直接接受Sd
				{
					printf("buc 22\r\n");
					//					{
					//						int i;
					//						printf("u4=%d\r\n",USART4_CL_RxLength);
					//						printf("原始数据：");		
					//						for (i = 0; i < USART4_CL_RxLength; i++)
					//						{	
					//							printf("%02x",USART4_CL_RxBuffer[i]);
					//						}
					//						printf("\r\n\r\n");
					//					}
					
						resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM1);			//第一次获取数据
			
						if(resvalue !=0)	//表示接受数据错误
						{
							printf("buc 33,res=%d\r\n",resvalue);
							TxErrFlagCount++;
							
							//表示连续5次都通信异常 ，则发送BCU通信异常	
							if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
							{
								can2_TxMFD1SubFault[1] |= 0x40;  //置1表示故障 
								
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
								
								debugprintf("BCU Sdr传输错误,va=%d!\r\n",resvalue);
								TxErrFlagCount =0;	//清除
							}	

							/*WHH 2016-11-3 疑问？？？*/
							if(resvalue == BCU_NACKError)	//表示获得NACK错误，则重新发送一次数据
							{
								SendBcuSdrOrTdr(&OldSdrText,SEND_SDR);				//重新发送SDR
								
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM1);			//第一次获取数据
								
								if(resvalue !=0)	//表示有错误发生
								{
									resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM2);  //第二次获取数据
									if(resvalue !=0)	//不处理
									{
									
									}
									else
									{
										//0.清除通信异常计数标志
										TxErrFlagCount = 0 ; 	
										
										can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常

										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
										sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO										
										
										//1.发送ACK,表示接受Bcu数据成功
										IsNoACK = BCU_ACK;
										MTDMFD_CL_Send_Data(&IsNoACK,1);

										//2.将数据copy到对象字典中 发送到can网络
										memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
										memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
										memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
										
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
										/* force emission of PDO by artificially changing last emitted */
										MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
										sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);							
									}
								}
								
								else	//表示没有错误
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO		
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}
									
							}
							
							else if( (resvalue == BCU_FrameError) ||(resvalue ==BCU_BCCError))		//协议图5条.表示接受信息出错
							{
								resvalue = GetBcuSdOrTd(USART4_CL_RxBuffer,&SdDataFrame,GET_SD,GET_NUM2);  //第二次获取数据
								
								if(resvalue!=0)		//表示有错误发生
								{
									//有错误继续下一次数据
								}
								else		//表示接受消息正确
								{
									//0.清除通信异常计数标志
									TxErrFlagCount = 0 ; 	
									
									can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
									sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
									
									//1.发送ACK,表示接受Bcu数据成功
									IsNoACK = BCU_ACK;
									MTDMFD_CL_Send_Data(&IsNoACK,1);

									//2.将数据copy到对象字典中 发送到can网络
									memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
									memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
									memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
									
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
									/* force emission of PDO by artificially changing last emitted */
									MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
									sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
								}
							}

							else		//表示其他错误 ，则直接返回，执行下次的发送数据
							{
								
							}
						}
						
						else		//表示接受数据正确
						{
							printf("buc 44\r\n");
							
							{
									int i;
										printf("u4=%d\r\n",USART4_CL_RxLength);
									printf("原始数据：");		
									for ( i = 0; i < USART4_CL_RxLength; i++)
									{	
										printf("%02x",USART4_CL_RxBuffer[i]);
									}
							}
							//0.清除通信异常计数标志
							TxErrFlagCount = 0 ; 	
							can2_TxMFD1SubFault[1] &= 0xBF;		//空调故障位 清0 表示通信正常
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO
							
							//1.发送ACK,表示接受Bcu数据成功
							IsNoACK = BCU_ACK;
							MTDMFD_CL_Send_Data(&IsNoACK,1);

							//2.将数据copy到对象字典中 发送到can网络
							memcpy(can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//复制8字节数据
							memcpy(can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);
							memcpy(can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);
							
							/*whh 216-11-12 sendOnePdoevent 占用不少时间，如果用了定时发送就不用再这里调用发送*/
	//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
	//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
	//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);		

							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_193_BCUSta1].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_194_BCUSta2].last_message.cob_id = 0;										
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_195_BCUSta3].last_message.cob_id = 0;									
							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);							
						}
					
				}
				
				
				//				if(g_BCURxSdOrTdFlag == BCU_RxSD)		//表示为SD数据
				//				{
				//						
				//					
				//						g_BCURxSdOrTdFlag =0; //清除接受标志
				//						resvalue = GetBcuSdOrTd(USART2_RS485_RxBuffer,&SdDataFrame);
				//						
				//						if(resvalue !=0)	//返回值有错误
				//						{
				//							TxErrFlagCount++;
				//		//							//发送NACK,表示接受Bcu数据失败
				//		//							IsNoACK = BCU_NACK;
				//		//							MTDMFD_CL_Send_Data(&IsNoACK,1);		
				//							
				//							if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
				//							{
				//								//can2_TxSubDevFault_BCU = 1;
				//								can2_TxSubDevFault[1] = can2_TxSubDevFault[1]|0x40;		//1
				//								
				//								/* force emission of PDO by artificially changing last emitted */
				//								MFDCAN2Slave_Data.PDO_status[TPDO_1A9_SubDecFault].last_message.cob_id = 0;
				//								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1A9_SubDecFault);		//发送PDO
				//								//debugprintf("BCU2传输错误,va=%d!\r\n",resvalue);
				//								TxErrFlagCount =0;	//清除
				//								//can2_TxSubDevFault_BCU = 0;//清除BCU故障
				//							}					
				//						}
				//						
				//						else		//将数据copy给对象字典，并发送到can网络上
				//						{
				//							debugprintf("Bcu sd ok!\r\n");
				//							can2_TxSubDevFault[1] = can2_TxSubDevFault[1]&0xBF;		//清零
				//							/* force emission of PDO by artificially changing last emitted */
				//							MFDCAN2Slave_Data.PDO_status[TPDO_1A9_SubDecFault].last_message.cob_id = 0;
				//							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1A9_SubDecFault);		//发送PDO
				//							
				//							memcpy(&can2_TxBCUSta1,(u8*)&SdDataFrame.BcuSdData+1,8);		//地址+1
				//							memcpy(&can2_TxBCUSta2,(u8*)&SdDataFrame.BcuSdData+9,8);	//首地址+9		
				//							memcpy(&can2_TxBCUSta3,(u8*)&SdDataFrame.BcuSdData+17,8);	//首地址+17	
				//						
				//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_193_BCUSta1);		//发送PDO
				//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_194_BCUSta2);
				//							sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_195_BCUSta3);
				//						}
				//				}
				//				else if(g_BCURxSdOrTdFlag == BCU_RxTD)		//表示为TD数据
				//				{
				//				
				//				}
				//			}	
				//		}
				//		
				//		else
				//		{
				//			printf("BCUerr=%d\r\n",err);
				//		}
			}
		}
	}
}



/*空压机状态查询任务*/
void AcpStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	
	//static uint8_t TimeFlagCount=0;
	/*1. 发送空调的SDR数据*/
	ACPSdrFrame SdrFrame;
	
	/*2.SD*/
	ACPSdFrame  SdFrame;
	static u8 TxErrFlagCount=0;
	s8 resvalue;
	
	while(1)
	{
		/*1.等待发送SDR事件标志*/
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&ACPEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			SdrFrame.StartAddressH8 = 0x00;
			SdrFrame.StartAddressL8 = 0x01;
			SdrFrame.RegisterNumberH8 = 0x00;
			SdrFrame.RegisterNumberL8 = 0x03;
			
			SendACPSdr(&SdrFrame);
	
			//	/*2 发送完成事件标志位*/
			//			/*2.发送 空压机 发送数据完成事件标志*/
			//			OSFlagPost((OS_FLAG_GRP*)&ACPEventFlags,
			//						 (OS_FLAGS	  )TxSDREndFlag,				//发送SDR完成标志
			//						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
			//						 (OS_ERR*	    )&err);			
			
			//			/*等待发送SDR完成事件标志*/
			//			//等待事件标志组
			//			OSFlagPend((OS_FLAG_GRP*)&ACPEventFlags,
			//								 (OS_FLAGS	  )TxSDREndFlag,			//等待 发送SDR完成标志位
			//								 (OS_TICK     )0,								
			//								 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
			//								 (CPU_TS*     )0,
			//								 (OS_ERR*	    )&err);	
		
			//			if(err == OS_ERR_NONE)	//表示没有错误
			//			{
			//				OSFlagPend((OS_FLAG_GRP*)&ACPEventFlags,
			//									 (OS_FLAGS	  )RxStartFlag,			//等待接受开始标志位
			//									 (OS_TICK     )20,							//等待20ms
			//									 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
			//									 (CPU_TS*     )0,
			//									 (OS_ERR*	    )&err);				
				
			//				if(err == OS_ERR_TIMEOUT)		//表示等待超时
			//				{
			//					//没有接收到数据，则表示帧传输错误
			//					TxErrFlagCount++;
			//					
			//					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
			//					{
			//						
			//		//					/* force emission of PDO by artificially changing last emitted */
			//		//					MFDCAN2Slave_Data.PDO_status[TPDO_1A9_SubDecFault].last_message.cob_id = 0;
			//		//					sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1A9_SubDecFault);		//发送PDO
			//		//					debugprintf("Acp1传输错误!\r\n");
			//						TxErrFlagCount =0;	//清除
			//					}
			//				}
			//				
			//				else if(err == OS_ERR_NONE)
			//				{
			//					OSFlagPend((OS_FLAG_GRP*)&ACPEventFlags,
			//										 (OS_FLAGS	  )RxEndFlag,			//等待 接受完成标志位
			//										 (OS_TICK     )0,						
			//										 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
			//										 (CPU_TS*     )0,
			//										 (OS_ERR*	    )&err);		
			
			/*2.接受SD数据*/
			{			
				resvalue = GetACPSd(USART3_RS485_RxBuffer,&SdFrame);
				
				if(resvalue !=0)	//返回值有错误
				{
					TxErrFlagCount++;
					
					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
					{											
						can2_TxMFD3SubFault[0] |= 0x04;  //空压机对应的位置置1
						
						MFDCAN2Slave_Data.PDO_status[TPDO_1AE_SubDecFault].last_message.cob_id = 0;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AE_SubDecFault);		//发送PDO
						debugprintf("ACP 传输异常\r\n");
						
						TxErrFlagCount =0;	//清除
					}					
				}
				
				else		//将数据copy给对象字典，并发送到can网络上
				{
					debugprintf("Acp ok!\r\n");
					
					can2_TxMFD3SubFault[0] &= 0xFB;  //空压机对应的位置  清零
					MFDCAN2Slave_Data.PDO_status[TPDO_1AE_SubDecFault].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AE_SubDecFault);		//发送PDO 
					
					/**/  //WHH 需要将设置对象字典，将数据发送至ACP，然后在发送到网络
					
					//2016-10-27  未完成
				}
			}			
		}
		
		else
		{
			printf("ACP EventFlags Err!\r\n");
		}
	}		
}


/*ATC（信号）状态查询任务*/
void ATCStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	/*1.发送门SDR数据*/
	ATCSdrText ATCSdrData;
	static u8 TimeFlagCount=0;
	
	/*SD*/
	ATCSdFrame SdFrmaeData;
	static u8 TxErrFlagCount=0;
	s8 resvalue;
	
	while(1)
	{
		/*等待发送SDR事件标志*/
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&ATCEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		
		if(err == OS_ERR_NONE)
		{
			
			//时间有效位
			if(can2_RxTime[1] >0) //月份大于0 ，表示时间有效
			{
				ATCSdrData.ATC_TimeVaild = 1;	
			}
			else
			{
				ATCSdrData.ATC_TimeVaild = 0;			
			}		
				
			//			if(can2_RxATCSdr1[0] == 1)		//表示设置时间
			//			{	
			//				TimeFlagCount++;
			//				if(TimeFlagCount >5)	//表示5次
			//				{
			//					TimeFlagCount = 0;
			//					can2_RxATCSdr1[0] = 0;
			//				}
			//			}
			
			ATCSdrData.ATC_TextCodeH8 = ATC_SDR_TEXTCODEH8;
			ATCSdrData.ATC_TextCodeL8 = ATC_SDR_TEXTCODEL8;
			ATCSdrData.ATC_TotalweightH8 = can2_RxATCSdr2[0];
			ATCSdrData.ATC_TotalweightL8 = can2_RxATCSdr2[1];
			
			/*VVVF1动作*/ 
			ATCSdrData.ATC_No1VVVF1Vaild = (can2_RxATCSdr1[0]>>7)&0x01;
			ATCSdrData.ATC_No3VVVF1Vaild = (can2_RxATCSdr1[0]>>6)&0x01;
			ATCSdrData.ATC_No6VVVF1Vaild = (can2_RxATCSdr1[0]>>5)&0x01;
			
			/*VVVF1电制有效*/
			ATCSdrData.ATC_No1VVVF1EleBrake = (can2_RxATCSdr1[0]>>4)&0x01;
			ATCSdrData.ATC_No3VVVF1EleBrake = (can2_RxATCSdr1[0]>>3)&0x01;
			ATCSdrData.ATC_No6VVVF1EleBrake = (can2_RxATCSdr1[0]>>2)&0x01;			
			
			/*BCU 动作*/
			ATCSdrData.ATC_BCU1vaild = (can2_RxATCSdr1[1]>>7)&0x01;
			ATCSdrData.ATC_BCU2vaild = (can2_RxATCSdr1[1]>>6)&0x01;
			ATCSdrData.ATC_BCU3vaild = (can2_RxATCSdr1[1]>>5)&0x01;					
			ATCSdrData.ATC_BCU4vaild = (can2_RxATCSdr1[1]>>4)&0x01;
			ATCSdrData.ATC_BCU5vaild = (can2_RxATCSdr1[1]>>3)&0x01;
			ATCSdrData.ATC_BCU6vaild = (can2_RxATCSdr1[1]>>2)&0x01;	

			/*滑行*/
			ATCSdrData.ATC_Slide1 = (can2_RxATCSdr1[2]>>7)&0x01;
			ATCSdrData.ATC_Slide6 = (can2_RxATCSdr1[2]>>6)&0x01;	
			
			ATCSdrData.ATC_No1VVVF1SlideStatu = (can2_RxATCSdr1[2]>>5)&0x01;
			ATCSdrData.ATC_No6VVVF1SlideStatu = (can2_RxATCSdr1[2]>>4)&0x01;			

			ATCSdrData.ATC_TrainNumber	= can2_RxATCSdr1[3]; //BCD码
			//ATCSdrData.ATC_TimeVaild = 0;	
			
			/*时间*/
			ATCSdrData.ATC_Year = DecToBcd(0);
			ATCSdrData.ATC_Month = DecToBcd(0);
			ATCSdrData.ATC_Day = DecToBcd(0);					
			ATCSdrData.ATC_Hour = DecToBcd(0);
			ATCSdrData.ATC_Minute = DecToBcd(0);
			ATCSdrData.ATC_Second = DecToBcd(0);				
			
			ATCSdrData.ATC_TBFroce1 = can2_RxATCSdr1[4];	
			ATCSdrData.ATC_TBFroce2 = can2_RxATCSdr1[5];	
			
			ATCSdrData.ATC_DoorStatu = can2_RxATCSdr2[3];	
			ATCSdrData.ATC_LoadRate = can2_RxATCSdr2[4];	
			
			/*发送Sdr*/
			SendATCSdr(&ATCSdrData);	//将数据放入到串口3的发送缓冲区并发送
			
			
			/*2.接受数据*/
			{
				resvalue = GetATCSd(USART3_RS485_RxBuffer,&SdFrmaeData);
				
				if(resvalue !=0)	//返回值有错误
				{
					TxErrFlagCount++;
					
					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
					{						
						can2_TxMFD2SubFault[0] = can2_TxMFD2SubFault[0]|0x02;		//1
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_1AD_SubDecFault].last_message.cob_id = 0;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AD_SubDecFault);		//发送PDO  MFD2板卡
						
						printf("ATC ERR res=%d!\r\n",resvalue);
						TxErrFlagCount =0;	//清除
					}					
				}
				
				else		//将数据copy给对象字典，并发送到can网络上
				{
					debugprintf("ATC ok!\r\n");
					
					/*发送 ATC通信正常*/
					TxErrFlagCount =0;	//清除
				
					can2_TxMFD2SubFault[0] = can2_TxMFD2SubFault[0]&0xFD;	//清零
					MFDCAN2Slave_Data.PDO_status[TPDO_1AD_SubDecFault].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AD_SubDecFault);		//发送PDO  MFD2板卡
					
					/*发送ATC时间*/
					can2_TxATCTime[0] = SdFrmaeData.ATCSdData.ATC_Year;
					can2_TxATCTime[1] = SdFrmaeData.ATCSdData.ATC_Month;
					can2_TxATCTime[2] = SdFrmaeData.ATCSdData.ATC_Day;
					can2_TxATCTime[3] = SdFrmaeData.ATCSdData.ATC_Hour;
					can2_TxATCTime[4] = SdFrmaeData.ATCSdData.ATC_Minute;
					can2_TxATCTime[5] = SdFrmaeData.ATCSdData.ATC_Second;
					MFDCAN2Slave_Data.PDO_status[TPDO_1AB_ATCTime].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_1AB_ATCTime);
					
					memcpy(can2_TxATCSta1,(u8*)&SdFrmaeData.ATCSdData+2,7);		//首地址+2	
					memcpy(can2_TxATCSta2,(u8*)&SdFrmaeData.ATCSdData+9,8);
					memcpy(can2_TxATCSta3,(u8*)&SdFrmaeData.ATCSdData+17,6);	//状态3的前量字节
					memcpy(&can2_TxATCSta3[6],(u8*)&SdFrmaeData.ATCSdData+33,2);  //状态3的后两字节
					memcpy(can2_TxATCSta4,(u8*)&SdFrmaeData.ATCSdData+38,4);	
					
					/* force emission of PDO by artificially changing last emitted */
					MFDCAN2Slave_Data.PDO_status[TPDO_196_ATCSta1].last_message.cob_id = 0;		
					MFDCAN2Slave_Data.PDO_status[TPDO_197_ATCSta2].last_message.cob_id = 0;
					MFDCAN2Slave_Data.PDO_status[TPDO_198_ATCSta3].last_message.cob_id = 0;
					MFDCAN2Slave_Data.PDO_status[TPDO_199_ATCSta4].last_message.cob_id = 0;

					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_196_ATCSta1);
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_197_ATCSta2);
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_198_ATCSta3);
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_199_ATCSta4);	
				}
			}			
		}
		else
		{
			printf("PIS EventFlags Err!\r\n");
		}		
	}
}


/*PIS（广播）状态查询任务*/
void PISStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	/*1.发送门SDR数据 变量定义*/
	PISSdrText SdrText;
	static u8 TimeFlagCount=0;
	
	/*2.接受SD数据 变量定义*/
	PISSdFrame SdFrmaeData;
	static u8 TxErrFlagCount=0;
	s8 resvalue;
	
	while(1)
	{
		/*等待发送SDR事件标志*/
		//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&PISEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		
		if(err == OS_ERR_NONE)
		{
				//时间有效位
				if(can2_RxTime[1] >0) //月份大于0 ，表示时间有效
				{
					can2_RxPISSdr1[0] |= 0x01;  //PIS,时间有效位
				}
				else
				{
					can2_RxPISSdr1[0] &= 0xFE;  //清除
				}			
			
				//			//表示设置时间
				//			if(can2_RxPISSdr1[0] == 1)		
				//			{	
				//				TimeFlagCount++;
				//				if(TimeFlagCount >5)	//表示5次
				//				{
				//					TimeFlagCount = 0;
				//					can2_RxPISSdr1[0] = 0;
				//				}
				//			}
			
				
				//位置的计算
				switch(ChooseBoard)
				{
					case MC1_MFD2_NODEID:  //1车  位置=0
						can2_RxPISSdr1[1] &= 0xEF; //清除位置位
						break;
					     
					case MC2_MFD2_NODEID:  //6车   位置=1
						can2_RxPISSdr2[1] |= 0x10; //设置位置位
						break;
					
					default:
						break;
				}
				
			//memset(&SdrText,0,sizeof(PISSdrText));
	
			SdrText.PIS_DataLongth = 0x1A;
			SdrText.PIS_TimeVaild = can2_RxPISSdr1[0];  //时间有效位
	
			memcpy((u8 *)&SdrText.Byte3Undef+1,&can2_RxPISSdr1[1],1);	 /*byte4*/
	
			/*BCD码*/
			SdrText.Year = DecToBcd(16);
			SdrText.Month = DecToBcd(8);
			SdrText.Day = DecToBcd(11);
			SdrText.Hour = DecToBcd(10);
			SdrText.Minute = DecToBcd(11);
			SdrText.Second = DecToBcd(12);	

			/*列车编号*/
			memcpy((u8 *)&SdrText.PIS_TrainNumberH8,can2_RxPISSdr2,2);
				
			/*列车目的地序号*/	
			memcpy((u8 *)&SdrText.PIS_TrainDesIDH8,&can2_RxPISSdr2[2],6);	
	
			/*发送PIS_SDR数据*/
			SendPISSdr(&SdrText);
			
			/*2.接受数据*/
			{
				resvalue = GetPISSd(USART2_RS485_RxBuffer,&SdFrmaeData);
				
				if(resvalue !=0)	//返回值有错误
				{
					TxErrFlagCount++;
					printf("res=%d\r\n",resvalue);
					
					if(TxErrFlagCount>=5)		//赋值给对象字典 子系统错误表示位
					{
						can2_TxMFD2SubFault[0] = can2_TxMFD2SubFault[0]|0x08;		//1
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_1AD_SubDecFault].last_message.cob_id = 0;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AD_SubDecFault);		//发送PDO  MFD2板卡
						
						printf("PIS传输异常!\r\n");
						TxErrFlagCount =0;	//清除
					}					
				}
				
				else		//将数据copy给对象字典，并发送到can网络上
				{
					debugprintf("PIS ok!\r\n");
					
					/*发送 ATC通信正常*/
					TxErrFlagCount =0;	//清除
					
					can2_TxMFD2SubFault[0] = can2_TxMFD2SubFault[0]&0xF7;	//清零
					MFDCAN2Slave_Data.PDO_status[TPDO_1AD_SubDecFault].last_message.cob_id = 0;
					sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AD_SubDecFault);		//发送PDO  MFD2板卡
						
					/*WHH 需要将设置对象字典，将数据发送至PIS，然后在发送到网络*/ 
					memcpy(can2_TxPISSta1,(u8*)&SdFrmaeData.PISSdData.PIS_DataLongth+2,8);	//首地址+2	
					memcpy(can2_TxPISSta2,(u8*)&SdFrmaeData.PISSdData.PIS_DataLongth+10,3);
					
					/* force emission of PDO by artificially changing last emitted */
					MFDCAN2Slave_Data.PDO_status[TPDO_19A_PISSta1].last_message.cob_id = 0;		
					MFDCAN2Slave_Data.PDO_status[TPDO_19B_PISSta2].last_message.cob_id = 0;

					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_19A_PISSta1);
					sendOnePDOevent(&MFDCAN2Slave_Data,TPDO_19B_PISSta2);
				}
			}			
		}
		
		else
		{
			printf("PIS EventFlags Err!\r\n");
		}
	}
}



/*
定义一个全部标号，或者在对象字典中   来表示当前是否需要发送TDR
*/
bool 	VVVF1TxTdrFlag = false; //是否需要发送TDR 0,不需要， 1 需要
bool 	VVVF1TxTcrFlag	= false;
//bool	VVVF1SetTimeFlag = false;
//bool  VVVF1TimeEffFlag = false;

/*VVVF1状态查询任务*/
void VVVF1StateQuery_Take(void *p_arg)
{
	OS_ERR err;
	static u8  TimeFlagCount=0;
	static u8  CallCount = 0;							//调用计数值
	u8 CallOddEvenCount=0; 								//函数调用的奇偶次数 (偶数的时候一直发送SDR,只有奇数才会判断是否发送TDR，TCR，或退出)
	
	VVVFSdrText 	SdrText;
	VVVFTdrText 	TdrText;
	VVVFTcrText   TcrText;
	
	VVVFSdFrame   SdFrameData;
	VVVFTdFrame   TdFrameData;
	
	static u16 BlockNo=0;	
	static u8 RxSdErrFlagCount=0;
	static u8 RxTdErrFlagCount=0;
	s8 resvalue;
	
	//	memset(&SdrText,0,sizeof(VVVFSdrText));	
	//	memset(&TdrText,0,sizeof(VVVFTdrText));

	while(1)
	{
	/*等待发送SDR事件标志*/
	//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&VVVF1EventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);

		if(err !=OS_ERR_NONE)
			printf("VVFErr=%d\r\n",err);
		
		/*1.发送数据*/
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			CallOddEvenCount = CallCount%2;				//得到奇偶数
			
			if(CallOddEvenCount ==0) 							//偶数就只能发送SDR
			{
				memset(&SdrText,0,sizeof(VVVFSdrText));
				
				SdrText.TextNumber = VVVF_SDR_TEXTNUM;
				SdrText.Command = VVVF_SDR_COMMAND;
				SdrText.State = VVVF_SDR_STATE;
				SdrText.Sender = VVVF_SDR_SENDER;
				SdrText.Receiver = VVVF_SDR_RECEIVER1;
				SdrText.OrderNo = VVVF_SDR_NO;
				
				/*BCD码*/
				SdrText.Year = DecToBcd(can2_RxTime[0]);
				SdrText.Month = DecToBcd(can2_RxTime[1]);
				SdrText.Day = DecToBcd(can2_RxTime[2]);
				SdrText.Hour = DecToBcd(can2_RxTime[3]);
				SdrText.Minute = DecToBcd(can2_RxTime[4]);
				SdrText.Second = DecToBcd(can2_RxTime[5]);
				
				//时间有效位
				if(can2_RxTime[1] >0) //月份大于0 ，表示时间有效
				{
					can2_RxVVVFCmd[0] |= 0x04;  //VVVF1,时间有效位
				}
				else
				{
					can2_RxVVVFCmd[0] &= 0xFB;  //清除
				}
				SdrText.TCU_TimeEffect = (can2_RxVVVFCmd[0]>>2)&0x01;
				
				//				//时间有效位
				//				if(VVVF1TimeEffFlag == true)
				//				{
				//					VVVF1TimeEffFlag = false;
				//					SdrText.TCU_TimeEffect =1;
				//				}
				//				else
				//				{
				//					SdrText.TCU_TimeEffect =0;
				//				}

				
				//时间设置
				if(((can2_RxVVVFCmd[0]>>3)&0x01) ==1 )
				{
					TimeFlagCount++;		//时间设置计算标志位
					SdrText.TCU_TimeSet = 1;	
					
					if(TimeFlagCount>=5)	
					{
						TimeFlagCount = 0;  //清零时间设置计数值
						can2_RxVVVFCmd[0] &= 0xF7;
					}					
				}
				else
				{
					SdrText.TCU_TimeSet = 0;
				}
				
				//				//时间设置位
				//				if(VVVF1SetTimeFlag == true)
				//				{
				//					TimeFlagCount++;		//时间设置计算标志位
				//					SdrText.TCU_TimeSet = 1;	
				//					
				//					if(TimeFlagCount>=5)	
				//					{
				//						TimeFlagCount = 0;  //清零时间设置计数值
				//						VVVF1SetTimeFlag = false;
				//					}
				//				}
				//				else
				//				{
				//					SdrText.TCU_TimeSet = 0;
				//				}

				SendVVVFSdrOrTdrOrTcr((u8*)&SdrText,VVVF_SEND_SDR);				//发送SDR	

			}
			
			else	//表示为奇数 则只能发送TDR，TCR，或者不发送
			{			
				if(VVVF1TxTdrFlag == true)						//表示需要发送Tdr
				{			
					memset(&TdrText,0,sizeof(VVVFTdrText));
					
					TdrText.TextNumber = VVVF_TDR_TEXTNUM;
					TdrText.Command = VVVF_TDR_COMMAND;
					TdrText.State = VVVF_TDR_STATE;
					TdrText.Sender = VVVF_TDR_SENDER;
					TdrText.Receiver = VVVF_TDR_RECEIVER1;
					TdrText.OrderNo = VVVF_TDR_NO;
					TdrText.CategoryCode = can2_RxVVVFCmd[2];		//VVVF1的类别代码字节
					TdrText.BlockNumH8 = (BlockNo>>8)&0xFF;
					TdrText.BlockNumL8 = (BlockNo)&0xFF;
					
					SendVVVFSdrOrTdrOrTcr((u8*)&TdrText,VVVF_SEND_TDR);		//发送TDR
					
					BlockNo ++;
					//					if(BlockNo >140)
					//					{
					//						BlockNo = 0;							//清除
					//						VVVF1TxTdrFlag = false;  //清除 ，不在发送TDR
					//					}
				}
						
				else if(VVVF1TxTcrFlag == true)	//发送TCR
				{
					memset(&TcrText,0,sizeof(VVVFTcrText));
					
					TcrText.TextNumber = VVVF_TCR_TEXTNUM;
					TcrText.Command = VVVF_TCR_COMMAND;
					TcrText.State = VVVF_TCR_STATE;
					TcrText.Sender = VVVF_TCR_SENDER;
					TcrText.Receiver = VVVF_TCR_RECEIVER1;
					TcrText.OrderNo = VVVF_TCR_NO;
					TcrText.CategoryCode = 0x02;
					
					SendVVVFSdrOrTdrOrTcr((u8*)&TcrText,VVVF_SEND_TCR);
					
				}
						
				else		//不发送任何数据
				{
					//printf("VVVF Not Send data!\r\n");
				}
			}

					
			if(CallCount>=10)		//因为有TDR，发送的次数为10次时为偶数，清除，后面会加1就是奇数了
			{
				CallCount =0;
			}			
			CallCount ++;		//每进入一次加1		
		
			
			/*2.等待接受数据*/
			{
				if(CallOddEvenCount == 0)  //表示为偶数 则接受Sd
				{
					resvalue = GetVVVFSdOrTd((u8 *)HDLCARxBuf,(u8*)&SdFrameData.VVVFSdData,VVVF1);
					
					if(resvalue !=0)	//返回值有错误
					{
						RxSdErrFlagCount++;
						if(RxSdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
						{
							//can2_TxSubDevFault_VVVF1_Or_SIV =1;
							//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]|0x20;		//1
							
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x20;
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
							
							//printf("VVVF1 传输异常!\r\n");
							RxSdErrFlagCount =0;	//清除
						}
					}
					else
					{
						RxSdErrFlagCount =0;	//清除
						//can2_TxSubDevFault_VVVF1_Or_SIV =0;
						//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]&0xDF;		//清零
						
						can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xDF;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
						
						/*copy-VVVF的数据，发送到can2网络上*/
						memcpy(can2_TxVVVF1Sta1,(u8*)&SdFrameData.VVVFSdData+6,8);  //首地偏移6字节  //WHH ,注意需要强转为(U8 *)的指针加的地址一次才加1
						memcpy(can2_TxVVVF1Sta2,(u8*)&SdFrameData.VVVFSdData+14,6);
						memcpy(can2_TxVVVF1Sta3,(u8*)&SdFrameData.VVVFSdData+20,8);
						memcpy(can2_TxVVVF1Sta4,(u8*)&SdFrameData.VVVFSdData+28,8);
						memcpy(can2_TxVVVF1Sta5,(u8*)&SdFrameData.VVVFSdData+36,6);
						
						//						{
						//							u8 buf[sizeof(VVVFSdText)];
						//							u8 i;
						//							printf("sta1\r\n");
						//							
						//							memcpy(buf,(u8*)&SdFrameData.VVVFSdData,sizeof(VVVFSdText));
						//							
						//							for(i=0;i<sizeof(VVVFSdText);i++)
						//							{
						//								printf("%02x",buf[i]);
						//							}
						//							printf("\r\n");
						//							
						//							printf("sta2=0x%x",can2_TxVVVF1Sta2[0]);
						//						}
						
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TDPO_19C_VVVFSta1].last_message.cob_id = 0;	
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TDPO_19D_VVVFSta2].last_message.cob_id = 0;		
						MFDCAN2Slave_Data.PDO_status[TDPO_19E_VVVFSta3].last_message.cob_id = 0;	
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TDPO_19F_VVVFSta4].last_message.cob_id = 0;	
						MFDCAN2Slave_Data.PDO_status[TDPO_1A0_VVVFSta5].last_message.cob_id = 0;	
						
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_19C_VVVFSta1);		//发送PDO
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_19D_VVVFSta2);
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_19E_VVVFSta3);		//发送PDO
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_19F_VVVFSta4);
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A0_VVVFSta5);		//发送PDO
					}
				}
				
				else		//表示为奇数，则接受Td
				{
					if(VVVF1TxTcrFlag == true)  //Tcr 不接受数据
					{
						VVVF1TxTcrFlag =false;
					}
					
					else if(VVVF1TxTdrFlag == true)	//Tdr接受数据
					{
						if(BlockNo >140)
						{
							BlockNo = 0;							//清除
							VVVF1TxTdrFlag = false;  //清除 ，不在发送TDR
						}

						resvalue = GetVVVFSdOrTd((u8 *)HDLCARxBuf,(u8*)&TdFrameData.VVVFTdData,VVVF1);
						
						if(resvalue !=0)	//返回值有错误
						{
							RxTdErrFlagCount++;
							if(RxTdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
							{
								//can2_TxSubDevFault_VVVF1_Or_SIV =1;
								//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]|0x20;		//1
								
								can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x20;
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
								
								printf("VVVF1_Rx TD 数据超时!\r\n");
								RxTdErrFlagCount =0;	//清除
							}
						}
						
						else
						{
							RxTdErrFlagCount =0;	//清除
							//can2_TxSubDevFault_VVVF1_Or_SIV =0;
							//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]&0xDF;		//清零
							
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xDF;
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO		  MFD1

							/*copy-VVVF的数据，发送到can2网络上*/	
							
							/*WHH TDR对象字典数据还没有定义*/
						}
					}
					
					else  //不接受任何数据
					{
					}
				}
			}	
			
		}
	}
}

/*
定义一个全部标号，或者在对象字典中   来表示当前是否需要发送TDR
*/
bool 	VVVF2TxTdrFlag = false; //是否需要发送TDR 0,不需要， 1 需要
bool 	VVVF2TxTcrFlag	= false;
//bool	VVVF2SetTimeFlag = false;
//bool  VVVF2TimeEffFlag = false;

/*VVVF2状态查询任务*/
void VVVF2StateQuery_Take(void *p_arg)
{
	OS_ERR err;
	static u8  TimeFlagCount=0;
	static u8  CallCount = 0;							//调用计数值
	u8 CallOddEvenCount=0; 								//函数调用的奇偶次数 (偶数的时候一直发送SDR,只有奇数才会判断是否发送TDR，TCR，或退出)
	
	VVVFSdrText 	SdrText;
	VVVFTdrText 	TdrText;
	VVVFTcrText   TcrText;
	
	VVVFSdFrame   SdFrameData;
	VVVFTdFrame   TdFrameData;
	
	static u16 BlockNo=0;	
	static u8 RxSdErrFlagCount=0;
	static u8 RxTdErrFlagCount=0;
	s8 resvalue;
	
	//	memset(&SdrText,0,sizeof(VVVFSdrText));	
	//	memset(&TdrText,0,sizeof(VVVFTdrText));

	while(1)
	{
	/*等待发送SDR事件标志*/
	//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&VVVF2EventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);

		if(err !=OS_ERR_NONE)
			printf("VVF2Err=%d\r\n",err);
		
		/*1.发送数据*/
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			CallOddEvenCount = CallCount%2;				//得到奇偶数
			
			if(CallOddEvenCount ==0) 							//偶数就只能发送SDR
			{
				memset(&SdrText,0,sizeof(VVVFSdrText));
				
				SdrText.TextNumber = VVVF_SDR_TEXTNUM;
				SdrText.Command = VVVF_SDR_COMMAND;
				SdrText.State = VVVF_SDR_STATE;
				SdrText.Sender = VVVF_SDR_SENDER;
				SdrText.Receiver = VVVF_SDR_RECEIVER2;
				SdrText.OrderNo = VVVF_SDR_NO;
				
				/*BCD码*/
				SdrText.Year = DecToBcd(can2_RxTime[0]);
				SdrText.Month = DecToBcd(can2_RxTime[1]);
				SdrText.Day = DecToBcd(can2_RxTime[2]);
				SdrText.Hour = DecToBcd(can2_RxTime[3]);
				SdrText.Minute = DecToBcd(can2_RxTime[4]);
				SdrText.Second = DecToBcd(can2_RxTime[5]);
				
				//时间有效位
				if(can2_RxTime[1] >0) //月份大于0 ，表示时间有效
				{
					can2_RxVVVFCmd[0] |= 0x01;  //VVVF2,时间有效位
				}
				else
				{
					can2_RxVVVFCmd[0] &= 0xFE;
				}
				SdrText.TCU_TimeEffect = can2_RxVVVFCmd[0]&0x01;				
				
				//				//时间有效位
				//				if(VVVF2TimeEffFlag == true)
				//				{
				//					VVVF2TimeEffFlag = false;
				//					SdrText.TCU_TimeEffect =1;
				//				}
				//				else
				//				{
				//					SdrText.TCU_TimeEffect =0;
				//				}

				//时间设置
				if(((can2_RxVVVFCmd[0]>>1)&0x01) ==1 )  //VVVF2,时间设置有效位
				{
					TimeFlagCount++;		//时间设置计算标志位
					SdrText.TCU_TimeSet = 1;	
					
					if(TimeFlagCount>=5)	
					{
						TimeFlagCount = 0;  //清零时间设置计数值
						can2_RxVVVFCmd[0] &= 0xFD;
					}					
				}
				else
				{
					SdrText.TCU_TimeSet = 0;
				}
				
				//				//时间设置位
				//				if(VVVF2SetTimeFlag == true)
				//				{
				//					TimeFlagCount++;		//时间设置计算标志位
				//					SdrText.TCU_TimeSet = 1;	
				//					
				//					if(TimeFlagCount>=5)	
				//					{
				//						 TimeFlagCount = 0;  //清零时间设置计数值
				//						 VVVF2SetTimeFlag = false;
				//					}
				//				}
				//				else
				//				{
				//					SdrText.TCU_TimeSet = 0;
				//				}

				SendVVVFSdrOrTdrOrTcr((u8*)&SdrText,VVVF_SEND_SDR);				//发送SDR	

			}
			
			else	//表示为奇数 则只能发送TDR，TCR，或者不发送
			{			
				if(VVVF2TxTdrFlag == true)						//表示需要发送Tdr
				{			
					memset(&TdrText,0,sizeof(VVVFTdrText));
					
					TdrText.TextNumber = VVVF_TDR_TEXTNUM;
					TdrText.Command = VVVF_TDR_COMMAND;
					TdrText.State = VVVF_TDR_STATE;
					TdrText.Sender = VVVF_TDR_SENDER;
					TdrText.Receiver = VVVF_TDR_RECEIVER2;
					TdrText.OrderNo = VVVF_TDR_NO;
					TdrText.CategoryCode = can2_RxVVVFCmd[2];		//VVVF2的类别代码字节
					TdrText.BlockNumH8 = (BlockNo>>8)&0xFF;
					TdrText.BlockNumL8 = (BlockNo)&0xFF;
					
					SendVVVFSdrOrTdrOrTcr((u8*)&TdrText,VVVF_SEND_TDR);		//发送TDR
					
					BlockNo ++;
					//					if(BlockNo >140)
					//					{
					//						BlockNo = 0;							//清除
					//						VVVF2TxTdrFlag = false;  //清除 ，不在发送TDR
					//					}
				}
						
				else if(VVVF2TxTcrFlag == true)	//发送TCR
				{
					memset(&TcrText,0,sizeof(VVVFTcrText));
					
					TcrText.TextNumber = VVVF_TCR_TEXTNUM;
					TcrText.Command = VVVF_TCR_COMMAND;
					TcrText.State = VVVF_TCR_STATE;
					TcrText.Sender = VVVF_TCR_SENDER;
					TcrText.Receiver = VVVF_TCR_RECEIVER2;
					TcrText.OrderNo = VVVF_TCR_NO;
					TcrText.CategoryCode = 0x02;
					
					SendVVVFSdrOrTdrOrTcr((u8*)&TcrText,VVVF_SEND_TCR);
					
				}
						
				else		//不发送任何数据
				{
					//printf("VVVF2 Not Send data!\r\n");
				}
			}

					
			if(CallCount>=10)		//因为有TDR，发送的次数为10次时为偶数，清除，后面会加1就是奇数了
			{
				CallCount =0;
			}			
			CallCount ++;		//每进入一次加1		
		
			/*2.等待接受数据*/
			{
				if(CallOddEvenCount == 0)  //表示为偶数 则接受Sd
				{
					resvalue = GetVVVFSdOrTd((u8 *)HDLCBRxBuf,(u8*)&SdFrameData.VVVFSdData,VVVF2);
					
					if(resvalue !=0)	//返回值有错误
					{
						RxSdErrFlagCount++;
						printf("VVVF2res=%d\r\n",resvalue);
						
						if(RxSdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
						{
							//can2_TxSubDevFault_VVVF2 =1;
							//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]|0x10;		//置1
							
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x10;		//置1
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO		MFD1
							
							//printf("VVVF2 传输异常!\r\n");
							RxSdErrFlagCount =0;	//清除
						}
					}
					else
					{
						RxSdErrFlagCount =0;	//清除
						//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]&0xEF;		//清零
						
						can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xEF;
												
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
						
						/*copy-VVVF的数据，发送到can2网络上*/
						memcpy(can2_TxVVVF2Sta1,(u8*)&SdFrameData.VVVFSdData+6,8);  //首地偏移6字节  //WHH ,注意需要强转为(U8 *)的指针加的地址一次才加1
						memcpy(can2_TxVVVF2Sta2,(u8*)&SdFrameData.VVVFSdData+14,6);
						memcpy(can2_TxVVVF2Sta3,(u8*)&SdFrameData.VVVFSdData+20,8);
						memcpy(can2_TxVVVF2Sta4,(u8*)&SdFrameData.VVVFSdData+28,8);
						memcpy(can2_TxVVVF2Sta5,(u8*)&SdFrameData.VVVFSdData+36,6);
						
						
				//						{
				//							u8 buf[sizeof(VVVFSdText)];
				//							u8 i;
				//							printf("sta1\r\n");
				//							
				//							memcpy(buf,(u8*)&SdFrameData.VVVFSdData,sizeof(VVVFSdText));
				//							
				//							for(i=0;i<sizeof(VVVFSdText);i++)
				//							{
				//								printf("%02x",buf[i]);
				//							}
				//							printf("\r\n");
				//							
				//							printf("sta2=0x%x",can2_TxVVVF1Sta2[0]);
											
				//							
				//														for(i=0;i<8;i++)
				//								printf("sta1=%02x\r\n",can2_TxVVVF2Sta1[i]);
				//														for(i=0;i<6;i++)
				//								printf("sta2=%02x\r\n",can2_TxVVVF2Sta2[i]);
				//														for(i=0;i<8;i++)
				//								printf("sta3=%02x\r\n",can2_TxVVVF2Sta3[i]);
				//							for(i=0;i<8;i++)
				//								printf("sta4=%02x\r\n",can2_TxVVVF2Sta4[i]);
				//														for(i=0;i<6;i++)
				//								printf("sta5=%02x\r\n",can2_TxVVVF2Sta5[i]);
				//						}						
										
						
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TDPO_1A1_VVVFSta1].last_message.cob_id = 0;	
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TDPO_1A2_VVVFSta2].last_message.cob_id = 0;		
						MFDCAN2Slave_Data.PDO_status[TDPO_1A3_VVVFSta3].last_message.cob_id = 0;	
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TDPO_1A4_VVVFSta4].last_message.cob_id = 0;	
						MFDCAN2Slave_Data.PDO_status[TDPO_1A5_VVVFSta5].last_message.cob_id = 0;	
						
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A1_VVVFSta1);		//发送PDO
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A2_VVVFSta2);
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A3_VVVFSta3);		//发送PDO
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A4_VVVFSta4);
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A5_VVVFSta5);		//发送PDO
					}
				}
				
				else		//表示为奇数，则接受Td
				{
					if(VVVF2TxTcrFlag == true)  //Tcr 不接受数据
					{
						VVVF2TxTcrFlag =false;
					}
					
					else if(VVVF2TxTdrFlag == true)	//Tdr接受数据
					{
						if(BlockNo >140)
						{
							BlockNo = 0;							//清除
							VVVF2TxTdrFlag = false;  //清除 ，不在发送TDR
						}

						resvalue = GetVVVFSdOrTd((u8 *)HDLCBRxBuf,(u8*)&TdFrameData.VVVFTdData,VVVF2);
						
						if(resvalue !=0)	//返回值有错误
						{
							RxTdErrFlagCount++;
							if(RxTdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
							{
								//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]|0x10;		//置1
								
								can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x10;		//置1
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
								
								printf("VVVF2_Rx TD 数据超时!\r\n");
								RxTdErrFlagCount =0;	//清除
							}
						}
						
						else
						{
							RxTdErrFlagCount =0;	//清除
							//can2_TxSubDevFault[1] = can2_TxSubDevFault[1]&0xEF;		//清零
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xEF;
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;							
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO		MFD1  

							/*copy-VVVF的数据，发送到can2网络上*/	
							
							/*WHH TDR对象字典数据还没有定义*/
						}
					}
					
					else  //不接受任何数据
					{
						
					}
				}
			}	
			
		}
	}
}


/*
定义一个全部标号，或者在对象字典中   来表示当前是否需要发送TDR
*/
bool 	SIVTxTdrFlag = false; //是否需要发送TDR 0,不需要， 1 需要
bool 	SIVTxTcrFlag	= false;
//bool	SIVSetTimeFlag = false;
//bool  SIVTimeEffFlag = false;
/*SIV状态查询任务*/
void SIVStateQuery_Take(void *p_arg)
{
	OS_ERR err;
	static u8  TimeFlagCount=0;
	static u8  CallCount = 0;							//调用计数值
	u8 CallOddEvenCount=0; 								//函数调用的奇偶次数 (偶数的时候一直发送SDR,只有奇数才会判断是否发送TDR，TCR，或退出)
	
	SIVSdrText 	SdrText;
	SIVTdrText 	TdrText;
	SIVTcrText  TcrText;
	
	SIVSdFrame   SdFrameData;
	SIVTdFrame   TdFrameData;
	
	static u16 BlockNo=0;	
	static u8 RxSdErrFlagCount=0;
	static u8 RxTdErrFlagCount=0;
	s8 resvalue;
	
	//	memset(&SdrText,0,sizeof(SIVSdrText));	
	//	memset(&TdrText,0,sizeof(SIVTdrText));

	while(1)
	{
	/*等待发送SDR事件标志*/
	//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&SIVEventFlags,
							 (OS_FLAGS	  )TxSDRFlag,					//发送SDR标志位
							 (OS_TICK     )0,									//阻塞等待
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);

		/*1.发送数据*/
		if(err == OS_ERR_NONE)	//表示没有错误
		{
			CallOddEvenCount = CallCount%2;				//得到奇偶数
			
			if(CallOddEvenCount ==0) 							//偶数就只能发送SDR
			{
				memset(&SdrText,0,sizeof(SIVSdrText));
				
				SdrText.TextNumber = SIV_SDR_TEXTNUM;
				SdrText.Command = SIV_SDR_COMMAND;
				SdrText.State = SIV_SDR_STATE;
				SdrText.Sender = SIV_SDR_SENDER;
				SdrText.Receiver = SIV_SDR_RECEIVER;
				SdrText.OrderNo = SIV_SDR_NO;
				
				/*BCD码*/
				SdrText.Year = DecToBcd(can2_RxTime[0]);
				SdrText.Month = DecToBcd(can2_RxTime[1]);
				SdrText.Day = DecToBcd(can2_RxTime[2]);
				SdrText.Hour = DecToBcd(can2_RxTime[3]);
				SdrText.Minute = DecToBcd(can2_RxTime[4]);
				SdrText.Second = DecToBcd(can2_RxTime[5]);
				
				//时间有效位
				if(can2_RxTime[1] >0) //月份大于0 ，表示时间有效
				{
					can2_RxSIVCmd[0] |= 0x01;  //SIV,时间有效位
				}
				else
				{
					can2_RxSIVCmd[0] &= 0xFE;
				}
				SdrText.SIV_TimeEffect = can2_RxSIVCmd[0]&0x01;		
				
				//				//时间有效位
				//				if(SIVTimeEffFlag == true)
				//				{
				//					SIVTimeEffFlag = false;
				//					SdrText.SIV_TimeEffect =1;
				//				}
				//				else
				//				{
				//					SdrText.SIV_TimeEffect =0;
				//				}
	
				//时间设置
				if(((can2_RxSIVCmd[0]>>1)&0x01) ==1 )  //SIV,时间设置有效位
				{
					TimeFlagCount++;		//时间设置计算标志位
					SdrText.SIV_TimeSet = 1;	
					
					if(TimeFlagCount>=5)	
					{
						TimeFlagCount = 0;  //清零时间设置计数值
						can2_RxSIVCmd[0] &= 0xFD;
					}					
				}
				else
				{
					SdrText.SIV_TimeSet = 0;
				}
				
				//				//时间设置位
				//				if(SIVSetTimeFlag == true)
				//				{
				//					TimeFlagCount++;		//时间设置计算标志位
				//					SdrText.SIV_TimeSet = 1;	
				//					
				//					if(TimeFlagCount>=5)	
				//					{
				//						TimeFlagCount = 0;  //清零时间设置计数值
				//						SIVSetTimeFlag = false;
				//					}
				//				}
				//				else
				//				{
				//					SdrText.SIV_TimeSet = 0;
				//				}

				SendSIVSdrOrTdrOrTcr((u8*)&SdrText,SIV_SEND_SDR);				//发送SDR	

			}
			
			else	//表示为奇数 则只能发送TDR，TCR，或者不发送
			{			
				if(SIVTxTdrFlag == true)						//表示需要发送Tdr
				{			
					memset(&TdrText,0,sizeof(SIVTdrText));
					
					TdrText.TextNumber = SIV_TDR_TEXTNUM;
					TdrText.Command = SIV_TDR_COMMAND;
					TdrText.State = SIV_TDR_STATE;
					TdrText.Sender = SIV_TDR_SENDER;
					TdrText.Receiver = SIV_TDR_RECEIVER;
					TdrText.OrderNo = SIV_TDR_NO;
					TdrText.SIV_CategoryCode = can2_RxSIVCmd[0];		//SIV别代码字节
					TdrText.SIV_TDBlackNoRqH8 = (BlockNo>>8)&0xFF;
					TdrText.SIV_TDBlackNoRqL8 = (BlockNo)&0xFF;
					
					SendSIVSdrOrTdrOrTcr((u8*)&TdrText,SIV_SEND_TDR);		//发送TDR
					
					BlockNo ++;
					//					if(BlockNo >140)
					//					{
					//						BlockNo = 0;							//清除
					//						SIVTxTdrFlag = false;  //清除 ，不在发送TDR
					//					}
				}
						
				else if(SIVTxTcrFlag == true)	//发送TCR
				{
					memset(&TcrText,0,sizeof(SIVTcrText));
					
					TcrText.TextNumber = SIV_TCR_TEXTNUM;
					TcrText.Command = SIV_TCR_COMMAND;
					TcrText.State = SIV_TCR_STATE;
					TcrText.Sender = SIV_TCR_SENDER;
					TcrText.Receiver = SIV_TCR_RECEIVER;
					TcrText.OrderNo = SIV_TCR_NO;
					TcrText.KindOfRecord = 0x02;
					
					SendSIVSdrOrTdrOrTcr((u8*)&TcrText,SIV_SEND_TCR);
					
				}
						
				else		//不发送任何数据
				{
					printf("SIV Not Send data!\r\n");
				}
			}

					
			if(CallCount>=10)		//因为有TDR，发送的次数为10次时为偶数，清除，后面会加1就是奇数了
			{
				CallCount =0;
			}			
			CallCount ++;		//每进入一次加1		
		
			
			/*2.等待接受数据*/
			{
				if(CallOddEvenCount == 0)  //表示为偶数 则接受Sd
				{
					resvalue = GetSIVSdOrTd((u8 *)HDLCARxBuf,(u8*)&SdFrameData.SIVSdData);
					
					if(resvalue !=0)	//返回值有错误
					{
						RxSdErrFlagCount++;
						if(RxSdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
						{
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x20;
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
							
							printf("SIV 传输异常!\r\n");
							RxSdErrFlagCount =0;	//清除
						}
					}
					else
					{
						RxSdErrFlagCount =0;	//清除
						
						can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xDF;
						
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;						
						sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
						
						/*copy-SIV的数据，发送到can2网络上*/
						memcpy(can2_TxSIVSd1,(u8*)&SdFrameData.SIVSdData+6,1);	//首地偏移6字节  //WHH ,注意需要强转为(U8 *)的指针加的地址一次才加1
						memcpy(&can2_TxSIVSd1[1],(u8*)&SdFrameData.SIVSdData+13,6);  
						memcpy(can2_TxSIVSd2,(u8*)&SdFrameData.SIVSdData+19,1);	//copy架线电压,后面有一个字节为空
						memcpy(&can2_TxSIVSd2[1],(u8*)&SdFrameData.SIVSdData+21,7);	
						memcpy(can2_TxSIVSd3,(u8*)&SdFrameData.SIVSdData+34,3); ////功率单位(V 相) 温度 ,后有6字节为空
						
						{
							u8 buf[sizeof(SIVSdText)];
							u8 i;
							printf("sta1\r\n");
							
							memcpy(buf,(u8*)&SdFrameData.SIVSdData,sizeof(SIVSdText));
							
							for(i=0;i<sizeof(SIVSdText);i++)
							{
								printf("%02x",buf[i]);
							}
							printf("\r\n");
						}
						
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[TDPO_1A6_SIVSta1].last_message.cob_id = 0;	
						/* force emission of PDO by artificially changing last emitted */	
						MFDCAN2Slave_Data.PDO_status[TDPO_1A7_SIVSta2].last_message.cob_id = 0;		
						MFDCAN2Slave_Data.PDO_status[TDPO_1A8_SIVSta3].last_message.cob_id = 0;	

						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A6_SIVSta1);		//发送PDO
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A7_SIVSta2);
						sendOnePDOevent(&MFDCAN2Slave_Data,TDPO_1A8_SIVSta3);		//发送PDO
					}
				}
				
				else		//表示为奇数，则接受Td
				{
					if(SIVTxTcrFlag == true)  //Tcr 不接受数据
					{
						SIVTxTcrFlag =false;
					}
					
					else if(SIVTxTdrFlag == true)	//Tdr接受数据
					{
						if(BlockNo >140)
						{
							BlockNo = 0;							//清除
							SIVTxTdrFlag = false;  //清除 ，不在发送TDR
						}

						resvalue = GetSIVSdOrTd((u8 *)HDLCARxBuf,(u8*)&TdFrameData.SIVTdData);
						
						if(resvalue !=0)	//返回值有错误
						{
							RxTdErrFlagCount++;
							if(RxTdErrFlagCount >=25 )  //连续25次没有接受到数据为传输异常
							{
								can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]|0x20;
								/* force emission of PDO by artificially changing last emitted */
								MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
								sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO  MFD1
								
								printf("SIV_Rx TD 数据超时!\r\n");
								RxTdErrFlagCount =0;	//清除
							}
						}
						
						else
						{
							RxTdErrFlagCount =0;	//清除
							
							can2_TxMFD1SubFault[1] = can2_TxMFD1SubFault[1]&0xDF;
							
							/* force emission of PDO by artificially changing last emitted */
							MFDCAN2Slave_Data.PDO_status[TPDO_1AC_SubDecFault].last_message.cob_id = 0;
							sendOnePDOevent(&MFDCAN2Slave_Data,(UNS8)TPDO_1AC_SubDecFault);		//发送PDO		  MFD1

							/*copy-SIV的数据，发送到can2网络上*/	
							
							/*WHH TDR对象字典数据还没有定义*/
						}
					}
					
					else  //不接受任何数据
					{
					}
				}
			}	
			
		}
		
		else
		{
			printf("SIV EventFlags Err!\r\n");
		}
	}

}


/*
//DI 采集任务函数
DI采集中应该加入延时滤波
*/
void DICheck_Take(void *p_arg)
{	
	OS_ERR err;
	
	u8 i=0;
	u8 DI110Buf[VOL110V_NUMBER+1] = {0};		//第一个下标0不使用，从下标1开始存储
	u8 DI24Buf[VOL24V_NUMBER+1] = {0};
	u8 sendreval;
	static u8 CallCount=0;
	
		//	u8 OldDI110Buf[VOL110V_NUMBER+1] = {0};		//第一个下标0不使用，从下标1开始存储
		//	u8 OldDI24Buf[VOL24V_NUMBER+1] = {0};
	while(1)
	{
		//请求信号量 ，阻塞等待信号量
		OSSemPend(&DI_Check_Sem,0,OS_OPT_PEND_BLOCKING,0,&err); 
		if(err)
		{
			printf("DI_Check_Sem=%d\r\n",err);
		}
		else	
		{
			CallCount++;
			//不同的板卡DI的含义不同
			switch(ChooseBoard)
			{
				case MC1_MFD1_NODEID:
				case MC2_MFD1_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					Vol_24VRead_Scan(DI24Buf);
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD1DI[0] = 0;
					can2_TxMFD1DI[1] = 0;

					for(i=0;i<sizeof(MCMFD1Di110VIndex);i++)
					{
						if(i<=7)
						{
							can2_TxMFD1DI[0] |= (DI110Buf[MCMFD1Di110VIndex[i]]<<(7-i));
						}
						else
						{
							can2_TxMFD1DI[1] |= (DI110Buf[MCMFD1Di110VIndex[i]]<<(15-i));
						}
					}
					
					for(i=0;i<sizeof(MCMFD1Di24VIndex);i++)
					{
						can2_TxMFD1DI[1] |= (DI24Buf[MCMFD1Di24VIndex[i]<<(3-i)]);		//(7-4-i)
					}

					//printf("MFD1di1=%#x,di2=%#x\r\n",can2_TxMFD1DI[0],can2_TxMFD1DI[1]);
					
					if(CallCount >=2)
					{
						CallCount = 0;
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,0);		//pdo号0  0x18d
					}
					break;
				
				case MC1_MFD2_NODEID:
				case MC2_MFD2_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD2DI[0] = 0;
					can2_TxMFD2DI[1] = 0;
					for(i=0;i<sizeof(MCMFD2DiIndex);i++)
					{
						can2_TxMFD2DI[0] |= (DI110Buf[MCMFD2DiIndex[i]]<<(7-i));		//数据占7位，从字节最高位开始放入
					}
					//			debugprintf("id=0x%x\r\n",can2_TxMFD2DI[0]);
					//			debugprintf("\r\n");
					
					if(CallCount >=2)
					{
						CallCount = 0;
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[1].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,1);		//pdo号1  0x18E		
					}
					break;
				
				case MC1_MFD3_NODEID:
				case MC2_MFD3_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD3DI[0] = 0;
					for(i=0;i<sizeof(MCMFD3DiIndex);i++)
					{
						can2_TxMFD3DI[0] |= (DI110Buf[MCMFD3DiIndex[i]]<<(7-i));	//数据占4位，从字节最高位开始放入
					}
					
					if(CallCount >=2)
					{
						CallCount = 0;
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[2].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,2);		//pdo号2  0x18F		
					}						
					break;		
					
				case T_MFD1_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD1DI[0] = 0;
				
					for(i=0;i<sizeof(TDiIndex);i++)
					{
						can2_TxMFD1DI[0] |= (DI110Buf[TDiIndex[i]]<<(7-i));
					}  
	
					if(CallCount >=2)
					{
						CallCount = 0;	

						//						/*比较第1次 与 第2次采集的数据是否都是1*/
						//						for(i=0;i<VOL110V_NUMBER+1;i++)
						//						{
						//							if((DI110Buf[i] ==1)&&(OldDI110Buf[i] ==1))
						//							{
						//								DI110Buf[i]= 1;
						//							}
						//							else
						//							{
						//								DI110Buf[i]= 0;	
						//							}						
						//						}
						//						
						//						/*将每个值赋值给对象字典*/
						//						can2_TxMFD1DI[0] = 0;
						//					
						//						for(i=0;i<sizeof(TDiIndex);i++)
						//						{
						//							can2_TxMFD1DI[0] |= (DI110Buf[TDiIndex[i]]<<(7-i));
						//						}
					
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,0);		//pdo号0  0x18d
						
						//printf("T_DI=%#x\r\n",can2_TxMFD1DI[0]);
					}
					//					memcpy(OldDI110Buf,DI110Buf,VOL110V_NUMBER+1);
					break;			
				
				case M_MFD1_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD1DI[0]=0;

					for(i=0;i<sizeof(MDiIndex);i++)
					{
						can2_TxMFD1DI[0] |=(DI110Buf[MDiIndex[i]]<<(7-i));
					}
					
					if(CallCount >=2)
					{
						CallCount = 0;
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,0);		//pdo号0  0x18d			
					}
					break;

				case T1_MFD1_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD1DI[0]=0;

					for(i=0;i<sizeof(T1DiIndex);i++)
					{
						can2_TxMFD1DI[0] |=(DI110Buf[T1DiIndex[i]]<<(7-i)); 
					}
					
					if(CallCount >=2)
					{
						CallCount = 0;	
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,0);		//pdo号0  0x18d			
					}
					break;
				
				case T2_MFD1_NODEID:
					/*扫描DI硬线状态*/
					Vol_110VRead_Scan(DI110Buf);
					//Vol_24VRead_Scan(DI24Buf);  只有110v
					
					/*将每个值赋值给对象字典*/
					can2_TxMFD1DI[0]=0;

					for(i=0;i<sizeof(T2DiIndex);i++)
					{
						can2_TxMFD1DI[0] |=(DI110Buf[T2DiIndex[i]]<<(7-i));
					}
	
					if(CallCount >=2)
					{
						CallCount = 0;				
						/* force emission of PDO by artificially changing last emitted */
						MFDCAN2Slave_Data.PDO_status[0].last_message.cob_id = 0;							
						sendOnePDOevent(&MFDCAN2Slave_Data,0);		//pdo号0  0x18d	
					}						
					break;
				
				//		case MC2_MFD1_NODEID:
				//			/*扫描DI硬线状态*/
				//			Vol_110VRead_Scan(DI110Buf);
				//			Vol_24VRead_Scan(DI24Buf);
				//			
				//			/*将每个值赋值给对象字典*/
				//			can2_TxMFD1DI[0] = 0;
				//			can2_TxMFD1DI[1] = 0;

				//			for(i=0;i<sizeof(MCMFD1Di110VIndex);i++)
				//			{
				//				if(i<7)
				//				{
				//					can2_TxMFD1DI[0] |= (DI110Buf[MCMFD1Di110VIndex[i]]<<(7-i));
				//				}
				//				else
				//				{
				//					can2_TxMFD1DI[1] |= (DI110Buf[MCMFD1Di110VIndex[i]]<<(15-i));
				//				}
				//			}
				//			
				//			for(i=0;i<sizeof(MCMFD1Di24VIndex);i++)
				//			{
				//				can2_TxMFD1DI[1] |= (DI24Buf[MCMFD1Di24VIndex[i]<<(3-i)]);		//(7-4-i)
				//			}

				//			break;
				//		
				//		case MC2_MFD2_NODEID:
				//			/*扫描DI硬线状态*/
				//			Vol_110VRead_Scan(DI110Buf);
				//			//Vol_24VRead_Scan(DI24Buf);  只有110v
				//			
				//			/*将每个值赋值给对象字典*/
				//			can2_TxMFD2DI[0] = 0;
				//			for(i=0;i<sizeof(MCMFD2DiIndex);i++)
				//			{
				//				can2_TxMFD2DI[0] |= (DI110Buf[MCMFD2DiIndex[i]]<<(7-i));		//数据占7位，从字节最高位开始放入
				//			}
				//			break;
				//		
				//		case MC2_MFD3_NODEID:
				//			/*扫描DI硬线状态*/
				//			Vol_110VRead_Scan(DI110Buf);
				//			//Vol_24VRead_Scan(DI24Buf);  只有110v
				//			
				//			/*将每个值赋值给对象字典*/
				//			can2_TxMFD3DI[0] = 0;
				//			for(i=0;i<sizeof(MCMFD3DiIndex);i++)
				//			{
				//				can2_TxMFD3DI[0] |= (DI110Buf[MCMFD3DiIndex[i]]<<(7-i));	//数据占4位，从字节最高位开始放入
				//			}
				//			break;	

				default:
					break;
			}		
		}
	}
}



	/*USART3接受处理任务函数*/
void USART3RxTake(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	static unsigned char StartFlag = 0;	
	//uint8_t RxChar;
	while(1)
	{
		OSTaskSemPend(0,OS_OPT_PEND_BLOCKING,&ts,&err);		//阻塞等待任务信号量
		if(err == OS_ERR_NONE)	//没有错误
		{
			switch(ChooseBoard)
			{	
				case MC1_MFD1_NODEID:			//门接受中断
				case T_MFD1_NODEID:	
				case M_MFD1_NODEID:
				case T1_MFD1_NODEID:
				case T2_MFD1_NODEID:
				case MC2_MFD1_NODEID:	
					//if(RxChar == 0x02)  //表示接受的为开始数据
					if((USART3_RS485_RxBuffer[0] ==  0x02 ) && (USART3_RS485_RxLength ==1 )) //表示第一字节 为0x02 才为开始
					{
						StartFlag = 1;

						g_Uart3RxStartFlag =1;	
						//						/*发送接受开始事件标志*/
						//						OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
						//							 (OS_FLAGS	  )RxStartFlag,				//发送奇接受开始 事件标志
						//							 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						//							 (OS_ERR*	    )&err);
												
						//printf("U3 Rx start!\r\n");
					}
					if((StartFlag == 1) && (USART3_RS485_RxLength ==8))		//门响应数据长度为8，表示数据接受完成
					{
						g_Uart3RxDataFlag = 1;
						StartFlag = 0;

						//						/*发送接受结束事件标志*/
						//						OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
						//							 (OS_FLAGS	  )RxEndFlag,				//发送奇接受结束 事件标志
						//							 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						//							 (OS_ERR*	    )&err);			
						
												
						//	printf("U3 Rx end!\r\n");
						//						{
						//							u8 i=0;
						//							for(i=0;i<8;i++)
						//							{
						//								printf("u3[%d]=0x%x\r\n",i,USART3_RS485_RxBuffer[i]);
						//							}
						//						}
					}
				break;
					
				case	MC1_MFD2_NODEID:		//ATC接受中断
				case	MC2_MFD2_NODEID:
					break;
				
				case  MC1_MFD3_NODEID:		//ACP接受中断
				case 	MC2_MFD3_NODEID:
					break;
					
				default:
					printf("uart3 Rx ChooseBoard Error !\r\n");
					break;
			}			
		}
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
							 (OS_TICK     )5,								//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_MFD_CANLED_Off(CAN2_TX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_MFD_CANLED_On(CAN2_TX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_MFD_CANLED_Off(CAN2_TX_LED);
		}
		
				//等待事件标志组
		OSFlagPend((OS_FLAG_GRP*)&CAN2LedEventFlag,
							 (OS_FLAGS	  )CAN2RxLED_FLAG,		//发送标志位
							 (OS_TICK     )5,								//20ms定时
							 (OS_OPT	    )OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
							 (CPU_TS*     )0,
							 (OS_ERR*	    )&err);
		if(err == OS_ERR_TIMEOUT)		//超时
		{
			MTD_MFD_CANLED_Off(CAN2_RX_LED);
		}
		else if(err == OS_ERR_NONE)
		{
			MTD_MFD_CANLED_On(CAN2_RX_LED);
			BSP_OS_TimeDlyMs(150);
			MTD_MFD_CANLED_Off(CAN2_RX_LED);
		}
		//BSP_OS_TimeDlyMs(10);
	}
}




//定时器1的回调函数
void tmr1_callback(void *p_tmr, void *p_arg)
{
	RunLed_Turn();	//运行灯翻转
}


/*
定时器，回调函数的定义
*/
void DoorTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&DoorEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);
}


void ACCTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&ACCEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);	
}


void BCUTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&BCUEventFlags,
						 (OS_FLAGS	  )TxSDROrTDRFlag,						//可以发送SDR标志,或者TDR
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);		
}

void ACPTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&ACPEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);			
}

void ATCTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&ATCEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);	
}

void PISTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	OSFlagPost((OS_FLAG_GRP*)&PISEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);	
}

void VVVF1TrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	
	OSFlagPost((OS_FLAG_GRP*)&VVVF1EventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);		
}

void VVVF2TrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	
	OSFlagPost((OS_FLAG_GRP*)&VVVF2EventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);		
}

void SIVTrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	
	OSFlagPost((OS_FLAG_GRP*)&SIVEventFlags,
						 (OS_FLAGS	  )TxSDRFlag,						//可以发送SDR标志
						 (OS_OPT	 	  )OS_OPT_POST_FLAG_SET,
						 (OS_ERR*	    )&err);		
}

void DITrmCallBack(void *p_tmr, void *p_arg)
{
	OS_ERR err;
	//发送信号量
	OSSemPost(&DI_Check_Sem,OS_OPT_POST_1,&err);
}

void assert_failed(u8* file, u32 line)
{
    debugprintf("fail");
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


