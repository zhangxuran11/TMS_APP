#ifndef __DELAY_H
#define __DELAY_H 			   
#include <sys.h>	  

//1,delay_us,添加参数等于0判断,如果参数等于0,则直接退出. 
//2,修改ucosii下,delay_ms函数,加入OSLockNesting的判断,在进入中断后,也可以准确延时.
//V1.2 20150411  
//修改OS支持方式,以支持任意OS(不限于UCOSII和UCOSIII,理论上任意OS都可以支持)
//添加:delay_osrunning/delay_ostickspersec/delay_osintnesting三个宏定义
//添加:delay_osschedlock/delay_osschedunlock/delay_ostimedly三个函数
//V1.3 20150521
//修正UCOSIII支持时的2个bug：
//delay_tickspersec改为：delay_ostickspersec
//delay_intnesting改为：delay_osintnesting
////////////////////////////////////////////////////////////////////////////////// 

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























