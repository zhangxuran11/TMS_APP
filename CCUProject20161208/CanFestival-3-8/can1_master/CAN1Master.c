/*
This file is part of CanFestival, a library implementing CanOpen Stack. 

Copyright (C): Edouard TISSERANT and Francis DUPIN

See COPYING file for copyrights details.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <stm32f4xx.h>
#include <stdbool.h>
#include "CAN1Master.h"
#include "CAN1MasterSlave.h"
#include "Uart.h"
#include "bsp_os.h"


extern CO_Data CCUCAN2Master_Data;
extern UNS8 sendEMCY(CO_Data* d, UNS16 errCode, UNS8 errRegister, const void *Specific, UNS8 SpecificLength);

extern s_BOARD CAN1MasterBoard;

void Switch_LED_Display(void)				//这个函数在发送紧急信号的时候进行调用
{
	if(CurrentUseCAN1 == UseState)		//当前使用的can1总线，切换到can2总线
	{
		CurrentUseCAN1 = NoUseState;
		CurrentUseCAN2 = UseState;
	}
	else
	{
		CurrentUseCAN1 = UseState;
		CurrentUseCAN2 = NoUseState;		
	}
}


/*
注意: CCU1 CCU2设备无法接受到本设备 (自己) 心跳，肯定会在进入心跳错误回调函数。这里应该加入判断 ，注意注意注意！！！
*/

/*****************************************************************************/
void CAN1Master_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	/*变量定义*/
	//CPU_SR_ALLOC();																	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
	
	EmcyMsgState CAN1EmcyState = NoEmcy;		//使用局部变量，不需要使用全局变量来标识
	UNS8	i, j, k;												 //i 表示循环变量，j用于存储can1有几个从设备没有心跳，k用于表示can2有几个从设备没有心跳
	UNS16 RecordErrorCoode;

		g_RxCAN1HeartFlag[heartbeatID] = NoRxHeart;	
		switch(heartbeatID)
		{
			case CCU1_NODEID:
					if(ChooseBoard == CCU2_NODEID)
					{
						CAN1EmcyState	= MainControlFaultSwitch;
						eprintf("CAN1Master_heartbeatError %d\r\n", heartbeatID);
					}
					break;
		
			case CCU2_NODEID:
					if(ChooseBoard == CCU1_NODEID)
					{
						CAN1EmcyState	= MainControlFaultSwitch;	//主控故障可切换
						eprintf("CAN1Master_heartbeatError %d\r\n", heartbeatID);
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
		
//			case ERM1_NODEID:

//			case ERM2_NODEID:
					CAN1EmcyState	= OnePointFaultSwitch;		//单点故障可切换，
					eprintf("CAN1Master_heartbeatError %d\r\n", heartbeatID);
					break;
			default :
					CAN1EmcyState =NoEmcy;
				
		}
		
//		OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
		
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
										debugprintf("ccu1 can2 发送 errCode=0x11xx (副控)单点故障，可切换\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN1EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x1100  | heartbeatID);
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送									
								}
								
								return ;				//直接返回
							}
														
							//表示can2也没有接收到副控心跳，就发送副控坏不可切换，这样总线还能使用
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{
								#ifdef DEBUG_PRINTF	
									debugprintf("ccu1 can2 发送 errCode=0x40xx  副控2坏，不可切换 \r\n");
								#endif 
							
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x4000 |	heartbeatID;
								sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里必须通过can1口发送	

								return ;
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
									debugprintf("ccu1 can2 发送 errCode=0x11xx  单点故障，可切换\r\n");
								#endif 
								
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x1100 | heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;
							}
							
							//errCode=0x10xx  单点故障，不可切换 
							else if((j==1) && (k>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 发送 errCode=0x10xx  单点故障，不可切换\r\n");
								#endif 
								
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x1000 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;					
							}
							
							//errCode=0x21xx 主干线断故障，可切换
							else if((j>1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 发送 errCode=0x21xx  主干线断故障，可切换\r\n");
								#endif
			
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x2100 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;						
							}
							
							//errCode=0x20xx 主干线断故障，不可切换
							else if((j>1) && (k>0) )
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu1 can2 发送 0x20xx 主干线断故障，不可切换\r\n");
								#endif
								
								CAN1EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x2000 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;						
							}
						
						}
				}
			
			}
			
			
			else		//ccu1不是主控，去监控ccu2 设备的心跳
			{
				//1.如果主控故障,这里有ccu1 监控ccu2的心跳
				//当前故障为主控故障，并且can2线也接受不到主控ccu2的心跳
				if((CAN1EmcyState == MainControlFaultSwitch) && (g_RxCAN2HeartFlag[heartbeatID] == NoRxHeart))
				{
					j=0;		//记录can1口的无心跳设备的个数
					k=0;		//记录can2口的无心跳设备的个数
					for(i=3;i<10/*12*/;i++)
					{
						if(g_RxCAN1HeartFlag[i]	==  NoRxHeart ) /*&&( g_RxCAN1HeartStateFlag[i] == InOperatState)*/		//计算是否能接受到其他设备的心跳
						{
							j++;  
						}
						
						if(g_RxCAN2HeartFlag[i] == NoRxHeart) /*&& (g_RxCAN2HeartStateFlag[i] == InOperatState)*/
						{
							k++;
						}
					}
					
					//errCode=0x30xx 主控2坏，不可切换
					if((j!=0 )&&(k!=0))
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu1 is not main 0x30xx 主控2坏，不可切换\r\n");
						#endif 
						
						CAN1EmcyState = 	MainControlFaultNotSwitch;
						RecordErrorCoode = 0x6000 |heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						RecordErrorCoode = 0x3000 |	heartbeatID;					
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						return ;
					}
					
					//errCode=0x30xx主控2坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu1 is not main can1 0x60xx 主控2坏，可切换\r\n");
						#endif 
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						return ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu1 is not main can1 0x30xx 主控2坏，可切换\r\n");
						#endif 
						
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						return ;
					}
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu1 is not main can1 0x60xx 主控2坏，可切换\r\n");
						#endif
						CCU1SwitchState = ChooseOperate;
						CCU2SwitchState = ChooseNotOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
					
						//	RecordErrorCoode = 0x3100 |	heartbeatID;
						//	sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						return ;
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
										debugprintf("ccu2 can2 发送 errCode=0x11xx (副控)单点故障，可切换\r\n");
									#endif 
									
									Switch_LED_Display();  //切换led显示
									CAN1EmcyState = OnePointFaultSwitch;
									RecordErrorCoode = (0x1100  | heartbeatID);
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送									
								}
								return ;				//直接返回
							}
														
							//表示can2也没有接收到副控心跳，就发送副控坏不可切换，这样总线还能使用
							/*不做处理，副控坏掉，不能影响到网络通信，故不切换。(或者可以发送一个单点故障不可切换指令，或者报警)*/
							else
							{							
									#ifdef DEBUG_PRINTF	
										debugprintf("ccu1 can2 发送 errCode=0x10xx  副控2坏，不可切换 \r\n");
									#endif 
								
									CAN1EmcyState = 	OnePointFaultNotSwitch;
									RecordErrorCoode = 0x1000 |	heartbeatID;
									sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送								
									
									return ;
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
									debugprintf("ccu2 can2 发送 0x11xx 可切换\r\n");
								#endif
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	OnePointFaultSwitch;
								RecordErrorCoode = 0x1100 | heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;
							}
							//errCode=0x10xx  单点故障，不可切换 
							else if((j==1) && (k>1))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 发送 0x10xx 不可切换\r\n");
								#endif
								
								CAN1EmcyState = 	OnePointFaultNotSwitch;
								RecordErrorCoode = 0x1000 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;					
							}
							
							//errCode=0x21xx 主干线断故障，可切换
							else if((j>1) && (k==0))
							{
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 发送 0x21xx 可切换\r\n");
								#endif 
								
								Switch_LED_Display();  //切换led显示
								CAN1EmcyState = 	MainLineFaultSwitch;
								RecordErrorCoode = 0x2100 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;						
							}
							//errCode=0x20xx 主干线断故障，不可切换
							else if((j>1) && (k>0) )
							{	
								#ifdef DEBUG_PRINTF
									debugprintf("ccu2 can2 发送 0x20xx 可切换\r\n");
								#endif
								
								CAN1EmcyState = 	MainControlFaultNotSwitch;
								RecordErrorCoode = 0x2000 |	heartbeatID;
								sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里必须通过can2口发送
								return ;						
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
							debugprintf("ccu2 is not main can1 0x31xx 主控1 不可切换\r\n");
						#endif 
						
						CAN1EmcyState = 	MainControlFaultNotSwitch;
						RecordErrorCoode = 0x6000 |heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送						
						
						RecordErrorCoode = 0x3000 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送						                                                                                                                 
						return ;
					}
					
					//errCode=0x30xx主控1坏，可切换
					else if ((0==j) && (k!=0) )  //can1线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu2 is not main can1 x60xx 主控1坏，可切换\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						return ;
					}
					else if((0!=j) && (0==k))	//can2线非常好
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu2 is not main can1 x30xx 主控1坏，可切换\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x3100 |	heartbeatID;
						sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送
						
						//RecordErrorCoode = 0x6100 |	heartbeatID;
						//sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
					}
					//errCode=0x30xx主控1坏，可切换
					else		//两路can都非常好 默认就使用can1发送
					{
						#ifdef DEBUG_PRINTF
							debugprintf("ccu2 is not main can1 0x60xx主1 可切换\r\n");
						#endif 
						
						CCU1SwitchState = ChooseNotOperate;
						CCU2SwitchState = ChooseOperate;
						CAN1EmcyState = 	MainControlFaultSwitch;
						RecordErrorCoode = 0x6100 |	heartbeatID;
						sendEMCY(&CCUCAN1Master_Data,RecordErrorCoode,*(CCUCAN1Master_Data.error_register),NULL,0);		//这里通过can1口发送
						
						//RecordErrorCoode = 0x3100 |	heartbeatID;
						//sendEMCY(&CCUCAN2Master_Data,RecordErrorCoode,*(CCUCAN2Master_Data.error_register),NULL,0);		//这里通过can2口发送					
						return ;
					}
				}
			}
		}
		
//		OS_CRITICAL_EXIT();	//退出临界区
//	}
}
/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup master RPDO 1 to receive TPDO 1 from id 2
 *  - setup master RPDO 2 to receive TPDO 2 from id 2
 ********************************************************/
void CAN1Master_initialisation(CO_Data* d)
{
	eprintf("CAN1Master_initialisation\r\n");
		
			/*W H H 在主站上电初始化的时候，将获取从站得心跳状态标识，全部置于初始*/
//			{						//在定义全局变量的时候已经初始化了，故不需要在初始化，节省时间。
//				u8 i;

//				for(i=0;i<sizeof(g_ReceiveHeartStateFlag);i++)
//				{	
//					printf("g_re=%d\r\n",sizeof(g_ReceiveHeartStateFlag));
//					g_ReceiveHeartStateFlag[i] = InPowerInitState; 		
//					g_RxCAN1MesStateFlag[i] = NoRxCanMesState;					
//				}
//			}
	
		
}

// Step counts number of times ConfigureSlaveNode is called
static int init_step = 0;

/*Froward declaration*/
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId);

///**/
//static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
//{
//	UNS32 abortCode;	
//	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
//		eprintf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4lx \r\n", nodeId, init_step, abortCode);

//	/* Finalise last SDO transfer with this node */
//	closeSDOtransfer(&CCUCAN1Master_Data, nodeId, SDO_CLIENT);

//	ConfigureSlaveNode(d, nodeId);
//}

/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup slave TPDO 1 transmit type
 *  - setup slave TPDO 2 transmit type
 *  - switch to operational mode
 *  - send NMT to slave
 ********************************************************
 * This an example of :
 * Network Dictionary Access (SDO) with Callback 
 * Slave node state change request (NMT) 
 ********************************************************
 * This is called first by CAN1Master_post_SlaveBootup
 * then it called again each time a SDO exchange is
 * finished.
 ********************************************************/
 
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	/* Master configure heartbeat producer time at 1000 ms 
	 * for slave node-id 0x02 by DCF concise */
	 
//	UNS8 Transmission_Type = 0x01;
//	UNS8 res;
//	eprintf("Master : ConfigureSlaveNode %2.2x\r\n", nodeId);

//	switch(++init_step){
//		case 1: /*First step : setup Slave's TPDO 1 to be transmitted on SYNC*/
//			eprintf("Master : set slave %2.2x TPDO 1 transmit type\r\n", nodeId);
//			res = writeNetworkDictCallBack (d, /*CO_Data* d*/
//					nodeId, /*UNS8 nodeId*/
//					0x1800, /*UNS16 index*/
//					0x02, /*UNS8 subindex*/
//					1, /*UNS8 count*/
//					0, /*UNS8 dataType*/
//					&Transmission_Type,/*void *data*/
//					CheckSDOAndContinue, /*SDOCallback_t Callback*/
//                    0); /* use block mode */
//					break;
//		
//		case 2:	/*Second step*/
//			eprintf("Master : set slave %2.2x TPDO 2 transmit type\r\n", nodeId);
//			writeNetworkDictCallBack (d, /*CO_Data* d*/
//					nodeId, /*UNS8 nodeId*/
//					0x1801, /*UNS16 index*/
//					0x02, /*UNS16 index*/
//					1, /*UNS8 count*/
//					0, /*UNS8 dataType*/
//					&Transmission_Type,/*void *data*/
//					CheckSDOAndContinue, /*SDOCallback_t Callback*/
//                    0); /* use block mode */
//					break;
//		case 1: 
		
		/****************************** START *******************************/
		
//			g_RxCAN1HeartStateFlag[nodeId] = InOperatState;					//W H H	，标记接受心跳的状态在从站为操作状态。
		
			/* Put the master in operational mode */
			setState(d, Operational);
		 
			/* Ask slave node to go in operational mode */
			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);

//	}
}


void CAN1Master_preOperational(CO_Data* d)
{
	eprintf("CAN1Master_preOperational\r\n");
}

void CAN1Master_operational(CO_Data* d)
{
	eprintf("CAN1Master_operational\r\n");
}

void CAN1Master_stopped(CO_Data* d)
{
	eprintf("CAN1Master_stopped\r\n");
}

void CAN1Master_post_sync(CO_Data* d)
{
	
	eprintf("CAN1Master_post_sync\r\n");
}

void CAN1Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
	UNS8 	CodeBitHig,CodeBitLow;
	u8 CanChangeFlag = false;

	CodeBitHig = (errCode & 0xF000)>>12;
	CodeBitLow = (UNS8)((errCode & 0x0F00)>>8);

	eprintf("Master received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\r\n", nodeID, errCode, errReg);
	
	/*whh 之前方法*/
		//	if(CodeBitHig == 1 || CodeBitHig == 2 || CodeBitHig == 4 || CodeBitHig == 5 /*|| CodeBitHig == 6  || CodeBitHig == 3 */ )
		//	{
		//		if(0 == CodeBitLow)
		//		{
		//			CanChangeFlag = false;	
		//		}
		//		else
		//		{
		//			CanChangeFlag = true;
		//		}
		//	}

	/*whh can2口发送个紧急报文，则需要切换到can2口*/
	if(CodeBitHig == 1 || CodeBitHig == 2 )    //表示can2口发送的紧急报文，要切换到can2线
	{
		if((1 == CodeBitLow) && (CurrentUseCAN1 == UseState))		//当前使用的can1总线，切换到can2总线
		{
			CurrentUseCAN1 = NoUseState;
			CurrentUseCAN2 = UseState;
		}
	}

	/*whh can1口发送个紧急报文，则需要切换到can1口*/
	else if(CodeBitHig == 4 || CodeBitHig == 5 ) //表示can1口发送的紧急报文，要切换到can1线
	{
		if((1 == CodeBitLow) && (CurrentUseCAN2 == UseState))		//当前使用的can2总线，切换到can1总线
		{
			CurrentUseCAN1 = UseState;
			CurrentUseCAN2 = NoUseState;		
		}		
	}
	
	else
	{
		if(1 == CodeBitLow)		//主控切换
		{
			if(CCU1SwitchState == ChooseOperate)
			{
				CCU1SwitchState = ChooseNotOperate;
				CCU2SwitchState = ChooseOperate;
			}
			else
			{
				CCU1SwitchState = ChooseOperate;
				CCU2SwitchState = ChooseNotOperate;				
			}
		}
		else
		{
			//CanChangeFlag = false;    //主控坏，不可切换，can1,can2网络暂时也不切换。
		}
	}
	
	
	//	if(CanChangeFlag)
	//	{
	//		if(CurrentUseCAN1 == UseState)		//当前使用的can1总线，切换到can2总线
	//		{
	//			CurrentUseCAN1 = NoUseState;
	//			CurrentUseCAN2 = UseState;
	//		}
	//		else
	//		{
	//			CurrentUseCAN1 = UseState;
	//			CurrentUseCAN2 = NoUseState;		
	//		}
	//	}
	
}

//char query_result = 0;
//char waiting_answer = 0;

//static void CheckSDO(CO_Data* d, UNS8 nodeId)
//{
//	UNS32 abortCode;	
//	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
//		eprintf("Master : Failed in changing Slave's transmit type AbortCode :%4.4lx \r\n", abortCode);

//	/* Finalise last SDO transfer with this node */
//	closeSDOtransfer(&CCUCAN1Master_Data, nodeId, SDO_CLIENT);
//}


static int MasterSyncCount = 0;
void CAN1Master_post_TPDO(CO_Data* d)
{
	eprintf("CAN1Master_post_TPDO MasterSyncCount = %d \r\n", MasterSyncCount);
	MasterSyncCount++;
}

void CAN1Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
	if(CCU1 ==1 )						// 表示CCU1设备,的can1通道
	{
		if(CCU1SwitchState == ChooseOperate)	//表示使用CCU1
		{
			eprintf("CCU1 CAN1Master_post_SlaveBootup %x\r\n", nodeid);
		
			ConfigureSlaveNode(d, nodeid);
		}
		else
		{
			setState(d, Operational);  //ccu1将自己启动为操作状态
			eprintf("CCU1 CAN1 Prot is Not Opreate CAN1 BUS !\r\n");
		}
	}
	
	else		//表示使用ccu2,的can1通道
	{
		if(CCU2SwitchState == ChooseOperate) 	//表示使用CCU2
		{
			eprintf("CCU2 CAN1Master_post_SlaveBootup %x\r\n", nodeid);
		
			ConfigureSlaveNode(d, nodeid);
		}
		else
		{
			setState(d, Operational);  //ccu2将自己启动为操作状态
			eprintf("CCU2 CAN1 Prot is Not Opreate CAN1 BUS !\r\n");
		}
	}
}
