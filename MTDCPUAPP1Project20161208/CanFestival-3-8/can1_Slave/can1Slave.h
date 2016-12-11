#include "MTDApp1CAN1Slave.h"

void CAN1Slave_heartbeatError(CO_Data* d, UNS8);

UNS8 CAN1Slave_canSend(Message *);

void CAN1Slave_initialisation(CO_Data* d);
void CAN1Slave_preOperational(CO_Data* d);
void CAN1Slave_operational(CO_Data* d);
void CAN1Slave_stopped(CO_Data* d);

void CAN1Slave_post_sync(CO_Data* d);
void CAN1Slave_post_TPDO(CO_Data* d);
void CAN1Slave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex);
void CAN1Slave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
