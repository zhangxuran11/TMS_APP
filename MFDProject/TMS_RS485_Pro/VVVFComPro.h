#ifndef  __VVVFCOMPRO_H__
#define  __VVVFCOMPRO_H__

#include "TMS_ComunDefine.h"

#define  VVVF_RX_BUF_SIZE  			20  	//定义接受 数据缓冲区的长度
#define  VVVF_TX_BUF_SIZE  			20  	//定义发送数据缓冲区的长度

#define VVVF1 0x01
#define VVVF2 0x02

/*定义开始2字节的数据*/
#define  VVVF_PAD	   						0x7E	//开路PAD
#define  VVVF_STARTFLAG					0x7E	//起始标志
#define  VVVF_ADDRESS1					0x00	//VVVF地址高8位
#define  VVVF_ADDRESS2					0x00	//VVVF地址低8位
#define  VVVF_CONTROLBYTE				0x13	//控制字
#define  VVVF_ENDFLAG						0x7E	//结束标志

#define  VVVF_SDR_TEXTNUM				20
#define  VVVF_SDR_COMMAND    		0xE8	//命令码
#define  VVVF_SDR_STATE    			0x80	//状态码
#define  VVVF_SDR_SENDER    		0x30
#define  VVVF_SDR_RECEIVER1    	0x40
#define  VVVF_SDR_RECEIVER2    	0x50
#define  VVVF_SDR_NO          	0x00

#define  VVVF_SD_TEXTNUM				41
#define  VVVF_SD_COMMAND    		0xE9	//命令码
#define  VVVF_SD_STATE    			0x80	//状态码
#define  VVVF_SD_SENDER1    		0x40
#define  VVVF_SD_SENDER2    		0x50
#define  VVVF_SD_RECEIVER  	  	0x30
#define  VVVF_SD_NO         	 	0x00

#define  VVVF_TDR_TEXTNUM				10
#define  VVVF_TDR_COMMAND    		0xE2	//命令码
#define  VVVF_TDR_STATE    			0x80	//状态码
#define  VVVF_TDR_SENDER    		0x30
#define  VVVF_TDR_RECEIVER1    	0x40
#define  VVVF_TDR_RECEIVER2  	  0x50
#define  VVVF_TDR_NO         	 	0x00

#define  VVVF_TD_TEXTNUM				110
#define  VVVF_TD_COMMAND    		0xE3	//命令码
#define  VVVF_TD_STATE    			0x80	//状态码
#define  VVVF_TD_SENDER1    		0x40
#define  VVVF_TD_SENDER2    		0x50
#define  VVVF_TD_RECEIVER  		  0x30
#define  VVVF_TD_NO         	 	0x00

#define  VVVF_TCR_TEXTNUM				10
#define  VVVF_TCR_COMMAND    		0xE6	//命令码
#define  VVVF_TCR_STATE    			0x80	//状态码
#define  VVVF_TCR_SENDER    		0x30
#define  VVVF_TCR_RECEIVER1    	0x40
#define  VVVF_TCR_RECEIVER2  		0x50
#define  VVVF_TCR_NO         	 	0x00

/*协议中的故障代码*/
#define  VVVF_HBT_FAULT					0x01		//HB脱口
#define	 VVVF_CONTROL_FAULT			0x02		//控制故障
#define  VVVF_MMOCD_FAULT				0x03		//牵引电机过电流故障
#define  VVVF_FOVD_FAULT				0x04		//FC过压1
#define  VVVF_FLVD_FAULT				0x05		//FC低电压
#define  VVVF_SOVD_FAULT				0x06		//FC过压2
#define  VVVF_PGD_FAULT					0x07		//后退检测
#define  VVVF_BSLP_FAULT				0x08		//大空转
#define  VVVF_BSLD_FAULT				0x09		//大滑行
#define  VVVF_LGD_FAULT					0x0B		//架空线接地
#define	 VVVF_FMEF_FAULT				0x0C		//PF故障
#define  VVVF_THD1_FAULT				0x0A		//冷却器升温
#define  VVVF_BCHF_FAULT				0x0D		//BCH元件过热
#define	 VVVF_T_PWMF_FAULT			0x0E		//T_PWMF指令异常	
#define	 VVVF_TEST_FAULT				0x0F		//TEST模拟故障
	
	
/*定义错误编码值*/
#define  VVVF_DataError  				-1			//接受的前4字节有不一致的错误
#define  VVVF_GetTimeoutError 	-2			//发送请求之后，20ms以内没有接收到数据
#define  VVVF_FrameError				-3			//检测到帧错误、奇偶校验错误时
#define  VVVF_CRCError          -4   		//BCC 符号不符合计划值时
#define  VVVF_GetError					-5			//表示接受数据错误
#define  VVVF_TransException		-6			//表示传输异常

/*定义发送接受VVVF1或者2*/
#define SEND_VVVF1			1
#define SEND_VVVF2			2
#define RECEIVE_VVVF1		1
#define RECEIVE_VVVF2		2

/*发送模式*/
#define VVVF_SEND_SDR		1
#define VVVF_SEND_TDR		2
#define VVVF_SEND_TCR		3

typedef struct 
{
	u8	TextNumber;				//文本数
	u8	Command;				//命令	E8h
	u8  State;					//状态
	u8  Sender;					//发送者		 30h监控装置
	u8  Receiver; 			//接受者   40hVVVF1,50hVVVF2
	u8	OrderNo;				//序号NO   00h				
	
	u8 Year;						//年	00~99（公历后面2 位数） BCD
	u8 Month;						//月	BCD
	u8 Day;							//日	BCD
	u8 Hour;						//时	BCD
	u8 Minute;					//分	BCD
	u8 Second;					//秒	BCD
	
	u8 TCU_TimeEffect		:1;			//时钟数据有效
	u8 TCU_TimeSet			:1;			//时钟数据设置
	u8 Byte13Undef6bit	:6;			//未使用6位
	
	u8 Ready[8];								//00h
}VVVFSdrText;

typedef struct
{
	u8 StartPAD[4];			//开始PAD
	u8 StartFlag;				//0x7E
	u8 Address1;				//00h	
	u8 Address2;				//00h	
	u8 Control;					//13h
	VVVFSdrText VVVFSdrData;
	u8 CRCH8;
	u8 CRCL8;
	u8 EndFlag;					//0x7E
}VVVFSdrFrame;

typedef struct
{
	u8	TextNumber;				//文本数
	u8	Command;				//命令
	u8  State;					//状态
	u8  Sender;					//发送者		 30h监控装置
	u8  Receiver; 			//接受者   40hVVVF1,50hVVVF2
	u8	OrderNo;				//序号NO   00h		
	
	/*byte7*/
	u8	Byte7bit0Undef		:1;//空
	u8	TCU_RBDRVL				:1;//(后退定速回零)
	u8	TCU_RBDRVM				:1;//(后退运行)
	u8 	TCU_PWMF					:1;//(扭矩指令异常)
	u8 	TCU_ElectricBrake	:1;//T_PWMF(指令异常)
	u8 	TCU_B							:1;//B(电制)
	u8 	TCU_Retral				:1;//R(后退)
	u8 	TCU_Forward				:1;//F(前进)

	/*byte8*/
	u8 	TCU_THN3					:1;//THN3
	u8 	Byte8bit1Undef		:1;//空
	u8 	TCU_CWN						:1;//CWN
	u8 	TCU_L_RD					:1;//VVVF1:L2RD, VVVF2:L3RD
	u8 	TCU_L1R_D					:1;//VVVF1:L1R1D, VVVF2:L1R2D
	u8 	TCU_CR_D					:1;//VVVF1:CR1D, VVVF2:CR2D
	u8 	TCU_FDR_D8				:1;//VVVF1:FDR1D, VVVF2:FDR2D	
	u8 	TCU_HBR_D					:1;//VVVF1:HBR1D, VVVF2:HBR2D		 
	
	/*byte9*/
	u8 	Byte9bit0Undef		:1;//空
	u8 	TCU_NRBRD					:1;//NRBRD(不缓解)
	u8 	TCU_UCORSR				:1;//UCORSR
	u8 	TCU_UCOR_D				:1;//VVVF1:UCOR2D, VVVF2:UCOR1D		
	u8 	TCU_FDR_D9				:1;//VVVF1:FDR2D, VVVF2:FDR1D		
	u8 	TCU_L_D9					:1;//VVVF1:L3D, VVVF2:L2D
	u8 	TCU_DFR_D					:1;//VVVF1:DFR1D, VVVF2:DFR2D
	u8 	TCU_UCOR					:1;//UCOR
	
	/*byte10*/
	u8 	TCU_RS						:1;//RS
	u8 	TCU_L_D10					:1;//VVVF1:L2D, VVVF2:L1D
	u8 	TCU_HT						:1;//HT(高加速)
	u8 	TCU_1A						:1;//1A(前进后退“开”)
	u8 	TCU_HB						:1;//HB(HB动作)
	u8 	TCU_EB						:1;//EB(紧急制动)
	u8 	TCU_L1D						:1;//	L1D
	u8 	TCU_EMCTQ					:1;//EMCTQ(紧急运行存储)

	/*byte11*/
	u8 	Byte11bit0Undef		:1;//空
	u8 	TCU_KS						:1;//ks
	u8 	TCU_TS1						:1;//TS1(补足制动试验)
	u8 	TCU_LFDR					:1;//LFDR(轻微故障)
	u8 	TCU_TS2						:1;//TS2(HB脱扣试验)
	u8	Byte11bit5Undef		:1;
	u8	Byte11bit6Undef		:1;
	u8	Byte11bit7Undef		:1;
		
	/*byte12*/
	u8 	TCU_WDSYS					:1;//WD_SYS
	u8 	TCU_REMF					:1;//REM_F
	u8 	TCU_CONTF					:1;//CONT_F
	u8 	TCU_CFDW					:1;//TCU_CFDW
	u8 	TCU_CFDV					:1;//CFD_V
	u8 	TCU_CFDU					:1;//CFD_U
	u8	Byte12bit6Undef		:1;
	u8	Byte12bit7Undef		:1;
	
	/*byte13*/
	u8 	TCU_PGEA					:1;//PGE_A(PG1故障)
	u8 	TCU_PGEB					:1;//PGE_B(PG2故障)
	u8 	TCU_PGDA					:1;//PGD_A
	u8 	TCU_PGDB					:1;//PGD_B
	u8 	TCU_SPG1					:1;//SPG1(PG选择)
	u8 	TCU_SPG2					:1;//SPG2
	u8 	Byte13bit6Undef		:1;//
	u8 	TCU_BSL						:1;//BSL

	/*byte14*/
	u8 	Byte14bit0Undef		:1;
	u8 	Byte14bit1Undef		:1;	
	u8 	Byte14bit2Undef		:1;
	u8 	TCU_FCLW					:1;//FCL_W
	u8 	TCU_FCLV					:1;//FCL_V
	u8 	TCU_FCLU					:1;//FCL_U
	u8 	Byte14bit6Undef		:1;
	u8 	TCU_FOVD					:1;//FOVD

	/*byte15*/
	u8 	TCU_CNTFDR				:1;//CNT_FDR
	u8 	TCU_CNTHBT				:1;//CNT_HBT
	u8 	TCU_HBOFF					:1;//HB_OFF
	u8 	TCU_MONCWN				:1;//MON_CWN
	u8	Byte15Undef4			:4;
	
	/*byte16*/
	u8 	TCU_CHOPC					:1;//CHOP_C
	u8 	TCU_MRSC					:1;//MRSC
	u8 	TCU_ARSC					:1;//ARSC
	u8 	TCU_GECHOP				:1;//GE_CHOP
	u8 	Byte16Undef3			:3;
	u8 	TCU_HDLTRG				:1;//HDL_TRG

	/*byte17*/
	u8 	TCU_FCF						:1;//FC_F
	u8 	TCU_FRF						:1;//FR_F
	u8 	TCU_MMOCD2F				:1;//MMOCD2_F
	u8 	TCU_PUDF					:1;//PUD_F
	u8 	TCU_CFDF					:1;//CFD_F
	u8 	TCU_HBTF					:1;//HBT_F
	u8 	TCU_RYTF					:1;//RYT_F
	u8 	TCU_FDRF					:1;//FDR_F
	
	/*byte18*/
	u8 	TCU_WDF						:1;//WD_F
	u8 	TCU_OVCRF					:1;//OVCR_F
	u8	TCU_CTLVF					:1;//TCU_CTLVF
	u8 	TCU_THD3F					:1;//THD3_F
	u8 	TCU_GPokF					:1;//GPok_F
	u8 	TCU_D10okF				:1;//D10ok_F
	u8 	TCU_OVTF					:1;//OVT_F
	u8 	TCU_THD2F					:1;//THD2_F

	/*byte19*/
	u8 	TCU_LGDF					:1;//LGD_F(架线接地)
	u8 	TCU_BSLBF					:1;//BSLB_F
	u8 	TCU_BSLPF					:1;//BSLP_F
	u8 	TCU_BSMF					:1;//BSM_F
	u8 	TCU_LVDF					:1;//LVD_F
	u8 	TCU_SOVDF					:1;//SOVD_F
	u8 	TCU_OVDF					:1;//OVD_F
	u8 	TCU_LOFMF					:1;//TCU_LOFMF
	
	/*byte20*/
	u8 	TCU_THD1F					:1;//THD1_F
	u8 	TCU_OVTC					:1;//OVTC(OVT接通)
	u8 	TCU_DETFMF				:1;//DET_FM_F
	u8 	TCU_CR1F					:1;//CR1_F
	u8 	TCU_BROHF					:1;//BROH_F
	u8 	TCU_TESTF					:1;//TEST_F
	u8 	TCU_SLDM					:1;//SLDM
	u8 	TCU_NVDF					:1;//NVD_F(无电压)

	/*other*/
	u8 	TCU_FM;							//FM  速度
	u8 	TCU_FMC;						//运算速度 FMC 
	u8 	TCU_ALPHA;					//变压率 ALPHA 
	u8 	TCU_VC;							//滤波电容器电压VC
	u8 	TCU_LineUValue;			//第三轨条电压VL
	u8 	TCU_IqP;						//力矩部分电流指令IqP
	u8 	TCU_TorqueCurrent;	//力矩电流 Iq
	u8 	TCU_IdP;						//磁通部分电流指令IdP
	u8 	TCU_Id;							//磁通部分电流Id
	u8 	TCU_LineIValue;			//第三轨电流 IL
	u8 	TCU_FS;							//滑差频率FS
	u8 	TCU_PWMSignal;			//PWM信号
	u8 	TCU_IM;							//电机电流实效IM
	u8 	TCU_EP;							//电制力信号 EP
	u8 	TCU_PGH;						//PG脉冲累计计时器（上位）
	u8 	TCU_PGL;						//PG脉冲累计计时器（下位）
	u8 	TCU_WHMP;						//牵引记录电力量（最下位WHMP）
	u8 	TCU_WHMB;						//再生记录电力量（最下位）WHMB
	u8 	TCU_TraceEventSum;	//跟踪数据（重大故障）件数
	u8 	TCU_HeadEventSum;		//标题数据（轻微故障）件数
	u8 	TCU_HDLEventSum	;		//HDL数据件数
	u8 	TCU_TE;							//牵引/制动力信号TE
	
}VVVFSdText;

typedef struct
{
	u8 StartPAD[4];			//开始PAD
	u8 StartFlag;				//0x7E
	u8 Address1;				//00h	
	u8 Address2;				//00h	
	u8 Control;					//13h
	VVVFSdText	 VVVFSdData;
	u8 CRCH8;
	u8 CRCL8;
	u8 EndFlag;					//0x7E
}VVVFSdFrame;

typedef struct
{
	u8	TextNumber;				//文本数
	u8	Command;				//命令
	u8  State;					//状态
	u8  Sender;					//发送者		 30h监控装置
	u8  Receiver; 			//接受者   40hVVVF1,50hVVVF2
	u8	OrderNo;				//序号NO   00h		
	u8 	CategoryCode;		//类别代码
	u8 	BlockNumH8;			//块号高位  0000h-0140h;
	u8 	BlockNumL8;			//
	u8 	Undef10byte[10];	//备用10字节
}VVVFTdrText;

typedef struct
{
	u8 StartPAD[4];			//开始PAD
	u8 StartFlag;				//0x7E
	u8 Address1;				//00h	
	u8 Address2;				//00h	
	u8 Control;					//13h
	VVVFTdrText	 VVVFTdrData;
	u8 CRCH8;
	u8 CRCL8;
	u8 EndFlag;					//0x7E
}VVVFTdrFrame;


typedef struct
{
	u8	FalutCode;			//故障代码
	
	u8 Year;						//年	00~99（公历后面2 位数） BCD
	u8 Month;						//月	BCD
	u8 Day;							//日	BCD
	u8 Hour;						//时	BCD
	u8 Minute;					//分	BCD
	u8 Second;					//秒	BCD
	
	u8 TCU_FM_Speed;		//FM  速度
	
	/*byte8*/
	u8	TCU_1BK						:1;//1BK
	u8	TCU_RBDRVL				:1;//(后退定速回零)
	u8	TCU_RBDRVM				:1;//(后退运行)
	u8 	TCU_PWMF					:1;//(扭矩指令异常)
	u8 	TCU_ElectricBrake	:1;//T_PWMF(指令异常)
	u8 	TCU_B							:1;//B(电制)
	u8 	TCU_Retral				:1;//R(后退)
	u8 	TCU_Forward				:1;//F(前进)

	/*byte10*/
	u8 	TCU_THN3					:1;//THN3
	u8 	Byte10bit1Undef		:1;//空
	u8 	TCU_CWN						:1;//CWN
	u8 	TCU_L_RD					:1;//VVVF1:L2RD, VVVF2:L3RD
	u8 	TCU_L1R_D					:1;//VVVF1:L1R1D, VVVF2:L1R2D
	u8 	TCU_CR_D					:1;//VVVF1:CR1D, VVVF2:CR2D
	u8 	TCU_FDR_D8				:1;//VVVF1:FDR1D, VVVF2:FDR2D	
	u8 	TCU_HBR_D					:1;//VVVF1:HBR1D, VVVF2:HBR2D		 
	
	/*byte11*/
	u8 	TCU_ATTC					:1;//ATTC
	u8 	TCU_NRBRD					:1;//NRBRD(不缓解)
	u8 	TCU_UCORSR				:1;//UCORSR
	u8 	TCU_UCOR_D				:1;//VVVF1:UCOR2D, VVVF2:UCOR1D		
	u8 	TCU_FDR_D9				:1;//VVVF1:FDR2D, VVVF2:FDR1D		
	u8 	TCU_L_D9					:1;//VVVF1:L3D, VVVF2:L2D
	u8 	TCU_DFR_D					:1;//VVVF1:DFR1D, VVVF2:DFR2D
	u8 	TCU_UCOR					:1;//UCOR
	
	/*byte12*/
	u8 	TCU_RS						:1;//RS
	u8 	TCU_L_D10					:1;//VVVF1:L2D, VVVF2:L1D
	u8 	TCU_HT						:1;//HT(高加速)
	u8 	TCU_1A						:1;//1A(前进后退“开”)
	u8 	TCU_HB						:1;//HB(HB动作)
	u8 	TCU_EB						:1;//EB(紧急制动)
	u8 	TCU_L1D						:1;//	L1D
	u8 	TCU_EMCTQ					:1;//EMCTQ(紧急运行存储)

	/*byte13*/
	u8 	Byte13bit0Undef		:1;//空
	u8 	TCU_KS						:1;//ks
	u8 	TCU_TS1						:1;//TS1(补足制动试验)
	u8 	TCU_LFDR					:1;//LFDR(轻微故障)
	u8 	TCU_TS2						:1;//TS2(HB脱扣试验)
	u8	Byte13bit5Undef		:1;
	u8	Byte13bit6Undef		:1;
	u8	Byte13bit7Undef		:1;
		
	/*byte14*/
	u8 	TCU_WDSYS					:1;//WD_SYS
	u8 	TCU_REMF					:1;//REM_F
	u8 	TCU_CONTF					:1;//CONT_F
	u8 	TCU_CFDW					:1;//TCU_CFDW
	u8 	TCU_CFDV					:1;//CFD_V
	u8 	TCU_CFDU					:1;//CFD_U
	u8	Byte14bit6Undef		:1;
	u8	Byte14bit7Undef		:1;
	
	/*byte15*/
	u8 	TCU_PGEA					:1;//PGE_A(PG1故障)
	u8 	TCU_PGEB					:1;//PGE_B(PG2故障)
	u8 	TCU_PGDA					:1;//PGD_A
	u8 	TCU_PGDB					:1;//PGD_B
	u8 	TCU_SPG1					:1;//SPG1(PG选择)
	u8 	TCU_SPG2					:1;//SPG2
	u8 	Byte15bit6Undef		:1;//
	u8 	TCU_BSL						:1;//BSL

	/*byte16*/
	u8 	Byte16bit0Undef		:1;
	u8 	Byte16bit1Undef		:1;	
	u8 	Byte16bit2Undef		:1;
	u8 	TCU_FCLW					:1;//FCL_W
	u8 	TCU_FCLV					:1;//FCL_V
	u8 	TCU_FCLU					:1;//FCL_U
	u8 	Byte16bit6Undef		:1;
	u8 	TCU_FOVD					:1;//FOVD

	/*byte17*/
	u8 	TCU_CNTFDR				:1;//CNT_FDR
	u8 	TCU_CNTHBT				:1;//CNT_HBT
	u8 	TCU_HBOFF					:1;//HB_OFF
	u8 	TCU_MONCWN				:1;//MON_CWN
	u8 	TCU_V_CHOP				:1;//V_CHOP
	u8	Byte17Undef2			:2;
	u8 	TCU_HDL_TRG				:1;//HDL_TRG	
	
	/*byte17*/
	u8 	TCU_CHOPC					:1;//CHOP_C
	u8 	TCU_MRSC					:1;//MRSC
	u8 	TCU_ARSC					:1;//ARSC
	u8 	TCU_GECHOP				:1;//GE_CHOP
	u8 	Byte16Undef3			:3;
	u8 	TCU_HDLTRG				:1;//HDL_TRG

	/*byte18*/
	u8 	TCU_FCF						:1;//FC_F
	u8 	TCU_FRF						:1;//FR_F
	u8 	TCU_MMOCD2F				:1;//MMOCD2_F
	u8 	TCU_PUDF					:1;//PUD_F
	u8 	TCU_CFDF					:1;//CFD_F
	u8 	TCU_HBTF					:1;//HBT_F
	u8 	TCU_RYTF					:1;//RYT_F
	u8 	TCU_FDRF					:1;//FDR_F
	
	/*byte19*/
	u8 	TCU_WDF						:1;//WD_F
	u8 	TCU_OVCRF					:1;//OVCR_F
	u8	TCU_CTLVF					:1;//TCU_CTLVF
	u8 	TCU_THD3F					:1;//THD3_F
	u8 	TCU_GPokF					:1;//GPok_F
	u8 	TCU_D10okF				:1;//D10ok_F
	u8 	TCU_OVTF					:1;//OVT_F
	u8 	TCU_THD2F					:1;//THD2_F

	/*byte20*/
	u8 	TCU_LGDF					:1;//LGD_F(架线接地)
	u8 	TCU_BSLBF					:1;//BSLB_F
	u8 	TCU_BSLPF					:1;//BSLP_F
	u8 	TCU_BSMF					:1;//BSM_F
	u8 	TCU_LVDF					:1;//LVD_F
	u8 	TCU_SOVDF					:1;//SOVD_F
	u8 	TCU_OVDF					:1;//OVD_F
	u8 	TCU_LOFMF					:1;//TCU_LOFMF
	
	/*byte21*/
	u8 	TCU_THD1F					:1;//THD1_F
	u8 	TCU_OVTC					:1;//OVTC(OVT接通)
	u8 	TCU_DETFMF				:1;//DET_FM_F
	u8 	TCU_CR1F					:1;//CR1_F
	u8 	TCU_BROHF					:1;//BROH_F
	u8 	TCU_TESTF					:1;//TEST_F
	u8 	TCU_SLDM					:1;//SLDM
	u8 	TCU_NVDF					:1;//NVD_F(无电压)
	
	/*byte22*/
	u8 	TCU_ContModeM			:1;//ContModeM
	u8 	Byte22Undef3bit		:3;
	u8	TCU_P_MODE				:4;//TCU_P_MODE
	
	/*byte23*/
	u8 	TCU_ATTM					:1;//ATTM	
	u8 	TCU_CWN_D					:1;//CWN_D	
	u8 	TCU_BCH_C					:1;//BCH_C
	u8 	TCU_FMR						:1;//FMR	
	u8 	TCU_RMR						:1;//RMR
	u8 	TCU_BM						:1;//BM
	u8	TCU_PM						:1;//PM
	u8 	CWN_M							:1;//CWN_M
	
	u8 	Undef9Byte[9];		
	
}VVVFTdHeardText;

typedef struct
{
	/*byte1*/
	u8	TCU_1BK						:1;//1BK
	u8	TCU_RBDRVL				:1;//(后退定速回零)
	u8	TCU_RBDRVM				:1;//(后退运行)
	u8 	TCU_PWMF					:1;//(扭矩指令异常)
	u8 	TCU_ElectricBrake	:1;//T_PWMF(指令异常)
	u8 	TCU_B							:1;//B(电制)
	u8 	TCU_Retral				:1;//R(后退)
	u8 	TCU_Forward				:1;//F(前进)

	/*byte2*/
	u8 	TCU_THN3					:1;//THN3
	u8 	Byte2bit1Undef		:1;//空
	u8 	TCU_CWN						:1;//CWN
	u8 	TCU_L_RD					:1;//VVVF1:L2RD, VVVF2:L3RD
	u8 	TCU_L1R_D					:1;//VVVF1:L1R1D, VVVF2:L1R2D
	u8 	TCU_CR_D					:1;//VVVF1:CR1D, VVVF2:CR2D
	u8 	TCU_FDR_D8				:1;//VVVF1:FDR1D, VVVF2:FDR2D	
	u8 	TCU_HBR_D					:1;//VVVF1:HBR1D, VVVF2:HBR2D		 
	
	/*byte3*/
	u8 	TCU_ATTC					:1;//ATTC
	u8 	TCU_NRBRD					:1;//NRBRD(不缓解)
	u8 	TCU_UCORSR				:1;//UCORSR
	u8 	TCU_UCOR_D				:1;//VVVF1:UCOR2D, VVVF2:UCOR1D		
	u8 	TCU_FDR_D9				:1;//VVVF1:FDR2D, VVVF2:FDR1D		
	u8 	TCU_L_D9					:1;//VVVF1:L3D, VVVF2:L2D
	u8 	TCU_DFR_D					:1;//VVVF1:DFR1D, VVVF2:DFR2D
	u8 	TCU_UCOR					:1;//UCOR
	
	/*byte4*/
	u8 	TCU_RS						:1;//RS
	u8 	TCU_L_D10					:1;//VVVF1:L2D, VVVF2:L1D
	u8 	TCU_HT						:1;//HT(高加速)
	u8 	TCU_1A						:1;//1A(前进后退“开”)
	u8 	TCU_HB						:1;//HB(HB动作)
	u8 	TCU_EB						:1;//EB(紧急制动)
	u8 	TCU_L1D						:1;//	L1D
	u8 	TCU_EMCTQ					:1;//EMCTQ(紧急运行存储)

	/*byte5*/
	u8 	Byte5bit0Undef		:1;//空
	u8 	TCU_KS						:1;//ks
	u8 	TCU_TS1						:1;//TS1(补足制动试验)
	u8 	TCU_LFDR					:1;//LFDR(轻微故障)
	u8 	TCU_TS2						:1;//TS2(HB脱扣试验)
	u8	Byte5bit5Undef		:1;
	u8	Byte5bit6Undef		:1;
	u8	Byte5bit7Undef		:1;
		
	/*byte6*/
	u8 	TCU_WDSYS					:1;//WD_SYS
	u8 	TCU_REMF					:1;//REM_F
	u8 	TCU_CONTF					:1;//CONT_F
	u8 	TCU_CFDW					:1;//TCU_CFDW
	u8 	TCU_CFDV					:1;//CFD_V
	u8 	TCU_CFDU					:1;//CFD_U
	u8	Byte6bit6Undef		:1;
	u8	Byte6bit7Undef		:1;
	
	/*byte7*/
	u8 	TCU_PGEA					:1;//PGE_A(PG1故障)
	u8 	TCU_PGEB					:1;//PGE_B(PG2故障)
	u8 	TCU_PGDA					:1;//PGD_A
	u8 	TCU_PGDB					:1;//PGD_B
	u8 	TCU_SPG1					:1;//SPG1(PG选择)
	u8 	TCU_SPG2					:1;//SPG2
	u8 	Byte7bit6Undef		:1;//
	u8 	TCU_BSL						:1;//BSL

	/*byte8*/
	u8 	Byte8bit0Undef		:1;
	u8 	Byte8bit1Undef		:1;	
	u8 	Byte8bit2Undef		:1;
	u8 	TCU_FCLW					:1;//FCL_W
	u8 	TCU_FCLV					:1;//FCL_V
	u8 	TCU_FCLU					:1;//FCL_U
	u8 	Byte8bit6Undef		:1;
	u8 	TCU_FOVD					:1;//FOVD

//	/*byte9*/		?????
//	u8 	TCU_CNTFDR				:1;//CNT_FDR
//	u8 	TCU_CNTHBT				:1;//CNT_HBT
//	u8 	TCU_HBOFF					:1;//HB_OFF
//	u8 	TCU_MONCWN				:1;//MON_CWN
//	u8 	TCU_V_CHOP				:1;//V_CHOP
//	u8	Byte9Undef2			:2;
//	u8 	TCU_HDL_TRG				:1;//HDL_TRG	
	
	/*byte9*/
	u8 	TCU_CHOPC					:1;//CHOP_C
	u8 	TCU_MRSC					:1;//MRSC
	u8 	TCU_ARSC					:1;//ARSC
	u8 	TCU_GECHOP				:1;//GE_CHOP
	u8 	Byte10Undef3			:3;
	u8 	TCU_HDLTRG				:1;//HDL_TRG

	/*byte10*/
	u8 	TCU_FCF						:1;//FC_F
	u8 	TCU_FRF						:1;//FR_F
	u8 	TCU_MMOCD2F				:1;//MMOCD2_F
	u8 	TCU_PUDF					:1;//PUD_F
	u8 	TCU_CFDF					:1;//CFD_F
	u8 	TCU_HBTF					:1;//HBT_F
	u8 	TCU_RYTF					:1;//RYT_F
	u8 	TCU_FDRF					:1;//FDR_F
	
	/*byte11*/
	u8 	TCU_WDF						:1;//WD_F
	u8 	TCU_OVCRF					:1;//OVCR_F
	u8	TCU_CTLVF					:1;//TCU_CTLVF
	u8 	TCU_THD3F					:1;//THD3_F
	u8 	TCU_GPokF					:1;//GPok_F
	u8 	TCU_D10okF				:1;//D10ok_F
	u8 	TCU_OVTF					:1;//OVT_F
	u8 	TCU_THD2F					:1;//THD2_F

	/*byte12*/
	u8 	TCU_LGDF					:1;//LGD_F(架线接地)
	u8 	TCU_BSLBF					:1;//BSLB_F
	u8 	TCU_BSLPF					:1;//BSLP_F
	u8 	TCU_BSMF					:1;//BSM_F
	u8 	TCU_LVDF					:1;//LVD_F
	u8 	TCU_SOVDF					:1;//SOVD_F
	u8 	TCU_OVDF					:1;//OVD_F
	u8 	TCU_LOFMF					:1;//TCU_LOFMF
	
	/*byte13*/
	u8 	TCU_THD1F					:1;//THD1_F
	u8 	TCU_OVTC					:1;//OVTC(OVT接通)
	u8 	TCU_DETFMF				:1;//DET_FM_F
	u8 	TCU_CR1F					:1;//CR1_F
	u8 	TCU_BROHF					:1;//BROH_F
	u8 	TCU_TESTF					:1;//TEST_F
	u8 	TCU_SLDM					:1;//SLDM
	u8 	TCU_NVDF					:1;//NVD_F(无电压)
	
	/*byte14*/
	u8 	TCU_ContModeM			:1;//ContModeM
	u8 	Byte22Undef3bit		:3;
	u8	TCU_P_MODE				:4;//TCU_P_MODE
	
		/*other*/
	u8 	TCU_FM;							//FM  速度
	u8 	TCU_FMC;						//运算速度 FMC 
	u8 	TCU_ALPHA;					//变压率 ALPHA 
	u8 	TCU_VC;							//滤波电容器电压VC
	u8 	TCU_LineUValue;			//第三轨条电压VL
	u8  TCU_TqP;						//力矩指令TqP
	u8 	TCU_Tq;							//力矩输出Tq
	u8 	TCU_IqP;						//力矩部分电流指令IqP
	u8 	TCU_TorqueCurrent;	//力矩电流 Iq
	u8 	TCU_IdP;						//磁通部分电流指令IdP
	u8 	TCU_Id;							//磁通部分电流Id
	u8 	TCU_LineIValue;			//第三轨电流 IL
	u8 	TCU_FS;							//滑差频率FS
	u8	TCU_LA;							//载重信号LA
	u8 	TCU_DA;							//电制动指令DA
	u8 	TCU_EP;							//电制力信号 EP
	u8 	TCU_TE;							//牵引/制动力信号TE
	u8 	TCU_PWMSignal;			//PWM信号
	
}VVVFSampleText;

typedef struct
{
	u8	TextNumber;				//文本数
	u8	Command;				//命令
	u8  State;					//状态
	u8  Sender;					//发送者		 30h监控装置
	u8  Receiver; 			//接受者   40hVVVF1,50hVVVF2
	u8	OrderNo;				//序号NO   00h		
	u8 	CategoryCode;		//类别代码
	u8 	BlockNumH8;			//块号高位  0000h-0140h;
	u8 	BlockNumL8;			//
	
	u8 	Undef1;					//00h
	u8 	Undef2;
	
	u8	TraceData[100]; 
}VVVFTdText;

typedef struct
{
	u8 StartPAD[4];			//开始PAD
	u8 StartFlag;				//0x7E
	u8 Address1;				//00h	
	u8 Address2;				//00h	
	u8 Control;					//13h
	VVVFTdText	 VVVFTdData;
	u8 CRCH8;
	u8 CRCL8;
	u8 EndFlag;					//0x7E
}VVVFTdFrame;

typedef struct
{
	u8	TextNumber;				//文本数
	u8	Command;				//命令
	u8  State;					//状态
	u8  Sender;					//发送者		 30h监控装置
	u8  Receiver; 			//接受者   40hVVVF1,50hVVVF2
	u8	OrderNo;				//序号NO   00h		
	u8 	CategoryCode;		//类别代码  02h
	
	u8	Undef12Byte[12];
}VVVFTcrText;

typedef struct
{
	u8 StartPAD[4];			//开始PAD
	u8 StartFlag;				//0x7E
	u8 Address1;				//00h	
	u8 Address2;				//00h	
	u8 Control;					//13h
	VVVFTcrText	 VVVFTcrData;
	u8 CRCH8;
	u8 CRCL8;
	u8 EndFlag;					//0x7E
}VVVFTcrFrame;


/*
	函数说明：
功能：发送制动器的状态请求数据。
参数:	
	pInData:指向应用层要发送数据的指针 (指向的类型为，VVVFSdrText,或者 VVVFSdText,VVVFTcrText)   
	pOutData:指向底层串口发送的数据
	u8 mode:发送的数据为SDR，还是TDR	TCR; 
返回值：无返回值
*/
void SendVVVFSdrOrTdrOrTcr(void * pInData,/*u8 * pOutData,*/u8 mode);  


/*
	函数说明：
功能：接受车门控制的状态响应的数据。
参数: 
	pInData:指向要底层硬件接受到的数据的指针 
	pOutData:监控设备，得到每个状态标识(放到S_TMS_BRAKE_SD_Text结构体)。
	VVVF_No: 表示获取VVVF1，或者VVVF2
//	u8 mode :接受的数据为SD，或者 TD0,TD1
返回值：0,无错，  其他有错。
*/
s8 GetVVVFSdOrTd(u8 *pInData,void *pOutData,u8 VVVF_No);

/*
	函数说明：
功能：检测接受数据时候的错误
参数: pInData:指向要底层硬件接受到的数据的指针 
			VVVF_No: 表示获取VVVF1，或者VVVF2
返回值：返回0，没有错误。返回其他值，错误值
*/
s8 CheckVVVFGetError(u8 *pInData,u8 VVVF_No);

#endif //__VVVFCOMPRO_H__
