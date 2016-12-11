#ifndef ___STM32I2C_H__
#define ___STM32I2C_H__

#include "stm32f4xx.h"
/*RTC 的GPOI引脚定义*/
#define RTC_I2C           			I2C1
#define RTC_I2C_RCC       			RCC_APB1Periph_I2C1

#define RTC_I2C_PROT       			GPIOB
#define RTC_I2C_GPIO_RCC   			RCC_AHB1Periph_GPIOB

#define RTC_I2C_SCL_GPIO        GPIO_Pin_8
#define RTC_I2C_SCL_GPIOSource  GPIO_PinSource8
#define RTC_I2C_SCL_AF     			GPIO_AF_I2C1

#define RTC_I2C_SDA_GPIO        GPIO_Pin_9
#define RTC_I2C_SDA_GPIOSource  GPIO_PinSource9
#define RTC_I2C_SDA_AF     			GPIO_AF_I2C1

#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xDE			//ISL1208 从机的地址

/* Exported types ------------------------------------------------------------*/
typedef enum
{
   I2C_FAIL = 0,
   I2C_OK   = 1
}I2C_Status;

/* Exported constants --------------------------------------------------------*/
#define I2C_TIMEOUT                               0x10000				//1000将会执行7次，故扩大10倍则可执行70次


void STM32_I2C_Configuration(void);
I2C_Status STM32_I2C_WriteByte(uint8_t* pBuffer, uint8_t WriteAddr);
uint8_t STM32_I2C_ReadByte(uint8_t ReadAddr);
I2C_Status STM32_I2C_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
I2C_Status STM32_I2C_BufferRead(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToRead);

#endif  //___STM32I2C_H__

