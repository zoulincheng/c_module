#include "stm32f0xx.h"
#include "basictype.h"
#include "iodef.h"
#include "arch_pwm.h"



//This define define the hardware use optocoupler
//#define PWM_OP_HARDWARE

static uint16_t timerPeriod = 0;
static uint16_t prescalerValue = 0;	//sysclk  precaler
static uint16_t channel1Pulse = 0;

static uint16_t pwmfreq = 0;
static uint16_t pwmduty = 0;

static u_char ioFlag = 0;


//TIM_TimeBaseStructure.TIM_Prescaler = 9;  10 ·ÖÆµ
//#define PWM_DEFAULT_FEQ		10	//10hz
#define PWM_DEFAULT_DUTY	750		//75%
//#define PWM_FEQ				5000		//hz
#define PWM_FEQ				500	//hz
//#define PWM_FEQ				8	//hz
//#define PWM_TIM_FREQ		24000000

/*
* PWM CLK is 1000000, pwm freq is now adjust from 50hz to 20khz
*/
#define PWM_TIM_FREQ		1000000

static void timPwmCfgIoPinOpenSwith(void);


/*
* when pwm pin is set to common io mode, need to init pwm agian
*/
static void timReInit(u_short dutyCycle)
{
	if (dutyCycle < 1000 && ioFlag == 1)
	{
		ioFlag = 0;
		timPwmInit();
	}	
}

/*
* Set pwm pin to common io mode by dutycyle and set ioFlag
*/
static void timSetIoflag(u_short dutyCycle)
{
	if (dutyCycle>=1000)
	{
		ioFlag = 1;
		timPwmCfgIoPinOpenSwith();
	}	
}


static void timUpdateSwitchPwmpinSta(u_short dutyCycle)
{
	if (dutyCycle == 0)
	{
	}
	else
	{
		timSetIoflag(dutyCycle);
	}
}

static void timPinConfig(void)
{
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	

	GPIO_InitStructure.GPIO_Pin = PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(PWM_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(PWM_PORT, GPIO_PinSource13, GPIO_AF_2);

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
}



/*
* 1000>dutyCycle >100 125->12.5% 375->37.5%
* 100>=dutyCycle>0    10->10%  20->%20  50->50%
*/
static void timGetFeqAndDutyCycle(u_short feqhz, u_short dutyCycle)
{
	//timerPeriod = (SystemCoreClock / 17570 ) - 1;
	timerPeriod = (PWM_TIM_FREQ / feqhz ) - 1;

	//timerPeriod = (SystemCoreClock / feqhz ) - 1;
	#if 0
	if (dutyCycle > 100)
	{
		channel1Pulse = (uint16_t) (((uint32_t) dutyCycle * (timerPeriod - 1)) / 1000);
	}
	else
	{
		channel1Pulse = (uint16_t) (((uint32_t) dutyCycle * (timerPeriod - 1)) / 100);
	}
	#else
	if (dutyCycle > 1000)
	{
		channel1Pulse = (uint16_t) (((uint32_t) 1000 * (timerPeriod - 1)) / 1000);
	}
	else
	{
		channel1Pulse = (uint16_t) (((uint32_t) dutyCycle * (timerPeriod - 1)) / 1000);
	}	
	#endif
}

static void timParamConfig(void)
{
	TIM_TimeBaseInitTypeDef	  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM1 Configuration ---------------------------------------------------
	Generate 7 PWM signals with 4 different duty cycles:
	TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)    
	=> TIM1CLK = PCLK2 = SystemCoreClock
	TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
	SystemCoreClock is set to 48 MHz for STM32F0xx devices

	The objective is to generate 7 PWM signal at 17.57 KHz:
	 - TIM1_Period = (SystemCoreClock / 17570) - 1
	The channel 1 and channel 1N duty cycle is set to 50%
	The channel 2 and channel 2N duty cycle is set to 37.5%
	The channel 3 and channel 3N duty cycle is set to 25%
	The channel 4 duty cycle is set to 12.5%
	The Timer pulse is calculated as follows:
	 - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100

	Note: 
	SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
	Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	function to update SystemCoreClock variable value. Otherwise, any configuration
	based on this variable will be incorrect. 
	----------------------------------------------------------------------- */	

	/*
	freq = 10 hz, dutycyle = 0, out 0, dutycyle = 100, out 1
	*/
	/* Time Base configuration */

	/* Compute the prescaler value */
 	prescalerValue = (uint16_t) (SystemCoreClock / PWM_TIM_FREQ) - 1;
 	//timerPeriod = (PWM_TIM_FREQ /  ) - 1; //
 	//timerPeriod = (PWM_TIM_FREQ / pwmfreq) - 1; //

	TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = timerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	#ifdef PWM_OP_HARDWARE
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	#else
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	#endif
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
}

static void timPwmOut(void)
{
	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


void timPwmUpdateParam(u_short freqhz, u_short dutyCycle)
{
	//update feqhz, dutyCycle
	pwmfreq = freqhz;
	pwmduty = dutyCycle;
	
	/*if ioFlag=1 and dutycle < 1000 */
	timReInit(dutyCycle);
	
	timGetFeqAndDutyCycle(freqhz, dutyCycle);
	
	//update the param
	timParamConfig( );

	#if 0
	if (dutyCycle == 0)
	{
		appClearSwitchFlag( );
	}
	else
	{
		appSetSwitchFlag( );
		timSetIoflag(dutyCycle);
	}
	#else
	timUpdateSwitchPwmpinSta(dutyCycle);
	#endif
}

void timPwmUpdateDutyCycle(u_short dutyCycle)
{
	//update feqhz, dutyCycle
	pwmduty = dutyCycle;
	/*
	if (dutyCycle < 1000 && ioFlag == 1)
	{
		ioFlag = 0;
		timPwmInit();
	}
	*/
	timReInit(dutyCycle);
	
	timGetFeqAndDutyCycle(pwmfreq, dutyCycle);
	//updtae the param
	timParamConfig( );
	#if 0
	if (dutyCycle == 0)
	{
		appClearSwitchFlag( );
	}
	else
	{
		appSetSwitchFlag( );
		/*
		if (dutyCycle>=1000)
		{
			ioFlag = 1;
			timPwmCfgIoPinOpenSwith();
		}	
		*/
		timSetIoflag(dutyCycle);
	}
	#else
	timUpdateSwitchPwmpinSta(dutyCycle);
	#endif
}


void timPwmClose(void)
{
	/* TIM1 counter enable */
	//TIM_Cmd(TIM1, ENABLE);
	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, DISABLE);	
}

/*
* PWM out dutcyle  100% 0
*/
void timPWMOn(void)
{
	//PWM_SET(1);
	//timPwmUpdateDutyCycle(100);
	timPwmUpdateDutyCycle(1000);
}

/*
* PWM out dutcyle  100% 1
*/
void timPWMOff(void)
{
	//PWM_SET(0);
	timPwmUpdateDutyCycle(0);
}




void timPwmSetFreq(u_short freq)
{
	pwmfreq = freq;
}

void timPwmInit(void)
{
	//pwmfreq = PWM_FEQ;
	pwmduty = 0;
	timPinConfig( );
	timPwmUpdateParam(pwmfreq, 0);
	timPwmOut( );
	//timPWMoff( );
}

void timPwmRecfgByFixFreq(u_short dutyCycle)
{
	pwmduty = dutyCycle;
	timPinConfig( );
	timPwmUpdateDutyCycle(dutyCycle);
	timPwmOut( );
}

void timPwmRecfgByFreqAndDutyCycle(u_short feqhz, u_short dutyCycle)
{
	pwmfreq = feqhz;
	pwmduty = dutyCycle;
	timPinConfig( );
	timPwmUpdateParam(feqhz, dutyCycle);
	timPwmOut( );	
}

void timPwmCfgIoPinAndClosePwm(void)
{
	GPIO_InitTypeDef stGpioInit;	

	timPwmClose( );
	stGpioInit.GPIO_Pin = PWM_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(PWM_PORT, &stGpioInit);

	PWM_SET(1);
}


static void timPwmCfgIoPinOpenSwith(void)
{
	GPIO_InitTypeDef stGpioInit;	

	timPwmClose( );
	stGpioInit.GPIO_Pin = PWM_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;	
	GPIO_Init(PWM_PORT, &stGpioInit);

	#ifdef PWM_OP_HARDWARE
	PWM_SET(0);
	#else
	PWM_SET(1);	
	#endif
	
}

u_short timPwmGetDuty(void)
{
	return pwmduty;
}
