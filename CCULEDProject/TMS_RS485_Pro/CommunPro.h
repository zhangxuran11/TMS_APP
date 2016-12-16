#ifndef __COMMUNPRO_H__
#define	__COMMUNPRO_H__

#include "TMS_ComunDefine.h"
/*
函数说明：
功能：要获得某位的数据值
参数: 
	Data:入口参数,数据指针。
	GetPosit： 要得到数据的位置
返回值：返回得到某位的值
*/
u8 GetDataBit(u8 * Data,u8 GetPosit);

/*
	函数说明：
功能：将10进制数转换为BCD码
参数: 
			输入：u8 Dec   待转换的十进制数据
返回值：  转换后的BCD码
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 DecToBcd(u8 Dec);

/*
	函数说明：
功能：将BCD码转换为10进制数
参数: 
			输入：u8 Bcd   待转换BCD码
返回值：  转换后的10进制数
思路：压缩BCD码一个字符所表示的十进制数据范围为0 ~ 99,进制为100
*/
u8 BcdToDec(u8 Bcd);

/*
函数说明：
功能：根据输入的数据生成bcc码
参数: 
	pData:入口参数，需要生成bcc的数据。
	len： pData的数据长度 (字节单位 )
返回值：返回生成的BCC码
*/
u8 GenerateBccChcek(void * pData,u8 len);

#endif //__COMMUNPRO_H__

