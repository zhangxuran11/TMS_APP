#include "CCUCAN1Master.h"

void CAN1Master_heartbeatError(CO_Data* d, UNS8);

UNS8 CAN1Master_canSend(Message *);

void CAN1Master_initialisation(CO_Data* d);
void CAN1Master_preOperational(CO_Data* d);
void CAN1Master_operational(CO_Data* d);
void CAN1Master_stopped(CO_Data* d);

void CAN1Master_post_sync(CO_Data* d);
void CAN1Master_post_TPDO(CO_Data* d);
void CAN1Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
void CAN1Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid);

extern EmcyMsgState CAN1EmcyState;
