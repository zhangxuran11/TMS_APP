/**
  ******************************************************************************
  * @file    can_stm32.c
  * @author  Zhenglin R&D Driver Software Team
  * @version V1.0.0
  * @date    26/04/2015
  * @brief   This file is can_stm32 file.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdbool.h>
#include "canfestival.h"
#include "timer.h"
#include "Uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Store the last timer value to calculate the elapsed time
static TIMEVAL last_time_set = TIMEVAL_MAX;
//extern __IO uint16_t CCR1_Val ;
__IO uint16_t CCR1_Val = 20000;//49152;	//whh 若CCR1_Val=20000,则更新周期为1M/20000 =50Hz。为20ms更新一次。
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  setTimer
  * @param  value:Set time value 0x0000-0xffff
  * @retval NONE
  */
void setTimer(TIMEVAL value)
{
//	TIM_SetAutoreload(TIM4, value);
	
	/*W H H,输出比较模式*/
		uint16_t capture = 0;
		capture = TIM_GetCapture1(TIM4);
		TIM_SetCompare1(TIM4, capture + value);
}
/**
  * @brief  getElapsedTime
  * @param  NONE
	* @retval TIMEVAL:Return current timer value
  */
TIMEVAL getElapsedTime(void)
{
	uint16_t timer = TIM_GetCounter(TIM4);

	return timer > last_time_set ? timer - last_time_set : last_time_set - timer; 	
}

/**
  * @brief  TIM4_Configuration
  * @param  NONE
  * @retval NONE
  */
static void TIM4_Configuration(void)
{
		/* 时钟及分频设置 */
	{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		//定义时基初始化结构体变量
		/* Time Base configuration */
		TIM_TimeBaseStructure.TIM_Prescaler = 84- 1;	//							////定时器分频
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;			//计数模式:向上计数
		TIM_TimeBaseStructure.TIM_Period = 0xFFFF;											//设置自动重装载值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					//时钟分割，无视之！
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;		//重新计数的起始值

		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);		
		 
		/* Prescaler configuration */
		TIM_PrescalerConfig(TIM4, 84-1, TIM_PSCReloadMode_Immediate);		//W H H 
	}
	
	/*
	    TIM4 Configuration: Output Compare Timing Mode:
    
    In this example TIM4 input clock (TIM4CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM4CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM4CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM4 counter clock at 6 MHz, the prescaler is computed as follows:
       Prescaler = (TIM4CLK / TIM4 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /6 MHz) - 1
                                              
    CC1 update rate = TIM4 counter clock / CCR1_Val = 146.48 Hz
    ==> Toggling frequency = 73.24 Hz
	
	*/
	
	//	{
	//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//		uint16_t PrescalerValue = 0;
	//		
	//		  /* Compute the prescaler value */
	//		PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 6000000) - 1;

	//		/* Time base configuration */
	//		TIM_TimeBaseStructure.TIM_Period = 65535;
	//		TIM_TimeBaseStructure.TIM_Prescaler = 0;
	//		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	//		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	//		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	
	//	  /* Prescaler configuration */
	//		TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);
	//		
	//	}
	
		/*输出比较模式的配置*/
	{
		TIM_OCInitTypeDef  TIM_OCInitStructure;		//定义输出比较初始化结构体变量

		/* Output Compare Timing Mode configuration: Channel1 */
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		

		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);		//禁止TIM_CCR1寄存器的预装载使
	}
		
    /* TIM IT enable */
		TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);		//中断使能
		/* TIM enable counter */
		TIM_Cmd(TIM4, ENABLE);				//W H H		//开启定时器（即设置CEN位）,一定要使能时钟
}
/**
  * @brief  NVIC_Configuration
  * @param  NONE
  * @retval NONE
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  RCC_Configuration
  * @param  NONE
  * @retval NONE
  */
static void RCC_Configuration(void)
{
		/* TIM4 clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}
/**
  * @brief  TIM4_start
  * @param  NONE
  * @retval NONE
  */
void TIM4_start(void)
{
		RCC_Configuration();

		/* configure TIM4 for remote and encoder */
		NVIC_Configuration();
		TIM4_Configuration();
}

/*关闭定时器*/
void TIM4_stop(void)	
{
	TIM_Cmd(TIM4, DISABLE);			
}

/**
  * @brief  initTimer
  * @param  NONE
  * @retval NONE
  */
void initTimer(void)
{
		TIM4_start();
}
/**
  * @brief  TIM4_IRQHandler
  * @param  NONE
  * @retval NONE
  */
void TIM4_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
  CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
	CPU_CRITICAL_EXIT();	//开中断
#endif	
	
	if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)		//捕获/比较中断
	{	
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);		//清除中断标志位
			last_time_set = TIM_GetCounter(TIM4);
			TimeDispatch();		
	}

#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntExit();    	//退出中断
#endif	
}
/******************* (C) COPYRIGHT 2015 Personal Electronics *****END OF FILE****/
