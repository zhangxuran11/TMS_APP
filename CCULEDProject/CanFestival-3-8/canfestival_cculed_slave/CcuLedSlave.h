#include "Ccu1LedObjDict.h"

void Ccu1LedSlave_heartbeatError(CO_Data* d, UNS8);

UNS8 Ccu1LedSlave_canSend(Message *);

void Ccu1LedSlave_initialisation(CO_Data* d);
void Ccu1LedSlave_preOperational(CO_Data* d);
void Ccu1LedSlave_operational(CO_Data* d);
void Ccu1LedSlave_stopped(CO_Data* d);

void Ccu1LedSlave_post_sync(CO_Data* d);
void Ccu1LedSlave_post_TPDO(CO_Data* d);
void Ccu1LedSlave_storeODSubIndex(CO_Data* d, UNS16 wIndex, UNS8 bSubindex);
void Ccu1LedSlave_post_emcy(CO_Data* d, UNS8 nodeID, UNS16 errCode, UNS8 errReg);
