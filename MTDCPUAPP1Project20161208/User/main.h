/**
  ******************************************************************************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Basic_EEPROM/main.h
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/*WHH  关于i2c接口的EEPROM 定义*/
/*在 c/c++文件中已经定义了 USE_STM324xG_EVAL*/
/* Includes ------------------------------------------------------------------*/
#ifdef USE_STM3210C_EVAL
 #include "stm3210c_eval.h"
#elif defined USE_STM32100E_EVAL
 #include "stm32100e_eval.h"
#elif defined USE_STM322xG_EVAL
 #include "stm322xg_eval.h"
#elif defined USE_STM324xG_EVAL
 #include "stm324xg_eval.h"
#elif defined USE_STM32L152_EVAL
 #include "stm32l152_eval.h"
#elif defined USE_STM32L152D_EVAL
 #include "stm32l152d_eval.h"
#endif

#include "stm32_eval_i2c_ee_cpal.h"
#include <stdio.h>
#include "stdbool.h"
#include "os.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Define EEPROM address, size and page size */
#ifdef USE_STM32L152D_EVAL
 #define EEPROM_ADDRESS        0xA0
 #define EEPROM_PAGE_SIZE      4
#else
 #define EEPROM_ADDRESS        0xA0
 #define EEPROM_PAGE_SIZE      32 
#endif /* USE_STM32L152D_EVAL */ 

#if defined (USE_STM32L152_EVAL) || defined (USE_STM32L152D_EVAL) || defined (USE_STM3210C_EVAL) || defined (USE_STM322xG_EVAL) || defined (USE_STM324xG_EVAL)
 #define sEE_DevStructure sEE2_DevStructure 		//WHH 修改表示使用哪个i2c设备
#elif defined USE_STM32100E_EVAL
 #define sEE_DevStructure sEE2_DevStructure 
#endif /* USE_STM32L152_EVAL || USE_STM3210C_EVAL || USE_STM322xG_EVAL || USE_STM324xG_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


/*WHH can1 2 led 灯定义*/

/*事件标志组 位含义定义*/
#define  CAN1TxLED_FLAG  		0x01
#define  CAN1RxLED_FLAG 		0x02
#define  CAN1LEDFLAG_VALUE 	0x00

#define  CAN2TxLED_FLAG  		0x01
#define  CAN2RxLED_FLAG 		0x02
#define  CAN2LEDFLAG_VALUE 	0x00

/*车号*/	
extern u16	CarNumber;
extern u8   CarNo;
extern char cartype[10];

/*任务控制块*/
extern OS_TCB CAN1SlaveTxTaskTCB;

/*1.VVVF动作判断 所需要的全局变量*/
/*VVVF电源 */
extern bool DI_VVVFPowertx_B1_10 ;	 			//VVVF电源  MC1
extern bool DI_VVVFPowertx_B1_30 ;	 			//VVVF电源  M
extern bool DI_VVVFPowertx_B1_60 ;	 			//VVVF电源  MC2

/*VVVF 电制有效位CRD*/
extern bool TCU1_CRDtx_SD_B1_10	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5）
extern bool TCU2_CRDtx_SD_B1_10	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
	
extern bool TCU1_CRDtx_SD_B1_30	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
extern bool TCU2_CRDtx_SD_B1_30	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
	
extern bool TCU1_CRDtx_SD_B1_60	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 
extern bool TCU2_CRDtx_SD_B1_60	; 				//VVVF1协议中的电制有效位 （char No.7 Bit5） 

/*VVVF“群开放UCORM*/
extern bool TCU1_UCORtx_SD_B1_10 ;    		//VVVF1协议中的（char No.17 Bit1）
extern bool TCU2_UCORtx_SD_B1_10 ;		 		//VVVF2协议中的（char No.17 Bit1）

extern bool TCU1_UCORtx_SD_B1_30 ;    		//VVVF1协议中的（char No.17 Bit1）
extern bool TCU2_UCORtx_SD_B1_30 ;		 		//VVVF2协议中的（char No.17 Bit1）

extern bool TCU1_UCORtx_SD_B1_60 ;    		//VVVF1协议中的（char No.17 Bit1）
extern bool TCU2_UCORtx_SD_B1_60 ;		 		//VVVF2协议中的（char No.17 Bit1）

/*VVVF BSLB_F大滑行有效位*/
extern bool TCU1_BSLBFtx_SD_B1_10 ; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
extern bool TCU2_BSLBFtx_SD_B1_10 ; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）
                                  
extern bool TCU1_BSLBFtx_SD_B1_30 ; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
extern bool TCU2_BSLBFtx_SD_B1_30 ; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）
                                  
extern bool TCU1_BSLBFtx_SD_B1_60 ; 			//VVVF1协议中的电制有效位 （char No.18 Bit1）
extern bool TCU2_BSLBFtx_SD_B1_60 ; 			//VVVF2协议中的电制有效位 （char No.18 Bit1）

/*VVVF通信状态*/
extern bool MDCU_VVVF1ComFaultx_B1_10 ;   //VVVF1通信状态
extern bool MDCU_VVVF2ComFaultx_B1_10 ;   //VVVF2通信状态

extern bool MDCU_VVVF1ComFaultx_B1_30 ;   //VVVF1通信状态
extern bool MDCU_VVVF2ComFaultx_B1_30 ;   //VVVF2通信状态

extern bool MDCU_VVVF1ComFaultx_B1_60 ;   //VVVF1通信状态
extern bool MDCU_VVVF2ComFaultx_B1_60 ;   //VVVF2通信状态

/*BCU 通信状态*/
extern bool MDCU_BCUComFaultrx_B1_10 ;  	//MC1 BCU通信状态 
extern bool MDCU_BCUComFaultrx_B1_20 ;  	//T BCU通信状态 
extern bool MDCU_BCUComFaultrx_B1_30 ;  	//M BCU通信状态 
extern bool MDCU_BCUComFaultrx_B1_40 ;  	//T1 BCU通信状态 
extern bool MDCU_BCUComFaultrx_B1_50 ;  	//T2 BCU通信状态 
extern bool MDCU_BCUComFaultrx_B1_60 ;  	//MC2 BCU通信状态 

/*BCU 滑行标志位*/
extern bool BCU_Axle1Skidtx_SD_B1_10  ;   //MC1 BCU滑行
extern bool BCU_Axle2Skidtx_SD_B1_10  ;
extern bool BCU_Axle3Skidtx_SD_B1_10  ;
extern bool BCU_Axle4Skidtx_SD_B1_10  ;

extern bool BCU_Axle1Skidtx_SD_B1_60  ;   //MC2 BCU滑行
extern bool BCU_Axle2Skidtx_SD_B1_60  ;
extern bool BCU_Axle3Skidtx_SD_B1_60  ;
extern bool BCU_Axle4Skidtx_SD_B1_60  ;

/*MTDCPU 处理ATC_Sdr逻辑*/
void  MTDCPUProATCSdrLogicalData(void);
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
