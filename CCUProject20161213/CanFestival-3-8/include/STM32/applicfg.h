/*
This file is part of CanFestival, a library implementing CanOpen Stack.

Copyright (C): Edouard TISSERANT and Francis DUPIN
AT91 Port: Peter CHRISTEN

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

#ifndef __APPLICFG_AVR__
#define __APPLICFG_AVR__

#include <string.h>
#include <stdio.h>

// Integers
#define INTEGER8 	signed char
#define INTEGER16 signed short
#define INTEGER24 signed long
#define INTEGER32 signed long
#define INTEGER40 signed long long
#define INTEGER48 signed long long
#define INTEGER56 signed long long
#define INTEGER64 signed long long

// Unsigned integers
#define UNS8   unsigned char
#define UNS16  unsigned short
#define UNS32  unsigned long
#define UNS24  unsigned long
#define UNS40  unsigned long long
#define UNS48  unsigned long long
#define UNS56  unsigned long long
#define UNS64  unsigned long long

// Reals
#define REAL32 float
#define REAL64 double

// Reals
#define REAL32 	float
#define REAL64 double

#include "can.h"

//W H H 利用状态机，来判断是否接受到心跳。
/*canfestival 接受心跳报文的定义*/

#define CCU1_NODEID  				1		//总线控制器1：节点号 1
#define CCU2_NODEID	 				2		//总线控制器2：节点号 2	
#define MC1_MTD_NODEID	 		3		//MC1车监控装置：节点号 3
#define T_MTD_NODEID   	 		4		//T车监控装置：节点号4
#define M_MTD_NODEID		 		5		//M车监控装置： 节点号5
#define T1_MTD_NODEID				6		//T1车监控装置：节点号6
#define T2_MTD_NODEID   		7		//T2车监控装置：节点号7
#define MC2_MTD_NODEID   		8		//MC2车监控装置：节点号8
#define DDU1_NODEID					9   //监控显示器1：节点号9   (司机显示单元)
#define DDU2_NODEID   			10	//监控显示器2：节点号10
#define ERM1_NODEID					11	//事件记录仪1: 节点号11
#define ERM2_NODEID					12  //事件记录仪2：节点号12


typedef  enum  { 
	NoRxHeart = 0,
	RxHeart = 1, 
}IsNoReceHeart;					//表示当前接受心跳所在的状态

typedef  enum  { 
	InPowerInitState = 0,
	InOperatState = 1, 
}CurReceHeartState;			//表示当前接受心跳所在的状态

typedef enum{
	NoRxCanMesState = 0,
	RxCanMesState =1,
	RxCanNodeGuardMesState =2,
	RxCanHeartMesState =3,
}RxCanMessageState;		//表示当前接受的can消息所在的状态


typedef enum{ 						//用来表示can1 口和can2口进入的状态，进行切换，can1与can2的状态 一定要互斥
	ChooseOperate=1,
	ChooseNotOperate=2,
}CCUCANState;

typedef enum{
	NoUseState=0,
	UseState =1,
}CurrentUserState;

typedef enum{
	NoEmcy=0,											//没有紧急消息
	OnePointFaultNotSwitch =1, 		//单点故障不可切换
	OnePointFaultSwitch=2,				//单点故障可切换
	MainLineFaultNotSwitch =3,		//主干线故障不可切换
	MainLineFaultSwitch =4,	  		//主干线故障可切换
	MainControlFaultNotSwitch=5, //主控1坏，不可切换
	MainControlFaultSwitch=6, 	 //主控1坏，可切换
}EmcyMsgState;

extern CCUCANState   CCU1SwitchState;		//CCU1 工作还是CCU2工作		,表示设备
extern CCUCANState   CCU2SwitchState;

extern CurrentUserState CurrentUseCAN1;	//表示当前使用can1，还是can2网络，表示通道
extern CurrentUserState CurrentUseCAN2;	

extern UNS8 CAN1_Heart;									//表示ccu1 或者ccu2 的can1接收到其他设备的心跳，表示自己能正常通信，can1口的心跳
extern UNS8 CAN2_Heart;			

extern  UNS8  ChooseBoard;			//这个全局变量用来接受旋钮的值，从而选择板子
#define CCU1  ChooseBoard				//表示为ccu1 
#define CCU2  ChooseBoard				//表示为ccu2


extern IsNoReceHeart  g_RxCAN1HeartFlag[15];								  //存放各个设备的心跳的标志，下标就是节点的ID号，故下标为0的数组没有使用。
extern IsNoReceHeart	g_RxCAN2HeartFlag[15];

//extern CurReceHeartState  g_RxCAN1HeartStateFlag[15];					//存放当前接受心跳时，从设备所在的状态，是上电刚初始化的时候，还是建立的连接的时候	
//extern CurReceHeartState  g_RxCAN2HeartStateFlag[15];

extern RxCanMessageState  g_RxCAN1MesStateFlag[15];						//存放当前主站接到从站的can消息 所处的状态 
extern RxCanMessageState	g_RxCAN2MesStateFlag[15];

//extern EmcyMsgState 	CAN1EmcyState;		//当can2总线故障，有can1总线发送紧急消息
//extern EmcyMsgState 	CAN2EmcyState;		//当can1总线故障，有can2总线发送紧急消息					


#define CANFESTIVAL_DEBUG_MSG(num, str, val) 

//#define CANFESTIVAL_DEBUG_MSG(num, str, val) \
//  {unsigned long value = val;\
//	 printf("  %s 0x%lX\r\n", str, value); \
//	}
  // printf(" 0x%X %s 0x%lX\r\n",num, str, value); \



/*
#define CANFESTIVAL_DEBUG_MSG(num, str, val)\
  {unsigned long value = val;\
   printf("%s(%d) : 0x%X %s 0x%lX\r\n",__FILE__, __LINE__,num, str, value); \
  }
*/

/* Definition of MSG_ERR */
#define MSG_ERR(num, str, val) CANFESTIVAL_DEBUG_MSG(num, str, val)

/* Definition of MSG_WAR */
// ---------------------
#define MSG_WAR(num, str, val) CANFESTIVAL_DEBUG_MSG(num, str, val)

typedef void* CAN_HANDLE;

typedef void* CAN_PORT;

#endif


