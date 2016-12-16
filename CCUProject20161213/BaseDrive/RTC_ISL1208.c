#include "RTC_ISL1208.h"  
#include "stm32I2C.h"
#include "os.h" 
#include "string.h"

const u8 * COMPILED_DATE=__DATE__;	//获得编译日期  
const u8 * COMPILED_TIME=__TIME__;	//获得编译时间  
const u8 Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};   
const u8 table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表   
  
RTC_TIME sRtcTime={0x16,0x07,0x04,0x14,0x10,0x05,0x03};  

static u8 IntToBCD(u8 i);//十进制转BCD  
static u8 BCDToInt(u8 bcd); //BCD转十进制  
static u8  RTC_Get_Week(u16 year,u8 month,u8 day);  
  
u8 IntToBCD(u8 i) //十进制转BCD  
{  
    return (((i / 10) << 4) | ((i % 10) & 0x0f));  
}  
  
u8 BCDToInt(u8 bcd) //BCD转十进制  
{  
    return  (bcd >> 4) * 10 + (0xf & bcd);  
}  

//比较两个字符串指定长度的内容是否相等  
//参数:s1,s2要比较的两个字符串;len,比较长度  
//返回值:1,相等;0,不相等  
u8 my_strcmp(u8*s1,u8*s2,u8 len)  
{  
    u8 i;  
    for(i=0;i<len;i++)if((*s1++)!=*s2++)return 0;  
    return 1;        
} 

/*  
******************************************************************  
功能: 初始化允许自动复位，设置允许写时钟，  
      去掉报警功能    
操作 REG_STATUS,REG_INTERRUPT,REG_RESERVED,REG_TRIMMING,REG_COMPENSATE       
*******************************************************************  
*/   
void ISL1208_Init()   
{ 
	u8 MonthBuf=0;
	u8 SR_REG_DATA	=	RTC_AUTO_RESET | RTC_WRITE_ENABLE ;		//0x90;	状态寄存器值得配置
	u8 INT_REG_DATA	= /*AMALR_INTERRUPT| ALARM_ENABLED | */ BATTERY_NO_OUTPUT /*| FREQUENCY_32768*/;	//0x11;	中断寄存器值 配置，LPMODE= 0 时 器件处于正常模式。
																																												//在后备电池供电模式中 若 FOBATB 位置为 1 则 FOUT/ IRQ 引脚被禁止
																																												//ALME 位置为 1 时 报警功能被使能 置为 0 时被禁止
																																												// IM 位置为 1 时 报警就会在中断模式中进行
	
	CPU_SR_ALLOC();		//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来  	
	STM32_I2C_Configuration();		//配置i2c的引脚，时钟。

	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
	/*调度器上锁*/
	STM32_I2C_BufferWrite(&SR_REG_DATA,REG_STATUS,1);			// 配置状态寄存器
	STM32_I2C_BufferWrite(&INT_REG_DATA,REG_INTERRUPT,1);	//配置中断寄存器
	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/
	 
	/*
		whh2016-11-17  
		初始化的时候读取月寄存器状态，如果月是0，则设置时间为编译时间
	*/
	STM32_I2C_BufferRead(&MonthBuf,REG_MONTH,1);
	if(MonthBuf == 0)
	{
		Auto_Time_Set(); //自动设置时间
	}
}   

void  ISL1208_SetTime(pRTC_TIME _pTime)
{	
	/*方法1：*/
//    STM32_I2C_WriteByte(&_pTime->_sec,  REG_SECOND);  
//    STM32_I2C_WriteByte(&_pTime->_min,  REG_MINUTE);  
//    STM32_I2C_WriteByte(&_pTime->_hour, REG_HOUR);  
//    STM32_I2C_WriteByte(&_pTime->_day,  REG_DATE);  
//    STM32_I2C_WriteByte(&_pTime->_month,REG_MONTH);  
//    STM32_I2C_WriteByte(&_pTime->_year, REG_YEAR);  
//    STM32_I2C_WriteByte(&_pTime->_week, REG_WEEK);
	
	/*方法2：*/
//	  STM32_I2C_BufferWrite(&_pTime->_sec,  REG_SECOND,1);  
//    STM32_I2C_BufferWrite(&_pTime->_min,  REG_MINUTE,1);  
//    STM32_I2C_BufferWrite(&_pTime->_hour, REG_HOUR,1);  
//    STM32_I2C_BufferWrite(&_pTime->_day,  REG_DATE,1);  
//    STM32_I2C_BufferWrite(&_pTime->_month,REG_MONTH,1);  
//    STM32_I2C_BufferWrite(&_pTime->_year, REG_YEAR,1);  
//    STM32_I2C_BufferWrite(&_pTime->_week, REG_WEEK,1);
	
		/*方法3：*/
	uint8_t SetData[7]={0};
	memcpy(SetData,_pTime,sizeof(SetData));
	STM32_I2C_BufferWrite(SetData, REG_SECOND,7);
}

/*
在使用stm32硬件i2c，读取的数据超过一个字节的，必须封装成DMA发送，
而只读取一个字节的数据，必须使用轮询的读取方式。
*/
void ISL1208_GetTime(pRTC_TIME _pTime)
{
		STM32_I2C_BufferRead(&_pTime->_sec,  REG_SECOND,1);		
    STM32_I2C_BufferRead(&_pTime->_min,  REG_MINUTE,1);  
    STM32_I2C_BufferRead(&_pTime->_hour, REG_HOUR,1);  
    STM32_I2C_BufferRead(&_pTime->_day,  REG_DATE,1);  
    STM32_I2C_BufferRead(&_pTime->_month,REG_MONTH,1);  
    STM32_I2C_BufferRead(&_pTime->_year, REG_YEAR,1);  
    STM32_I2C_BufferRead(&_pTime->_week, REG_WEEK,1);  
      
    _pTime->_sec=BCDToInt(_pTime->_sec);  
    _pTime->_min=BCDToInt(_pTime->_min);  
	
		{
			const u8 _hr=_pTime->_hour;
			if(_hr & HOUR_MODE_24 )/* 24h format */
			{
				_pTime->_hour= BCDToInt(_hr & MASK_HOUR_24);
			}
			else	/* 12h format */
			{
				_pTime->_hour = BCDToInt(_hr & MASK_HOUR_12);  
				
				if (_hr & HOUR_NOW_PM)	/* PM flag set */
					_pTime->_hour += 12;
			}
		}
	
    _pTime->_day=BCDToInt(_pTime->_day);  
    _pTime->_month=BCDToInt(_pTime->_month);  
    _pTime->_year=BCDToInt(_pTime->_year);  
    _pTime->_week=BCDToInt(_pTime->_week); 
}

/*自动设置时间*/
void 	Auto_Time_Set(void)
{
    u8 temp[3];  
    u8 i;  
    u8 mon,date,week;  
    u16 year;  
    u8 sec,min,hour;  
    for(i=0;i<3;i++)temp[i]=COMPILED_DATE[i];     
    for(i=0;i<12;i++)if(my_strcmp((u8*)Month_Tab[i],temp,3))break;     
    mon=i+1;//得到月份  
    if(COMPILED_DATE[4]==' ')date=COMPILED_DATE[5]-'0';   
    else date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';    
    year=10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';        
    hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';    
    min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';    
    sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';    
    week=RTC_Get_Week(year+2000,mon,date);  
    sRtcTime._day=IntToBCD(date);  
    sRtcTime._hour=IntToBCD(hour) | HOUR_MODE_24;  
    sRtcTime._min=IntToBCD(min);  
    sRtcTime._month=IntToBCD(mon);  
    sRtcTime._sec=IntToBCD(sec);  
    sRtcTime._year=IntToBCD(year);  
    sRtcTime._week=IntToBCD(week);  
  
    ISL1208_SetTime(&sRtcTime); 
}

 /*手动设置时间*/
void Manual_Set_Time(pRTC_TIME _pTime)
{
	CPU_SR_ALLOC();	//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来
	RTC_TIME  RtcTime;
	//u8	CheckWeek; 

	//	/*检测星期*/
	//	CheckWeek = RTC_Get_Week(2000+_pTime->_year,_pTime->_month,_pTime->_day); 
	//	
	//	if(CheckWeek != _pTime->_week)
	//	{
	//		_pTime->_week =CheckWeek;					//校验星期
	//	}
	
	RtcTime._year = IntToBCD(_pTime->_year) ;
	RtcTime._month= IntToBCD(_pTime->_month) & MASK_MONTH;
	RtcTime._day = IntToBCD(_pTime->_day) & MASK_DATE;
	RtcTime._hour = (IntToBCD(_pTime->_hour) & MASK_HOUR_24) | HOUR_MODE_24;
	RtcTime._min = IntToBCD(_pTime->_min) & MASK_MINUTE;
	RtcTime._sec = IntToBCD(_pTime->_sec)& MASK_SECOND;
	RtcTime._week = IntToBCD(_pTime->_week) & MASK_WEEK;

	OS_CRITICAL_ENTER();
	ISL1208_SetTime(&RtcTime); 
	OS_CRITICAL_EXIT();//退出临界区
}

u8 RTC_Get_Week(u16 year,u8 month,u8 day)  
{     
    u16 temp2;  
    u8 yearH,yearL;  
      
    yearH=year/100; yearL=year%100;   
		// 如果为21世纪,年份数加100    
    if (yearH>19)yearL+=100;  
    // 所过闰年数只算1900年之后的    
    temp2=yearL+yearL/4;  
    temp2=temp2%7;   
    temp2=temp2+day+table_week[month-1];  
    if (yearL%4==0&&month<3)temp2--;  
    return(temp2%7);  
}
