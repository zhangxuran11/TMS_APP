#ifndef  __OLED_SSD1325_H__
#define  __OLED_SSD1325_H__

#include "stm32f4xx.h"


/*whh 600 行开始*/
extern const unsigned char szASC16[];
extern const unsigned char F6x8[1][1];
extern const unsigned char  F4x6[1][1];

/*下面是正确的字库，但是内存不够了，也没有使用这个字体，就屏蔽掉*/
//extern const unsigned char F6x8[][6];
//extern const unsigned char  F4x6[95][3];

#define USE_SMALL_FONT			/* 定义此行表示使用小字库 */


/* 字体代码 */
typedef enum
{
	FC_ST_16 = 0,		/* 宋体15x16点阵 （宽x高） */
	FC_ST_12 = 1,		/* 宋体12x12点阵 （宽x高） */
	FC_ST_8	 = 2,		/* 宋体 8*8点阵   (宽x高) */	
}FONT_CODE_E;

/* 字体属性结构, 用于LCD_DispStr() */
typedef struct
{
	uint16_t FontCode;		/* 字体代码 0 表示16点阵 */
	uint16_t FrontColor;	/* 字体颜色 */
	uint16_t BackColor;		/* 文字背景颜色，透明 */
	uint16_t Space;				/* 文字间距，单位 = 像素 */
}FONT_T;

/* 可供外部模块调用的函数 */


#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64


/*W H H*/
#define 		XLevelL										0x02
#define 		XLevelH										0x10
#define 		XLevel										((XLevelH&0x0F)*16+XLevelL)

#define 		Max_Column								0x3F			// 128/2-1 (Total Columns Devided by 2)
#define 		Max_Row										0x3F			// 64-1
#define			Brightness								0xFF


	
#define			COMPONENT_NULL									0x00			//空组件，用以重置
#define			COMPONENT_LABEL_ENABLED					0x01		/*Label，在屏幕上绘制一个标签
																											此时 index => 组件编号
																											 x0    => 左上角x坐标
																											 y0    => 左上角y坐标
																											 x1    => 4 (绘制4*6文字) / 6 (绘制6*10文字)
																											 z     => 绘制层，应填0
																											 chr[] => Label文字*/
#define			COMPONENT_LABEL_DISABLED				0x02		/*禁用的Label，与COMPONENT_LABEL_ENABLED参数一致，颜色会变灰*/
#define			COMPONENT_LABEL_HIDDEN					0x03		/*无效*/
#define			COMPONENT_BUTTON_ENABLED 				0x04		/* Button，在屏幕上绘制一个按钮
																											此时 index => 组件编号，此button可被选中，参见Update_Component方法
																											 x0    => 左上角x坐标
																											 y0    => 左上角y坐标
																											 x1    => 右下角x坐标
																											 y1    => 右下角y坐标
																											 z     => 绘制层，应填0
																											 chr[] => Button文字，库会默认居中*/
#define			COMPONENT_BUTTON_DISABLED 		 	0x05		/* 禁用的Button，此时Button不能被Update_Component方法选中*/
#define			COMPONENT_BUTTON_HIDDEN  				0x06		/*无效*/
#define			COMPONENT_RADIO_TRUE  					0x07		/*adio，在屏幕上绘制一个已经被选中选框
																											此时Radio文字为6*10，4*6文字请参见COMPONENT_SRADIO
																											此时 index => 组件编号，此Radio可被选中，参见Update_Component方法
																											 x0    => 左上角x坐标
																											 y0    => 左上角y坐标
																											 z     => 绘制层，应填0
																											 chr[] => Radio文字*/
#define			COMPONENT_RADIO_FALSE  					0x08		/*Radio，在屏幕上绘制一个已经未被选中选框*/
#define			COMPONENT_RADIO_DISABLED  			0x09		/*禁用的Radio，此时Radio不能被Update_Component方法选中*/
#define			COMPONENT_RADIO_HIDDEN  				0x0A		/*无效*/
#define			COMPONENT_TEXT_ENABLED					0x0B		/*Text，在屏幕上绘制文本框
																											文本框与标签不同，文本框包含方框且应限制文字个数
																											此时 index => 组件编号，此Text可被选中，参见Update_Component方法
																										 x0    => 左上角x坐标
																										 y0    => 左上角y坐标
																										 x1    => 右下角x坐标
																										 y1    => 右下角y坐标
																										 z     => 绘制层，应填0
																										 c1    => 4 (绘制4*6文字) / 6 (绘制6*10文字)
																										 chr[] => Text文字*/
#define			COMPONENT_TEXT_DISABLED					0x0C		/* 禁用的Text，此时Text不能被Update_Component方法选中*/																										 
#define			COMPONENT_TEXT_HIDDEN						0x0D		/*无效*/
#define			COMPONENT_PBAR_ENABLED					0x0E		/*PBar，在屏幕上绘制进度条
																											进度条的文字颜色会被自动处理为反色
																											此时 index => 组件编号
																											 x0    => 左上角x坐标
																											 y0    => 左上角y坐标
																											 x1    => 右下角x坐标
																											 y1    => 右下角y坐标
																											 z     => 绘制层，应填0
																											 c1    => 进度条左边距，显示百分比时，此值应为0
																											 c2    => 进度条长度，应小于 y1-y0
																											 chr[] => PBar文字*/
#define			COMPONENT_PBAR_DISABLED					0x0F		/*禁用的PBar*/
#define			COMPONENT_WINDOW_ENABLED				0x10		/*无效*/
#define			COMPONENT_WINDOW_HIDDEN					0x11		/*无效*/
#define			COMPONENT_CHART_ENABLED					0x12		/*无效*/
#define			COMPONENT_CHART_HIDDEN					0x13		/*无效*/
#define			COMPONENT_LISTITEM_ENABLED		  0x14		/*ListItem，在屏幕上绘制顶部菜单列表
																											进度条的文字颜色会被自动处理为反色
																											此时 index => 组件编号，此ListItem可被选中，参见Update_Component方法
																											 x0    => 左上角x坐标
																											 z     => 绘制层，应填0
																											 chr[] => ListItem文字*/
#define			COMPONENT_LISTITEM_DISABLED			0x15		/*禁用的ListItem，此时ListItem不能被Update_Component方法选中*/
#define			COMPONENT_LISTITEM_HIDDEN				0x16		/*无效*/
#define			COMPONENT_SRADIO_TRUE  					0x17		/*sRadio，在屏幕上绘制一个已经被选中选框
																											此时sRadio文字为4*6，6*10文字请参见COMPONENT_RADIO
																									此时 index => 组件编号，此sRadio可被选中，参见Update_Component方法
																											 x0    => 左上角x坐标
																											 y0    => 左上角y坐标
																											 z     => 绘制层，应填0
																											 chr[] => sRadio文字*/
#define			COMPONENT_SRADIO_FALSE          0x18		/* sRadio，在屏幕上绘制一个已经未被选中选框*/
#define			COMPONENT_SRADIO_DISABLED  		  0x19		/*禁用的sRadio，此时sRadio不能被Update_Component方法选中*/
#define			COMPONENT_SRADIO_HIDDEN  				0x1A		/* 无效*/
#define			COMPONENT_BLANK                 0x1B		/* 无效*/

#define			COMPONENT_MAX_INDEX 						0x15


void Set_Start_Column(unsigned char d);
void Set_Addressing_Mode(unsigned char d);
void Set_Column_Address(unsigned char a, unsigned char b);
void Set_Page_Address(unsigned char a, unsigned char b);
void Set_Start_Line(unsigned char d);
void Set_Contrast_Control(unsigned char d);
void Set_Area_Brightness(unsigned char d);
void Set_Segment_Remap(unsigned char d);
void Set_Entire_Display(unsigned char d);
void Set_Inverse_Display(unsigned char d);
void Set_Multiplex_Ratio(unsigned char d);
void Set_Dim_Mode(unsigned char a, unsigned char b);
void Set_Master_Config(unsigned char d);
void Set_Display_On_Off(unsigned char d);
void Set_Start_Page(unsigned char d);
void Set_Common_Remap(unsigned char d);
void Set_Display_Offset(unsigned char d);
void Set_Display_Clock(unsigned char d);
void Set_Area_Color(unsigned char d);
void Set_Precharge_Period(unsigned char d);
void Set_Common_Config(unsigned char d);
void Set_VCOMH(unsigned char d);
void Set_Read_Modify_Write(unsigned char d);
void Set_NOP(void);

void Delay_ns(__IO uint32_t del);
void ClearDisplay(int xStart, int yStart, int xEnd, int yEnd);
void Checkerboard_12864(void);void Fill_RAM_12864(unsigned char a);void Set_Start_Line_12864(unsigned char d);
void Set_Display_On_Off_12864(unsigned char d);
void LCD_ClearBar(uint8_t x1_Pos, uint8_t x2_Pos);
void OLED_12864_Init(void);
/*画矩形*/
void Draw_Rectangle_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);
//void LCD_TestHorizontal(uint8_t shift);
//void LCD_TestVertical(uint8_t shift);
void Show_Frame_12864(void);			//W H H
void Set_Window(int xStart, int yStart, int xEnd, int yEnd);
void ClearDisplay(int xStart, int yStart, int xEnd, int yEnd);
/*设置列地址*/
void Set_Column_Address_12864(unsigned char a, unsigned char b);
/*设置行地址*/
void Set_Row_Address_12864(unsigned char a, unsigned char b);
/*设置当前对比度*/
void Set_Contrast_Current_12864(unsigned char d);
/*设置当前范围*/
void Set_Current_Range_12864(unsigned char d);
/*设置映射格式*/
void Set_Remap_Format_12864(unsigned char d);
/*设置起始行*/
void Set_Start_Line(unsigned char d);
/*设置显示偏移位置*/
void Set_Display_Offset_12864(unsigned char d);
/*设置显示模式*/
void Set_Display_Mode_12864(unsigned char d);
/*设置多路复用率*/
void Set_Multiplex_Ratio_12864(unsigned char d);
/*设置主配置*/
void Set_Master_Config_12864(unsigned char d);
/*设置显示开关*/
void Set_Display_On_Off_12864(unsigned char d);
/*设置相位长度*/
void Set_Phase_Length_12864(unsigned char d);
/*设置帧频率*/
void Set_Frame_Frequency_12864(unsigned char d);
/*设置显示的时钟*/
void Set_Display_Clock_12864(unsigned char d);
/*设置预先充电补偿*/
void Set_Precharge_Compensation_12864(unsigned char a, unsigned char b);
/*设置充电电压*/
void Set_Precharge_Voltage_12864(unsigned char d);
/*设置输出com电压*/
void Set_VCOMH_12864(unsigned char d);
/*设置段的低电压*/
void Set_VSL_12864(unsigned char d);
/*无命令*/
void Set_NOP_12864(void);
/*图形加速命令操作*/
void GA_Option_12864(unsigned char d);
/*画矩形*/
void Draw_Rectangle_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);
/*将一块区域，复制到另一块区域*/
void Copy_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f);
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Full Screen)	填充屏幕
//
//    a: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_RAM_12864(unsigned char a);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Regular Pattern (Partial or Full Screen)	填充背景
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Devided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//    e: Two Pixels Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fill_Block_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Checkboard (Full Screen)	检测12864
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Checkerboard_12864(void);
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Gray Scale Bar (Full Screen)	显示灰度条
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Grayscale_12864(void);
void Show_Font57_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
void Show_String_12864(unsigned char a, unsigned char *Data_Pointer, unsigned char b, unsigned char c);
void OLED_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);			//W H H
/*5*7 的字体*/
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Frame (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Frame_12864(void);			//W H H

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Show Pattern (Partial or Full Screen)
//
//    a: Column Address of Start
//    b: Column Address of End (Total Columns Divided by 2)
//    c: Row Address of Start
//    d: Row Address of End
//		k: 0,正常显示，1，相反的显示
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Show_Pattern_12864(unsigned char *Data_Pointer, unsigned char a, unsigned char b, unsigned char c, unsigned char d,unsigned char k);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Vertical Scrolling (Full Screen)
//
//    a: Scrolling Direction
//       "0x00" (Upward)
//       "0x01" (Downward)
//    b: Set Numbers of Row Scroll per Step
//    c: Set Time Interval between Each Scroll Step
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll_12864(unsigned char a, unsigned char b, unsigned char c);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Continuous Horizontal Scrolling (Partial or Full Screen)
//
//    a: Set Numbers of Column Scroll per Step
//    b: Set Numbers of Row to Be Scrolled
//    c: Set Time Interval between Each Scroll Step in Terms of Frame Frequency
//    d: Delay Time
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Horizontal_Scroll_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

void Fade_Scroll_12864(unsigned char a, unsigned char b, unsigned char c, unsigned char d);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Deactivate Scrolling (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Deactivate_Scroll_12864(void);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade In (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_In_12864(void);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Fade Out (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Fade_Out_12864(void);


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Sleep Mode
//
//    "0x01" Enter Sleep Mode
//    "0x00" Exit Sleep Mode
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Sleep_12864(unsigned char a);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table_12864(void);

void con_4_byte(unsigned char DATA);

void con_8_byte(unsigned char DATA);

void Show_dot(unsigned char x, unsigned char y, unsigned char color);

/****************************************************
   写入一个16*16汉字
******************************************************/
void OLED_Draw_16x16Char(unsigned char x,unsigned char y,unsigned char coder);	//write chinese word of1616
void OLED_Draw_16x16String(unsigned char x,unsigned char y,unsigned char m,unsigned char endm);//write chinese word of1616;


void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 mode);
void Show_Font57(unsigned char b, unsigned char c, unsigned char d,unsigned char k);
void Show_Font68(unsigned char x, unsigned char y, unsigned char c,unsigned char k);
void Next_Frame(void);
void Show_XGS(void);
void Draw_Dot(u8 x, u8 y, u8 z, u8 color);
void Clean_Screen(u8 x0, u8 y0, u8 x1, u8 y1, u8 z);
void Draw_Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color);
void Draw_String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color);
void Draw_5x7Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color);
void Draw_5x7String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color);
void Draw_4x6Char(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color);
void Draw_4x6String(u8 x,u8 y,u8 z,u8 chr[],u8 font_color,u8 back_color);
void Draw_Icon(u8 x,u8 y,u8 z,u8 chr,u8 font_color,u8 back_color);
void Draw_Notification(u8 title[], u8 chr[]);
void Draw_Reverse(u8 x0, u8 y0, u8 x1, u8 y1, u8 z);
u8   Draw_Menu(u8 menu[][12], u8 x0, u8 y0, u8 x1, u8 y1, u8 default_select);
void Draw_Component(u8 type, u8 index, u8 x0, u8 y0, u8 x1, u8 y1, u8 z, u8 c0, u8 c1, u8 chr[]);
void Draw_Logo(u8 z, u8 chr[]);
void Update_Component(u8 select_index);
void Change_Index(s8 dx);
void Click_Component(void);
void Recycle_Component(u8 indexA, u8 indexB);
u8 Return_Index_Available(u8 type);


/*W H H*/

#endif 	 //__OLED_SSD1325_H__
