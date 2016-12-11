#include "MTD_CPU_CapturePwm.h"
#include "Uart.h"

//定时器5通道1输入捕获pwm2配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void MTDCPU_PWM2IN_TIM5_CH1_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  		//TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能GPIOA时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 					//GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉  					
	GPIO_Init(GPIOA,&GPIO_InitStructure); 							//初始化PA0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //PA0复用位定时器5的通道1
  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  								//定时器分频   //设置用来作为TIMx时钟频率除数的预分频值  
	TIM_TimeBaseStructure.TIM_Period=arr;   									//自动重装载值  //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	
	//初始化TIM5输入捕获参数
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;							 		//CC1S=01 	选择输入端 IC1映射到TI1上
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//上升沿捕获
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //映射到TI1上
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;					  //配置输入分频,不分频 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;													//IC1F=0000 配置输入滤波器 不滤波

	TIM_PWMIConfig(TIM5, &TIM5_ICInitStructure);     //PWM输入配置           

  /* Select the TIM5 Input Trigger: TI1FP1 */
	TIM_SelectInputTrigger(TIM5, TIM_TS_TI1FP1);     //选择有效输入端     选择IC1为始终触发源    

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM5, TIM_SlaveMode_Reset);  //配置为主从复位模式
  TIM_SelectMasterSlaveMode(TIM5, TIM_MasterSlaveMode_Enable);                                       	
		
		
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);		//允许更新中断 ,允许CC1IE捕获中断	
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update|TIM_IT_CC1); //清除中断标志位
  TIM_Cmd(TIM5,ENABLE ); 	//使能定时器5
 
	/*配置中断优先级*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;				//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
}

void 	MTDCPU_PWM1IN_TIM2_CH2_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  		//TIM2时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能GPIOA时钟	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 					//GPIOA1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉  				
	GPIO_Init(GPIOA,&GPIO_InitStructure); 							//初始化PA1

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2); 		//PA1复用位定时器2的通道2
  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  								//定时器分频   //设置用来作为TIMx时钟频率除数的预分频值  
	TIM_TimeBaseStructure.TIM_Period=arr;   									//自动重装载值  //设置在下一个更新事件装入活动的自动重装载寄存器周期的值  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2;							 		//CC1S=01 	选择输入端 IC1映射到TI2上
  TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;			//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  //映射到TI2上
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;					  //配置输入分频,不分频 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;													//IC1F=0000 配置输入滤波器 不滤波

	TIM_PWMIConfig(TIM2, &TIM2_ICInitStructure);     //PWM输入配置           

  /* Select the TIM2 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM2, TIM_TS_TI2FP2);     //选择有效输入端     选择IC2为始终触发源    

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);  //配置为主从复位模式
  TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);                                       	
		
		
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC2,ENABLE);	 	 //允许更新中断 ,允许CC2IE捕获中断	
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update|TIM_IT_CC2); //清除中断标志位
	
  TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
 
	/*配置中断优先级*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;				//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器、
}


/*
连续的两个上升沿或下降,它等于包含TIM时钟周期的个数(捕获寄存器中捕获的为TIM的计数个数n)

高电平的周期或低电平的周期,边沿信号的计数个数m

    frequency=f（TIM时钟频率）/n。
    duty cycle=（高电平计数个数/n），

*/
__IO uint16_t TIM5IC1Value = 0;		////选择IC1为始终触发源，故ic1为获得的频率。
__IO uint16_t PWM2DutyCycle = 0;
__IO uint32_t PWM2Frequency = 0;

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif
	
//	RCC_ClocksTypeDef RCC_Clocks;
//	RCC_GetClocksFreq(&RCC_Clocks);  
		
	if(TIM_GetITStatus(TIM5,TIM_IT_CC1) !=RESET )//捕获1发生捕获事件
	{
//		/* Clear TIM5 Capture compare interrupt pending bit */
//		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);		//清除中断
		
		/* Get the Input Capture value */
		TIM5IC1Value = TIM_GetCapture1(TIM5);		//得到n的值	 //读取IC1捕获寄存器的值，即为PWM周期的计数值		
		
		if(TIM5IC1Value !=0)
		{
			/* Duty cycle computation */
			PWM2DutyCycle = (TIM_GetCapture2(TIM5) * 100) / TIM5IC1Value;		//	读取IC2捕获寄存器的值，并计算占空比
			
			/* Frequency computation 
		 TIM5 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */

		//	PWM2Frequency =  /*(RCC_Clocks.HCLK_Frequency)/2 /*/TIM5IC1Value;  //时钟的频率看arr,prc ,故频率	PWM2Frequency = TIMFre / TIM5IC1Value;   这里没有写定时器频率，因为在初始化的时候才会分频
			PWM2Frequency =  TIM5IC1Value;
		}
		
		else
		{
			PWM2DutyCycle = 0;
			PWM2Frequency = 0;
		}
	}
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
		
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    	//退出中断
	#endif
}



__IO uint16_t TIM2IC1Value = 0;		////选择IC1为始终触发源，故ic1为获得的频率。
__IO uint16_t PWM1DutyCycle = 0;
__IO uint32_t PWM1Frequency = 0;

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM2CH2_CAPTURE_STA=0;	//输入捕获状态		    				
u32	TIM2CH2_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
//定时器5中断服务程序	 
void TIM2_IRQHandler(void)
{ 	
	#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
		CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
		CPU_CRITICAL_ENTER();	//关中断
			OSIntEnter();     	//进入中断	 中断嵌套向量表加1
		CPU_CRITICAL_EXIT();	//开中断
	#endif
	
//	RCC_ClocksTypeDef RCC_Clocks;
//	RCC_GetClocksFreq(&RCC_Clocks); 
	if(TIM_GetITStatus(TIM2,TIM_IT_CC2) !=RESET )//捕获1发生捕获事件
	{
//		/* Clear TIM2 Capture compare interrupt pending bit */
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);		//清除中断
		
		/* Get the Input Capture value */
		TIM2IC1Value = TIM_GetCapture2(TIM2);		//得到n的值	 //读取IC2捕获寄存器的值，即为PWM周期的计数值		
		
		if(TIM2IC1Value !=0)
		{
			/* Duty cycle computation */
			PWM1DutyCycle = (TIM_GetCapture1(TIM2) * 100) / TIM2IC1Value;		//	读取IC2捕获寄存器的值，并计算占空比
			
			/* Frequency computation 
		 TIM2 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */

			//PWM1Frequency = /*(RCC_Clocks.HCLK_Frequency)/2 /*/ TIM2IC1Value;  //时钟的频率看arr,prc ,故频率	PWM2Frequency = TIMFre / TIM5IC1Value;   这里没有写定时器频率，因为在初始化的时候才会分频
			PWM1Frequency =  TIM2IC1Value; 
		}
		
		else
		{
			PWM1DutyCycle = 0;
			PWM1Frequency = 0;
		}
	}
	
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC2|TIM_IT_Update); //清除中断标志位
	
	#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
		OSIntExit();    	//退出中断
	#endif	
}



void PWM_Test_GPIO(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}
