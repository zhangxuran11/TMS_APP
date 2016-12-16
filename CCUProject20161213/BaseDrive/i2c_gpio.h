/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : i2c_gpio.h
*	版    本 : V1.0
*	说    明 : 头文件。
*********************************************************************************************************
*/

#ifndef _I2C_GPIO_H
#define _I2C_GPIO_H

#include <inttypes.h>

#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */

/* 产生I2C总线起始信号 */
void i2c_Start(void);
/* 产生I2C总线停止信号 */
void i2c_Stop(void);
/*
  向I2C总线发送8bit数据
*/
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);

uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);

void i2c_Configuration(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

#endif
