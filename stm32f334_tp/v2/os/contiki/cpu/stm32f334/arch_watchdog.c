#include "stm32f0xx.h"
#include "basictype.h"



void watch_dog_init(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag( );  //clear flag
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //enable access IWDG_PR and IWDG_RLR reg
	IWDG_SetPrescaler(IWDG_Prescaler_32);		   //set  f = 40000/32 T = 1/f

	IWDG_SetReload(40000/64);						//reload number  cnt = 40000/64  time = cnt*T=32/64=0.5S
	
	IWDG_ReloadCounter( );

	IWDG_Enable( );
}

void watch_dog_feed(void)
{
	/* Update WWDG counter */
	//WWDG_SetCounter(127);	
	IWDG_ReloadCounter( );
}

//WWDG CONFIG
void wdg_config(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
	{
		RCC_ClearFlag( );
	}

	WWDG_SetPrescaler( WWDG_Prescaler_8);  //��Ƶֵ 48M/4096/8(683us)
	WWDG_SetWindowValue(0x50);				//���ô���ֵ ��0x40-0x50֮����бȽ�
	WWDG_Enable(0x7F);					//������ֵ��Χ:0x40-0x7f
										//��������:0x7f-0x40 = 63
										//���ι��ʱ��:683us*63 = 43ms
}

void wdg_feed(void)
{
	WWDG_Enable(0x7F);
}