/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "Uart.h"
#include "os.h"


/*
中断优先级的问题。
假如现在同时有两个抢占式优先级别相同的中断发生，那么处理的顺序是谁的响应优先级高则谁优先进
入中断，还有一点需要注意的是，如果此时进入这个中断之后又来了一个抢占式优先级相同但是响应优先级
更高的中断，这时不会打断已有的中断。
*/



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;
	
	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);
	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);
	
	printf ("\n\n[Hard fault handler - all numbers in hex]\r\n");
	printf ("R0 = %x\r\n", stacked_r0);
	printf ("R1 = %x\r\n", stacked_r1);
	printf ("R2 = %x\r\n", stacked_r2);
	printf ("R3 = %x\r\n", stacked_r3);
	printf ("R12 = %x\r\n", stacked_r12);
	printf ("LR [R14] = %x subroutine call return address\r\n", stacked_lr);
	printf ("PC [R15] = %x program counter\r\n", stacked_pc);
	printf ("PSR = %x\r\n", stacked_psr);
	printf ("BFAR = %lx\r\n", (*((volatile unsigned long *)(0xE000ED38))));
	printf ("CFSR = %lx\r\n", (*((volatile unsigned long *)(0xE000ED28))));
	printf ("HFSR = %lx\r\n", (*((volatile unsigned long *)(0xE000ED2C))));
	printf ("DFSR = %lx\r\n", (*((volatile unsigned long *)(0xE000ED30))));
	printf ("AFSR = %lx\r\n", (*((volatile unsigned long *)(0xE000ED3C))));
	printf ("SCB_SHCSR = %x\r\n", SCB->SHCSR);

  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
//		 printf("HardFault_Handler\r\n");			//程序卡死在这里，等待看门狗进行复位
  }
	
//	 /* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
//	OSIntExit();
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
//		printf("MemManage_Handler\r\n");
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
//		printf("BusFault_Handler\r\n");
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
//		printf("UsageFault_Handler\r\n");
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	while(1)
	{
//		printf("SVC_Handler\r\n");
	}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{

//}


extern u8 g_RxDataFlag;
extern u8 g_FrameErrorFlag;
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/
u8 g_RxDataFlag=0;
u8 g_FrameErrorFlag=0;
/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{	
unsigned char ch;	

#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
  CPU_CRITICAL_ENTER();	//关中断
		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
	CPU_CRITICAL_EXIT();	//开中断
#endif	
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)		//接收中断
	{
		g_RxDataFlag=1;	//将开始接受数据的标志位设置为1
		
		if(USART_GetITStatus(USART3,USART_IT_PE)!= RESET || USART_GetITStatus(USART3,USART_IT_FE)!= RESET)		//这段代码将会将程序卡死到 B . 汇编代码部分
		{
			g_FrameErrorFlag=1;		//将奇偶错误 帧错误 标志设置1
				
			USART_ClearITPendingBit(USART3,USART_IT_RXNE);		//清除中断标志
			USART_ClearITPendingBit(USART3,USART_IT_PE);			//清除奇偶错误标志
		}
		
			USART_ClearITPendingBit(USART3,USART_IT_FE);			//清除帧错误标志
		/* Read one byte from the receive data register */
		ch = (USART_ReceiveData(USART3));		//读取接收到的数据
		(void)ch;		//防止警告

	}
	
	//溢出-如果发生溢出需要先读SR,再读DR寄存器则可清除不断入中断的问题[牛人说要这样]
 if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
 {
		USART_ClearFlag(USART3,USART_FLAG_ORE); //读SR其实就是清除标志
		USART_ReceiveData(USART3);    //读DR
 }
	
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntExit();    	//退出中断
#endif	
}

// /*外部线0的中断执行函数*/
//void EXTI0_IRQHandler(void)
//{
//    ExtiLine0_IRQ();  
//}
// /*外部线6的中断执行函数*/
//void EXTI9_5_IRQHandler(void)
//{
//    ExtiLine6_IRQ();
//}
//

///**
//  * @brief  This function handles CAN1 RX0 request.
//  * @param  None
//  * @retval None
//  */
//void CAN1_RX0_IRQHandler(void)
//{
//#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
//	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//  CPU_CRITICAL_ENTER();	//关中断
//		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
//	CPU_CRITICAL_EXIT();	//开中断
//#endif
//	{
//	CanRxMsg RxMessage; 
//	 u8 i;
//    if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
//    {

//        CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);                    //清除中断标志                
////     		
////				if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)	
////					return 1;		//没有接收到数据,直接退出
//			
//				CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);                     //读取数据  
//				
//				for(i=0;i<8;i++)
//					printf("CAN1 Rx CAN2 Data,Rx1buf[%d]:0x%x\r\n",i,RxMessage.Data[i]);
//			printf("\r\n");

//    }
//	}


//#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
//	OSIntExit();    	//退出中断
//#endif
//}

///**
//  * @brief  This function handles CAN2 RX0 request.
//  * @param  None
//  * @retval None
//  */
//extern void Ican_Receive_ISR(void);
//void CAN2_RX0_IRQHandler(void)
//{

////	iCANMSG g_piCANMSG;

//#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
//	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//  CPU_CRITICAL_ENTER();	//关中断
//		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
//	CPU_CRITICAL_EXIT();	//开中断
//#endif

//{		CanRxMsg RxMessage;
//		u8 i;
//	
//    if(CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
//    {
//        			   
//        CAN_ClearITPendingBit(CAN2,CAN_IT_FMP0);                        //清除中断标志   
//				
////				if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)	
////					return 1;		//没有接收到数据,直接退出
//			
//        CAN_Receive(CAN2,CAN_FIFO0,&RxMessage);                         //读取数据   
//				
//				for(i=0;i<8;i++)
//					printf("CAN2 Rx CAN1 Data,Rx2buf[%d]:0x%x\r\n",i,RxMessage.Data[i]);		
//			printf("\r\n");		
//			
//    }
//	}

//	
//#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
//	OSIntExit();    	//退出中断
//#endif	
//}



//extern void MCP2515_INT1_IRQ(void);		//执行can3的中断函数
//void EXTI9_5_IRQHandler(void)
//{
//		MCP2515_INT1_IRQ();
//}


///*SD卡的中断服务程序*/		/*SD驱动中已经写入中断服务*/
//void SDIO_IRQHandler(void)
//{
//#if SYSTEM_SUPPORT_OS  	//使用UCOS操作系统
//	CPU_SR_ALLOC();				//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
//  CPU_CRITICAL_ENTER();	//关中断
//		OSIntEnter();     	//进入中断	 中断嵌套向量表加1
//	CPU_CRITICAL_EXIT();	//开中断
//#endif
//	
//	/* Process All SDIO Interrupt Sources */
//	SD_ProcessIRQSrc();
//	
//#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
//	OSIntExit();    	//退出中断
//#endif	
//}

