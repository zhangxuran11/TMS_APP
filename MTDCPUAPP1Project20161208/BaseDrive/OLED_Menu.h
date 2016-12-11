#ifndef __OLED_MENU_H__
#define __OLED_MENU_H__

#include "stm32f4xx.h"

#define MC1_CAR_TYPE	"MC1(1)"
#define T_CAR_TYPE		"T(4)"
#define M_CAR_TYPE		"M(3)"
#define T1_CAR_TYPE		"T1(5)"
#define T2_CAR_TYPE		"T2(6)"
#define MC2_CAR_TYPE	"MC2(2)"


typedef struct 
{ 
 u8 CurrentIndex;//当前状态索引号 
 u8 Up; 		//向上(UP) 键时转向的状态索引号
 u8 Down; 	//向下(Down)键时转向的状态索引号
 u8 Enter; 	//确定(OK按键)键时转向的状态索引号
 u8 Exit; 	//退出(set按键) 键时转向的状态索引号
 void (*CurrentOperation)(void); //当前状态应该执行的操作 
} Menu_table; 

extern Menu_table  MenuTable[5];
extern void (*CurrentOperationIndex)(void);  //定义函数指针
extern const char CarTypeArr[6][10];

void  MainMenu(void);
void  ChooseSetMeun(void);
void  SetCarMenu(void);

void  KeySetMenu(void);

#endif //__OLED_MENU_H__
