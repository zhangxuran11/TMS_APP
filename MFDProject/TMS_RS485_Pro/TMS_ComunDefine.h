#ifndef __TMS_DATETYPEDEFINE_H__
#define __TMS_DATETYPEDEFINE_H__

/* data typedef */
typedef   signed          char s8;
typedef   signed 		 short int s16;
typedef   signed           int s32;
typedef   signed     long long s64;

typedef unsigned          char u8;
typedef unsigned 		 short int u16;
typedef unsigned           int u32;
typedef unsigned     long long u64;

#define	 DEBUG_RS485		0			//如果为1 ，会打印485协议测试调试信息，否则不打印	 //W H H 

typedef struct test
{
	u8 u1 :1;
	u8 u2 :1;
	u8 u3 :1;
	u8 u4 :1;
	u8 u5 :1;
	u8 u6 :1;
	u8 u7 :1;
	u8 u8 :1;
	
	u8 u9  :1;
	u8 u10 :1;
	u8 u11 :1;
	u8 u12 :1;
	u8 u13 :1;
	u8 u14 :1;
	u8 u15 :1;
	u8 u16 :1;
	
	u8 u17 :1;
	u8 u18 :1;
	u8 u19 :1;
	u8 u20 :1;
	u8 u21 :1;
	

}TMS_Test;

#endif //__TMS_DATETYPEDEFINE_H__

