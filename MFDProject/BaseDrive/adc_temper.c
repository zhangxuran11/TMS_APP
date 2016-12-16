#include "adc_temper.h"
//#include "Gpio.h"

__IO uint16_t ADC3ConvertedValue = 0;
__IO uint32_t ADC3ConvertedVoltage = 0;

/**
  * @brief  ADC3 channel06 with DMA configuration
  * @param  None
  * @retval None
  */
void ADC1_CH16_Config(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
//    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
    
    /* Enable ADC1,  and GPIO clocks ****************************************/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
	  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);  
	  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);  
	  ADC_TempSensorVrefintCmd(ENABLE);
	

    /* Configure ADC3 Channel6 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;			 //独立模式
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;		 //ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;		//不使用DMA
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);			//ADC通用初始化
    
    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;			//12位分辨率
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//非扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//不使用连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//规则通道外部触发边沿选择,此处未用到
    //ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			//数据右对齐
    ADC_InitStructure.ADC_NbrOfConversion = 1;									//1个转换在规则序列中 也就是只转换规则序列1 
    ADC_Init(ADC1, &ADC_InitStructure);													//ADC1初始化
    
    /* ADC1 regular channel6 configuration *************************************/
   // ADC_RegularChannelConfig(ADC3, ADC_Channel_6, 1, ADC_SampleTime_3Cycles);
    
    /* Enable DMA request after last transfer (Single-ADC mode) */
   // ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
    
    /* Enable ADC3 DMA */
   // ADC_DMACmd(ADC3, ENABLE);
  //  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles );	
	
		ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles );		//规则通道组设置 通道1  480个周期,提高采样时间可以提高精确度	
		ADC_TempSensorVrefintCmd(ENABLE);  //开启温度传感器
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
}

//获得ADC值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_AdcValue(void)   
{
	 //设置指定ADC的规则组通道，一个序列，采样时间
	//ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	
  
	ADC_SoftwareStartConv(ADC1);		//使能软件转换功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	          //返回最近一次ADC1规则组的转换结果
}

//获取通道ch的转换值，取number次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u32 Get_Adc_Average(u8 number)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<number;t++)
	{
		temp_val+=Get_AdcValue();                     //求ADC平均值
		delay_ms(5);
	}
	return temp_val/number;
} 

//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
short Get_Temprate(void)
{
  u32 AdcValue;
  double temperate;
	short result;
  AdcValue=Get_Adc_Average(20);  
	printf("\r\nADC值是:%d\r\n",AdcValue);   
  temperate=(float)AdcValue*(3.3/4096);      
  temperate=(temperate-0.76)/0.0025+25;    
  printf("\r\n芯片内部温度是:%f C\r\n",temperate);	
	result = temperate*100; //扩大100倍
	return result;
}

void ADC_Temper_Test(void)
{
 // LEDGpio_Init();
	ADC1_CH16_Config();
   
    while (1)
    {
			Get_Temprate();	
		//	LED_Turn (0);
			delay_ms(1000);

    }
}

