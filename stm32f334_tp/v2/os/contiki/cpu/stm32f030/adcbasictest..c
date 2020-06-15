#include "stm32f0xx.h"
#include "basictype.h"
#include "iodef.h"
#include "arch_adc.h"
#include "sysprintf.h"
#include "contiki.h"



PROCESS(adcb_test, "adc_test");
/**
  * @brief  ADC Configuration
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  ADC_InitTypeDef     ADC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;
  
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Configure ADC Channel11 as analog input */
#ifdef USE_STM320518_EVAL
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
#else
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
#endif /* USE_STM320518_EVAL */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* ADCs DeInit */  
  ADC_DeInit(ADC1);
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStructure);
  
  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure); 
#if 0  
  /* Convert the ADC1 Channel 11 with 239.5 Cycles as sampling time */ 
#ifdef USE_STM320518_EVAL
  ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_239_5Cycles);
#else
  ADC_ChannelConfig(ADC1, ADC_Channel_17, ADC_SampleTime_239_5Cycles);
#endif /* USE_STM320518_EVAL */
#endif
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles); 
  ADC_VrefintCmd(ENABLE);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
  
}


//{
/* Performs the AD conversion */




PROCESS_THREAD(adcb_test, ev, data)
{
	static struct etimer et_led;
	 uint16_t  ADC1ConvertedValue = 0;
	//int i = 0;
	//u_short ADC1ConvertedValue;
	PROCESS_BEGIN();
	ADC_Config( );
	etimer_set(&et_led, 1000);
	while(1)
	{
		PROCESS_YIELD_UNTIL(etimer_expired(&et_led));
		etimer_reset(&et_led);
	    /* Test EOC flag */
	    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	    
	    /* Get ADC1 converted data */
	    ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
	    
	    /* Compute the voltage */

		XPRINTF((10, "%d\r\n", ADC1ConvertedValue));
	}
	PROCESS_END();
}



void adcbTest(void)
{
	process_start(&adcb_test, NULL);
}

