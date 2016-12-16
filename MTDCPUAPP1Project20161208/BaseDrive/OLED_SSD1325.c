/*
*********************************************************************************************************
*
*	模块名称 : OLED显示器驱动模块
*	文件名称 : 
*	版    本 : 
*	说    明 : OLED 屏(128x64)底层驱动程序，驱动芯片型号为 SSD1325.  
*				OLED模块挂在FSMC总线上。
*	修改记录 :
*		版本号  日期        作者    说明
*
*驱动有问题 ，需要在进行修改。//W H H 2016-07-11
*	
*
*********************************************************************************************************
*/

#include "OLED_SSD1325.h"
#include "delay.h"
#include "string.h"
#include "Hz16.h"
#include "ole_font.h"
#include "Uart.h"


/* 下面2个宏任选 1个; 表示显示方向 */
//#define DIR_NORMAL			/* 此行表示正常显示方向 */
//#define DIR_180				/* 此行表示翻转180度 */

//uint8_t LCD_Font_X_Size;
//uint8_t LCD_Font_Y_Size;
//uint8_t LCD_Font_offset;
//const uint8_t *LCD_Font;


u8 OLED_PIXEL[2][65][129];		//WHH 不能修改这个OLED刷屏全局变量
//u8 OLED_STATUS[10][128];
//u8 OLED_LAYER_SHIFT[3][2];

struct OLED_COMPONENT
{
	u8 type;
	u8 x0;
	u8 y0;
	u8 x1;
	u8 y1;
	u8 c0;
	u8 c1;
	u8 z;
	u8 chr[33];
};

u8 record_data[80];

struct OLED_COMPONENT OLED_COMPONENT_LIST[20];		//WHH 内存不够用，OLED暂时没有用到那么多组件，先注释掉，释放些静态内存。
u8 COMPONENT_INDEX = 0x00;

typedef struct 
{
	u8 hour;
	u8 min;
	u8 sec;			
	u8 w_year;
	u8  w_month;
	u8  w_date;
	u8  week;		 
}_calendar_obj;					 
// _calendar_obj calendar;
		

#ifdef OLED_SPI3_EN  //W H H 采用spi的接口定义。本程序采用并口
	/*
		SPI 模式接线定义 (只需要6根杜邦线连接)  本例子采用软件模拟SPI时序

	   【OLED模块排针】 【开发板TFT接口（STM32口线）】
	        VCC ----------- 3.3V
	        GND ----------- GND
	        CS ----------- TP_NCS   (PI10)
	        RST ----------- NRESET (也可以不连接)
					D0/SCK ----------- TP_SCK   (PB3)
					D1/SDIN ----------- TP_MOSI  (PB5)
	*/
	#define RCC_OLED_PORT (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOI)		/* GPIO端口时钟 */

	#define OLED_CS_PORT	GPIOI
	#define OLED_CS_PIN		GPIO_Pin_10

	#define OLED_SCK_PORT	GPIOB
	#define OLED_SCK_PIN	GPIO_Pin_3

	#define OLED_SDIN_PORT	GPIOB
	#define OLED_SDIN_PIN	GPIO_Pin_5


	/* 定义IO = 1和 0的代码 （不用更改） */
	#define SSD_CS_1()		OLED_CS_PORT->BSRRL = OLED_CS_PIN
	#define SSD_CS_0()		OLED_CS_PORT->BSRRH = OLED_CS_PIN

	#define SSD_SCK_1()		OLED_SCK_PORT->BSRRL = OLED_SCK_PIN
	#define SSD_SCK_0()		OLED_SCK_PORT->BSRRH = OLED_SCK_PIN

	#define SSD_SDIN_1()	OLED_SDIN_PORT->BSRRL = OLED_SDIN_PIN
	#define SSD_SDIN_0()	OLED_SDIN_PORT->BSRRH = OLED_SDIN_PIN

#else

	/* 定义LCD驱动器的访问地址
		TFT接口中的RS引脚连接FSMC_A0引脚，由于是16bit模式，RS对应A1地址线，因此
		OLED_RAM的地址是+2
	*/
	#define OLED_BASE   ((uint32_t)(0x6C000000))
	#define OLED_CMD		*(__IO uint16_t *)(OLED_BASE)								// D/C=0: Command
	#define OLED_DATA		*(__IO uint16_t *)(OLED_BASE + (1 << 19))		/* FSMC_A18接D/C ,D/C=1: Data，采用的8bit模式*/

	/*
	1.OLED模块 8080 模式接线定义 (需要15根杜邦线连接）
	   【OLED模块排针】 【开发板TFT接口（STM32口线）】
		    VCC ----------- 3.3V
		    GND ----------- GND
		    CS ----------- NCS 		 (PG12)
	      RST ----------- NRESET (也可以不连接)
				D/C ----------- RS   (FSMC_A18)
		    WE ----------- NWE  (FSMC_NWE)
		    OE ----------- NOE  (FSMC_NOE)
				D0/SCK ----------- DB0  (FSMC_D0)
				D1/SDIN ----------- DB1  (FSMC_D1)
		    D2 ----------- DB2  (FSMC_D2)
		    D3 ----------- DB3  (FSMC_D3)
		    D4 ----------- DB4  (FSMC_D4)
		    D5 ----------- DB5  (FSMC_D5)
		    D6 ----------- DB6  (FSMC_D6)
		    D7 ----------- DB7  (FSMC_D7)
	*/


#endif

/* 12864 OLED的显存镜像，占用1K字节. 共8行，每行128像素 */
//static uint8_t s_ucGRAM[8][128];			//128*8*8bit  用的u8类型，显示16*16的字体,

/* 为了避免刷屏拉幕感太强，引入刷屏标志。
0 表示显示函数只改写缓冲区，不写屏。1 表示直接写屏（同时写缓冲区） */
//static uint8_t s_ucUpdateEn = 1;

static void OLED_ConfigGPIO(void);
static void oled_Command(uint8_t _ucCmd);
static void oled_Data(uint8_t _ucData);

///*
//*********************************************************************************************************
//*	函 数 名: OLED_DispStr
//*	功能说明: 在屏幕指定坐标（左上角为0，0）显示一个字符串
//*	形    参:
//*		_usX : X坐标，对于12864屏，范围为【0 - 127】
//*		_usY : Y坐标，对于12864屏，范围为【0 - 63】
//*		_ptr  : 字符串指针
//*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
//*	返 回 值: 无
//*********************************************************************************************************
//*/
//void OLED_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
//{
//	uint32_t i;
//	uint8_t code1;
//	uint8_t code2;
//	uint32_t address;
//	uint8_t buf[24 * 24 / 8];	/* 最大支持24点阵汉字 */
//	uint8_t m, width;
//	uint8_t font_width,font_height, font_bytes;
//	//	uint16_t x, y;
//	const uint8_t *pAscDot;	

//#ifdef USE_SMALL_FONT		
//	const uint8_t *pHzDot;
//#else	
//	uint32_t AddrHZK;
//#endif	

//	/* 如果字体结构为空指针，则缺省按16点阵 */
//	if (_tFont->FontCode == FC_ST_12)
//	{
//		//		font_height = 12;
//		//		font_width = 12;
//		//		font_bytes = 24;
//		//		pAscDot = g_Ascii12;
//		//	#ifdef USE_SMALL_FONT		
//		//		pHzDot = g_Hz12;
//		//	#else		
//		//		AddrHZK = HZK12_ADDR;
//		//	#endif		
//	}

//	else
//	{
//		/* 缺省是16点阵 */
//		font_height = 16;
//		font_width = 16;
//		font_bytes = 32;
//		pAscDot = szASC16;
//	#ifdef USE_SMALL_FONT		
//		pHzDot = g_Hz16;
//	#else
//		AddrHZK = HZK16_ADDR;
//	#endif
//	}

//	/* 开始循环处理字符 */
//	while (*_ptr != 0)
//	{
//		code1 = *_ptr;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
//		if (code1 < 0x80)
//		{
//			/* 将ascii字符点阵复制到buf */
//			memcpy(buf, &pAscDot[code1 * (font_bytes / 2)], (font_bytes / 2));
//			width = font_width / 2;
//		}
//		else
//		{
//			code2 = *++_ptr;
//			if (code2 == 0)
//			{
//				break;
//			}

//			/* 计算16点阵汉字点阵地址
//				ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32
//				;
//			*/
//			#ifdef USE_SMALL_FONT
//				m = 0;
//				while(1)
//				{
//					address = m * (font_bytes + 2);
//					m++;
//					if ((code1 == pHzDot[address + 0]) && (code2 == pHzDot[address + 1]))
//					{
//						address += 2;
//						memcpy(buf, &pHzDot[address], font_bytes);
//						break;
//					}
//					else if ((pHzDot[address + 0] == 0xFF) && (pHzDot[address + 1] == 0xFF))
//					{
//						/* 字库搜索完毕，未找到，则填充全FF */
//						memset(buf, 0xFF, font_bytes);
//						break;
//					}
//				}
//			#else	/* 用全字库 */
//				/* 此处需要根据字库文件存放位置进行修改 */
//				if (code1 >=0xA1 && code1 <= 0xA9 && code2 >=0xA1)
//				{
//					address = ((code1 - 0xA1) * 94 + (code2 - 0xA1)) * font_bytes + AddrHZK;
//				}
//				else if (code1 >=0xB0 && code1 <= 0xF7 && code2 >=0xA1)
//				{
//					address = ((code1 - 0xB0) * 94 + (code2 - 0xA1) + 846) * font_bytes + AddrHZK;
//				}
//				memcpy(buf, (const uint8_t *)address, font_bytes);
//			#endif

//				width = font_width;
//		}

//		//		y = _usY;
//		//		/* 开始刷LCD */
//		//		for (m = 0; m < font_height; m++)	/* 字符高度 */
//		//		{
//		//			x = _usX;
//		//			for (i = 0; i < width; i++)	/* 字符宽度 */
//		//			{
//		//				if ((buf[m * ((2 * width) / font_width) + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
//		////				if((buf[m*((2 * width) / font_width)+ i / 8 ] & (0x01<<(i%8)))!=0x00)  //W H H
//		//				{
//		////					OLED_PutPixel(x, y, _tFont->FrontColor);	/* 设置像素颜色为文字色 */
//		//					Show_dot(x,y,0xFF);
//		//				}
//		//				else
//		//				{
//		////					OLED_PutPixel(x, y, _tFont->BackColor);	/* 设置像素颜色为文字背景色 */
//		//					Show_dot(x,y,0x00);
//		//				}

//		//				x++;
//		//			}
//		//			y++;
//		//		}
//				
//		for(m = 0; m < font_height; m++)
//		{
//	    oled_Command(0x75); 				/* Set Row Address */
//	    oled_Command(_usY+m); 	 		 /* Start = Row */
//	    oled_Command(_usY+15); 	  /* End = Row+16 */
//		 
//	    oled_Command(0x15); 		/* Set Column Address */
//	    oled_Command(_usX); 	 		 /* Start = Column */
//	    oled_Command(_usX+7); 	  /* End = Column+16 */
//      
//			 for (i=0;i<2;i++) /* 2*8 column , a nibble of command is a dot*/
//	     {
//					con_4_byte(buf[2*m+i]);
//	     }
//		}

//		if (_tFont->Space > 0)
//		{
//			/* 如果文字底色按_tFont->usBackColor，并且字间距大于点阵的宽度，那么需要在文字之间填充(暂时未实现) */
//		}
//		_usX += width/2 + _tFont->Space;	/* 列地址递增 */
//		_ptr++;			/* 指向下一个字符 */
//	}
//}






/*W H H*/

/*
*********************************************************************************************************
*	函 数 名: OLED_ConfigGPIO
*	功能说明: 配置OLED控制口线，设置为8位80XX总线控制模式或SPI模式
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void OLED_ConfigGPIO(void)
{
#ifdef OLED_SPI3_EN
	/* 1.配置GPIO,模拟SPI */
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_OLED_PORT, ENABLE);	/* 打开GPIO时钟 */

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽输出模式 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO口最大速度 */

		GPIO_InitStructure.GPIO_Pin = OLED_CS_PIN;
		GPIO_Init(OLED_CS_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = OLED_SCK_PIN;
		GPIO_Init(OLED_SCK_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = OLED_SDIN_PIN;
		GPIO_Init(OLED_SDIN_PORT, &GPIO_InitStructure);
	}
#else
	/* 1.配置GPIO, 设置为FSMC总线。 只用了其中8位。 */
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* 使能FSMC时钟 */
		RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

		/* 使能 GPIO时钟 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

		/* 设置 PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE),  PD.14(D0), PD.15(D1) 为复用推挽输出 */

		GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);

		GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
		                             GPIO_Pin_14 |GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* 设置 PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7),  为复用推挽输出 */

		GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ;
		GPIO_Init(GPIOE, &GPIO_InitStructure);

		/* 设置 PD.13(A18 (RS))  为复用推挽输出 */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* 设置 PG12 (LCD/CS)) 为复用推挽输出 */
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOG, &GPIO_InitStructure);
	}

	/* 2.配置FSMC总线参数 */
	{
		FSMC_NORSRAMInitTypeDef  init;
		FSMC_NORSRAMTimingInitTypeDef  timing;

		/*-- FSMC Configuration ------------------------------------------------------*/
		/*----------------------- SRAM Bank 4 ----------------------------------------*/
		/* FSMC_Bank1_NORSRAM4 configuration */
		timing.FSMC_AddressSetupTime = 1;
		timing.FSMC_AddressHoldTime = 0;
		timing.FSMC_DataSetupTime = 2;
		timing.FSMC_BusTurnAroundDuration = 0;
		timing.FSMC_CLKDivision = 0;
		timing.FSMC_DataLatency = 0;
		timing.FSMC_AccessMode = FSMC_AccessMode_A;

		/*
		 LCD configured as follow:
		    - Data/Address MUX = Disable
		    - Memory Type = SRAM
		    - Data Width = 	8bit
		    - Write Operation = Enable
		    - Extended Mode = Enable
		    - Asynchronous Wait = Disable
		*/
		init.FSMC_Bank = FSMC_Bank1_NORSRAM4;
		init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
		init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
		init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
		init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
		init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;	/* 注意旧库无这个成员 */
		init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
		init.FSMC_WrapMode = FSMC_WrapMode_Disable;
		init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
		init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
		init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
		init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
		init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

		init.FSMC_ReadWriteTimingStruct = &timing;
		init.FSMC_WriteTimingStruct = &timing;

		FSMC_NORSRAMInit(&init);

		/* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
		FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
	}
#endif
}


// W H H start
/*
*********************************************************************************************************
*	函 数 名: OLED_WriteCmd
*	功能说明: 向SSD1306发送一字节命令
*	形    参:  命令字
*	返 回 值: 无
*********************************************************************************************************
*/
static void oled_Command(uint8_t _ucCmd)
{
#ifdef OLED_SPI3_EN
	uint8_t i;

	SSD_CS_0();

	SSD_SCK_0();
	SSD_SDIN_0();	/* 0 表示后面传送的是命令 1表示后面传送的数据 */
	SSD_SCK_1();

	for (i = 0; i < 8; i++)
	{
		if (_ucCmd & 0x80)
		{
			SSD_SDIN_1();
		}
		else
		{
			SSD_SDIN_0();
		}
		SSD_SCK_0();
		_ucCmd <<= 1;
		SSD_SCK_1();
	}

	SSD_CS_1();
#else
	
	OLED_CMD = _ucCmd;		// FSMC   8080端口模式
#endif
}

/*
*********************************************************************************************************
*	函 数 名: OLED_WriteData
*	功能说明: 向SSD1306发送一字节数据
*	形    参:  命令字
*	返 回 值: 无
*********************************************************************************************************
*/
static void oled_Data(uint8_t _ucData)
{
#ifdef OLED_SPI3_EN
	uint8_t i;

	SSD_CS_0();

	SSD_SCK_0();
	SSD_SDIN_1();	/* 0 表示后面传送的是命令 1表示后面传送的数据 */
	SSD_SCK_1();

	for (i = 0; i < 8; i++)
	{
		if (_ucData & 0x80)
		{
			SSD_SDIN_1();
		}
		else
		{
			SSD_SDIN_0();
		}
		SSD_SCK_0();
		_ucData <<= 1;
		SSD_SCK_1();
	}

	SSD_CS_1();
#else
	
	OLED_DATA = _ucData;		// FSMC   8080端口模式
#endif
}


//-----------------------------------------------------------------------------
//设置窗口位置
//			   StartPoint(xStart,yStart) left up
//         EndPoint(xEnd,yEnd) right down
//-----------------------------------------------------------------------------
void Set_Window(int xStart, int yStart, int xEnd, int yEnd)
{
	oled_Command(0x15);           // Set Column Address
	oled_Command(xStart);         // Begin
	oled_Command(xEnd);           // End

	oled_Command(0x75);           // Set Row Address
	oled_Command(yStart);	  	  	// Begin
	oled_Command(yEnd);           // End
}


//----------------------------------------------------------------------------------------
// 清屏
//----------------------------------------------------------------------------------------
void ClearDisplay(int xStart, int yStart, int xEnd, int yEnd)
{
	oled_Command(0x24);			// Rectangle
	oled_Command(xStart);		// Col Start
	oled_Command(yStart);		// Row Start
	oled_Command(xEnd);			// Col End
	oled_Command(yEnd);			// Row End
	oled_Command(0x00);			// 2 Pixel color (black)
	delay_ms(5);						//
}

/*延时，系统进入空转*/
void Delay_ns(__IO uint32_t del)
{ 
  int32_t i;
  for(i=del/6;i>0;i--){__NOP;}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Instruction Setting  指令设置
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*设置列地址*/
void Set_Column_Address_12864(unsigned char a, unsigned char b)
{
	oled_Command(0x15);			// Set Column Address
	oled_Command(a);			//   Default => 0x00
	oled_Command(b);			//   Default => 0x3F (Total Columns Devided by 2)
}

/*设置行地址*/
void Set_Row_Address_12864(unsigned char a, unsigned char b)
{
	oled_Command(0x75);			// Set Row Address
	oled_Command(a);				//   Default => 0x00
	oled_Command(b);				//   Default => 0x4F
}

/*设置当前对比度*/
void Set_Contrast_Current_12864(unsigned char d)
{
	oled_Command(0x81);			// Set Contrast Value
	oled_Command(d);				//   Default => 0x40
}	

/*设置当前范围*/
void Set_Current_Range_12864(unsigned char d)
{
	oled_Command(0x84|d);			// Set Current Range
						//   Default => 0x84
						//     0x84 (0x00) => Quarter Current Range
						//     0x85 (0x01) => Half Current Range
						//     0x86 (0x02) => Full Current Range
}

/*设置映射格式*/
void Set_Remap_Format_12864(unsigned char d)
{
	oled_Command(0xA0);			// Set Re-Map & Data Format
	oled_Command(d);			//   Default => 0x00
						//     Column Address 0 Mapped to SEG0
						//     Disable Nibble Re-Map
						//     Horizontal Address Increment
						//     Scan from COM0 to COM[N-1]
						//     Disable COM Split Odd Even
}

/*设置起始行*/
void Set_Start_Line_12864(unsigned char d)
{
	oled_Command(0xA1);			// Set Display Start Line
	oled_Command(d);			//   Default => 0x00
}

/*设置显示偏移位置*/
void Set_Display_Offset_12864(unsigned char d)
{
	oled_Command(0xA2);			// Set Display Offset
	oled_Command(d);			//   Default => 0x00
}

/*设置显示模式*/
void Set_Display_Mode_12864(unsigned char d)
{
	oled_Command(0xA4|d);			// Set Display Mode
						//   Default => 0xA4
						//     0xA4 (0x00) => Normal Display
						//     0xA5 (0x01) => Entire Display On, All Pixels Turn On at GS Level 15
						//     0xA6 (0x02) => Entire Display Off, All Pixels Turn Off
						//     0xA7 (0x03) => Inverse Display
}

/*设置多路复用率*/
void Set_Multiplex_Ratio_12864(unsigned char d)
{
	oled_Command(0xA8);			// Set Multiplex Ratio
	oled_Command(d);			//   Default => 0x5F
}

/*设置主配置*/
void Set_Master_Config_12864(unsigned char d)
{
	oled_Command(0xAD);			// Set Master Configuration
	oled_Command(0x02|d);			//   Default => 0x03
						//     0x02 (0x00) => Select External VCC Supply
						//     0x03 (0x01) => Select Internal DC/DC Voltage Converter
}

/*设置显示开关*/
void Set_Display_On_Off_12864(unsigned char d)
{
	oled_Command(0xAE|d);			// Set Display On/Off
						//   Default => 0xAE
						//     0xAE (0x00) => Display Off
						//     0xAF (0x01) => Display On
}

/*设置相位长度*/
void Set_Phase_Length_12864(unsigned char d)
{
	oled_Command(0xB1);			// Phase 1 & 2 Period Adjustment
	oled_Command(d);				//   Default => 0x53 (5 Display Clocks [Phase 2] / 3 Display Clocks [Phase 1])
						//     D[3:0] => Phase 1 Period in 1~15 Display Clocks
						//     D[7:4] => Phase 2 Period in 1~15 Display Clocks
}

/*设置帧频率*/
void Set_Frame_Frequency_12864(unsigned char d)
{
	oled_Command(0xB2);			// Set Frame Frequency (Row Period)
	oled_Command(d);				//   Default => 0x25 (37 Display Clocks)
}

/*设置显示的时钟*/
void Set_Display_Clock_12864(unsigned char d)
{
	oled_Command(0xB3);			// Display Clock Divider/Osciallator Frequency
	oled_Command(d);				//   Default => 0x41
						//     D[3:0] => Display Clock Divider
						//     D[7:4] => Oscillator Frequency
}

/*设置预先充电补偿*/
void Set_Precharge_Compensation_12864(unsigned char a, unsigned char b)
{
	oled_Command(0xB4);			// Set Pre-Charge Compensation Level
	oled_Command(b);				//   Default => 0x00 (No Compensation)

	if(a == 0x20)
	{
		oled_Command(0xB0);		// Set Pre-Charge Compensation Enable
		oled_Command(0x08|a);	//   Default => 0x08
						//     0x08 (0x00) => Disable Pre-Charge Compensation
						//     0x28 (0x20) => Enable Pre-Charge Compensation
	}
}

/*设置充电电压*/
void Set_Precharge_Voltage_12864(unsigned char d)
{
	oled_Command(0xBC);			// Set Pre-Charge Voltage Level
	oled_Command(d);			//   Default => 0x10 (Connect to VCOMH)
}

/*设置输出com电压*/
void Set_VCOMH_12864(unsigned char d)
{
	oled_Command(0xBE);			// Set Output Level High Voltage for COM Signal
	oled_Command(d);			//   Default => 0x1D (0.81*VREF)
}

/*设置段的低电压*/
void Set_VSL_12864(unsigned char d)
{
	oled_Command(0xBF);			// Set Segment Low Voltage Level
	oled_Command(0x02|d);			//   Default => 0x0E
						//     0x02 (0x00) => Keep VSL Pin Floating
						//     0x0E (0x0C) => Connect a Capacitor between VSL Pin & VSS
}

/*无命令*/
void Set_NOP_12864(void)
{
	oled_Command(0xE3);			// Command for No Operation
}

/*图形加速命令操作*/
void GA_Option_12864(unsigned char d)
{
	oled_Command(0x23);			// Graphic Acceleration Command Options
	oled_Command(d);			//   Default => 0x01
						//     Enable Fill Rectangle
						//     Disable Wrap around in Horizontal Direction During Copying & Scrolling
						//     Disable Reverse Copy
}

/*画矩形*/
void Draw_Rectangle_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
	oled_Command(0x24);			// Draw Rectangle
	oled_Command(a);			//   Column Address of Start
	oled_Command(c);			//   Row Address of Start
	oled_Command(b);			//   Column Address of End (Total Columns Devided by 2)
	oled_Command(d);			//   Row Address of End
	oled_Command(e);			//   Gray Scale Level
	delay_us(200);
}

/*将一块区域，复制到另一块区域*/
void Copy_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f)
{
	oled_Command(0x25);	// Copy
	oled_Command(a);			//   Column Address of Start
	oled_Command(c);			//   Row Address of Start
	oled_Command(b);			//   Column Address of End (Total Columns Devided by 2)
	oled_Command(d);			//   Row Address of End
	oled_Command(e);			//   Column Address of New Start
	oled_Command(f);			//   Row Address of New Start
	delay_us(200);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)	填充屏幕
//
//    a: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM_12864(unsigned char a)
{
	GA_Option_12864(0x01);
	Draw_Rectangle_12864(0x00,0x3F,0x00,0x5F,a);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)	填充背景
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//    e: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e)
{
	GA_Option_12864(0x01);
	Draw_Rectangle_12864(a,b,c,d,e);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)	检测12864
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Checkerboard_12864(void)
{
unsigned char i,j;
	Set_Column_Address_12864(0x00,0x3F);
	Set_Row_Address_12864(0x00,0x5F);

	for(i=0;i<40;i++)
	{
		for(j=0;j<64;j++)
		{
			oled_Data(0xF0);
		}
		for(j=0;j<64;j++)
		{
			oled_Data(0x0F);
		}
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)	显示灰度条
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale_12864(void)
{
	//   Level 16 => Column 1~8
		Fill_Block_12864(0x00,0x03,0x00,0x3F,0xFF);

	//   Level 15 => Column 9~16
		Fill_Block_12864(0x04,0x07,0x00,0x3F,0xEE);

	//   Level 14 => Column 17~24
		Fill_Block_12864(0x08,0x0B,0x00,0x3F,0xDD);

	//   Level 13 => Column 25~32
		Fill_Block_12864(0x0C,0x0F,0x00,0x3F,0xCC);

	//   Level 12 => Column 33~40
		Fill_Block_12864(0x10,0x13,0x00,0x3F,0xBB);

	//   Level 11 => Column 41~48
		Fill_Block_12864(0x14,0x17,0x00,0x3F,0xAA);

	//   Level 10 => Column 49~56
		Fill_Block_12864(0x18,0x1B,0x00,0x3F,0x99);

	//   Level 9 => Column 57~64
		Fill_Block_12864(0x1C,0x1F,0x00,0x3F,0x88);

	//   Level 8 => Column 65~72
		Fill_Block_12864(0x20,0x23,0x00,0x3F,0x77);

	//   Level 7 => Column 73~80
		Fill_Block_12864(0x24,0x27,0x00,0x3F,0x66);

	//   Level 6 => Column 81~88
		Fill_Block_12864(0x28,0x2B,0x00,0x3F,0x55);

	//   Level 5 => Column 89~96
		Fill_Block_12864(0x2C,0x2F,0x00,0x3F,0x44);

	//   Level 4 => Column 97~104
		Fill_Block_12864(0x30,0x33,0x00,0x3F,0x33);

	//   Level 3 => Column 105~112
		Fill_Block_12864(0x34,0x37,0x00,0x3F,0x22);

	//   Level 2 => Column 113~120
		Fill_Block_12864(0x38,0x3B,0x00,0x3F,0x11);

	//   Level 1 => Column 121~128
		Fill_Block_12864(0x3C,0x3F,0x00,0x3F,0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Character (5x7)		显示字符
//
//    a: Database
//    b: Ascii
//    c: Start X Address
//    d: Start Y Address
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Font57_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
unsigned char *Src_Pointer=0;
unsigned char i,Font,MSB,LSB;

	switch(a)
	{
		case 1:
			Src_Pointer=(unsigned char *)&Ascii_1[(b-1)][0];		//W H H
			break;
		case 2:
			Src_Pointer=(unsigned char *)&Ascii_2[(b-1)][0];
			break;
	}

	Set_Remap_Format_12864(0x54);
	for(i=0;i<=4;i+=2)
	{
		LSB=*Src_Pointer;
		Src_Pointer++;
		if(i == 4)
		{
			MSB=0x00;
		}
		else
		{
			MSB=*Src_Pointer;
			Src_Pointer++;
		}
 		Set_Column_Address_12864(c,c);
		Set_Row_Address_12864(d,d+7);

		Font=((MSB&0x01)<<4)|(LSB&0x01);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_Data(Font);

		Font=((MSB&0x02)<<3)|((LSB&0x02)>>1);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_Data(Font);

		Font=((MSB&0x04)<<2)|((LSB&0x04)>>2);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_Data(Font);

		Font=((MSB&0x08)<<1)|((LSB&0x08)>>3);
		Font=Font|(Font<<1)|(Font<<2)|(Font<<3);
		oled_Data(Font);

		Font=((MSB&0x10)<<3)|((LSB&0x10)>>1);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_Data(Font);

		Font=((MSB&0x20)<<2)|((LSB&0x20)>>2);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_Data(Font);

		Font=((MSB&0x40)<<1)|((LSB&0x40)>>3);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_Data(Font);

		Font=(MSB&0x80)|((LSB&0x80)>>4);
		Font=Font|(Font>>1)|(Font>>2)|(Font>>3);
		oled_Data(Font);
		c++;
	}
	Set_Remap_Format_12864(0x50);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show String
//
//    a: Database
//    b: Start X Address
//    c: Start Y Address
//    * Must write "0" in the end...
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_String_12864(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c)
{
unsigned char *Src_Pointer;

	Src_Pointer=Data_Pointer;
	Show_Font57_12864(1,96,b,c);			// No-Break Space
						//   Must be written first before the string start...

	while(1)
	{
		Show_Font57_12864(a,*Src_Pointer,b,c);
		Src_Pointer++;
		b+=3;
		if(*Src_Pointer == 0) break;
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Frame (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Frame_12864(void)			//W H H
{
	GA_Option_12864(0x00);
	Draw_Rectangle_12864(0x00,0x3F,0x00,0x3F,0xFF);
	Fill_Block_12864(0x00,0x00,0x01,0x3E,0x0F);
	Fill_Block_12864(0x3F,0x3F,0x01,0x3E,0xF0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Divided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//		k: 0,正常显示，1，相反的显示
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern_12864(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d,unsigned char k)
{
unsigned char *Src_Pointer;
unsigned char i,j;
	
	Src_Pointer=Data_Pointer;
	Set_Column_Address_12864(a,b);
	Set_Row_Address_12864(c,d);
	switch(k)
  {
		case 0x00:
			for(i=0;i<(d-c+1);i++)
			{
				for(j=0;j<(b-a+1);j++)
				{
					oled_Data(*Src_Pointer);
					Src_Pointer++;
				}
			}
			break;
		case 0x01:
			for(i=0;i<(d-c+1);i++)
			{
				for(j=0;j<(b-a+1);j++)
				{
					oled_Data(~(*Src_Pointer));
					Src_Pointer++;
				}
			}
			break;		
	}
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)
//       "0x01" (Downward)
//    b: Set Numbers of Row Scroll per Step
//    c: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll_12864(unsigned char a, unsigned char b, unsigned char c)
{
	unsigned int i,j;	

	switch(a)
	{
		case 0:
			for(i=0;i<80;i+=b)
			{
				Set_Start_Line_12864(i);
				for(j=0;j<c;j++)
				{
					delay_us(200);
				}
			}
			break;
		case 1:
			for(i=0;i<80;i+=b)
			{
				Set_Start_Line_12864(80-i);
				for(j=0;j<c;j++)
				{
					delay_us(200);
				}
			}
			break;
	}
	Set_Start_Line_12864(0x00);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Continuous Horizontal Scrolling (Partial or Full Screen)
//
//    a: Set Numbers of Column Scroll per Step
//    b: Set Numbers of Row to Be Scrolled
//    c: Set Time Interval between Each Scroll Step in Terms of Frame Frequency
//    d: Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Horizontal_Scroll_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	GA_Option_12864(0x03);
	oled_Command(0x26);			// Horizontal Scroll Setup
	oled_Command(a);
	oled_Command(b);
	oled_Command(c);
	oled_Command(0x2F);			// Activate Scrolling
	delay_us(d);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Continuous Horizontal Fade Scrolling (Partial or Full Screen)
//
//    a: Set Numbers of Column Scroll per Step
//    b: Set Numbers of Row to Be Scrolled
//    c: Set Time Interval between Each Scroll Step in Terms of Frame Frequency
//    d: Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Scroll_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	GA_Option_12864(0x01);
	oled_Command(0x26);			// Horizontal Scroll Setup
	oled_Command(a);
	oled_Command(b);
	oled_Command(c);
	oled_Command(0x2F);			// Activate Scrolling
	delay_us(d);
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Deactivate Scrolling (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Deactivate_Scroll_12864(void)
{
	oled_Command(0x2E);			// Deactivate Scrolling
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade In (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_In_12864(void)
{
unsigned int i;	

	Set_Display_On_Off_12864(0x01);
	for(i=0;i<(Brightness+1);i++)
	{
		Set_Contrast_Current_12864(i);
		delay_us(200);
		delay_us(200);
		delay_us(200);
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade Out (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Out_12864(void)
{
unsigned int i;	

	for(i=(Brightness+1);i>0;i--)
	{
		Set_Contrast_Current_12864(i-1);
		delay_us(200);
		delay_us(200);
		delay_us(200);
	}
	Set_Display_On_Off_12864(0x00);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Sleep Mode
//
//    "0x01" Enter Sleep Mode
//    "0x00" Exit Sleep Mode
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Sleep_12864(unsigned char a)
{
	switch(a)
	{
		case 0:
			Set_Display_On_Off_12864(0x00);
			Set_Display_Mode_12864(0x01);
			break;
		case 1:
			Set_Display_Mode_12864(0x00);
			Set_Display_On_Off_12864(0x01);
			break;
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table_12864(void)
{
	oled_Command(0xB8);			// Set Gray Scale Table
	oled_Command(0x01);			//   Gray Scale Level 1
	oled_Command(0x11);			//   Gray Scale Level 3 & 2
	oled_Command(0x22);			//   Gray Scale Level 5 & 4
	oled_Command(0x32);			//   Gray Scale Level 7 & 6
	oled_Command(0x43);			//   Gray Scale Level 9 & 8
	oled_Command(0x54);			//   Gray Scale Level 11 & 10
	oled_Command(0x65);			//   Gray Scale Level 13 & 12
	oled_Command(0x76);			//   Gray Scale Level 15 & 14
}

/*W H H*/

/*
*********************************************************************************************************
*	函 数 名: OLED_12864_Init()
*	功能说明: 初始化OLED屏
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Initialization

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void OLED_12864_Init(void)
{
	OLED_ConfigGPIO();		//GPIO的初始化

	/* 上电延迟 */
	delay_ms(20);
 
	Set_Display_On_Off_12864(0x00);		// Display Off (0x00/0x01)
	Set_Display_Clock_12864(0x91);		// Set Clock as 135 Frames/Sec
	Set_Multiplex_Ratio_12864(0x3F);	// 1/64 Duty (0x0F~0x5F)
	Set_Display_Offset_12864(0x4C);		// Shift Mapping RAM Counter (0x00~0x5F)
	Set_Start_Line_12864(0x00);				// Set Mapping RAM Display Start Line (0x00~0x5F)
	Set_Master_Config_12864(0x00);		// Disable Embedded DC/DC Converter (0x00/0x01)
	Set_Remap_Format_12864(0x50);			// Set Column Address 0 Mapped to SEG0
																		//     Disable Nibble Remap
																		//     Horizontal Address Increment
																		//     Scan from COM[N-1] to COM0
																		//     Enable COM Split Odd Even
	Set_Current_Range_12864(0x02);		// Set Full Current Range
	Set_Gray_Scale_Table_12864();			// Set Pulse Width for Gray Scale Table
	Set_Contrast_Current_12864(/*0x7f*/Brightness);	// Set Scale Factor of Segment Output Current Control
	Set_Frame_Frequency_12864(0x51);	// Set Frame Frequency
	Set_Phase_Length_12864(0x55);			// Set Phase 1 as 5 Clocks & Phase 2 as 5 Clocks
	Set_Precharge_Voltage_12864(0x10);// Set Pre-Charge Voltage Level
	Set_Precharge_Compensation_12864(0x20,0x02);	// Set Pre-Charge Compensation
	Set_VCOMH_12864(0x1C);						// Set High Voltage Level of COM Pin
	Set_VSL_12864(0x0D);							// Set Low Voltage Level of SEG Pin
	Set_Display_Mode_12864(0x00);			// Normal Display Mode (0x00/0x01/0x02/0x03)

	Fill_RAM_12864(0x00);							// Clear Screen

	Set_Display_On_Off_12864(0x01);		// Display On (0x00/0x01)
}




/******************************************************
      写满oled
*************************************************/
void Fill_Screen_12864(unsigned char m)
{
    unsigned int j,i;
 //Column Address
	    oled_Command(0x15); 	/* Set Column Address */
	    oled_Command(0x00); 	  /* Start = 0 */
	    oled_Command(0x3F); 	  /* End = 127 */
// Row Address
			oled_Command(0x75); 	/* Set Row Address */
	    oled_Command(0x00); 	  /* Start = 0 */
	    oled_Command(0x50); 	  /* End = 80 */
	
			for (j=0;j<80;j++) /* 80 row */
			{
				 for (i=0;i<64;i++) /* 64*2=128 column  a nibble of command is a dot*/
					{
						oled_Data(m);
					}
			}
}

/**************************************
  转换子程序
****************************************/
void con_4_byte(unsigned char DATA)			//相当于发送1字节的数据
{
   unsigned char data_4byte[4];
   unsigned char i;
   unsigned char d,k;
   d=DATA;
 
  for(i=0;i<4;i++)  // 一两位的方式写入  2*4=8位
  {
     k=d&0xc0;    //当K=0时 为D7,D6位 当K=1时 为D5,D4位

  /****出现4种可能，16阶色素,一个地址两个像素，一个像素对应4位***/

     switch(k)
     {
			 case 0x00:
					 data_4byte[i]=0x00;
//					 delay_us(20);
						 break;
				 case 0x40:  // 0100,0000
					 data_4byte[i]=0x0f;
//					 delay_us(20);
						 break;	
			 case 0x80:  //1000,0000
					 data_4byte[i]=0xf0;
//					 delay_us(20);
						 break;
				 case 0xc0:   //1100,0000
					 data_4byte[i]=0xff;
//					 delay_us(20);
						 break;	 
				 default:
						 break;
	   }
      d=d<<2;                               		//左移两位
      
	  
	  oled_Data(data_4byte[i]);                  /* 8 column  a nibble of command is a dot*/
  }

}



void con_8_byte(unsigned char DATA)
{
   unsigned char data_4byte[8];
   unsigned char i;
   unsigned char d,k;
   d=DATA;
 
  for(i=0;i<8;i++)   // ????????  2*4=8?
  {
     k=d&0xc0;     //?i=0? ?D7,D6? ?i=1? ?D5,D4?

     /****?4???,16???,????????????,???????????4?***/

     switch(k)
     {
				case 0x00:
				 data_4byte[i]=0x00;
				 delay_us(20);
					 break;
			 case 0x40:  // 0100,0000
				 data_4byte[i]=0x0f;
				 delay_us(20);
					 break;	
			 case 0x80:  //1000,0000
				 data_4byte[i]=0xf0;
				 delay_us(20);
					 break;
			 case 0xc0:   //1100,0000
				 data_4byte[i]=0xff;
				 delay_us(20);
					 break;	 
			 default:
					 break;
	   }
      d=d<<1;                                //????
      
	  
	  oled_Data(data_4byte[i]);                /* 8 column  a nibble of command is a dot*/
   }

}

void Show_dot(unsigned char x, unsigned char y, unsigned char color)
{
    Draw_Rectangle_12864(x,x,y,y,color);
}

///****************************************************
//   写入一个16*16汉字
//******************************************************/
//void OLED_Draw_16x16Char(unsigned char x,unsigned char y,unsigned char coder)//write chinese word of1616
//{
//    unsigned char i,j;

//	for(j=0;j<16;j++)
//	 {
//	    oled_Command(0x75); 				/* Set Row Address */
//	    oled_Command(y+j); 	 		 /* Start = Row */
//	    oled_Command(y+15); 	  /* End = Row+16 */
//		 
//	    oled_Command(0x15); 		/* Set Column Address */
//	    oled_Command(x); 	 		 /* Start = Column */
//	    oled_Command(x+7); 	  /* End = Column+16 */
//      
//			 for (i=0;i<2;i++) /* 2*8 column , a nibble of command is a dot*/
//	     {
//					con_4_byte(g_DrawHz16[coder][(j<<1)+i]);
//	     }
//	 }
//}

///****************************************************
//   写入一串16*16汉字
//******************************************************/
//void OLED_Draw_16x16String(unsigned char x,unsigned char y,unsigned char m,unsigned char endm)
//{
//  unsigned char i;
//  for(i=m;i<endm;i++)
//  {
//		OLED_Draw_16x16Char(x,y,i);
//		x=x+8;            // 8*2=16间隔一个汉字
//  }
//}

/****************************************************
   写入一个8*16字母
******************************************************/
void OLED_Draw_8x16Char(unsigned char x,unsigned char y,unsigned char coder)//write chinese word of1616
{
   unsigned char /*i,*/j;
	 for(j=0;j<16;j++)
	 {
	    oled_Command(0x75); 		/* Set Row Address */
	    oled_Command(y+j); 	 	  /* Start = Row */
	    oled_Command(y+15); 	  /* End = Row+8 */
		 
	    oled_Command(0x15); 		/* Set Column Address */
	    oled_Command(x); 	  		/* Start = Column */
	    oled_Command(x+3); 	 	  /* End = Column+8 */
      con_4_byte(szASC16[16*coder+j]);
	 }

}





void Next_Frame(void)
{
	u8 i,j,temp;

	Set_Remap_Format_12864(0x50);
	Set_Column_Address_12864(0x00, 0x3F);
	Set_Row_Address_12864(0x00, 0x7F);
	
	for(i=0;i<64;i++)
	{
		for(j=0;j<128;j+=2)
		{
			temp = 0x00;
			temp |= OLED_PIXEL[0][i][j]>>4|(OLED_PIXEL[0][i][j+1]>>4<<4);
			//if(OLED_PIXEL[1][i][j]) 	temp |= OLED_PIXEL[1][i][j]>>4;
			//if(OLED_PIXEL[1][i][j+1]) temp |= OLED_PIXEL[1][i][j+1]>>4<<4;
			if(OLED_PIXEL[1][i][j])		temp = (temp&0xF0)|OLED_PIXEL[1][i][j]>>4;
			if(OLED_PIXEL[1][i][j+1]) temp = (temp&0x0F)|OLED_PIXEL[1][i][j+1]>>4<<4;
			oled_Data(temp);
			//oled_Data(((u8)OLED_PIXEL[i][j+1]<<4));
			//oled_Data(OLED_PIXEL[i][j]>>4|OLED_PIXEL[i][j+1]>>4<<4);
			//oled_Data(OLED_PIXEL[i][j]>>4|OLED_PIXEL[i][j+1]>>4<<4);
		}
	}
}

void Show_XGS(void)
{
	u8 i,j;
	
	for(i=0;i<64;i++)
		for(j=0;j<128;j++)
			OLED_PIXEL[0][i][j]=(i+j)*0xFF/192;
}

void Draw_Dot(u8 x, u8 y, u8 z, u8 color)
{
	OLED_PIXEL[z][x][y] = color;
}

void Clean_Screen(u8 x0, u8 y0, u8 x1, u8 y1, u8 z)
{
	u8 i,j;
	
	for(i=x0;i<x1;i++)
		for(j=y0;j<y1;j++)
			OLED_PIXEL[z][i][j] = 0x00;
}

void Draw_Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color)
{
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';
	for(t=0;t<6;t++)
	{
		temp=F6x8[chr][t];
		//if(size==12)temp=oled_asc2_1206[chr][t];
		//else temp=oled_asc2_1608[chr][t];
		y=y0+8;
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_PIXEL[z][y][x] = font_color;
			else OLED_PIXEL[z][y][x] = back_color;
			temp<<=1;
			y--;
			if(y==y0)
			{
				y=y0+8;
				x++;
				break;
			}
		}
	}         
}

void Draw_5x7Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color)
{
	u8 temp,t,t1;
	chr=chr-' ';
	for(t=0;t<5;t++)
	{
		temp=Ascii_1[chr][t]<<1;  /*F5x7[chr][t]<<1;*/
		//if(size==12)temp=oled_asc2_1206[chr][t];
		//else temp=oled_asc2_1608[chr][t];
		for(t1=0;t1<7;t1++)
		{
			if(temp&0x80)OLED_PIXEL[z][x+(7-t1)][y+t] = font_color;
			else OLED_PIXEL[z][x+(7-t1)][y+t] = back_color;
			temp<<=1;
		}
	}         
}

void Draw_4x6Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color)
{
	u8 temp,t,t1;
	chr=chr-' ';
	if(y+4>128)return;
	if(x+6>64)return;
	for(t=0;t<6;t++)
	{
		temp=F4x6[chr][t/2]<<((t%2)*4);
		//if(size==12)temp=oled_asc2_1206[chr][t];
		//else temp=oled_asc2_1608[chr][t];
		for(t1=0;t1<4;t1++)
		{
			if(temp&0x80)OLED_PIXEL[z][x+t][y+t1] = font_color;
			else OLED_PIXEL[z][x+t][y+t1] = back_color;
			temp<<=1;
		}
	}         
}

void Draw_16x16Char(u8 x,u8 y,u8 z,u8 chr1,u8 chr2,u8 font_color,u8 back_color)
{
	u16 temp,t,t1;

	//uint32_t i;
	uint8_t code1;
	uint8_t code2;
	uint32_t address;
	uint8_t buf[16*16/ 8];	/* 最大支持16点阵汉字 */
	uint8_t m, width;
	uint8_t font_width,font_height, font_bytes;
	const uint8_t *pAscDot;	
	const uint8_t *pHzDot;
	
	font_height = 16;
	font_width = 16;
	font_bytes = 32;
	pAscDot = szASC16;
	pHzDot = g_Hz16;
	
		/* 开始循环处理字符 */
	code1 = chr1;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
		if (code1 < 0x80)
		{
			/* 将ascii字符点阵复制到buf */
			memcpy(buf, &pAscDot[code1 * (font_bytes / 2)], (font_bytes / 2));
			width = font_width / 2;
			
			for(t=0;t<font_height;t++)
			{
				temp = buf[t];
				for(t1=0;t1<width;t1++)
				{
					if(temp&0x80)OLED_PIXEL[z][x+t][y+t1] = font_color;
					else OLED_PIXEL[z][x+t][y+t1] = back_color;
					temp<<=1;
				}
			}
			
		}
		else
		{
				code2 = chr2;
				if (code2 == 0)
				{
					return ;
				}

			/* 计算16点阵汉字点阵地址
				ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32;
			*/		
			//开始搜索字库
				m = 0;
				while(1)
				{
					address = m * (font_bytes + 2);
					m++;
					if ((code1 == pHzDot[address + 0]) && (code2 == pHzDot[address + 1]))
					{
						address += 2;
						memcpy(buf, &pHzDot[address], font_bytes);
						break;
					}
					else if ((pHzDot[address + 0] == 0xFF) && (pHzDot[address + 1] == 0xFF))
					{
						/* 字库搜索完毕，未找到，则填充全FF */
						memset(buf, 0x00, font_bytes);
						break;
					}
				}
		
				for(t=0;t<font_height;t++)
				{
					temp = buf[2*t]<<8;
					temp |=buf[2*t+1]&0xff;
					for(t1=0;t1<font_width;t1++)
					{
						if(temp&0x8000)OLED_PIXEL[z][x+t][y+t1] = font_color;
						else OLED_PIXEL[z][x+t][y+t1] = back_color;
						temp<<=1;
					}
				}
		}
}

//void Draw_Icon(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color)
//{
//	u8 temp,t,t1;
//	for(t=0;t<5;t++)
//	{
//		temp=Icon[chr][t];
//		for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)OLED_PIXEL[z][x+t][y+t1] = font_color;
//			else OLED_PIXEL[z][x+t][y+t1] = back_color;
//			temp<<=1;
//		}
//	}         
//}

void Draw_4x6String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color)
{
	u8 j;

    for (j = 0; chr[j] != '\0' && y+j*4<128; j++)
    {    
        Draw_4x6Char(x,y+j*4,z,chr[j],font_color,back_color);
    }     
}

void Draw_5x7String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color)
{
	u8 j;

    for (j = 0; chr[j] != '\0'; j++)
    {    
        Draw_5x7Char(x,y+j*6,z,chr[j],font_color,back_color);
    }     
}

void Draw_String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color)
{
	u8 j;

    for (j = 0; chr[j] != '\0' && x+j*6<128; j++)
    {
			Draw_Char(x+j*6,y,z,chr[j],font_color,back_color);
    }     
}

void Draw_16x16String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color)
{
	u8 j;
	u8 k;
	k=0;
		/* 开始循环处理字符 */
		for(j=0;chr[j]!='\0'/*&& x+j*8<128*/;j++)
		{
			k++;
			if(chr[j]<0x80)
			{
				Draw_16x16Char(x,y+j*8,z,chr[j],chr[j+1],font_color,back_color);
			}
			else
			{
				Draw_16x16Char(x,y+j*8,z,chr[j],chr[j+1],font_color,back_color);
				j++;//如果是汉字需要加两次
			}

		}
}

//void Draw_Notification(u8 title[], u8 chr[])
//{
//	u8 i,j, x, y;
//	
//	Clean_Screen(0, 0, 64, 128, 1);
//	
//	for (j=8; j<=119; j++)
//		for(i=8;i<=55;i++)
//		OLED_PIXEL[1][i][j] = 0x01;
//	
//	for (j=8; j<=119; j++)
//	{    
//		OLED_PIXEL[1][7][j] = 0xFF;
//		OLED_PIXEL[1][56][j] = 0xFF;
//		OLED_PIXEL[1][19][j] = 0xFF;
//		OLED_PIXEL[1][57][j] = 0x80;
//	} 
//	
//	for (j=8; j<=55; j++)
//	{    
//		OLED_PIXEL[1][j][7] = 0xFF;
//		OLED_PIXEL[1][j][120] = 0xFF;
//	} 
//	
//	OLED_PIXEL[1][56][7] = 0x80;
//	OLED_PIXEL[1][56][120] = 0x80;

//	Draw_String(11,9,1,title,0xFF,0x01);
//	
//	x = 0; y = 0;
//	for (j = 0; chr[j] != '\0'; j++)
//	{    
//		switch(chr[j])
//		{
//			case '|':
//				y++;
//				x = 0;
//				break;
//			default:
//				Draw_Char(11+x*6,22+y*8,1,chr[j],0xFF,0x01);
//				x++;
//		}
//	}   
//	
//	Next_Frame();
//	
//	while(!Keyboard_Now) Update_Keyboard();
//	while(Keyboard_Now) Update_Keyboard();
//	
//	Clean_Screen(0, 0, 64, 128, 1);
//	Next_Frame();
//}

void Draw_Reverse(u8 x0, u8 y0, u8 x1, u8 y1, u8 z)
{
	u8 i, j;
	for(i=x0;i<=x1;i++)
		for(j=y0;j<=y1;j++)
			OLED_PIXEL[z][i][j] = ~OLED_PIXEL[z][i][j];
}


//u8 Draw_Menu(u8 menu[][12], u8 x0, u8 y0, u8 x1, u8 y1, u8 default_select)
//{
//	u8 n = menu[0][0], dx;
//	u8 temp,t,t1;
//	u8 i, j, x = 0, y = 0, select = 1, startline = 0;

//	dx = x1-x0;

//	if(default_select>0)select=default_select;

//	Clean_Screen(0, 0, 64, 128, 1);

//	while(Keyboard_Now != 'e' && Keyboard_Now != 's')
//	{
//		u8 temp_x0;
//		
//		if((select-1)*8<startline)startline=(select-1)*8;
//		if(select*8>startline+dx)startline=select*8-dx;
//		
//		for (j=x0; j<=x1; j++)
//			for(i=y0;i<=y1;i++)
//				OLED_PIXEL[1][j][i] = 0x01;
//		
//		x=y0+1;
//		for (j = 0; menu[startline/8+1][j] != '\0'; j++)
//			for(t=0;t<6;t++)
//			{
//				temp=F6x8[menu[startline/8+1][j]-' '][t];
//				temp>>=startline%8;
//				temp<<=startline%8;
//				y=x0+8-startline%8;
//				for(t1=0;t1<8-startline%8;t1++)
//				{
//					if(temp&0x80 && t1<8-startline%8){
//						OLED_PIXEL[1][y][x] = 0xFE;
//					}else 
//						OLED_PIXEL[1][y][x] = 0x01;
//					temp<<=1;
//					y--;
//					if(y==x0)
//					{
//						//y=i*8;
//						x++;
//						break;
//					}
//				}
//			}   
//	
//		x=0;
//		for(i=startline/8+1;i<(dx-8+startline%8)/8+startline/8+2;i++)
//		{
//			x++;
//			Draw_String(y0+1,x0+x*8-startline%8,1,menu[i+1],0xFE,0x01);
//		}

//		temp_x0 = (select-(startline/8)-1)*8+(8-startline%8)+x0-7;

//		Draw_Reverse(temp_x0, y0+1, temp_x0+7, y1-1, 1);
//		
//		for (j=x1+2; j<64; j++)
//			for(i=y0;i<=y1;i++)
//				OLED_PIXEL[1][j][i] = 0x00;

//		for (j=y0+1; j<=y1-1; j++)
//		{    
//			OLED_PIXEL[1][x0][j] = 0xFF;
//			OLED_PIXEL[1][x1][j] = 0xFF;
//			OLED_PIXEL[1][x1+1][j] = 0x80;
//		} 
//		
//		for (j=x0+1; j<=x1-1; j++)
//		{    
//			OLED_PIXEL[1][j][y0] = 0xFF;
//			OLED_PIXEL[1][j][y1] = 0xFF;
//		} 
//		
//		OLED_PIXEL[1][x1][y0] = 0x80;
//		OLED_PIXEL[1][x1][y1] = 0x80;

//		Update_Keyboard();
//		Next_Frame();
//		
//		if(Keyboard_Now == 'u' && select > 1){
//			select --;
//			while(Keyboard_Now == 'u')Update_Keyboard();
//		}
//		if(Keyboard_Now == 'd' && select < n){
//			select ++;
//			while(Keyboard_Now == 'd')Update_Keyboard();
//		}
//		
//	}
//	if(Keyboard_Now == 's')select = default_select;
//	while(Keyboard_Now == 'e' || Keyboard_Now == 's') Update_Keyboard();
//	//Clean_Screen(0, 0, 64, 128, 1);
//	return select;
//}

void Draw_Component(u8 type, u8 index, u8 x0, u8 y0, u8 x1, u8 y1, u8 z, u8 c0, u8 c1, u8 chr[])
{
	u8 j;
	
	OLED_COMPONENT_LIST[index].type = type;
	OLED_COMPONENT_LIST[index].x0 = x0;
	OLED_COMPONENT_LIST[index].x1 = x1;
	OLED_COMPONENT_LIST[index].y0 = y0;
	OLED_COMPONENT_LIST[index].y1 = y1;
	OLED_COMPONENT_LIST[index].c0 = c0;
	OLED_COMPONENT_LIST[index].c1 = c1;
	OLED_COMPONENT_LIST[index].z = z;
	for(j=0;j<32;j++)
		OLED_COMPONENT_LIST[index].chr[j] = 0x00;
	for(j=0;chr[j]!='\0';j++)
		OLED_COMPONENT_LIST[index].chr[j] = chr[j];
}

//void Draw_Logo(u8 z, u8 chr[])
//{
//	int i0 = 0, j0 = 0, x = 0, y = 0;
//	u8 temp;

//	for(i0=0;i0<1024;i0++)
//	{
//		temp = gImage_logo_0[i0];
//		for(j0=0;j0<8;j0++)
//		{
//			if(temp&0x80)OLED_PIXEL[z][x][y] = 0xFF;
//			else OLED_PIXEL[z][x][y] = 0x00;
//			temp<<=1;
//			y++;
//			if(y==128){
//				x++;
//				y=0;
//			}
//		}
//	}
//}

/*
参考OLED.txt
*/
void Update_Component(u8 select_index)
{
	u8 i,j,t,main_i;
	u8 str_temp[10];
	struct OLED_COMPONENT temp;
	
	Clean_Screen(0, 0, 64, 128, 0);
	Clean_Screen(0, 0, 64, 128, 1);
	
	for(main_i=0;main_i<COMPONENT_MAX_INDEX;main_i++)
	{
		temp = OLED_COMPONENT_LIST[main_i];
		switch(temp.type)
		{
			case 0x01:
				if(temp.x1==4)
					Draw_4x6String(temp.x0, temp.y0, temp.z, temp.chr, 0xFF, 0x01);
				else if(temp.x1 == 16)		//16*16字体，靠左边显示
					Draw_16x16String(temp.x0, temp.y0, temp.z, temp.chr, 0xFF, 0x01);
				else if(temp.x1 == 15)		//16*16字体，居中显示
				{
					for(j=0;temp.chr[j]!='\0';j++);				
					Draw_16x16String( temp.x0,temp.y0+(128-temp.y0-j*8)/2, temp.z, temp.chr, 0xFF, 0x01); //字体居中
				}
				else 
					Draw_String(temp.y0, temp.x0, temp.z, temp.chr, 0xFF, 0x01);
				break;
			case 0x02:
				if(temp.x1==4)
					Draw_4x6String(temp.x0, temp.y0, temp.z, temp.chr, 0x80, 0x01);
				else if(temp.x1 == 16)
					Draw_16x16String(temp.x0, temp.y0, temp.z, temp.chr, 0xFF, 0x01);				
				else
					Draw_String(temp.y0, temp.x0, temp.z, temp.chr, 0x80, 0x01);
				break;
			case 0x04:
				for (j=temp.x0+1; j<=temp.x1-1; j++)
					for (i=temp.y0+1; i<=temp.y1-1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
			
				for (j=temp.y0+1; j<=temp.y1-1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xFE;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xFE;
					//OLED_PIXEL[temp.z][temp.x1+1][j] = 0x80;
				} 
				for (j=temp.x0+1; j<=temp.x1-1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xFE;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xFE;
				} 
				OLED_PIXEL[temp.z][temp.x1][temp.y0] = 0x80;
				OLED_PIXEL[temp.z][temp.x1][temp.y1] = 0x80;
				for(j=0;temp.chr[j]!='\0';j++);
				//Draw_String(temp.y0+(temp.y1-temp.y0-j*6)/2, temp.x0+(temp.x1-temp.x0-8)/2, temp.z, temp.chr, 0xA0, 0x01);				
					Draw_16x16String( temp.x0+(temp.x1-temp.x0-16)/2,temp.y0+(temp.y1-temp.y0-j*8)/2,/*temp.x0,temp.y0,*/ temp.z, temp.chr, 0xFF, 0x01); //字体居中
	
				if(select_index == main_i) Draw_Reverse(temp.x0+1, temp.y0+1, temp.x1-1, temp.y1-1, temp.z);
				break;
			case 0x05:
				for (j=temp.x0+1; j<=temp.x1-1; j++)
					for (i=temp.y0+1; i<=temp.y1-1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
			
				for (j=temp.y0+1; j<=temp.y1-1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xA0;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xA0;
				} 
				for (j=temp.x0+1; j<=temp.x1-1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xA0;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xA0;
				}

				for(j=0;temp.chr[j]!='\0';j++);
				//Draw_String(temp.y0+(temp.y1-temp.y0-j*6)/2, temp.x0+(temp.x1-temp.x0-8)/2, temp.z, temp.chr, 0xA0, 0x01);
				Draw_16x16String( temp.x0+(temp.x1-temp.x0-16)/2,temp.y0+(temp.y1-temp.y0-j*8)/2,/*temp.x0,temp.y0,*/ temp.z, temp.chr, 0xA0, 0x01);//字体居中
				break;
				
			case COMPONENT_RADIO_TRUE:
				
				for(t=0;temp.chr[t]!='\0';t++);
				for (j=temp.x0; j<=temp.x0+7; j++)
					for (i=temp.y0; i<=temp.y0+9+t*6; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
				
				for (j=0; j<4; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0+1][temp.y0+2+j] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+6][temp.y0+2+j] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+1] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+6] = 0xF0;
				} 
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+4] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+4] = 0xF0;
				
				Draw_String(temp.y0+8, temp.x0, temp.z, temp.chr, 0xFF, 0x00);
				
				if(select_index == main_i){
					Draw_Reverse(temp.x0, temp.y0, temp.x0+7, temp.y0+7+t*6+2, temp.z);
				}
				break;
				
			case COMPONENT_RADIO_FALSE:
				
				for(t=0;temp.chr[t]!='\0';t++);
				for (j=temp.x0; j<=temp.x0+7; j++)
					for (i=temp.y0; i<=temp.y0+9+t*6; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
			
				for (j=0; j<4; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0+1][temp.y0+2+j] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+6][temp.y0+2+j] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+1] = 0xF0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+6] = 0xF0;
				} 
				Draw_String(temp.y0+8, temp.x0, temp.z, temp.chr, 0xFF, 0x00);
				
				if(select_index == main_i){
					Draw_Reverse(temp.x0, temp.y0, temp.x0+7, temp.y0+7+t*6+2, temp.z);
				}
				break;
				
			case COMPONENT_RADIO_DISABLED:
				
				for (j=0; j<4; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0+1][temp.y0+2+j] = 0xA0;
					OLED_PIXEL[temp.z][temp.x0+6][temp.y0+2+j] = 0xA0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+1] = 0xA0;
					OLED_PIXEL[temp.z][temp.x0+2+j][temp.y0+6] = 0xA0;
					OLED_PIXEL[temp.z][temp.x0+2][temp.y0+2+j] = 0x70;
					OLED_PIXEL[temp.z][temp.x0+3][temp.y0+2+j] = 0x70;
					OLED_PIXEL[temp.z][temp.x0+4][temp.y0+2+j] = 0x70;
					OLED_PIXEL[temp.z][temp.x0+5][temp.y0+2+j] = 0x70;
				}
				Draw_String(temp.y0+8, temp.x0, temp.z, temp.chr, 0xA0, 0x00);
				break;
				
			case COMPONENT_TEXT_ENABLED:
				
				for (j=temp.x0; j<=temp.x1; j++)
					for (i=temp.y0; i<=temp.y1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
			
				for (j=temp.y0; j<=temp.y1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xFF;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xFF;
				} 
				for (j=temp.x0; j<=temp.x1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xFF;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xFF;
				} 

				for(j=0;temp.chr[j]!='\0';j++);
				if(temp.c0==4)
					Draw_4x6String(temp.x0+(temp.x1-temp.x0-6)/2+1, temp.y0+(temp.y1-temp.y0-j*4)/2, temp.z, temp.chr, 0xFF, 0x00);
				else
					Draw_String(temp.y0+(temp.y1-temp.y0-j*6)/2, temp.x0+(temp.x1-temp.x0-8)/2, temp.z, temp.chr, 0xFF, 0x00);
				if(select_index == main_i) Draw_Reverse(temp.x0+1, temp.y0+1, temp.x1-1, temp.y1-1, temp.z);
				break;
			case 0x0C:
				for (j=temp.x0; j<=temp.x1; j++)
					for (i=temp.y0; i<=temp.y1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
			
				for (j=temp.y0; j<=temp.y1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xB0;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xB0;
				} 
				for (j=temp.x0; j<=temp.x1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xB0;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xB0;
				} 

				for(j=0;temp.chr[j]!='\0';j++);
				Draw_String(temp.y0+(temp.y1-temp.y0-j*6)/2, temp.x0+(temp.x1-temp.x0-8)/2, temp.z, temp.chr, 0xB0, 0x00);
				
				if(select_index == main_i) Draw_Reverse(temp.x0+1, temp.y0+1, temp.x1-1, temp.y1-1, temp.z);
				break;
			case 0x0E:
				for (j=temp.x0+1; j<=temp.x1-1; j++)
					for (i=temp.y0+1; i<=temp.y1-1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
			
				for (j=temp.y0+1; j<=temp.y1-1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xFF;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xFF;
				} 
				for (j=temp.x0+1; j<=temp.x1-1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xFF;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xFF;
				}

				Draw_String(temp.y0+3, temp.x0+(temp.x1-temp.x0-8)/2, temp.z, temp.chr, 0xFF, 0x00);
				Draw_Reverse(temp.x0+1, temp.c0+1, temp.x1-1, temp.c1-1, temp.z);
				break;
			case 0x10:
				for (j=temp.x0+1; j<=temp.x1-1; j++)
					for (i=temp.y0+1; i<=temp.y1-1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
			
				for (j=temp.y0+1; j<=temp.y1-1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x0][j] = 0xFF;
					OLED_PIXEL[temp.z][temp.x0+10][j] = 0xFF;
					OLED_PIXEL[temp.z][temp.x1][j] = 0xFF;
				} 
				for (j=temp.x0+1; j<=temp.x1-1; j++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0] = 0xFF;
					OLED_PIXEL[temp.z][j][temp.y1] = 0xFF;
				}
				
				for(j=0;temp.chr[j]!='\0';j++);
				Draw_4x6String(temp.x0+3, temp.y0+3, temp.z, temp.chr, 0xFF, 0x01);
				break;
			case 0x12:
				
			
				for (j=temp.x0; j<=temp.x1; j++)
					for (i=temp.y0; i<=temp.y1; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
				
				for (j=temp.y0+10; j<=temp.y1; j++)
				{    
					OLED_PIXEL[temp.z][temp.x1-8][j] = 0xFF;
				} 
				t=0;
				for (j=temp.x1-8; j>=temp.x0; j--,t++)
				{    
					OLED_PIXEL[temp.z][j][temp.y0+10] = 0xFF;
					if(t%12==0){
						OLED_PIXEL[temp.z][j][temp.y0+11] = 0xFF;
						OLED_PIXEL[temp.z][j][temp.y0+12] = 0xFF;
						for (i=temp.y0+14; i<=temp.y1; i++)
						{    
							if(i%2==0)OLED_PIXEL[temp.z][j][i] = 0x50;
						} 
						sprintf((char*)str_temp, "%d", t/12*20);
						Draw_4x6String(j-2, temp.y0, temp.z, str_temp, 0xFF, 0x01);
					}
					if(t%6==0){
						OLED_PIXEL[temp.z][j][temp.y0+11] = 0xFF;
					}
				}
				/*
				for(j=0;temp.chr[j]!=0x00;j++)
				{
					u8 y;
					y = temp.x1-8-temp.chr[j];
					OLED_PIXEL[temp.z][y][j+temp.y0+11] = 0xFF;
				}
				*/
				for(j=0;j<temp.y1-temp.y0-10;j++)
				{
					u8 y;
					y = temp.x1-8-record_data[j]*0.6;
					if(y<temp.x0)y=temp.x0;
					OLED_PIXEL[temp.z][y][j+temp.y0+11] = 0xFF;
				}
				
				//for(j=0;temp.chr[j]!='\0';j++);

				//sprintf(str_temp, "%d", temp.c0);
				for(j=0;temp.chr[j]!='\0';j++);
				Draw_4x6String(temp.x1-5, temp.y1-4*j+4, temp.z, temp.chr, 0xFF, 0x01);
				
				break;
				
			case COMPONENT_LISTITEM_ENABLED:
				
				Draw_4x6String(temp.x0, 0, temp.z, temp.chr, 0xFF, 0x01);
				if(select_index == main_i) Draw_Reverse(temp.x0-1, 0, temp.x0+5, 127, temp.z);
				break;
			
			case COMPONENT_LISTITEM_DISABLED:
				
				Draw_4x6String(temp.x0, temp.y0, temp.z, temp.chr, 0x80, 0x01);
				break;
			
			case COMPONENT_SRADIO_TRUE:
				
				for(t=0;temp.chr[t]!='\0';t++);
				for (j=temp.x0; j<=temp.x0+6; j++)
					for (i=temp.y0; i<=temp.y0+6+t*4; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
				
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+4] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+4] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+4] = 0xF0;
				
				Draw_4x6String(temp.x0+1, temp.y0+6, temp.z, temp.chr, 0xFE, 0x01);
				
				if(select_index == main_i){
					Draw_Reverse(temp.x0, temp.y0, temp.x0+6, temp.y0+6+t*4+2, temp.z);
				}
				break;
				
			case COMPONENT_SRADIO_FALSE:
				
				for(t=0;temp.chr[t]!='\0';t++);
				for (j=temp.x0; j<=temp.x0+6; j++)
					for (i=temp.y0; i<=temp.y0+6+t*4; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
				
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+4] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+4] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+2] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+3] = 0xF0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+4] = 0xF0;
				
				Draw_4x6String(temp.x0+1, temp.y0+6, temp.z, temp.chr, 0xFE, 0x01);
				
				if(select_index == main_i){
					Draw_Reverse(temp.x0, temp.y0, temp.x0+6, temp.y0+6+t*4+2, temp.z);
				}
				break;
				
			case COMPONENT_SRADIO_DISABLED:
				
				for(t=0;temp.chr[t]!='\0';t++);
				for (j=temp.x0; j<=temp.x0+6; j++)
					for (i=temp.y0; i<=temp.y0+6+t*4; i++)
						OLED_PIXEL[temp.z][j][i] = 0x00;
				
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+2] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+3] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+2][temp.y0+4] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+2] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+3] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+3][temp.y0+4] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+2] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+3] = 0xA0;
				OLED_PIXEL[temp.z][temp.x0+4][temp.y0+4] = 0xA0;
				
				Draw_4x6String(temp.x0+1, temp.y0+6, temp.z, temp.chr, 0xA0, 0x01);

				break;
			
			case COMPONENT_BLANK:
				
				for (j=0; j<64; j++)
					for (i=0; i<128; i++)
						OLED_PIXEL[temp.z][j][i] = 0x01;
			
				break;
		}
	}
}

u8 Return_Index_Available(u8 type)
{
	if(type == COMPONENT_BUTTON_ENABLED) return 0;
	if(type == COMPONENT_RADIO_TRUE) return 0;
	if(type == COMPONENT_RADIO_FALSE) return 0;
	if(type == COMPONENT_TEXT_ENABLED) return 0;
	if(type == COMPONENT_LISTITEM_ENABLED) return 0;
	if(type == COMPONENT_SRADIO_TRUE) return 0;
	if(type == COMPONENT_SRADIO_FALSE) return 0;
	return 1;
}

void Change_Index(s8 dx)
{
	int i;
	i=COMPONENT_INDEX+dx;
	while((i<=COMPONENT_MAX_INDEX&&i>=0) && Return_Index_Available(OLED_COMPONENT_LIST[i].type)){
		i+=dx;
	}
	if((i<=COMPONENT_MAX_INDEX&&i>=0) && (!Return_Index_Available(OLED_COMPONENT_LIST[i].type))){
		COMPONENT_INDEX = i;
	}
}

void Click_Component(void)
{
	switch(OLED_COMPONENT_LIST[COMPONENT_INDEX].type)
	{
		case COMPONENT_RADIO_TRUE:
			OLED_COMPONENT_LIST[COMPONENT_INDEX].type = COMPONENT_RADIO_FALSE;
			break;
		case COMPONENT_RADIO_FALSE:
			OLED_COMPONENT_LIST[COMPONENT_INDEX].type = COMPONENT_RADIO_TRUE;
			break;
		case COMPONENT_SRADIO_TRUE:
			OLED_COMPONENT_LIST[COMPONENT_INDEX].type = COMPONENT_SRADIO_FALSE;
			break;
		case COMPONENT_SRADIO_FALSE:
			OLED_COMPONENT_LIST[COMPONENT_INDEX].type = COMPONENT_SRADIO_TRUE;
			break;
	}
}

void Recycle_Component(u8 indexA, u8 indexB)
{
	u8 i;
	for(i=indexA;i<=indexB;i++)
		Draw_Component(COMPONENT_NULL, i, 0, 0, 0, 0, 0, 0, 0, "");
}



