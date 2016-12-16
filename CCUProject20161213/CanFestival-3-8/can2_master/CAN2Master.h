#include "CCUCAN2Master.h"

void CAN2Master_heartbeatError(CO_Data* d, UNS8);

UNS8 CAN2Master_canSend(Message *);

void CAN2Master_initialisation(CO_Data* d);
void CAN2Master_preOperational(CO_Data* d);
void CAN2Master_operational(CO_Data* d);
void CAN2Master_stopped(CO_Data* d);

void CAN2Master_post_sync(CO_Data* d);
void CAN2Master_post_TPDO(CO_Data* d);
void CAN2Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
void CAN2Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid);
