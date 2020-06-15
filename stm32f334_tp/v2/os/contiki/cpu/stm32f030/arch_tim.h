#ifndef _ARCH_TIM_H
#define _ARCH_TIM_H
#include "iodef.h"

#if 0
#define 	TIM_AC_A_PIN	GPIO_Pin_6  		//16  17  19
#define 	TIM_AC_A_PORT	GPIOA
#define 	TIM_AC_A_TIM	TIM16				//CH1
#define		TIM_AC_AIO_CLK	RCC_AHBPeriph_GPIOA //RCC_AHBPeriph_GPIOC
#define		TIM_AC_AT_CKL	RCC_APB2Periph_TIM16
#define 	TIM_AC_A_PSOURCE	GPIO_PinSource6
#define 	TIM_AC_A_AF		GPIO_AF_2
#define		TIM_AC_A_CH		TIM_Channel_1
#define		TIM_AC_A_IT		TIM_IT_CC1
#define 	TIM_AC_A_IRQ	TIM16_IRQn


#define		TIM_AC_B_PIN	GPIO_Pin_7
#define 	TIM_AC_B_PORT	GPIOA
#define 	TIM_AC_B_TIM	TIM17				//CH1
#define		TIM_AC_BIO_CLK	RCC_AHBPeriph_GPIOA //RCC_AHBPeriph_GPIOC
#define		TIM_AC_BT_CKL	RCC_APB2Periph_TIM17
#define 	TIM_AC_B_PSOURCE	GPIO_PinSource7
#define 	TIM_AC_B_AF		GPIO_AF_2
#define		TIM_AC_B_CH		TIM_Channel_1
#define		TIM_AC_B_IT		TIM_IT_CC1
#define 	TIM_AC_B_IRQ	TIM17_IRQn

#define 	TIM_AC_C_PIN	GPIO_Pin_1
#define 	TIM_AC_C_PORT	GPIOB
#define		TIM_AC_C_TIM	TIM3				//CH4
#define		TIM_AC_CIO_CLK	RCC_AHBPeriph_GPIOB //RCC_AHBPeriph_GPIOC
#define		TIM_AC_CT_CKL	RCC_APB1Periph_TIM3
#define 	TIM_AC_C_PSOURCE	GPIO_PinSource1
#define 	TIM_AC_C_AF		GPIO_AF_1
#define		TIM_AC_C_CH		TIM_Channel_4
#define		TIM_AC_C_IT		TIM_IT_CC4
#define 	TIM_AC_C_IRQ	TIM3_IRQn
#else
//AC_A ->tim3  ch1
#define 	TIM_AC_A_PIN		GPIO_Pin_6  		//16  17  19
#define 	TIM_AC_A_PORT		GPIOA
#define 	TIM_AC_A_TIM		TIM3				//CH1
#define		TIM_AC_AIO_CLK		RCC_AHBPeriph_GPIOA //
#define		TIM_AC_AT_CKL		RCC_APB1Periph_TIM3
#define 	TIM_AC_A_PSOURCE	GPIO_PinSource6
#define 	TIM_AC_A_AF			GPIO_AF_1
#define		TIM_AC_A_CH			TIM_Channel_1
#define		TIM_AC_A_IT			TIM_IT_CC1
#define 	TIM_AC_A_IRQ		TIM3_IRQn

//AC_B ->tim3  ch2
#define		TIM_AC_B_PIN		GPIO_Pin_7
#define 	TIM_AC_B_PORT		GPIOA
#define 	TIM_AC_B_TIM		TIM3				//
#define		TIM_AC_BIO_CLK		RCC_AHBPeriph_GPIOA //
#define		TIM_AC_BT_CKL		RCC_APB1Periph_TIM3
#define 	TIM_AC_B_PSOURCE	GPIO_PinSource7
#define 	TIM_AC_B_AF			GPIO_AF_1
#define		TIM_AC_B_CH			TIM_Channel_2
#define		TIM_AC_B_IT			TIM_IT_CC2
#define 	TIM_AC_B_IRQ		TIM3_IRQn

//AC_C  ->tim3 ch3
#define 	TIM_AC_C_PIN		GPIO_Pin_0
#define 	TIM_AC_C_PORT		GPIOB
#define		TIM_AC_C_TIM		TIM3				//
#define		TIM_AC_CIO_CLK		RCC_AHBPeriph_GPIOB //
#define		TIM_AC_CT_CKL		RCC_APB1Periph_TIM3
#define 	TIM_AC_C_PSOURCE	GPIO_PinSource0
#define 	TIM_AC_C_AF			GPIO_AF_1
#define		TIM_AC_C_CH			TIM_Channel_3
#define		TIM_AC_C_IT			TIM_IT_CC3
#define 	TIM_AC_C_IRQ		TIM3_IRQn
#endif

#define 	TIM_FREQ_MAX_CNT	25

typedef uint32_t(*CAPTURE)(TIM_TypeDef* TIMx);

typedef struct _tim_dev{
	GPIO_TypeDef 	*port;
	uint16_t		pin;
	uint16_t		srcpin;
	uint16_t		afpin;
	uint32_t 		ioCLK;
	uint32_t   		timCLK;
	TIM_TypeDef* 	timx;
	uint16_t		timch;
	uint16_t		timit;
	uint8_t			timirq;
	CAPTURE			fcapture;
}TIM_DEV;


typedef struct _acfreq_obj{
	u_long ac_cnt1;
	u_long ac_cnt2;
	u_long ac_freq;
	u_long freq_a[TIM_FREQ_MAX_CNT];
	u_char ac_flag;
	u_short freqCnt; ////when capture freq is not in 49.7-50.3 freqCnt+1 

	u_long _poll;//for poll event count
	u_long Capture;    //for count time
}ACFREQ_OBJ;

/*
*
*/
typedef struct _freq_obj{
	u_long phaseAFreq;
	u_long phaseBFreq;
	u_long phaseCFreq;
	u_short AFcnt;//when capture freq is not in 49.7-50.3 AFcnt+1 
	u_short BFcnt;
	u_short CFcnt;
}FREQ_OBJ;

FREQ_OBJ *timGetFreqObj(void);
void timProcessStart(void);

#endif
