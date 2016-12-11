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
#ifdef USE_XENO
//#define eprintf(...) if(0){}
#define eprintf(...)
#elif defined USE_RTAI
#define eprintf(...)
#else
#define eprintf(...) printf (__VA_ARGS__)
#endif

#include "canfestival.h"
#

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


/*
#define CAN_FIFO_LENGTH 100

#define DECLARE_A_CAN_FIFO \
static Message FIFO[CAN_FIFO_LENGTH];\
static int FIFO_First = 0;\
static int FIFO_Last = 0;\
\
static void PutInFIFO(Message *m)\
{\
	FIFO[FIFO_Last++] = *m;\
	FIFO_Last %= CAN_FIFO_LENGTH;\
}\
\
static void GetFromFIFO(Message *m)\
{\
	*m = FIFO[FIFO_First++];\
	FIFO_First %= CAN_FIFO_LENGTH;\
}\
\
static void TransmitMessage(CO_Data* d, UNS32 id)\
{\
	Message m;\
	GetFromFIFO(&m);\
	canDispatch(d, &m);\
}
*/
