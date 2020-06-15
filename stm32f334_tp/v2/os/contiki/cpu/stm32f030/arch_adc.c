#include "basictype.h"

#include "arch_adc.h"
#include "sysprintf.h"
#include "contiki.h"

#define  ADC_MAX_CHECK_COUNT	16
#define  ADC_ISETA_FLAG			0
#define  ADC_OUT_FLAG			1
#define  ADC_REF_FLAG			2


#define   INDEX_TL431			2
#define   INDEX_ISETA			1
#define   INDEX_REF				0

#define   TL_431REF				2480

/*
ADC_Channel_1				TL431 OUT
ADC_Channel_11				ISETA
ADC_Channel_17				INNER REF
*/
#define  ADC_CHANNEL_NUM		3



//static volatile uint16_t adcDataV[2]={0};
static volatile uint16_t adcDataV[ADC_MAX_CHECK_COUNT][ADC_CHANNEL_NUM]={0};


static u_short iseta[ADC_MAX_CHECK_COUNT] = {0};
static u_short aOut[ADC_MAX_CHECK_COUNT] = {0};
static u_short iref[ADC_MAX_CHECK_COUNT] = {0};

static volatile u_short adcIseta = 0;
static volatile u_long adcOut = 0;
static volatile u_short adcRef = 0;


static ADC_OBJ_DATA adcObj = {0x00};

static void adcDMAConfig(void);

PROCESS(adc_test_process, "adc_test");
/**
  * @brief  ADC Configuration
  * @param  None
  * @retval None
  */
static void adcCfg(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/*chip TL431AQDBZRQ1  adc simple pin*/
	GPIO_InitStructure.GPIO_Pin = ADC_REF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC_REF_PORT, &GPIO_InitStructure);

	/*iseta  simple*/
	GPIO_InitStructure.GPIO_Pin = MODA_ISET;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MODA_ISET_PORT, &GPIO_InitStructure);
	

	/* ADC1 DeInit */  
	ADC_DeInit(ADC1);
	/* Initialize ADC structure */
	/*��ʼ��ADC�ṹ�壬�˾����ӣ����ӵĻ���·ADC���ݻύ��*/
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
	ADC_Init(ADC1, &ADC_InitStructure); 

	/* Convert the ADC1 in0 and in9 with 239.5 Cycles as sampling time */ 
	//ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles); 
	//ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_239_5Cycles); 
	//ADC_ChannelConfig(ADC1, ADC_Channel_17 , ADC_SampleTime_239_5Cycles); 

	/* Convert the ADC1 temperature sensor  with 55.5 Cycles as sampling time */ 
	//ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor , ADC_SampleTime_55_5Cycles);  
	//ADC_TempSensorCmd(ENABLE);

	/* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
	//ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles); 
	//ADC_VrefintCmd(ENABLE);

	/* Convert the ADC1 in0 and in9 with 239.5 Cycles as sampling time */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_1 , ADC_SampleTime_239_5Cycles); 
	ADC_ChannelConfig(ADC1, ADC_Channel_11 , ADC_SampleTime_239_5Cycles);
	
	/* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
	ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_239_5Cycles); 
	ADC_VrefintCmd(ENABLE);	//ADC_ChannelConfig(ADC1, ADC_Channel_17 , ADC_SampleTime_239_5Cycles); 

	/* Convert the ADC1 Vbat with 55.5 Cycles as sampling time */ 
	//ADC_ChannelConfig(ADC1, ADC_Channel_Vbat , ADC_SampleTime_55_5Cycles);  
	//ADC_VbatCmd(ENABLE);


	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

	/* Enable ADC_DMA */
	ADC_DMACmd(ADC1, ENABLE);  

	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

	/* ADC1 regular Software Start Conv */

	adcDMAConfig( );
	ADC_StartOfConversion(ADC1);
}


static void adcFillAdcData(void)
{
	int i = 0;
	for (i = 0; i < ADC_MAX_CHECK_COUNT; i++)
	{
		iseta[i] = adcDataV[i][INDEX_ISETA];		//
		aOut[i] = adcDataV[i][INDEX_TL431];
		iref[i] = adcDataV[i][INDEX_REF];
	}
}


/**
  * @brief  DMA channel1 configuration
  * @param  None
  * @retval None
  */
static void adcDMAConfig(void)
{
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	//ADC_TypeDef	  *ptgADC = ADC1;
	
	/* DMA1 clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	/* DMA1 Channel1 Config */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x40012440;//0x40012440
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adcDataV;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_NUM*ADC_MAX_CHECK_COUNT;
	//DMA_InitStructure.DMA_BufferSize = 3*ADC_MAX_CHECK_COUNT;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	#if 1
	/*���һ��DMA�жϱ�־*/
	DMA_ClearITPendingBit(DMA1_IT_TC1);                                 
	
	/*ʹ��DMA��������ж�*/
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE); 
	/*ѡ��DMA1ͨ���ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;  
	/*�ж�ʹ��*/ 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
	/*���ȼ���Ϊ0 */
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;  
	/*ʹ�� DMA �ж�*/ 
	NVIC_Init(&NVIC_InitStructure); 	
	#endif	
	/* DMA1 Channel1 enable */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}


static int adcFilterMax(u_short *pdata, u_char count)
{
	int i = 0;
	int index = 0;
	u_short temp = pdata[0];
	for (i = 0; i < count; i++)
	{
		if (temp < pdata[i])
		{
			temp = pdata[i];
			index = i;
		}
	}
	return index;
}


static int adcFilterMin(u_short *pdata, u_char count)
{
	int i = 0;
	int index = 0;
	u_short temp = pdata[0];
	for (i = 0; i < count; i++)
	{
		if (temp > pdata[i])
		{
			temp = pdata[i];
			index = i;
		}
	}

	return index;
}

static void adcClearMaxMin(u_short *pdata, int indexMax, int indexMin)
{
	pdata[indexMax] = 0;
	pdata[indexMin] = 0;
}

static u_short adcGetAverage(const u_short *pdata, u_char totalSize, u_char validSize)
{
	u_long sum = 0;
	int i = 0;
	for (i = 0; i < totalSize; i++)
	{
		sum += pdata[i];
	}

	return (u_short)(sum/validSize);
}


static u_short adcValueGet(u_short *pd)
{
	int indexMax = 0;
	int indexMin = 0;
	u_short adcv = 0;	
	indexMax = adcFilterMax(pd, ADC_MAX_CHECK_COUNT);
	indexMin = adcFilterMin(pd, ADC_MAX_CHECK_COUNT);
	adcClearMaxMin(pd, indexMax, indexMin);
	adcv = adcGetAverage(pd, ADC_MAX_CHECK_COUNT, ADC_MAX_CHECK_COUNT-2);
	return adcv;
}

static void adcCompulateADC(u_char adcFlag)
{
	int indexMax;
	int indexMin;
	if (adcFlag == ADC_ISETA_FLAG)
	{
		//indexMax = adcFilterMax(iseta, ADC_MAX_CHECK_COUNT);
		//indexMin = adcFilterMin(iseta, ADC_MAX_CHECK_COUNT);
		//adcClearMaxMin(iseta, indexMax, indexMin);
		adcIseta = adcValueGet(iseta);//adcGetAverage(iseta, ADC_MAX_CHECK_COUNT, ADC_MAX_CHECK_COUNT-2);		
	}
	else if (adcFlag == ADC_OUT_FLAG)
	{
		//indexMax = adcFilterMax(aOut, ADC_MAX_CHECK_COUNT);
		//indexMin = adcFilterMin(aOut, ADC_MAX_CHECK_COUNT);
		//adcClearMaxMin(aOut, indexMax, indexMin);
		adcOut= adcValueGet(aOut);//adcGetAverage(aOut, ADC_MAX_CHECK_COUNT, ADC_MAX_CHECK_COUNT-2);	
	}
	else if (adcFlag == ADC_REF_FLAG)
	{
		//indexMax = adcFilterMax(iref, ADC_MAX_CHECK_COUNT);
		//indexMin = adcFilterMin(iref, ADC_MAX_CHECK_COUNT);
		//adcClearMaxMin(iref, indexMax, indexMin);
		adcRef= adcValueGet(iref);//adcGetAverage(iref, ADC_MAX_CHECK_COUNT, ADC_MAX_CHECK_COUNT-2);	
	}
}

static void irefAdcV(void)
{
	#if 1
	//XPRINTF((8, "ref=%d cv=%d outv=%d\r\n", iref[0], aCurrent[0], aOut[0]));

	#endif
}

static void adcGetData(void)
{
	adcFillAdcData( );
	adcCompulateADC(ADC_ISETA_FLAG);
	adcCompulateADC(ADC_OUT_FLAG);
	adcCompulateADC(ADC_REF_FLAG);
}



/**
  * @brief  DMA1_Channel1�жϷ�����
  * @param  ��
  * @retval ��
  */
void DMA1_Channel1_IRQHandler()  
{  
	/*�ж�DMA��������ж�*/ 
	//static u_long adcCheckCount = 0;

	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
	}	
	/*���DMA�жϱ�־λ*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	//adcDS[adcCheckCount%ADC_MAX_CHECK_COUNT] = adcDataV[1];
	//adcV[adcCheckCount%ADC_MAX_CHECK_COUNT] = adcDataV[0];

	//if ((adcCheckCount+1)%ADC_MAX_CHECK_COUNT == 0)
	//{
	//	adcCompulateADC(ADC_DS_FLAG);
	//	adcCompulateADC(ADC_VS_FLAG);
	//}
	//xprintf((".\r\n"));
	//adcCheckCount++;
}  



void adcInit(void)
{
	adcCfg( );
	//adcDMAConfig( );
	process_start(&adc_test_process, NULL);
}


/*
u_short adcGetConvertCurrent(void)
{
	adcFillAdcData( );
	adcCompulateADC(ADC_CURRENT_FLAG);
	return adcCurrent;
}
*/

u_short adcGetConvertOut(void)
{
	adcFillAdcData( );
	adcCompulateADC(ADC_OUT_FLAG);
	//XPRINTF((0, "adcV=%d\r\n", adcCheckV));
	return adcOut;
}


ADC_OBJ_DATA * adcGetObj(void)
{
	return &adcObj;
}


void adcPrint(void)
{
	int i = 0;
	xprintf("TL431\r\n");
	for (i = 0; i < ADC_MAX_CHECK_COUNT; i++)
	{
		xprintf("%d ", adcDataV[i][0]);
	}
	xprintf("\r\n");
	xprintf("ISETA\r\n");
	for (i = 0; i < ADC_MAX_CHECK_COUNT; i++)
	{
		xprintf("%d ",adcDataV[i][1]);
	}
	xprintf("\r\n");
	xprintf("INNER REF\r\n");

	for (i = 0; i < ADC_MAX_CHECK_COUNT; i++)
	{
		xprintf("%d ",adcDataV[i][2]);
	}
	xprintf("\r\n");
	xprintf("--------------\r\n");
}

//		data21 = (*Data2<<8)|*Data1;
//		data12 = (*Data1<<8)|*Data2;
//		v = data21*3300/4096;
#define  ADC_TIME		50
PROCESS_THREAD(adc_test_process, ev, data)
{
	static struct etimer et_led;
	static u_short refAdc = 0;
	const u_char *Data2 = (const u_char *)(0X1FFFF7BB);//����
	const u_char *Data1 = (const u_char *)(0X1FFFF7BA);//ss
	static u_short vcc = 0;
	static u_short v431 = 0;
	static u_char i = 0;

	static u_long cnt = 0;
	//u_short ADC1ConvertedValue;
	PROCESS_BEGIN();
	refAdc = (*Data2<<8)|*Data1;
	etimer_set(&et_led, ADC_TIME);
	
	while(1)
	{
		PROCESS_YIELD_UNTIL(etimer_expired(&et_led));
		etimer_reset(&et_led);
		cnt ++;
		#if 0
		adcGetData( );
		XPRINTF((12, "0 ADC_CURRENT = %d adc_out=%d ref=%d iref=%d\r\n", adcCurrent, adcOut, adcRef, refAdc));
		vcc = refAdc*3300/adcRef;
		adcObj.currentv = adcCurrent*vcc/4096; //adccv=adcCurrent*3300/4096,  ref = refadc*3300/4096  ref:inter ref voltage
		//adcObj.outv = adcOut*vcc*12/2/4096;		//resister divde  10k, 2k,
		adcObj.outv = adcOut*vcc*88/20/4096;		//resister divde  680k, 200k,
		
		adcObj.vcc = vcc;

		XPRINTF((12, "1 cv=%d, ov = %d, vcc = %d\r\n",adcObj.currentv, adcObj.outv, vcc));
		
		#endif
		adcGetData( );
		vcc = refAdc*3300/adcRef;
		v431 = adcOut*vcc/4096;
		adcObj.vcc = vcc;
		adcObj.iset = adcIseta*vcc/4096;

		i++;
		if (cnt %20 == 0)
		{
			XPRINTF((10, "vcc = %d ref=%d iref=%d adcObj.iset=%d, v431=%d\r\n", vcc, adcRef, refAdc, adcObj.iset, v431));
			adcPrint();
		}
		
	}
	PROCESS_END();
}




