#ifndef  __CAN12CALLBACK_H__
#define  __CAN12CALLBACK_H__

#include "stm32f4xx.h"
#include "canfestival.h"
#include "can1Slave.h"
#include "can2Slave.h"

UNS32 SetMTDTimeCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);			//设置本地时间
//UNS32 SetAccTempCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);		//设置空调温度
UNS32 SetCarNoCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);				/*设置车辆编号*/
UNS32 SetWhellCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);				/*设置轮径的回调函数*/
UNS32 Can3SnedProDataCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex); /*接受HMI发送的ATO模式 等参数  回调函数*/
UNS32 Can3SnedACCSdrCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);	/*通过can3发送空调温度*/
UNS32 Can3SnedDelCmdCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);  /*can3发送 删除故障记录 命令*/
UNS32 Can3SendPISSdrCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);  /*can3发送，PIS的sdr数据*/
UNS32 MDCURxATCDICallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);  		/*接受MDCU发送的ATC需要的DI数据*/ 
UNS32 MDCURxATCSbuFaultCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);   /*接受MDCU发送的ATC需要的子设备 故障数据*/
UNS32 MDCURxATCVVVFProCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex); /*接受MDCU 发送的VVVF协议数据*/
UNS32 MDCURxATCBCUProCallBack(CO_Data* d, const indextable * unsused_indextable, UNS8 unsused_bSubindex);	/*接受MDCU 发送的BCU协议数据*/

#endif 	//__CAN12CALLBACK_H__

