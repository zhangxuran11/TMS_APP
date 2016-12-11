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

#include "Slave.h"
//#include "Master.h"
#include "TestMasterSlave.h"
#include "emcy.h"


extern s_BOARD SlaveBoard;
/*****************************************************************************/
void TestSlave_heartbeatError(CO_Data* d, UNS8 heartbeatID)
{
	g_RxCAN2HeartFlag[heartbeatID] = NoRxHeart;
	eprintf("TestSlave_heartbeatError %d\r\n", heartbeatID);
}

void TestSlave_initialisation(CO_Data* d)
{
	eprintf("TestSlave_initialisation\r\n");
}

void TestSlave_preOperational(CO_Data* d)
{
	eprintf("TestSlave_preOperational\r\n");
	
}

void TestSlave_operational(CO_Data* d)
{
	eprintf("TestSlave_operational\r\n");
}

void TestSlave_stopped(CO_Data* d)
{
	eprintf("TestSlave_stopped\r\n");
}

void TestSlave_post_sync(CO_Data* d)
{
      eprintf("TestSlave_post_sync\r\n");
      
//				door_T_1 +=1;
//				door_T_2 +=1;
//				door_M_1 +=1;
//				door_M_2 +=1;
//				door_T1_1 +=1;
//				door_T1_2 +=1;
//				door_T2_1 +=1;
//				door_T2_2 +=1;
//				door_mc2_1 +=1;
//				door_mc2_2 +=1;
//				door_mc1_1 +=1;
//				door_mc1_2 +=1;
}

void TestSlave_post_TPDO(CO_Data* d)
{
		//	SlaveMap13 +=1;
//        door_mc1_1 += 1;
	eprintf("TestSlave_post_TPDO\r\n");
	
	/* send an error and recover inmediately every 12 cycles */
//	if(SlaveMap13 % 12 == 0)
//	{
//		EMCY_setError(&TestSlave_Data, 0x4200, 0x08, 0x0000);
//		EMCY_errorRecovered(&TestSlave_Data, 0x4200);
//	}
}

void TestSlave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex)
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
	eprintf("TestSlave_storeODSubIndex : %4.4x %2.2x\r\n", wIndex,  bSubindex);
}

void TestSlave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg)
{
	eprintf("Slave received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\r\n", nodeID, errCode, errReg);
}
