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
#include "CAN3Master.h"
#include "CAN3MasterSlave.h"

extern s_BOARD CAN3MasterBoard;
/*****************************************************************************/
void CAN3Master_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	eprintf("CCULed Heart Error!\r\n");
	eprintf("CAN3Master_heartbeatError %d\r\n", heartbeatID);
}

/********************************************************
 * ConfigureSlaveNode is responsible to
 *  - setup master RPDO 1 to receive TPDO 1 from id 2
 *  - setup master RPDO 2 to receive TPDO 2 from id 2
 ********************************************************/
void CAN3Master_initialisation(CO_Data* d)
{
//	UNS32 PDO1_COBID = 0x0182; 
//	UNS32 PDO2_COBID = 0x0282;
//	UNS32 size = sizeof(UNS32); 
//	UNS32 SINC_cicle=0;
//	UNS8 data_type = 0;
	
	eprintf("CAN3Master_initialisation\r\n");

	/*****************************************
	 * Define RPDOs to match slave ID=2 TPDOs*
	 *****************************************/
//	writeLocalDict( &CCUCAN3Master_Data, /*CO_Data* d*/
//			0x1400, /*UNS16 index*/
//			0x01, /*UNS8 subind*/ 
//			&PDO1_COBID, /*void * pSourceData,*/ 
//			&size, /* UNS8 * pExpectedSize*/
//			RW);  /* UNS8 checkAccess */
//			
//	writeLocalDict( &CCUCAN3Master_Data, /*CO_Data* d*/
//			0x1401, /*UNS16 index*/
//			0x01, /*UNS8 subind*/ 
//			&PDO2_COBID, /*void * pSourceData,*/ 
//			&size, /* UNS8 * pExpectedSize*/
//			RW);  /* UNS8 checkAccess */
					
			
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

/**/
static void CheckSDOAndContinue(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		eprintf("Master : Failed in initializing slave %2.2x, step %d, AbortCode :%4.4x \r\n", nodeId, init_step, abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&CCUCAN3Master_Data, nodeId, SDO_CLIENT);
	ConfigureSlaveNode(d, nodeId);
}

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
 * This is called first by CAN3Master_post_SlaveBootup
 * then it called again each time a SDO exchange is
 * finished.
 ********************************************************/
 
static void ConfigureSlaveNode(CO_Data* d, UNS8 nodeId)
{
	/* Master configure heartbeat producer time at 1000 ms 
	 * for slave node-id 0x02 by DCF concise */
//	 
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
//		
		/****************************** START *******************************/
		
			/* Put the master in operational mode */
			setState(d, Operational); 
			/* Ask slave node to go in operational mode */
			masterSendNMTstateChange (d, nodeId, NMT_Start_Node);
			
//	}
}


void CAN3Master_preOperational(CO_Data* d)
{
	eprintf("CAN3Master_preOperational\r\n");
}

void CAN3Master_operational(CO_Data* d)
{
	eprintf("CAN3Master_operational\r\n");
}

void CAN3Master_stopped(CO_Data* d)
{
	eprintf("CAN3Master_stopped\r\n");
}

void CAN3Master_post_sync(CO_Data* d)
{
	
	eprintf("CAN3Master_post_sync\r\n");
}

void CAN3Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
	eprintf("Master received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\r\n", nodeID, errCode, errReg);
}

static void CheckSDO(CO_Data* d, UNS8 nodeId)
{
	UNS32 abortCode;	
	if(getWriteResultNetworkDict (d, nodeId, &abortCode) != SDO_FINISHED)
		eprintf("Master : Failed in changing Slave's transmit type AbortCode :%4.4x \r\n", abortCode);

	/* Finalise last SDO transfer with this node */
	closeSDOtransfer(&CCUCAN3Master_Data, nodeId, SDO_CLIENT);
}


static int MasterSyncCount = 0;
void CAN3Master_post_TPDO(CO_Data* d)
{
	eprintf("CAN3Master_post_TPDO MasterSyncCount = %d \r\n", MasterSyncCount);

	MasterSyncCount++;
}

void CAN3Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid)
{
	eprintf("CAN3Master_post_SlaveBootup %x\r\n", nodeid);
	
	ConfigureSlaveNode(d, nodeid);
}
