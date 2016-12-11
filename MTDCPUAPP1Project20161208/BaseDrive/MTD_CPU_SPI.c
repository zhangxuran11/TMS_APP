#include "MTD_CPU_SPI.h"


/*
	STM32F4XX 时钟计算.
		HCLK = 168M
		PCLK1 = HCLK / 4 = 42M
		PCLK2 = HCLK / 2 = 84M

		SPI2、SPI3 在 PCLK1, 时钟42M
		SPI1       在 PCLK2, 时钟84M

		STM32F4 支持的最大SPI时钟为 37.5 Mbits/S, 因此需要分频。
*/

/*
	引脚定义 
	SPI_MISO   PA6			SPI1_MISO		主入从出
	SPI_MOSI   PA7			SPI1_MOSI		主出从入
	SPI_SCK		 PA5			SPI1_SCK		时钟
	SPI_NSS　  PA4			SPI1_NSS  	片选
*/

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI1的初始化
void MTDCPU_SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  	SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	//使能SPI1时钟
 
	/* 配置 SCK, MISO 、 MOSI 为复用功能 */
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PA5复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //PA6复用为 SPI1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PA7复用为 SPI1
 
  //GPIOA 5,6,7初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6 |GPIO_Pin_7;	//PA4~7复用功能输出	,PA4片选引脚用软件控制
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;																//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;															//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;													//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;															//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);																			//初始化

	GPIO_InitStructure.GPIO_Mode =	GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = 	MTDCPU_SPI1_CS_CPIO_PIN;
	GPIO_Init(MTDCPU_SPI1_CS_GPIO, &GPIO_InitStructure);
		
	/* 配置片选口线为推挽输出模式 */
	MTDCPU_SPI1_CS_HIGH();		/* 片选置高，不选中 */
	
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);		//复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);	//停止复位SPI1

	/* SPI_CPOL和SPI_CPHA结合使用决定时钟和数据采样点的相位关系、
	   配置: 总线空闲是高电平,第2个边沿（上升沿采样数据)
	*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;												//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;										//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;													//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;												//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;														//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//定义波特率预分频的值:波特率预分频值为8
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;									//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;														//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure); 															  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设

	MTDCPU_SPI1_SendByte(0xff);//启动传输		 
}   

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void MTDCPU_SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;								//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI1,ENABLE);						 //使能SPI1
} 

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 MTDCPU_SPI1_SendByte(u8 TxData)
{	
	/* 等待上个数据未发送完毕 */	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//等待发送区空  	
	/* 通过SPI硬件发送1个字节 */
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
	
	
	/* 等待接收一个字节任务完成 */	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //等待接收完一个byte  
	/* 返回从SPI总线读到的数据 */
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	
 		    
}

/*从设备中读取以字节的数据*/
u8 MTDCPU_SPI1_ReadByte(void)
{
	return (MTDCPU_SPI1_SendByte(0xFF));
}






