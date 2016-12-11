#include "MTDCAN3Master.h"

void CAN3Master_heartbeatError(CO_Data* d, UNS8);

UNS8 CAN3Master_canSend(Message *);

void CAN3Master_initialisation(CO_Data* d);
void CAN3Master_preOperational(CO_Data* d);
void CAN3Master_operational(CO_Data* d);
void CAN3Master_stopped(CO_Data* d);

void CAN3Master_post_sync(CO_Data* d);
void CAN3Master_post_TPDO(CO_Data* d);
void CAN3Master_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
void CAN3Master_post_SlaveBootup(CO_Data* d, UNS8 nodeid);
