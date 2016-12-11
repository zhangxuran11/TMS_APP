
/* File generated by gen_cfile.py. Should not be modified. */

#ifndef CCU1LEDOBJDICT_H
#define CCU1LEDOBJDICT_H

#include "data.h"

/* Prototypes of function provided by object dictionnary */
UNS32 Ccu1LedObjDict_valueRangeTest (UNS8 typeValue, void * value);
const indextable * Ccu1LedObjDict_scanIndexOD (UNS16 wIndex, UNS32 * errorCode, ODCallback_t **callbacks);

/* Master node data struct */
extern CO_Data Ccu1LedObjDict_Data;
extern UNS8 HeartMap_CAN1_CANBC;		/* Mapped at index 0x2000, subindex 0x01 */
extern UNS8 HeartMap_CAN2_CANBC;		/* Mapped at index 0x2000, subindex 0x02 */
extern UNS8 HeartMap_CAN1_CCU1;		/* Mapped at index 0x2000, subindex 0x03 */
extern UNS8 HeartMap_CAN2_CCU1;		/* Mapped at index 0x2000, subindex 0x04 */
extern UNS8 HeartMap_CAN1_CCU2;		/* Mapped at index 0x2000, subindex 0x05 */
extern UNS8 HeartMap_CAN2_CCU2;		/* Mapped at index 0x2000, subindex 0x06 */
extern UNS8 HeartMap_CAN1_DDU1;		/* Mapped at index 0x2000, subindex 0x07 */
extern UNS8 HeartMap_CAN2_DDU1;		/* Mapped at index 0x2000, subindex 0x08 */
extern UNS8 HeartMap_CAN1_DDU2;		/* Mapped at index 0x2000, subindex 0x09 */
extern UNS8 HeartMap_CAN2_DDU2;		/* Mapped at index 0x2000, subindex 0x0A */
extern UNS8 HeartMap_CAN1_EMR1;		/* Mapped at index 0x2000, subindex 0x0B */
extern UNS8 HeartMap_CAN2_EMR1;		/* Mapped at index 0x2000, subindex 0x0C */
extern UNS8 HeartMap_CAN1_EMR2;		/* Mapped at index 0x2000, subindex 0x0D */
extern UNS8 HeartMap_CAN2_EMR2;		/* Mapped at index 0x2000, subindex 0x0E */
extern UNS8 HeartMap_CAN1_MTD1;		/* Mapped at index 0x2000, subindex 0x0F */
extern UNS8 HeartMap_CAN2_MTD1;		/* Mapped at index 0x2000, subindex 0x10 */
extern UNS8 HeartMap_CAN1_MTD2;		/* Mapped at index 0x2000, subindex 0x11 */
extern UNS8 HeartMap_CAN2_MTD2;		/* Mapped at index 0x2000, subindex 0x12 */
extern UNS8 HeartMap_CAN1_MTD3;		/* Mapped at index 0x2000, subindex 0x13 */
extern UNS8 HeartMap_CAN2_MTD3;		/* Mapped at index 0x2000, subindex 0x14 */
extern UNS8 HeartMap_CAN1_MTD4;		/* Mapped at index 0x2000, subindex 0x15 */
extern UNS8 HeartMap_CAN2_MTD4;		/* Mapped at index 0x2000, subindex 0x16 */
extern UNS8 HeartMap_CAN1_MTD5;		/* Mapped at index 0x2000, subindex 0x17 */
extern UNS8 HeartMap_CAN2_MTD5;		/* Mapped at index 0x2000, subindex 0x18 */
extern UNS8 HeartMap_CAN1_MTD6;		/* Mapped at index 0x2000, subindex 0x19 */
extern UNS8 HeartMap_CAN2_MTD6;		/* Mapped at index 0x2000, subindex 0x1A */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x1B */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x1C */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x1D */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x1E */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x1F */
extern UNS8 HeartMap_Undefined;		/* Mapped at index 0x2000, subindex 0x20 */
extern INTEGER8 Ccu1LedTPDOMap;		/* Mapped at index 0x2001, subindex 0x00*/

#endif // CCU1LEDOBJDICT_H
