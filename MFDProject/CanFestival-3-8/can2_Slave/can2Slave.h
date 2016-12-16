#include "MFDCAN2Slave.h"

void CAN2Slave_heartbeatError(CO_Data* d, UNS8);

UNS8 CAN2Slave_canSend(Message *);

void CAN2Slave_initialisation(CO_Data* d);
void CAN2Slave_preOperational(CO_Data* d);
void CAN2Slave_operational(CO_Data* d);
void CAN2Slave_stopped(CO_Data* d);

void CAN2Slave_post_sync(CO_Data* d);
void CAN2Slave_post_TPDO(CO_Data* d);
void CAN2Slave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex);
void CAN2Slave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
