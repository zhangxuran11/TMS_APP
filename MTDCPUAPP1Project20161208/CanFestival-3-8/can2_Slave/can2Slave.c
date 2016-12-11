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
	switch(ChooseBoard)
	{
			case MC1_MTD_NODEID:
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x183 	 DI
//						0x1800, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//			
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x18C		 MC1_Door
//						0x1804, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1F2		 MC1_Door
//						0x1843, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */				

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x192		 MC1_ACC
//						0x1807, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1E6		 MC1_ACC
//						0x1840, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
////	EventTime = 0xF8;					
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AA		 ATC
//						0x1813, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AB		 ATC
//						0x1814, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AC		 ATC
//						0x1815, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AD		 ATC
//						0x1816, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x198		 BCU
//						0x180A, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x199		 BCU
//						0x180B, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x19A		 BCU
//						0x180C, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			
//	
//		EventTime = 0x12C;						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B6		 MC1_VVVF1
//						0x181F, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B7		 MC1_VVVF1
//						0x1820, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B8		 MC1_VVVF1
//						0x1821, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B9		 MC1_VVVF1
//						0x1822, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BA		 MC1_VVVF1
//						0x1823, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BB		 MC1_VVVF2
//						0x1824, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BC		 MC1_VVVF2
//						0x1825, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BD		 MC1_VVVF2
//						0x1826, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BE		 MC1_VVVF2
//						0x1827, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1BF		 MC1_VVVF2
//						0x1828, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//											
//EventTime = 0x190;	
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x18A   ATC 时间
//						0x1803, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//	
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1DA   MC1车异常数据
//						0x183D, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B2   MC1_PIS
//						0x181B, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B3   MC1_PIS
//						0x181C, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */						

EventTime = 0x3E8;  //1s	发送车辆编号
				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x200   MC1_CarNo
						0x184B, /*UNS16 index*/
						0x05, /*UNS8 subind*/ 
						&EventTime, /*void * pSourceData,*/ 
						&size, /* UNS8 * pExpectedSize*/
						RW);  /* UNS8 checkAccess */		
						
				break;
			
			
		case M_MTD_NODEID:
			
//	EventTime = ObjTimer200MS;				
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x185 	 DI
//						0x1801, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//			
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1DC   M车异常数据
//						0x183E, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x18E		 M_Door
//						0x1805, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1F4		 M_Door
//						0x1844, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */				
//EventTime = 0x190;
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x194		 M_ACC
//						0x1808, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1E8		 M_ACC
//						0x1841, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//	EventTime = ObjTimer200MS;	
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x19E		 M_BCU
//						0x180D, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x19F		 M_BCU
//						0x180E, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1A0		 M_BCU
//						0x180F, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//	
//	EventTime = 0x12C;		
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C0	 M_VVVF1
//						0x1829, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C1	 M_VVVF1
//						0x182A, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C2	 M_VVVF1
//						0x182B, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C3	 M_VVVF1
//						0x182C, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C4	 M_VVVF1
//						0x182D, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//		EventTime = 0x12C;							
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C5	 M_VVVF2
//						0x182E, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */							
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C6	 M_VVVF2
//						0x182F, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C7	 M_VVVF2
//						0x1830, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C8	 M_VVVF2
//						0x1831, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1C9	 M_VVVF2
//						0x1832, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

EventTime = 0x3E8;  //1s	发送车辆编号
				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x202   M_CarNo
						0x184C, /*UNS16 index*/
						0x05, /*UNS8 subind*/ 
						&EventTime, /*void * pSourceData,*/ 
						&size, /* UNS8 * pExpectedSize*/
						RW);  /* UNS8 checkAccess */	
						
				break;
			
			case MC2_MTD_NODEID:
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x188 	 DI
//						0x1802, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//			
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x191		 MC2_Door
//						0x1806, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1F7		 MC2_Door
//						0x1845, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */				

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x197		 MC2_ACC
//						0x1809, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1EB		 MC2_ACC
//						0x1842, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AE		 ATC
//						0x1817, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1AF		 ATC
//						0x1818, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B0		 ATC
//						0x1819, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B1		 ATC
//						0x181A, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */
//						
//EventTime = 0x12C;					
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CA	 MC2_VVVF1
//						0x1833, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CB	 MC2_VVVF1
//						0x1834, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CC	 MC2_VVVF1
//						0x1835, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CD	 MC2_VVVF1
//						0x1836, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CE	 MC2_VVVF1
//						0x1837, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1CF	 MC2_VVVF2
//						0x1838, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */							
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1D0	 MC2_VVVF2
//						0x1839, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1D1	 MC2_VVVF2
//						0x183A, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1D2	 MC2_VVVF2
//						0x183B, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1D3	 MC2_VVVF2
//						0x183C, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		
//						
//EventTime = 0x190;
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1F8   ATC 时间
//						0x1846, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */			

//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1DF   MC2车异常数据
//						0x183F, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B4   MC2_PIS
//						0x181D, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */	
//						
//				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x1B5   MC2_PIS
//						0x181E, /*UNS16 index*/
//						0x05, /*UNS8 subind*/ 
//						&EventTime, /*void * pSourceData,*/ 
//						&size, /* UNS8 * pExpectedSize*/
//						RW);  /* UNS8 checkAccess */		

EventTime = 0x3E8;  //1s	发送车辆编号
				writeLocalDict( &MTDApp1CAN2Slave_Data, /*CO_Data* d*/		//0x205   MC2_CarNo
						0x184D, /*UNS16 index*/
						0x05, /*UNS8 subind*/ 
						&EventTime, /*void * pSourceData,*/ 
						&size, /* UNS8 * pExpectedSize*/
						RW);  /* UNS8 checkAccess */
						
				break;
			
			default:
				printf("APP1 init Board Error!\r\n");
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
		UNS8 	CodeBitHig,CodeBitLow;
	u8 CanChangeFlag = false;

	CodeBitHig = (errCode & 0xF000)>>12;
	CodeBitLow = (UNS8)((errCode & 0x0F00)>>8);

	eprintf("Master received EMCY message. Node: %2.2x  ErrorCode: %4.4x  ErrorRegister: %2.2x\r\n", nodeID, errCode, errReg);
	
	if(CodeBitHig == 1 || CodeBitHig == 2 || CodeBitHig == 4 || CodeBitHig == 5 /*|| CodeBitHig == 6  || CodeBitHig == 3 */ )
	{
		if(0 == CodeBitLow)
		{
			CanChangeFlag = false;	
		}
		else
		{
			CanChangeFlag = true;
		}
	}
	
	else
	{
		if(0 == CodeBitLow)		//主控切换
		{
			CanChangeFlag = true;
		}
		else
		{
			CanChangeFlag = false;
		}
	}
	
	if(CanChangeFlag)
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
}
