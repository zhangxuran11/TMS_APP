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

#include "can2Slave.h"
#include <stdbool.h>
#include "can2MasterSlave.h"
#include "can2Slave.h"
#include "emcy.h"
#include "bsp.h"

#define ObjTimer200MS 	    0xC8

extern s_BOARD CAN2SlaveBoard;
/*****************************************************************************/
void CAN2Slave_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	eprintf("CAN2Slave_heartbeatError %d\r\n", heartbeatID);
}

void CAN2Slave_initialisation(CO_Data* d)
{
	UNS16 EventTime=0;
	UNS32 size = sizeof(UNS16); 
	
	eprintf("CAN2Slave_initialisation\r\n");
	EventTime = ObjTimer200MS;		//定时时间设置为200ms
	
	switch(ChooseBoard & 0x0F)
	{
		case 1:		//MFD1
		//			writeLocalDict( &MFDCAN2Slave_Data, /*CO_Data* d*/		//0x1A1 	 VVVF2
		//					0x1814, /*UNS16 index*/
		//					0x05, /*UNS8 subind*/ 
		//					&EventTime, /*void * pSourceData,*/ 
		//					&size, /* UNS8 * pExpectedSize*/
		//					RW);  /* UNS8 checkAccess */		
		
			break;
		
		case 2:		//MFD2
			break;
		
		case 3:		//MFD3
			break;
		
		case 4:
			break;
		default:
			printf("MFD Chooboard Error!\r\n");
			break;
	}
}

void CAN2Slave_preOperational(CO_Data* d)
{
	eprintf("CAN2Slave_preOperational\r\n");
	
}

void CAN2Slave_operational(CO_Data* d)
{
	eprintf("CAN2Slave_operational\r\n");
}

void CAN2Slave_stopped(CO_Data* d)
{
	eprintf("CAN2Slave_stopped\r\n");
}

void CAN2Slave_post_sync(CO_Data* d)
{
  eprintf("CAN2Slave_post_sync\r\n");
}

void CAN2Slave_post_TPDO(CO_Data* d)
{

	eprintf("CAN2Slave_post_TPDO\r\n");
}

void CAN2Slave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex)
{
	/*TODO : 
	 * - call getODEntry for index and subindex, 
	 * - save content to file, database, flash, nvram, ...
	 * 
	 * To ease flash organisation, index of variable to store
	 * can be established by scanning d->objdict[d->ObjdictSize]
	 * for variables to store.
	 * 
	 * */
	eprintf("CAN2Slave_storeODSubIndex : %4.4x %2.2x\r\n", wIndex,  bSubindex);
}

void CAN2Slave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{

}
