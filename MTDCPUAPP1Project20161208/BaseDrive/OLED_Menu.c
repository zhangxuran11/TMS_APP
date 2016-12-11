#include "OLED_Menu.h"
#include "MTD_CPU_Key.h"
#include "OLED_SSD1325.h"
#include "RTC_ISL1208.h"
#include "applicfg.h"
#include "MTDApp1CAN1Slave.h"
#include "MTDApp1CAN2Slave.h"
#include "Uart.h"
#include "stdbool.h"
#include "os.h"

extern u8 ChooseBoard;
extern u8 can1CarNumberFlag ;
extern u8 can2CarNumberFlag ;
extern u8 can1UpdateTimeFlag;
extern u8 can2UpdateTimeFlag;

	/*车厢号*/
extern	u16		CarNumber;
extern	u8    CarNo;
extern	char 	cartype[10];

u16	LocalCarNumber;		//用于保存按键设置时的值
u8 	LocalCarNo;
char LocalCartype[10];
const char CarTypeArr[6][10] = {"MC1(1)","T(4)","M(3)","T1(5)","T2(6)","MC2(2)"};

void (*CurrentOperationIndex)(void);  //定义函数指针
u8 KeyFuncIndex=0;				//全局变量来记录按键功能索引,默认为0
s8 ChooseUpDownCount=1;		//选择设置界面的up键计数
u8 CarSetUpFlag=false;		//车辆编号的设置	的up标志
u8 CarSetDownFlag=false;	//车辆编号的设置	的Down标志
s8 CarNumMenuOkKeyCount=0;//车辆编号设置界面OK按键的计数
u8 CarSetOk1Flag=false;		//按键设置标志位
u8 CarSetOk2Flag=false;		//按键设置标志位
u8 CarSetOk3Flag=false;		//按键设置标志位

Menu_table  MenuTable[5]=
{
  {0,0,0,0,1,(*MainMenu)}, 			//0	第1级 主菜单
	{1,1,1,2,0,(*ChooseSetMeun)},	//1 第2级	选择设置菜单(第1级的子菜单)
	{2,2,2,2,1,(*SetCarMenu)},		//2 第3才	车辆编号，车号，车型等设置			
};

/*
按键设置菜单
*/
void  KeySetMenu(void)
{
	u8 KeyValue=0;		
	KeyValue=MTDCPU_KEY_Scan(0);	//获取按键值
	switch(KeyValue)
	{
		case MTDCPU_UP:					//上按键
			switch(KeyFuncIndex)	//判断当前所处于的界面
			{
				case 1:
					ChooseUpDownCount--;		//向上翻页，故使用--
					if(ChooseUpDownCount<1)
					{
						ChooseUpDownCount=3;
					}
					break;					
				case 2:
					CarSetUpFlag = true;
					break;
			}
			KeyFuncIndex = MenuTable[KeyFuncIndex].Up;
			break;
		case MTDCPU_DOWN:			//下按键
			switch(KeyFuncIndex)//判断当前所处于的界面
			{
				case 1:
					ChooseUpDownCount++;	//向下翻页，故使用--
					if(ChooseUpDownCount>3)
					{
						ChooseUpDownCount=1;
					}
					break;
				case 2:
					CarSetDownFlag=true;
					break;
			}			
			KeyFuncIndex = MenuTable[KeyFuncIndex].Down;
			break;
		case MTDCPU_SET:
			KeyFuncIndex = MenuTable[KeyFuncIndex].Exit;
			break;
		case MTDCPU_OK:
			switch(KeyFuncIndex)
			{
				case 2:
					switch(ChooseUpDownCount)		//判断是选择进入
					{
						case 1:		//车辆编号设置

							CarNumMenuOkKeyCount++;
		//							if(CarNumMenuOkKeyCount>3)
		//							{
		//								//CarNumMenuOkKeyCount=0;	//置位
		//								MenuTable[KeyFuncIndex].Enter = 0;	//返回到主界面
		//							}
							if(CarNumMenuOkKeyCount>3)
							{
								CarSetOk1Flag = true;	
							}
							break;
						case 2:		//车号设置
							CarSetOk2Flag = true;	
							break;
						case 3: 	//车型设置
							CarSetOk3Flag = true;	
							break;
					};
					break;
			}
			KeyFuncIndex = MenuTable[KeyFuncIndex].Enter;
			break;
	}
	CurrentOperationIndex = MenuTable[KeyFuncIndex].CurrentOperation;
	(*CurrentOperationIndex)();		//执行当前操作函数
}

/*主界面*/
void  MainMenu(void)
{
	/*RTC 变量*/
	static 	RTC_TIME time;
	char pbuf[20];
	
	CPU_SR_ALLOC();
								
	if(CarSetOk1Flag==true)		//保存值
	{					
		debugprintf("111!\r\n");
		CarSetOk1Flag = false;
		
		if(CarNumber != LocalCarNumber)
		{
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					can1_MC1TxCarNo[0] = LocalCarNumber/100;
					can1_MC1TxCarNo[1] = LocalCarNumber%100;
					can2_MC1TxCarNo[0] = LocalCarNumber/100;
					can2_MC1TxCarNo[1] = LocalCarNumber%100;				
				
				/*WHH 这里不需要直接发送，在定时器中断中会发送这个车辆编号的*/
				//					/* force emission of PDO by artificially changing last emitted */
				//					MTDApp1CAN1Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					MTDApp1CAN2Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					
				//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,75);		//pdo 0x200  75
				//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,75);				
					break;
				
				case T_MTD_NODEID:
					break;
				
				case M_MTD_NODEID:
					can1_MTxCarNo[0] = LocalCarNumber/100;
					can1_MTxCarNo[1] = LocalCarNumber%100;		
					can2_MTxCarNo[0] = LocalCarNumber/100;
					can2_MTxCarNo[1] = LocalCarNumber%100;					
					break;
				
				case T1_MTD_NODEID:
					break;
				
				case T2_MTD_NODEID:
					break;
				
				case MC2_MTD_NODEID:
					can1_MC2TxCarNo[0] = LocalCarNumber/100;
					can1_MC2TxCarNo[1] = LocalCarNumber%100;	
					can2_MC2TxCarNo[0] = LocalCarNumber/100;
					can2_MC2TxCarNo[1] = LocalCarNumber%100;					
					break;
			}
		}
		
		CarNumber =  LocalCarNumber;
		CarNo = LocalCarNo;
		strcpy(cartype,LocalCartype);		//whh 增加关联 车厢类型
	}


	else if(CarSetOk2Flag == true)		//保存值
	{
		debugprintf("222!\r\n");
		CarSetOk2Flag = false;
		
		if(CarNumber != LocalCarNumber)
		{
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					can1_MC1TxCarNo[0] = LocalCarNumber/100;
					can1_MC1TxCarNo[1] = LocalCarNumber%100;
					can2_MC1TxCarNo[0] = LocalCarNumber/100;
					can2_MC1TxCarNo[1] = LocalCarNumber%100;				
				/*WHH 这里不需要直接发送，在定时器中断中会发送这个车辆编号的*/
				//					/* force emission of PDO by artificially changing last emitted */
				//					MTDApp1CAN1Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					MTDApp1CAN2Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					
				//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,75);		//pdo 0x200  75
				//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,75);				
					break;
				
				case T_MTD_NODEID:
					break;
				
				case M_MTD_NODEID:
					can1_MTxCarNo[0] = LocalCarNumber/100;
					can1_MTxCarNo[1] = LocalCarNumber%100;		
					can2_MTxCarNo[0] = LocalCarNumber/100;
					can2_MTxCarNo[1] = LocalCarNumber%100;					
					break;
				
				case T1_MTD_NODEID:
					break;
				
				case T2_MTD_NODEID:
					break;
				
				case MC2_MTD_NODEID:
					can1_MC2TxCarNo[0] = LocalCarNumber/100;
					can1_MC2TxCarNo[1] = LocalCarNumber%100;	
					can2_MC2TxCarNo[0] = LocalCarNumber/100;
					can2_MC2TxCarNo[1] = LocalCarNumber%100;					
					break;
			}
		}
		
		CarNumber =  LocalCarNumber;
		CarNo = LocalCarNo;
		strcpy(cartype,LocalCartype);		//whh 增加关联 车厢类型
	}	
				
	else if(CarSetOk3Flag == true)		//保存值
	{
		debugprintf("333!\r\n");
		CarSetOk3Flag=false;
		
		if(CarNumber != LocalCarNumber)
		{
			switch(ChooseBoard)
			{
				case MC1_MTD_NODEID:
					can1_MC1TxCarNo[0] = LocalCarNumber/100;
					can1_MC1TxCarNo[1] = LocalCarNumber%100;
					can2_MC1TxCarNo[0] = LocalCarNumber/100;
					can2_MC1TxCarNo[1] = LocalCarNumber%100;				
				/*WHH 这里不需要直接发送，在定时器中断中会发送这个车辆编号的*/
				//					/* force emission of PDO by artificially changing last emitted */
				//					MTDApp1CAN1Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					MTDApp1CAN2Slave_Data.PDO_status[75].last_message.cob_id = 0;	
				//					
				//					sendOnePDOevent(&MTDApp1CAN1Slave_Data,75);		//pdo 0x200  75
				//					sendOnePDOevent(&MTDApp1CAN2Slave_Data,75);				
					break;
				
				case T_MTD_NODEID:
					break;
				
				case M_MTD_NODEID:
					can1_MTxCarNo[0] = LocalCarNumber/100;
					can1_MTxCarNo[1] = LocalCarNumber%100;		
					can2_MTxCarNo[0] = LocalCarNumber/100;
					can2_MTxCarNo[1] = LocalCarNumber%100;				
					break;
				
				case T1_MTD_NODEID:
					break;
				
				case T2_MTD_NODEID:
					break;
				
				case MC2_MTD_NODEID:
					can1_MC2TxCarNo[0] = LocalCarNumber/100;
					can1_MC2TxCarNo[1] = LocalCarNumber%100;		
					can2_MC2TxCarNo[0] = LocalCarNumber/100;
					can2_MC2TxCarNo[1] = LocalCarNumber%100;					
					break;
			}
		}
		
		CarNumber =  LocalCarNumber;
		CarNo = LocalCarNo;
		strcpy(cartype,LocalCartype);		//whh 增加关联 车厢类型
	}
				
	/*在主菜单的时候要将所有计数清除置位*/
	ChooseUpDownCount=1;			//选择设置界面的up键计数
	CarSetUpFlag=false;		//车辆编号的设置	的up标志
	CarSetDownFlag=false;	//车辆编号的设置	的Down标志
	CarNumMenuOkKeyCount=0;		//车辆编号设置界面OK按键的计数
	CarSetOk1Flag=false;	
	CarSetOk2Flag=false;	
	CarSetOk3Flag=false;	
	
	/*将原始数据复制到菜单本地需要的参数变量中*/  //必须放在if后面
	strcpy(LocalCartype,cartype);
	LocalCarNumber = CarNumber;
	LocalCarNo = CarNo;
		
	//	if(can1CarNumberFlag == true )
	//	{
	//		debugprintf("can1car !\r\n");
	//		CarNo = can1_Car_Number_1;
	//		CarNumber = can1_Car_Number_2;		//车辆编号
	//		can1CarNumberFlag = false;
	//	}
	//	
	//	if(can2CarNumberFlag == true )
	//	{
	//		debugprintf("can2car !\r\n");
	//		CarNo = can2_Car_Number_1;
	//		CarNumber = can2_Car_Number_2;		//车辆编号
	//		can2CarNumberFlag =false;
	//	}
	//	
	//	if(can1UpdateTimeFlag == true)
	//	{
	//		debugprintf("can1Data !\r\n");
	//		time._year = can1_Car_DateTime[1];
	//		time._month = can1_Car_DateTime[2];
	//		time._day = can1_Car_DateTime[3];
	//		time._hour = can1_Car_DateTime[4];
	//		time._min = can1_Car_DateTime[5];
	//		time._sec = can1_Car_DateTime[6];
	//		Manual_Set_Time(&time);
	//		can1UpdateTimeFlag = false;
	//	}
	//	
	//	if(can2UpdateTimeFlag == true)
	//	{
	//		debugprintf("can2Data !\r\n");
	//		time._year = can1_Car_DateTime[1];
	//		time._month = can1_Car_DateTime[2];
	//		time._day = can1_Car_DateTime[3];
	//		time._hour = can1_Car_DateTime[4];
	//		time._min = can1_Car_DateTime[5];
	//		time._sec = can1_Car_DateTime[6];
	//		Manual_Set_Time(&time);
	//		can2UpdateTimeFlag =false;
	//	}
	
	//进入临界区 代码应该写在这里
	OS_CRITICAL_ENTER();
		ISL1208_GetTime(&time);		//获取时间
	OS_CRITICAL_EXIT();//退出临界区
	
	Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
	
	//sprintf(pbuf,"%d%s%02d%s%02d%02d%02d%02d",time._year+2000,"-",time._month,"-",time._day,time._hour,time._min,time._sec);
	sprintf(pbuf,"%d%s%02d%s%02d%s%02d%s%02d",time._year+2000,"-",time._month,"-",time._day," ",time._hour,":",time._min);
		
	Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);     /* 绘制文本 */

	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%04d","车辆编号: ",CarNumber);	
	Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
	
	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%01d","车    号: ",CarNo);		
	Draw_Component(COMPONENT_LABEL_ENABLED,  3, 32, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	 	/* 绘制文本 */
		
	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%s", "车型编号: ",cartype);	
	Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */               
	
	Update_Component(4);                                                                              /* 更新组件状态 */
	Next_Frame(); 

}
/*
二级菜单，选择设置菜单
*/
void  ChooseSetMeun(void)
{
	char pbuf[20];
	
	/*重新初始化设置*/
	CarSetUpFlag=false;		//车辆编号的设置	的up标志
	CarSetDownFlag=false;	//车辆编号的设置	的Down标志
	CarNumMenuOkKeyCount=0;		//车辆编号设置界面OK按键的计数
	CarSetOk1Flag=false;	
	CarSetOk2Flag=false;	
	CarSetOk3Flag=false;	
	
	/*将原始数据复制到菜单本地需要的参数变量中*/  //必须放在if后面
	strcpy(LocalCartype,cartype);
	LocalCarNumber = CarNumber;
	LocalCarNo = CarNo;
	
	Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
	
	sprintf(pbuf,"%s","选择设置");
	Draw_Component(COMPONENT_LABEL_ENABLED,  1, 0,  0,  15,  0,   0, 0, 0, (unsigned char *)pbuf);     /* 绘制文本,字体居中 */

	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%04d","车辆编号: ",CarNumber);	
	Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
	
	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%01d","车    号: ",CarNo);		
	Draw_Component(COMPONENT_LABEL_ENABLED,  3, 32, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf); 	 	/* 绘制文本 */
		
	memset(pbuf,0,sizeof(pbuf));
	sprintf(pbuf,"%s%s", "车型编号: ",cartype);	
	Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */               
	
	Update_Component(4);                                                                              /* 更新组件状态 */
	
	Draw_Reverse(ChooseUpDownCount*16,0,(ChooseUpDownCount+1)*16,128,0);	//车辆编号反色
	Next_Frame(); 
}

/*
三级子菜单，设置车辆编号，车号，车辆类型。
*/
void  SetCarMenu(void)
{
	char pbuf[20];
	s8 ThouBit;	//千位
	s8 HundBit;	//百位
	s8 TenBit;	//十位、
	s8 UnitBit;	//个位；

	switch(ChooseUpDownCount)		//判断是选择进入
	{
		case 1:		//车辆编号设置界面		
			ThouBit = LocalCarNumber /1000 %10;
			HundBit	= LocalCarNumber /100 %10;
			TenBit = LocalCarNumber  /10 %10;
			UnitBit = LocalCarNumber %10;
			
			MenuTable[KeyFuncIndex].Enter = 2;	//将界面保存在车辆编号设置
			switch(CarNumMenuOkKeyCount)
			{
				case 0:		//千位
					if(CarSetUpFlag == true)
					{
						CarSetUpFlag = false;
						ThouBit += 1;
						if(ThouBit>9)
						{
							ThouBit=0;
						}
					}
					if(CarSetDownFlag == true)
					{
						CarSetDownFlag = false;
						ThouBit -=1;
						if(ThouBit<0)
						{
							ThouBit=9;
						}
					}
					break;
				case 1:		//百位
					if(CarSetUpFlag == true)
					{
						CarSetUpFlag = false;
						HundBit += 1;
						if(HundBit>9)
						{
							HundBit=0;
						}
					}
					if(CarSetDownFlag == true)
					{
						CarSetDownFlag = false;
						HundBit -=1;
						if(HundBit<0)
						{
							HundBit=9;
						}				
					}
					break;
				case 2:		//十位
					if(CarSetUpFlag == true)
					{
						CarSetUpFlag = false;
						TenBit += 1;
						if(TenBit>9)
						{
							TenBit=0;
						}
					}
					if(CarSetDownFlag == true)
					{
						CarSetDownFlag = false;
						TenBit -=1;
						if(TenBit<0)
						{
							TenBit=9;
						}				
					}
					break;
				case 3:		//个位,表示为车号
					if(CarSetUpFlag == true)
					{
						CarSetUpFlag = false;
						UnitBit += 1;
						if(UnitBit>6)
						{
							UnitBit=1;
						}
					}
					if(CarSetDownFlag == true)
					{
						CarSetDownFlag = false;
						UnitBit -=1;
						if(UnitBit<1)
						{
							UnitBit=6;
						}				
					}				
				
					MenuTable[KeyFuncIndex].Enter = 0;	//最后一次Ok按键要返回到主界面					
					break;

			}
			
			LocalCarNumber = ThouBit*1000 + HundBit*100+ TenBit*10 +UnitBit;  //计算车辆编号
			LocalCarNo = UnitBit;	//车号跟着车厢号一起变更
			strcpy(LocalCartype,CarTypeArr[UnitBit-1]);		//车辆编号 跟 车辆类型绑定

			Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
			memset(pbuf,0,sizeof(pbuf));
			sprintf(pbuf,"%s","车辆编号: ");	
			Draw_Component(COMPONENT_LABEL_ENABLED,  2, /*16-4*/12, 0,  15,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
			
			memset(pbuf,0,sizeof(pbuf));
			sprintf(pbuf,"%04d",LocalCarNumber);		//车厢编号值	
			Draw_Component(COMPONENT_LABEL_ENABLED,  3, /*32-4*/28, 0,  15,  0,   0, 0, 0, (unsigned char *)pbuf); 	 	/* 绘制文本 */
	
		
			memset(pbuf,0,sizeof(pbuf));
			sprintf(pbuf,"%s%s", "SET取消   ","OK确认");	
			Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */  
		
			Recycle_Component(1,1);//回收组件1
			Update_Component(4);                                                                              /* 更新组件状态 */
			Draw_Reverse(48,0,48,128,0);		//画一条线	
			Draw_Reverse(/*32-3*/29,48+CarNumMenuOkKeyCount*8,/*48-5*/43,48+CarNumMenuOkKeyCount*8+6,0);			//某个数字取反	
			Next_Frame(); 	
			break;
		
		case 2:		//车号设置界面	

			if(CarSetUpFlag == true)
			{
				CarSetUpFlag = false;
				LocalCarNo++;
				if(LocalCarNo>6)
				{
					LocalCarNo=1;
				}
			}
			if(CarSetDownFlag == true)
			{
				CarSetDownFlag = false;
				LocalCarNo--;
				if(LocalCarNo<1)
				{
					LocalCarNo=6;
				}
			}
			LocalCarNumber = (LocalCarNumber/10)*10+LocalCarNo;  //车厢编号 与车号绑定
			strcpy(LocalCartype,CarTypeArr[LocalCarNo-1]);		//车号 跟 车辆类型绑定
			
			Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
			memset(pbuf,0,sizeof(pbuf));
			sprintf(pbuf,"%s%01d","车号: ",LocalCarNo);	
			Draw_Component(COMPONENT_LABEL_ENABLED,  2, 16, 0,  15,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
			
			memset(pbuf,0,sizeof(pbuf));
			sprintf(pbuf,"%s%s", "SET取消   ","OK确认");	
			Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */  
		
			Recycle_Component(1,1);//回收组件1
			Recycle_Component(3,3);//回收组件1
			Update_Component(4);                                                                              /* 更新组件状态 */
			Draw_Reverse(48,0,48,128,0);		//画一条线	
			Draw_Reverse(/*16+2*/18,/*16*5+4*/84,/*32-2*/30,/*16*5+10*/90,0);	//车号取反色
			Next_Frame(); 
			MenuTable[KeyFuncIndex].Enter = 0;	//返回到主界面
			break;
		
		case 3:		//车型编号设置界面
			{
				//char CarTypeArr[6][10] = {"MC1(1)","MC2(2)","M(3)","T(4)","T1(5)","T2(6)"};		//WHH 将局部变量修改为全局变量
				s8 i=0;
		
				while(1)		//计算出当前车型所在的位置
				{
					if(i>5)
					{
						debugprintf("没有匹配的车型!\r\n");
						break;
					}
					if(strcmp(LocalCartype,CarTypeArr[i]) == 0)
					{
						break;
					}
					else
					{
						i++;
					}
				}
				
				if(CarSetUpFlag == true)		//上按键操作
				{
					CarSetUpFlag = false;
					i++;
					if(i>5)
					{
						i=0;
					}
				}
				if(CarSetDownFlag == true)	//下按键操作
				{
					CarSetDownFlag = false;
					i--;
					if(i<0)
					{
						i=5;
					}
				}
				
				LocalCarNumber = (LocalCarNumber/10)*10+(i+1);  //计算车辆编号
				LocalCarNo = i+1;	//车号跟着车辆类型一起变更				
				strcpy(LocalCartype,CarTypeArr[i]);
				//memcpy(cartype,CarTypeArr[i],10);  //将车型 copy到全局变量
				
				Clean_Screen(0, 0, 64, 128, 0);                                                                     /* 清屏 */
				memset(pbuf,0,sizeof(pbuf));
				sprintf(pbuf,"%s","车型编号: ");	
				Draw_Component(COMPONENT_LABEL_ENABLED,  2, /*16-4*/12, 0,  15,  0,   0, 0, 0, (unsigned char *)pbuf); 	  /* 绘制文本 */
			
				memset(pbuf,0,sizeof(pbuf));
				sprintf(pbuf,"%s",LocalCartype);		//车型编号值
				Draw_Component(COMPONENT_LABEL_ENABLED,  3, /*32-4*/28, 0,  15,  0,   0, 0, 0, (unsigned char *)pbuf); 	 	/* 绘制文本 */
				
				memset(pbuf,0,sizeof(pbuf));
				sprintf(pbuf,"%s%s", "SET取消   ","OK确认");	
				Draw_Component(COMPONENT_LABEL_ENABLED,  4, 48, 0,  16,  0,   0, 0, 0, (unsigned char *)pbuf);    /* 绘制文本 */  
			
				Recycle_Component(1,1);//回收组件1
				Update_Component(4);                                                                              /* 更新组件状态 */
				Draw_Reverse(48,0,48,128,0);		//画一条线	
				Draw_Reverse(/*32-3*/29,16*3-10,/*48-5*/43,16*6-10,0);	//车型取反色
				Next_Frame(); 
				MenuTable[KeyFuncIndex].Enter = 0;	//返回到主界面
			}

			break;
	}
}


