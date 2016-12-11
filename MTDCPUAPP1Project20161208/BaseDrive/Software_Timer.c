#include "Software_Timer.h"
#include "os.h"
#include "Uart.h"
/*
	��������Ӳ����ʱ����TIM�� ����ʹ TIM2 - TIM5
	TIM3 ��TIM4 ��16λ
	TIM2 ��TIM5 ��32λ
*/
#define TIM_HARD		TIM4
#define TIM_HARD_IRQn	TIM4_IRQn
#define TIM_HARD_RCC	RCC_APB1Periph_TIM4

/* ����������ʱ���ṹ����� */
static SOFT_TMR s_tTmr[TMR_COUNT];

/*
	ȫ������ʱ�䣬��λ1ms
	����Ա�ʾ ��32λ�ļ�24.85�죬�����Ĳ�Ʒ��������ʱ�䳬�������������뿼���������
*/
__IO int64_t g_iRunTime = 0;

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

///* ���� TIM��ʱ�жϵ���ִ�еĻص�����ָ�� */
//static void (*s_TIM_CallBack1)(void);
//static void (*s_TIM_CallBack2)(void);
//static void (*s_TIM_CallBack3)(void);
//static void (*s_TIM_CallBack4)(void);

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitTimer
*	����˵��: ����systick�жϣ�����ʼ��������ʱ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitTimer(void)				//��������������ʱ�ӽ��ĵĳ�ʼ������������delay.c�������漴��
{
	uint8_t i;

	/* �������е�������ʱ�� */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* ȱʡ��1���Թ���ģʽ */
	}

	/*
		����systic�ж�����Ϊ1ms��������systick�жϡ�		

    	SystemCoreClock �ǹ̼��ж����ϵͳ�ں�ʱ�ӣ�����STM32F4XX,һ��Ϊ 168MHz

    	SysTick_Config() �������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�.
	    	-- SystemCoreClock / 1000  ��ʾ��ʱƵ��Ϊ 1000Hz�� Ҳ���Ƕ�ʱ����Ϊ  1ms
	    	-- SystemCoreClock / 500   ��ʾ��ʱƵ��Ϊ 500Hz��  Ҳ���Ƕ�ʱ����Ϊ  2ms
	    	-- SystemCoreClock / 2000  ��ʾ��ʱƵ��Ϊ 2000Hz�� Ҳ���Ƕ�ʱ����Ϊ  500us

    	���ڳ����Ӧ�ã�����һ��ȡ��ʱ����1ms�����ڵ���CPU���ߵ͹���Ӧ�ã��������ö�ʱ����Ϊ 10ms
    */
//	SysTick_Config(SystemCoreClock / 1000);			��delay_init()�������Ѿ���ʼ���δ�ʱ��Ϊ1ms�ж�һ�Ρ��ʲ���Ҫ�ٵ���

//	bsp_InitHardTimer();	/* ����Ӳ����ʱ�ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: SysTick_ISR
*	����˵��: SysTick�жϷ������ÿ��1ms����1��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//extern void bsp_RunPer1ms(void);
//extern void bsp_RunPer10ms(void);

void SysTick_ISR(void)
{
//	static uint8_t  s_count = 0;
	uint8_t i;

	/* ÿ��1ms����������ʱ���ļ��������м�һ���� */
	for (i = 0; i < TMR_COUNT; i++)
	{
		bsp_SoftTimerDec(&s_tTmr[i]);
	}

	/* ȫ������ʱ��ÿ1ms��1 */
//	g_iRunTime++;
//	
//	if (g_iRunTime == 0x7FFFFFFFFFFFFFFF)	/* ��������� int64_t ���ͣ������Ϊ 0x7FFFFFFF */
//	{
//		g_iRunTime = 0;
//	}

//	bsp_RunPer1ms();		/* ÿ��1ms����һ�δ˺������˺����� bsp.c */

//	if (++s_count >= 10)
//	{
//		s_count = 0;

//		bsp_RunPer10ms();	/* ÿ��10ms����һ�δ˺������˺����� bsp.c */
//	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SoftTimerDec
*	����˵��: ÿ��1ms�����ж�ʱ��������1�����뱻SysTick_ISR�����Ե��á�
*	��    ��:  _tmr : ��ʱ������ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* �����ʱ����������1�����ö�ʱ�������־ */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* ������Զ�ģʽ�����Զ���װ������ */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_StartTimer
*	����˵��: ����һ����ʱ���������ö�ʱ���ڡ�
*	��    ��:  	_id     : ��ʱ��ID��ֵ��0,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
	CPU_SR_ALLOC();	//�������壬���ڱ���SR״̬.
	
	if (_id >= TMR_COUNT)
	{
		/* ��ӡ������Դ�����ļ������������� */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* �����쳣�������ȴ����Ź���λ */
	}
														
 	CPU_CRITICAL_ENTER();		/* ���ж� */		//��ر�ȫ���жϣ�Ӱ��δ�ʱ����ֵ
//	OS_CRITICAL_ENTER();		//�õ���ȥ�����ķ�ʽ����֤���´��벻�����
	
	s_tTmr[_id].Count = _period;		/* ʵʱ��������ֵ */
	s_tTmr[_id].PreLoad = _period;		/* �������Զ���װֵ�����Զ�ģʽ������ */
	s_tTmr[_id].Flag = 0;				/* ��ʱʱ�䵽��־ */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1���Թ���ģʽ */

	CPU_CRITICAL_EXIT();   /* ���ж� */
//	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartAutoTimer
*	����˵��: ����һ���Զ���ʱ���������ö�ʱ���ڡ�
*	��    ��:  	_id     : ��ʱ��ID��ֵ��0,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*				_period : ��ʱ���ڣ���λ10ms
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
	CPU_SR_ALLOC();	//�������壬���ڱ���SR״̬.
	if (_id >= TMR_COUNT)
	{
		/* ��ӡ������Դ�����ļ������������� */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* �����쳣�������ȴ����Ź���λ */
	}

	CPU_CRITICAL_ENTER();		/* ���ж� */

	s_tTmr[_id].Count = _period;			/* ʵʱ��������ֵ */
	s_tTmr[_id].PreLoad = _period;		/* �������Զ���װֵ�����Զ�ģʽ������ */
	s_tTmr[_id].Flag = 0;							/* ��ʱʱ�䵽��־ */
	s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* �Զ�����ģʽ */

	CPU_CRITICAL_EXIT();   /* ���ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StopTimer
*	����˵��: ֹͣһ����ʱ��
*	��    ��:  	_id     : ��ʱ��ID��ֵ��0,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_StopTimer(uint8_t _id)
{
	CPU_SR_ALLOC();	//�������壬���ڱ���SR״̬.
	if (_id >= TMR_COUNT)
	{
		/* ��ӡ������Դ�����ļ������������� */
		printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
		while(1); /* �����쳣�������ȴ����Ź���λ */
	}

	CPU_CRITICAL_ENTER();		/* ���ж� */

	s_tTmr[_id].Count = 0;				/* ʵʱ��������ֵ */
	s_tTmr[_id].Flag = 0;				/* ��ʱʱ�䵽��־ */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* �Զ�����ģʽ */

	CPU_CRITICAL_EXIT();   /* ���ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CheckTimer
*	����˵��: ��ⶨʱ���Ƿ�ʱ
*	��    ��:  	_id     : ��ʱ��ID��ֵ��0,TMR_COUNT-1�����û���������ά����ʱ��ID���Ա��ⶨʱ��ID��ͻ��
*				_period : ��ʱ���ڣ���λ1ms
*	�� �� ֵ: ���� 0 ��ʾ��ʱδ���� 1��ʾ��ʱ��
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

///*		//�������uCOSiii�Ѿ��ṩ��OSTimeGet() ���������ȡϵͳ��ǰ�Ľ��ģ������ȫ�ֱ���OSTickCtr;�δ������
//*********************************************************************************************************
//*	�� �� ��: bsp_GetRunTime
//*	����˵��: ��ȡCPU����ʱ�䣬��λ1ms������Ա�ʾ 24.85�죬�����Ĳ�Ʒ��������ʱ�䳬�������������뿼���������
//*	��    ��:  ��
//*	�� �� ֵ: CPU����ʱ�䣬��λ1ms
//*********************************************************************************************************
//*/
//int64_t bsp_GetRunTime(void)
//{
//	int32_t runtime;
//	
//	CPU_SR_ALLOC();																	//�������壬���ڱ���SR״̬.
// 	CPU_CRITICAL_ENTER();		/* ���ж� */

//	runtime = g_iRunTime;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */

//	CPU_CRITICAL_EXIT();   /* ���ж� */

//	return runtime;
//}

/*
*********************************************************************************************************
*	�� �� ��: SysTick_Handler
*	����˵��: ϵͳ��શ�ʱ���жϷ�����������ļ��������˸ú�����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//�δ�ʱ�����жϷ�������� delay���ж��е���


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitHardTimer
*	����˵��: ���� TIM4������us����Ӳ����ʱ��TIM4���������У�����ֹͣ.
*			TIM4������TIM2 - TIM5 ֮���TIM, ��ЩTIM��4��ͨ��, ���� APB1 �ϣ�����ʱ��=SystemCoreClock / 2
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void bsp_InitHardTimer(void)
//{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	uint16_t usPeriod;
//	uint16_t usPrescaler;
//	uint32_t uiTIMxCLK;

//  	/* ʹ��TIMʱ�� */
//	RCC_APB1PeriphClockCmd(TIM_HARD_RCC, ENABLE);

//    /*-----------------------------------------------------------------------
//		system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

//		HCLK = SYSCLK / 1     (AHB1Periph)
//		PCLK2 = HCLK / 2      (APB2Periph)
//		PCLK1 = HCLK / 4      (APB1Periph)

//		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
//		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

//		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
//		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11

//	----------------------------------------------------------------------- */
//	uiTIMxCLK = SystemCoreClock / 2;

//	usPrescaler = uiTIMxCLK / 1000000 ;	/* ��Ƶ������ 1us */
//	usPeriod = 0xFFFF;

//	/* Time base configuration */
//	TIM_TimeBaseStructure.TIM_Period = usPeriod;
//	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

//	TIM_TimeBaseInit(TIM_HARD, &TIM_TimeBaseStructure);

//	//TIM_ARRPreloadConfig(TIMx, ENABLE);

//	/* TIMx enable counter */
//	TIM_Cmd(TIM_HARD, ENABLE);

//	/* ����TIM��ʱ�ж� (Update) */
//	{
//		NVIC_InitTypeDef NVIC_InitStructure;	/* �жϽṹ���� misc.h �ж��� */

//		NVIC_InitStructure.NVIC_IRQChannel = TIM_HARD_IRQn;

//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	/* �ȴ������ȼ��� */
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
//	}
//}

/*
*********************************************************************************************************
*	�� �� ��: bsp_StartHardTimer
*	����˵��: ʹ��TIM2-5�����ζ�ʱ��ʹ��, ��ʱʱ�䵽��ִ�лص�����������ͬʱ����4����ʱ�����������š�
*             ��ʱ��������10us ����Ҫ�ķ��ڵ��ñ�������ִ��ʱ�䣬�����ڲ������˲�����С��
*			 TIM2��TIM5 ��32λ��ʱ������ʱ��Χ�ܴ�
*			 TIM3��TIM4 ��16λ��ʱ����
*	��    ��: _CC : ����ͨ������1��2��3, 4
*             _uiTimeOut : ��ʱʱ��, ��λ 1us.       ����16λ��ʱ������� 65.5ms; ����32λ��ʱ������� 4294��
*             _pCallBack : ��ʱʱ�䵽�󣬱�ִ�еĺ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack)
//{
//    uint32_t cnt_now;
//    uint32_t cnt_tar;

//    /*
//        ִ�����������䣬ʱ�� = 18us (ͨ���߼������ǲ���IO��ת)
//        bsp_StartTimer2(3, 500, (void *)test1);
//    */
//    if (_uiTimeOut < 5)
//    {
//        ;
//    }
//    else
//    {
//        _uiTimeOut -= 5;
//    }

//    cnt_now = TIM_GetCounter(TIM_HARD);    	/* ��ȡ��ǰ�ļ�����ֵ */
//    cnt_tar = cnt_now + _uiTimeOut;			/* ���㲶��ļ�����ֵ */
//    if (_CC == 1)
//    {
//        s_TIM_CallBack1 = (void (*)(void))_pCallBack;

//        TIM_SetCompare1(TIM_HARD, cnt_tar);      	/* ���ò���Ƚϼ�����CC1 */
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC1);
//		TIM_ITConfig(TIM_HARD, TIM_IT_CC1, ENABLE);	/* ʹ��CC1�ж� */

//    }
//    else if (_CC == 2)
//    {
//		s_TIM_CallBack2 = (void (*)(void))_pCallBack;

//        TIM_SetCompare2(TIM_HARD, cnt_tar);      	/* ���ò���Ƚϼ�����CC2 */
//		TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC2);
//		TIM_ITConfig(TIM_HARD, TIM_IT_CC2, ENABLE);	/* ʹ��CC2�ж� */
//    }
//    else if (_CC == 3)
//    {
//        s_TIM_CallBack3 = (void (*)(void))_pCallBack;

//        TIM_SetCompare3(TIM_HARD, cnt_tar);      	/* ���ò���Ƚϼ�����CC3 */
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC3);
//		TIM_ITConfig(TIM_HARD, TIM_IT_CC3, ENABLE);	/* ʹ��CC3�ж� */
//    }
//    else if (_CC == 4)
//    {
//        s_TIM_CallBack4 = (void (*)(void))_pCallBack;

//        TIM_SetCompare4(TIM_HARD, cnt_tar);      	/* ���ò���Ƚϼ�����CC4 */
//		TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC4);
//		TIM_ITConfig(TIM_HARD, TIM_IT_CC4, ENABLE);	/* ʹ��CC4�ж� */
//    }
//	else
//    {
//        return;
//    }
//}

/*
*********************************************************************************************************
*	�� �� ��: TIMx_IRQHandler
*	����˵��: TIM �жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void TIM2_IRQHandler(void)
//void TIM3_IRQHandler(void)
//void TIM4_IRQHandler(void)
//void TIM5_IRQHandler(void)
//{
//    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC1))
//    {
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC1);
//        TIM_ITConfig(TIM_HARD, TIM_IT_CC1, DISABLE);	/* ����CC1�ж� */

//        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
//        s_TIM_CallBack1();
//    }

//    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC2))
//    {
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC2);
//        TIM_ITConfig(TIM_HARD, TIM_IT_CC2, DISABLE);	/* ����CC2�ж� */

//        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
//        s_TIM_CallBack2();
//    }

//    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC3))
//    {
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC3);
//        TIM_ITConfig(TIM_HARD, TIM_IT_CC3, DISABLE);	/* ����CC3�ж� */

//        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
//        s_TIM_CallBack3();
//    }

//    if (TIM_GetITStatus(TIM_HARD, TIM_IT_CC4))
//    {
//        TIM_ClearITPendingBit(TIM_HARD, TIM_IT_CC4);
//        TIM_ITConfig(TIM_HARD, TIM_IT_CC4, DISABLE);	/* ����CC4�ж� */

//        /* �ȹر��жϣ���ִ�лص���������Ϊ�ص�����������Ҫ������ʱ�� */
//        s_TIM_CallBack4();
//    }
//}