#include "basictype.h"
#include "arch_tim.h"
#include "contiki.h"
#include "sysprintf.h"
#include "appac.h"

#if 0
static volatile u_long ac_a_cnt1 = 0;
static volatile u_long ac_a_cnt2 = 0;
static volatile u_long ac_a_freq = 0;
static volatile u_char ac_a_flag = 0;

static volatile u_long ac_b_cnt1 = 0;
static volatile u_long ac_b_cnt2 = 0;
static volatile u_long ac_b_freq = 0;
static volatile u_char ac_b_flag = 0;

static volatile u_long ac_c_cnt1 = 0;
static volatile u_long ac_c_cnt2 = 0;
static volatile u_long ac_c_freq = 0;
static volatile u_char ac_c_flag = 0;

/*
typedef struct _tim_dev{
	GPIO_TypeDef 	*port;
	uint16_t		pin;
	uint32_t 		ioCLK;
	uint32_t   		timCLK;
	TIM_TypeDef* 	timx;
}TIM_DEV;
*/

const TIM_DEV tim_dev[3]={
	{TIM_AC_A_PORT, TIM_AC_A_PIN,TIM_AC_A_PSOURCE, TIM_AC_A_AF, TIM_AC_AIO_CLK, TIM_AC_AT_CKL, TIM_AC_A_TIM, TIM_AC_A_CH, TIM_AC_A_IT, TIM_AC_A_IRQ},
	{TIM_AC_B_PORT, TIM_AC_B_PIN,TIM_AC_B_PSOURCE, TIM_AC_B_AF, TIM_AC_BIO_CLK, TIM_AC_BT_CKL, TIM_AC_B_TIM, TIM_AC_B_CH, TIM_AC_B_IT, TIM_AC_B_IRQ},
	{TIM_AC_C_PORT, TIM_AC_C_PIN,TIM_AC_C_PSOURCE, TIM_AC_C_AF, TIM_AC_CIO_CLK, TIM_AC_CT_CKL, TIM_AC_C_TIM, TIM_AC_C_CH, TIM_AC_C_IT, TIM_AC_C_IRQ},
};


PROCESS(ac_freq_process, "ac_freq");

//	/* Connect pin to Periph */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
static void _timIoInit(const TIM_DEV *ptim)
{
	GPIO_InitTypeDef stGpio;

	//enable gpio clk
  	RCC_AHBPeriphClockCmd(ptim->ioCLK, ENABLE);	

	//tim clock
  	if (ptim->timx == TIM3)
  	{
  		RCC_APB1PeriphClockCmd(ptim->timCLK, ENABLE);
  	}
  	else
  	{
  		RCC_APB2PeriphClockCmd(ptim->timCLK, ENABLE);
  	}

  	stGpio.GPIO_Mode = GPIO_Mode_AF;
  	stGpio.GPIO_Pin = ptim->pin;
  	stGpio.GPIO_OType = GPIO_OType_PP;
  	stGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	stGpio.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(ptim->port, &stGpio);

  	GPIO_PinAFConfig(ptim->port, ptim->srcpin, ptim->afpin);
}

static void _timTimCfg(const TIM_DEV *ptim)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef stTimeBase;


	//48MHz   ->1MHz  48/48 = 1MHz 
	
	stTimeBase.TIM_Prescaler = 48-1;
	stTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	stTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
	stTimeBase.TIM_Period = 0xffff;			//20000us = 20ms
	TIM_TimeBaseInit(ptim->timx, &stTimeBase);

	TIM_ICInitStructure.TIM_Channel = ptim->timch;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ptim->timx, &TIM_ICInitStructure);

	/* TIM enable counter */
	//TIM_Cmd(ptim->timx, ENABLE);

	/* Enable the CC2 Interrupt Request */
	//TIM_ClearFlag(ptim->timx, ptim->timirq);
	//TIM_ITConfig(ptim->timx, ptim->timit, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = ptim->timirq;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	/* Enable the CC1 Interrupt Request */
	TIM_ITConfig(ptim->timx, ptim->timit, ENABLE);
	//TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	//TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);}


/*PA6   TIM16  CH1*/
void _tim16cfg(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef stTimeBase;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect TIM pins to AF2 */
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);


	stTimeBase.TIM_Prescaler = 48-1;
	stTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	stTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
	stTimeBase.TIM_Period = 0xffff;			//20000us = 20ms
	TIM_TimeBaseInit(TIM3, &stTimeBase);
	//TIM_ARRPreloadConfig(ptim->timx, DISABLE);	

	/* TIM16 configuration: Input Capture mode ---------------------
	The external signal is connected to TIM16 CH1 pin (PA.6)  
	The Rising edge is used as active edge,
	The TIM6 CCR1 is used to compute the frequency value 
	------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	/* TIM enable counter */
	//TIM_Cmd(TIM16, ENABLE);

	/* Enable the CC1 Interrupt Request */
	//TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);

	/* Enable the TIM16 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the CC1 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	
}


void timInit(void)
{
	_timIoInit(&tim_dev[0]);
	_timTimCfg(&tim_dev[0]);

	_timIoInit(&tim_dev[1]);
	_timTimCfg(&tim_dev[1]);

	_timIoInit(&tim_dev[2]);
	_timTimCfg(&tim_dev[2]);		
	//_tim16cfg( );
}


/*
TIM16   channel 1
*/
void TIM16_IRQHandler(void)
{
	const TIM_DEV *ptim = &tim_dev[0];
	u_long Capture = 0;
	
	if(TIM_GetITStatus(TIM16, TIM_IT_CC1) == SET) 
	{
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
		if(ac_a_flag == 0)
		{
			/* Get the Input Capture value */
			ac_a_cnt1 = TIM_GetCapture1(ptim->timx);
			ac_a_flag = 1;
		}
		else if(ac_a_flag == 1)
		{
			/* Get the Input Capture value */
			ac_a_cnt2 = TIM_GetCapture1(ptim->timx); 

			/* Capture computation */
			if (ac_a_cnt2 > ac_a_cnt1)
			{
				Capture = (ac_a_cnt2 - ac_a_cnt1); 
			}
			else  if (ac_a_cnt2 < ac_a_cnt1)
			{
				Capture = ((0xFFFF - ac_a_cnt1) + ac_a_cnt2); 
			}
			else
			{
				
			}

			/* Frequency computation */ 
			if (Capture != 0)
			{
				ac_a_freq  = (uint32_t) 10000000000 / Capture;
			}
			else
			{
				ac_a_freq = 0;
			}
			ac_a_flag = 0;
		}

		xprintf("c t16 capture = %d\r\n", Capture);
	}

	if(TIM_GetITStatus(TIM16, TIM_IT_Update) == SET) 
	{
		TIM_ClearITPendingBit(TIM16, TIM_IT_Update);

		//xprintf("ut16\r\n");
	}
}




/*
TIM3   channel 1
*/
void ac_a_irq_handler(const TIM_DEV *ptim)
{
	static u_char _poll = 0;
	u_long Capture = 0;
	/* Clear TIM1 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(ptim->timx, ptim->timit);
	if(ac_a_flag == 0)
	{
		/* Get the Input Capture value */
		ac_a_cnt1 = TIM_GetCapture1(ptim->timx);
		ac_a_flag = 1;
	}
	else if(ac_a_flag == 1)
	{
		/* Get the Input Capture value */
		ac_a_cnt2 =  TIM_GetCapture1(ptim->timx);

		/* Capture computation */
		if (ac_a_cnt2 > ac_a_cnt1)
		{
			Capture = (ac_a_cnt2 - ac_a_cnt1); 
		}
		else  if (ac_a_cnt2 < ac_a_cnt1)
		{
			Capture = ((0xFFFF - ac_a_cnt1) + ac_a_cnt2); 
		}
		else
		{
			
		}

		/* Frequency computation */ 
		if (Capture != 0)
		{
			ac_a_freq  = (uint32_t) 1000000000 / Capture;
		}
		else
		{
			ac_a_freq = 0;
		}
		ac_a_flag = 0;
	}

	//xprintf("c t16\r\n");
	//xprintf("c t16 capture = %d freq = %d\r\n", Capture, ac_a_freq);
	_poll++;
	if (_poll % 10 == 0)
	{
		process_poll(&ac_freq_process);
		_poll = 0;
	}
}


/*
TIM3   channel 2
*/
void ac_b_irq_handler(const TIM_DEV *ptim)
{
	static u_char _poll = 0;
	u_long Capture = 0;
	/* Clear TIM1 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(ptim->timx, ptim->timit);
	if(ac_b_flag == 0)
	{
		/* Get the Input Capture value */
		ac_b_cnt1 = TIM_GetCapture2(ptim->timx);
		ac_b_flag = 1;
	}
	else if(ac_b_flag == 1)
	{
		/* Get the Input Capture value */
		ac_b_cnt2 =  TIM_GetCapture2(ptim->timx);

		/* Capture computation */
		if (ac_b_cnt2 > ac_b_cnt1)
		{
			Capture = (ac_b_cnt2 - ac_b_cnt1); 
		}
		else  if (ac_b_cnt2 < ac_b_cnt1)
		{
			Capture = ((0xFFFF - ac_b_cnt1) + ac_b_cnt2); 
		}
		else
		{
			
		}

		/* Frequency computation */ 
		if (Capture != 0)
		{
			ac_b_freq  = (uint32_t) 1000000000 / Capture;
		}
		else
		{
			ac_b_freq = 0;
		}
		
		ac_b_flag = 0;
	}

	//xprintf("c t16\r\n");
	//xprintf("c t16 capture = %d freq = %d\r\n", Capture, ac_a_freq);
	_poll++;
	if (_poll % 10 == 0)
	{
		process_poll(&ac_freq_process);
		_poll = 0;
	}
}



/*
TIM3   channel 3
*/
void ac_c_irq_handler(const TIM_DEV *ptim)
{
	static u_char _poll = 0;
	u_long Capture = 0;
	/* Clear TIM1 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(ptim->timx, ptim->timit);
	if(ac_c_flag == 0)
	{
		/* Get the Input Capture value */
		ac_c_cnt1 = TIM_GetCapture3(ptim->timx);
		ac_c_flag = 1;
	}
	else if(ac_c_flag == 1)
	{
		/* Get the Input Capture value */
		ac_c_cnt2 =  TIM_GetCapture3(ptim->timx);

		/* Capture computation */
		if (ac_c_cnt2 > ac_c_cnt1)
		{
			Capture = (ac_c_cnt2 - ac_c_cnt1); 
		}
		else  if (ac_c_cnt2 < ac_c_cnt1)
		{
			Capture = ((0xFFFF - ac_c_cnt1) + ac_c_cnt2); 
		}
		else
		{
			
		}

		/* Frequency computation */ 
		if (Capture != 0)
		{
			ac_c_freq  = (uint32_t) 1000000000 / Capture;
		}
		else
		{
			ac_c_freq = 0;
		}
		
		ac_c_flag = 0;
	}

	//xprintf("c t16\r\n");
	//xprintf("c t16 capture = %d freq = %d\r\n", Capture, ac_a_freq);
	_poll++;
	if (_poll % 10 == 0)
	{
		process_poll(&ac_freq_process);
		_poll = 0;
	}
}




void TIM3_IRQHandler(void)
{
	const TIM_DEV *pAtim = &tim_dev[0];
	const TIM_DEV *pBtim = &tim_dev[1];
	const TIM_DEV *pCtim = &tim_dev[2];
	
	if(TIM_GetITStatus(pAtim->timx, pAtim->timit) == SET) 
	{
		ac_a_irq_handler(pAtim);
	}
	else if(TIM_GetITStatus(pBtim->timx, pBtim->timit) == SET) 
	{
		ac_b_irq_handler(pBtim);
	}
	else if(TIM_GetITStatus(pCtim->timx, pCtim->timit) == SET) 
	{
		//xprintf("ac_c\r\n");
		ac_c_irq_handler(pCtim);
	}
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

	
void ac_freq_handler(struct etimer *pet, process_event_t ev, process_data_t data)
{
	static u_long acAFreqPollEvent = 0;
	
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		//XPRINTF((0, "TIMER event\r\n"));
		XPRINTF((0, "afreq = %d bfreq=%d cfreq=%d,apoll=%d\r\n", ac_a_freq,  ac_b_freq, ac_c_freq, acAFreqPollEvent));
		etimer_set(pet, 5000);
		if (acAFreqPollEvent != 0)
		{
			acAFreqPollEvent = 0;
		}
		else
		{
			//TODO:NOAC 
		}
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		acAFreqPollEvent++;
	}
}


PROCESS_THREAD(ac_freq_process, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	timInit( );
	etimer_set(&_et, 1000);
	while(1)
	{
		PROCESS_YIELD();
		ac_freq_handler(&_et, ev, data);
	}
	PROCESS_END();
}


void timProcessStart(void)
{
	process_start(&ac_freq_process, NULL);
}

#endif

static  ACFREQ_OBJ acFreqObj[3] = {0};
static FREQ_OBJ freqObj = {0};

/*
typedef struct _tim_dev{
	GPIO_TypeDef 	*port;
	uint16_t		pin;
	uint32_t 		ioCLK;
	uint32_t   		timCLK;
	TIM_TypeDef* 	timx;
}TIM_DEV;
*/

const TIM_DEV tim_dev[3]={
	{TIM_AC_A_PORT, TIM_AC_A_PIN,TIM_AC_A_PSOURCE, TIM_AC_A_AF, TIM_AC_AIO_CLK, TIM_AC_AT_CKL, TIM_AC_A_TIM, TIM_AC_A_CH, TIM_AC_A_IT, TIM_AC_A_IRQ, TIM_GetCapture1},
	{TIM_AC_B_PORT, TIM_AC_B_PIN,TIM_AC_B_PSOURCE, TIM_AC_B_AF, TIM_AC_BIO_CLK, TIM_AC_BT_CKL, TIM_AC_B_TIM, TIM_AC_B_CH, TIM_AC_B_IT, TIM_AC_B_IRQ, TIM_GetCapture2},
	{TIM_AC_C_PORT, TIM_AC_C_PIN,TIM_AC_C_PSOURCE, TIM_AC_C_AF, TIM_AC_CIO_CLK, TIM_AC_CT_CKL, TIM_AC_C_TIM, TIM_AC_C_CH, TIM_AC_C_IT, TIM_AC_C_IRQ, TIM_GetCapture3},
};


PROCESS(ac_freq_process, "ac_freq");
PROCESS(ac_freq_process_b, "acb_freq");
PROCESS(ac_freq_process_c, "acc_freq");

//	/* Connect pin to Periph */
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
//	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
static void _timIoInit(const TIM_DEV *ptim)
{
	GPIO_InitTypeDef stGpio;

	//enable gpio clk
  	RCC_AHBPeriphClockCmd(ptim->ioCLK, ENABLE);	

	//tim clock
  	if (ptim->timx == TIM3)
  	{
  		RCC_APB1PeriphClockCmd(ptim->timCLK, ENABLE);
  	}
  	else
  	{
  		RCC_APB2PeriphClockCmd(ptim->timCLK, ENABLE);
  	}

  	stGpio.GPIO_Mode = GPIO_Mode_AF;
  	stGpio.GPIO_Pin = ptim->pin;
  	stGpio.GPIO_OType = GPIO_OType_OD;
  	stGpio.GPIO_PuPd = GPIO_PuPd_UP;
  	stGpio.GPIO_Speed = GPIO_Speed_50MHz;

  	GPIO_Init(ptim->port, &stGpio);

  	GPIO_PinAFConfig(ptim->port, ptim->srcpin, ptim->afpin);
}

static void _timTimCfg(const TIM_DEV *ptim)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef stTimeBase;


	//48MHz   ->1MHz  48/48 = 1MHz 
	
	stTimeBase.TIM_Prescaler = 48-1;
	stTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	stTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
	stTimeBase.TIM_Period = 0xffff;			//20000us = 20ms
	TIM_TimeBaseInit(ptim->timx, &stTimeBase);

	TIM_ICInitStructure.TIM_Channel = ptim->timch;
	//TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(ptim->timx, &TIM_ICInitStructure);

	/* TIM enable counter */
	//TIM_Cmd(ptim->timx, ENABLE);

	/* Enable the CC2 Interrupt Request */
	//TIM_ClearFlag(ptim->timx, ptim->timirq);
	//TIM_ITConfig(ptim->timx, ptim->timit, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = ptim->timirq;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	/* Enable the CC1 Interrupt Request */
	TIM_ITConfig(ptim->timx, ptim->timit, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	

	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);}


/*PA6   TIM16  CH1*/
void _tim16cfg(void)
{
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef stTimeBase;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect TIM pins to AF2 */
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);


	stTimeBase.TIM_Prescaler = 48-1;
	stTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
	stTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
	stTimeBase.TIM_Period = 0xffff;			//20000us = 20ms
	TIM_TimeBaseInit(TIM3, &stTimeBase);
	//TIM_ARRPreloadConfig(ptim->timx, DISABLE);	

	/* TIM16 configuration: Input Capture mode ---------------------
	The external signal is connected to TIM16 CH1 pin (PA.6)  
	The Rising edge is used as active edge,
	The TIM6 CCR1 is used to compute the frequency value 
	------------------------------------------------------------ */
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;

	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	/* TIM enable counter */
	//TIM_Cmd(TIM16, ENABLE);

	/* Enable the CC1 Interrupt Request */
	//TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);

	/* Enable the TIM16 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the CC1 Interrupt Request */
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	
	/* TIM enable counter */
	TIM_Cmd(TIM3, ENABLE);

	
}


void timInit(void)
{
	_timIoInit(&tim_dev[0]);
	_timTimCfg(&tim_dev[0]);

	_timIoInit(&tim_dev[1]);
	_timTimCfg(&tim_dev[1]);

	_timIoInit(&tim_dev[2]);
	_timTimCfg(&tim_dev[2]);		
	//_tim16cfg( );
}


static u_long _freqAvg(ACFREQ_OBJ *pobj)
{
	int i = 0;
	u_long avg = 0;
	for (i = 10; i < 20; i++)
	{
		avg += pobj->freq_a[i];
		//xprintf("-i=%d--%d\r\n", i, pobj->freq_a[i]);
	}

	avg /= 10;
	return avg;
}

void _freq_isnot_49_7Hz_50_3Hz(ACFREQ_OBJ *pobj,u_long freq)
{
	if ((freq > (APP_AC_FREQ+APP_AC_FREQ_DELT))||( freq < (APP_AC_FREQ-APP_AC_FREQ_DELT)))
	{
		pobj->freqCnt++;
	}
}


void ac_freq_irq_handler(const TIM_DEV *ptim, ACFREQ_OBJ *pobj)
{
	/* Clear TIM1 Capture compare interrupt pending bit */
	TIM_ClearITPendingBit(ptim->timx, ptim->timit);
	if(pobj->ac_flag == 0)
	{
		/* Get the Input Capture value */
		pobj->ac_cnt1 = ptim->fcapture(ptim->timx);
		pobj->ac_flag = 1;
	}
	else if(pobj->ac_flag == 1)
	{
		/* Get the Input Capture value */
		pobj->ac_cnt2 =  ptim->fcapture(ptim->timx);
		/* Capture computation */
		if (pobj->ac_cnt2 > pobj->ac_cnt1)
		{
			pobj->Capture = (pobj->ac_cnt2 - pobj->ac_cnt1); 
		}
		else  if (pobj->ac_cnt2 < pobj->ac_cnt1)
		{
			pobj->Capture = ((0xFFFF - pobj->ac_cnt1) + pobj->ac_cnt2); 
		}
		else
		{
			
		}

		/* Frequency computation */ 
		if (pobj->Capture != 0)
		{
			pobj->ac_freq  = (uint32_t) 1000000000 / pobj->Capture;
			pobj->freq_a[pobj->_poll++%TIM_FREQ_MAX_CNT] = (uint32_t) 1000000000 / pobj->Capture;
			pobj->Capture = 0;
			_freq_isnot_49_7Hz_50_3Hz(pobj, pobj->ac_freq);
			//pobj->_poll++;
			if ((pobj->_poll) % TIM_FREQ_MAX_CNT  == 0)
			{
				pobj->ac_freq = _freqAvg(pobj);
				if (&acFreqObj[0] == pobj)	//phase a
				{
					process_poll(&ac_freq_process);
				}
				else if (&acFreqObj[1] == pobj) //phase b
				{
					process_poll(&ac_freq_process_b);
				}
				else 
				{
					process_poll(&ac_freq_process_c);
				}
				//xprintf("pobj->pool = %d\r\n", pobj->_poll);
				//pobj->_poll = 0;
			}
		}
		else
		{
			pobj->ac_freq = 0;
		}
		pobj->ac_flag = 0;
	}
}



void TIM3_IRQHandler(void)
{
	const TIM_DEV *pAtim = &tim_dev[0];
	const TIM_DEV *pBtim = &tim_dev[1];
	const TIM_DEV *pCtim = &tim_dev[2];
	ACFREQ_OBJ *pAobj = &acFreqObj[0];
	ACFREQ_OBJ *pBobj = &acFreqObj[1];
	ACFREQ_OBJ *pCobj = &acFreqObj[2];
	
	if(TIM_GetITStatus(pAtim->timx, pAtim->timit) == SET) 
	{
		//xprintf("ac_a\r\n");
		ac_freq_irq_handler(pAtim, pAobj);
	}
	if(TIM_GetITStatus(pBtim->timx, pBtim->timit) == SET) 
	{
		//xprintf("ac_b\r\n");
		ac_freq_irq_handler(pBtim, pBobj);
	}
	if(TIM_GetITStatus(pCtim->timx, pCtim->timit) == SET) 
	{
		//xprintf("ac_c\r\n");
		ac_freq_irq_handler(pCtim, pCobj);
	}

	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}


static void update_freqobj(FREQ_OBJ *pObj, ACFREQ_OBJ *pAobj, ACFREQ_OBJ *pBobj, ACFREQ_OBJ *pCobj)
{
	// pObj->phaseAFreq = afreq;
	// pObj->phaseBFreq = bfreq;
	// pObj->phaseCFreq = cfreq;
	pObj->phaseAFreq = pAobj->ac_freq;
	pObj->phaseBFreq = pBobj->ac_freq;
	pObj->phaseCFreq = pCobj->ac_freq;
	pObj->AFcnt = pAobj->freqCnt;
	pObj->BFcnt = pBobj->freqCnt;
	pObj->CFcnt = pCobj->freqCnt;
	
	pAobj->ac_freq = 0;
	pBobj->ac_freq = 0;
	pCobj->ac_freq = 0;	
	pAobj->freqCnt = 0;
	pBobj->freqCnt = 0;
	pCobj->freqCnt = 0;
}

static void update_freqobj_a(FREQ_OBJ *pObj, ACFREQ_OBJ *pAobj)
{
	pObj->phaseAFreq = pAobj->ac_freq;
	pObj->AFcnt = pAobj->freqCnt;
	
	pAobj->ac_freq = 0;
	pAobj->freqCnt = 0;
}

static void update_freqobj_b(FREQ_OBJ *pObj, ACFREQ_OBJ *pBobj)
{
	pObj->phaseBFreq = pBobj->ac_freq;
	pObj->BFcnt = pBobj->freqCnt;
	
	pBobj->ac_freq = 0;
	pBobj->freqCnt = 0;
}

static void update_freqobj_c(FREQ_OBJ *pObj, ACFREQ_OBJ *pCobj)
{
	pObj->phaseCFreq = pCobj->ac_freq;
	pObj->CFcnt = pCobj->freqCnt;
	
	pCobj->ac_freq = 0;
	pCobj->freqCnt = 0;
}


extern void app_post_msg(void);

#if 0	
void ac_freq_handler(struct etimer *pet, process_event_t ev, process_data_t data)
{
	static u_long acAFreqPollEvent = 0;
	
	//every 5 seconds check phase A,B,C freq and update freq
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		ACFREQ_OBJ *pAobj = &acFreqObj[0];
		ACFREQ_OBJ *pBobj = &acFreqObj[1];
		ACFREQ_OBJ *pCobj = &acFreqObj[2];		
		etimer_set(pet, 1000);
		update_freqobj(&freqObj, pAobj, pBobj, pCobj);
		if (acAFreqPollEvent != 0)
		{
			//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
			XPRINTF((10, "1 afreq = %d bfreq=%d cfreq=%d,apoll=%d\r\n", freqObj.phaseAFreq,  freqObj.phaseBFreq, freqObj.phaseCFreq, acAFreqPollEvent));
			acAFreqPollEvent = 0;
		}
		else
		{
			//update_freqobj(&freqObj, 0, 0, 0);
			//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
			XPRINTF((10, "0 afreq = %d bfreq=%d cfreq=%d,apoll=%d\r\n", freqObj.phaseAFreq,  freqObj.phaseBFreq, freqObj.phaseCFreq, acAFreqPollEvent));
		}
		XPRINTF((11, "Phase A freq = %3d.%03dHz, Phase B freq = %3d.%03dHz, Phase C freq = %3d.%03dHz\r\n", 
				freqObj.phaseAFreq/1000,  freqObj.phaseAFreq%1000, 
				freqObj.phaseBFreq/1000,  freqObj.phaseBFreq%1000, 
				freqObj.phaseCFreq/1000,  freqObj.phaseCFreq%1000));
		// XPRINTF((6, "Phase B freq = %6dHz\r\n", freqObj.phaseBFreq));
		// XPRINTF((6, "Phase C freq = %6dHz\r\n", freqObj.phaseCFreq));
		app_post_msg( );
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		acAFreqPollEvent++;
	}
}
#else
/*
void ac_freq_handler(struct etimer *pet, process_event_t ev, process_data_t data)
{
	//every 5 seconds check phase A,B,C freq and update freq
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		ACFREQ_OBJ *pAobj = &acFreqObj[0];
		ACFREQ_OBJ *pBobj = &acFreqObj[1];
		ACFREQ_OBJ *pCobj = &acFreqObj[2];		
		etimer_set(pet, 2000);

		//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
		update_freqobj_a(&freqObj, pAobj);

		XPRINTF((6, "TIMER POOL\r\n"));		
		app_post_msg( );
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		ACFREQ_OBJ *pAobj = &acFreqObj[0];
		ACFREQ_OBJ *pBobj = &acFreqObj[1];
		ACFREQ_OBJ *pCobj = &acFreqObj[2];
		
		etimer_stop(pet);

		//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
		update_freqobj_a(&freqObj, pAobj);
		
		XPRINTF((6, "tim interrupt POOL %d\r\n", clock_time( )));
		etimer_set(pet, 2000);
		{
			app_post_msg( );
		}
	}
}
*/

void ac_freq_handler_a(struct etimer *pet, process_event_t ev, process_data_t data)
{
	//every 5 seconds check phase A,B,C freq and update freq
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		ACFREQ_OBJ *pAobj = &acFreqObj[0];
		etimer_set(pet, 2000);

		//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
		update_freqobj_a(&freqObj, pAobj);

		XPRINTF((4, "a --TIMER POOL\r\n"));		
		app_post_msg( );
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		ACFREQ_OBJ *pAobj = &acFreqObj[0];
		
		etimer_stop(pet);

		//update_freqobj(&freqObj, pAobj, pBobj, pCobj);
		update_freqobj_a(&freqObj, pAobj);
		
		XPRINTF((4, "a-- tim interrupt POOL %d\r\n", clock_time( )));
		etimer_set(pet, 2000);
		{
			app_post_msg( );
		}
	}
}

void ac_freq_handler_b(struct etimer *pet, process_event_t ev, process_data_t data)
{
	//every 5 seconds check phase A,B,C freq and update freq
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		ACFREQ_OBJ *pBobj = &acFreqObj[1];
		etimer_set(pet, 2000);

		update_freqobj_b(&freqObj, pBobj);

		XPRINTF((4, "b--TIMER POOL\r\n"));		
		app_post_msg( );
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		ACFREQ_OBJ *pBobj = &acFreqObj[1];
		
		etimer_stop(pet);
		update_freqobj_b(&freqObj, pBobj);
		
		XPRINTF((4, "b--tim interrupt POOL %d\r\n", clock_time( )));
		etimer_set(pet, 2000);
		{
			app_post_msg( );
		}
	}
}

void ac_freq_handler_c(struct etimer *pet, process_event_t ev, process_data_t data)
{
	//every 5 seconds check phase A,B,C freq and update freq
	if (PROCESS_EVENT_TIMER == ev && data == pet)
	{
		ACFREQ_OBJ *pCobj = &acFreqObj[2];		
		etimer_set(pet, 2000);

		update_freqobj_c(&freqObj, pCobj);

		XPRINTF((4, "c--TIMER POOL\r\n"));		
		app_post_msg( );
	}
	else if (PROCESS_EVENT_POLL == ev)
	{
		ACFREQ_OBJ *pCobj = &acFreqObj[2];
		
		etimer_stop(pet);
		
		update_freqobj_c(&freqObj, pCobj);
		
		XPRINTF((4, "c--tim interrupt POOL %d\r\n", clock_time( )));
		etimer_set(pet, 2000);
		{
			app_post_msg( );
		}
	}
}

#endif

/*
PROCESS_THREAD(ac_freq_process, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	timInit( );
	etimer_set(&_et, 1000);
	while(1)
	{
		PROCESS_YIELD();
		ac_freq_handler(&_et, ev, data);
	}
	PROCESS_END();
}
*/
PROCESS_THREAD(ac_freq_process, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	timInit( );
	etimer_set(&_et, 1000);
	while(1)
	{
		PROCESS_YIELD();
		ac_freq_handler_a(&_et, ev, data);
	}
	PROCESS_END();
}



PROCESS_THREAD(ac_freq_process_b, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	timInit( );
	etimer_set(&_et, 1000);
	while(1)
	{
		PROCESS_YIELD();
		ac_freq_handler_b(&_et, ev, data);
	}
	PROCESS_END();
}

PROCESS_THREAD(ac_freq_process_c, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	timInit( );
	etimer_set(&_et, 1000);
	while(1)
	{
		PROCESS_YIELD();
		ac_freq_handler_c(&_et, ev, data);
	}
	PROCESS_END();
}




void timProcessStart(void)
{
	process_start(&ac_freq_process, NULL);
	process_start(&ac_freq_process_b, NULL);
	process_start(&ac_freq_process_c, NULL);
}

FREQ_OBJ *timGetFreqObj(void)
{	
	return &freqObj;
}



