#include "stm32f30x.h"

#include "basictype.h"
#include "iodef.h"
#include "arch_dac.h"
#include "sysprintf.h"

/**
  * @brief  Configures the DAC1 channel 1 with output buffer disabled.
  * @param  None
  * @retval None
  */
static void dac_config(void)
{
	DAC_InitTypeDef    DAC_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Configure PA.04 (DAC1_OUT1) in analog mode -------------------------*/
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = DAC_SETCUR_A_PIN;
	GPIO_Init(DAC_SETCUR_A_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DAC_SETCUR_B_PIN;
	GPIO_Init(DAC_SETCUR_B_PORT, &GPIO_InitStructure);


	/* Enable DAC clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	DAC_DeInit(DAC1); 
	
	/* DAC1 channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	//DAC_InitStructure.DAC_Trigger = DAC_Trigger_Software;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
	DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Disable;
	DAC_Init(DAC1, DAC_Channel_1, &DAC_InitStructure);

	DAC_InitStructure.DAC_Buffer_Switch = DAC_BufferSwitch_Enable;
	DAC_Init(DAC1, DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC1 Channel1 */
	DAC_Cmd(DAC1, DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC1, DAC_Channel_2, ENABLE);
}

void dac_cfg(void)
{
	dac_config( );
}


/*
	1.2V -> 1200mV 
	voltage < 3.3V 3300
*/
void dac_ch1_set_voltage(int voltage)
{
	uint16_t value;

	value = (uint16_t)((voltage * 4096)/3300);

	DAC_SetChannel1Data(DAC1, DAC_Align_12b_R, value);
	//DAC_SoftwareTriggerCmd(DAC1, DAC_Channel_1, ENABLE);
	XPRINTF((10, "value 1 = %d\r\n", value));
}

/*
	1.2V -> 1200mV 
	voltage < 3.3V 3300
*/
void dac_ch2_set_voltage(int voltage)
{
	uint16_t value;

	value = (uint16_t)((voltage * 4096)/3300);

	DAC_SetChannel2Data(DAC1,DAC_Align_12b_R, value);	
	//DAC_SoftwareTriggerCmd(DAC1, DAC_Channel_2, ENABLE);
	XPRINTF((10, "value 2 = %d\r\n", value));
}

uint16_t  dac_get_value(int ch)
{
	uint16_t value = 0;
	if (ch == 1)
	{
		value = DAC_GetDataOutputValue(DAC1, DAC_Channel_1);
	}
	else if (ch == 2) 
	{
		value = DAC_GetDataOutputValue(DAC1, DAC_Channel_2);
	}

	return value;
}







