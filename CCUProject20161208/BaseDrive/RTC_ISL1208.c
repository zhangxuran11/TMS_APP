#include "RTC_ISL1208.h"  
#include "stm32I2C.h"
#include "os.h" 
#include "string.h"

const u8 * COMPILED_DATE=__DATE__;	//获得编译日期  
const u8 * COMPILED_TIME=__TIME__;	//获得编译时间  
const u8 Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};   
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表   
  
RTC_TIME sRtcTime={0x16,0x07,0x04,0x14,0x10,0x05,0x03};  

static int IntToBCD(int i);//十进制转BCD  
static int BCDToInt(int bcd); //BCD转十进制  
static u8  RTC_Get_Week(u16 year,u8 month,u8 day);  
  
int IntToBCD(int i) //十进制转BCD  
{  
    return (((i / 10) << 4) + ((i % 10) & 0x0f));  
}  
  
int BCDToInt(int bcd) //BCD转十进制  
{  
    return (0xff & (bcd >> 4)) * 10 + (0xf & bcd);  
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
	
	u8 SR_REG_DATA	=	RTC_AUTO_RESET | RTC_WRITE_ENABLE | RTC_STATE_POWERON;		//0x91;	状态寄存器值得配置
	u8 INT_REG_DATA	= AMALR_INTERRUPT| ALARM_ENABLED | FREQUENCY_1HZ;						//0xca;	中断寄存器值 配置
	
	CPU_SR_ALLOC();			//变量定义，用于保存SR状态.将CPU状态寄存器的值保存起来  
	
	STM32_I2C_Configuration();		//配置i2c的引脚，时钟。
	
	OS_CRITICAL_ENTER();//进入临界区				调度器枷锁的方式，保证以下代码不被打断
	/*调度器上锁*/
	
	STM32_I2C_WriteByte(&SR_REG_DATA,REG_STATUS);			// 配置状态寄存器
	STM32_I2C_WriteByte(&INT_REG_DATA,REG_INTERRUPT);	//配置中断寄存器
	
	OS_CRITICAL_EXIT();	//退出临界区		 /*调度器开锁*/
	
}   

void  ISL1208_SetTime(pRTC_TIME _pTime)
{	
    STM32_I2C_WriteByte(&_pTime->_sec,  REG_SECOND);  
    STM32_I2C_WriteByte(&_pTime->_min,  REG_MINUTE);  
    STM32_I2C_WriteByte(&_pTime->_hour, REG_HOUR);  
    STM32_I2C_WriteByte(&_pTime->_day,  REG_DATE);  
    STM32_I2C_WriteByte(&_pTime->_month,REG_MONTH);  
    STM32_I2C_WriteByte(&_pTime->_year, REG_YEAR);  
    STM32_I2C_WriteByte(&_pTime->_week, REG_WEEK);
}


void ISL1208_GetTime(pRTC_TIME _pTime)
{
//		u8 ReadData[7]={0};
	  STM32_I2C_BufferRead((u8 *)_pTime,  REG_SECOND,7);  
		
//		STM32_I2C_BufferRead(&_pTime->_sec,  REG_SECOND,1);		
//    STM32_I2C_BufferRead(&_pTime->_min,  REG_MINUTE,1);  
//    STM32_I2C_BufferRead(&_pTime->_hour, REG_HOUR,1);  
//    STM32_I2C_BufferRead(&_pTime->_day,  REG_DATE,1);  
//    STM32_I2C_BufferRead(&_pTime->_month,REG_MONTH,1);  
//    STM32_I2C_BufferRead(&_pTime->_year, REG_YEAR,1);  
//    STM32_I2C_BufferRead(&_pTime->_week, REG_WEEK,1);  
      
    _pTime->_sec=BCDToInt(_pTime->_sec);  
    _pTime->_min=BCDToInt(_pTime->_min);  
    _pTime->_hour=BCDToInt(_pTime->_hour);  
    _pTime->_day=BCDToInt(_pTime->_day);  
    _pTime->_month=BCDToInt(_pTime->_month);  
    _pTime->_year=BCDToInt(_pTime->_year);  
    _pTime->_week=BCDToInt(_pTime->_week); 
}


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
    sRtcTime._hour=IntToBCD(hour);  
    sRtcTime._min=IntToBCD(min);  
    sRtcTime._month=IntToBCD(mon);  
    sRtcTime._sec=IntToBCD(sec);  
    sRtcTime._year=IntToBCD(year);  
    sRtcTime._week=IntToBCD(week);  
  
    ISL1208_SetTime(&sRtcTime); 
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
