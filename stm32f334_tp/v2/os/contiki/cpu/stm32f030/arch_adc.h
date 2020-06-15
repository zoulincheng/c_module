#ifndef _ARCH_ADC_H
#define _ARCH_ADC_H

#include "iodef.h"

extern void adcInit(void);
u_short adcGetConvertDs(void);
u_short adcGetConvertDv(void);

//define current voltage simple adc pin  PA4  ADCIN4
//#define ADC_CURRENT_PIN			GPIO_Pin_4	//ADCIN0
//#define ADC_CURRENT_PORT		GPIOA
//#define ADC_CURRENT_SET(a)		_IO_SET(ADC_CURRENT_PORT, ADC_CURRENT_PIN, a)

//define  OUT voltage  simple adc pin  
//#define ADC_OUT_PIN				GPIO_Pin_1	//ADCIN9
//#define ADC_OUT_PORT			GPIOB

//LM5170_MODA  device   adc interface
#define ADC_IN10_PIN		GPIO_Pin_0
#define ADC_IN11_PIN		GPIO_Pin_1
#define ADC_IN12_PIN		GPIO_Pin_2
#define ADC_IN13_PIN		GPIO_Pin_3
#define ADC_10_13_PORT		GPIOC

#define ADC_IN0_PIN			GPIO_Pin_0	//PA0
#define ADC_IN0_PORT		GPIOA


#define MODA_HIGHV			ADC_IN10_PIN
#define MODA_HIGHV_PORT		ADC_10_13_PORT

#define MODA_ISET			ADC_IN11_PIN
#define MODA_ISET_PORT		ADC_10_13_PORT

#define MODA_IOUT1			ADC_IN12_PIN
#define MODA_IOUT1_PORT		ADC_10_13_PORT

#define MODA_IOUT2			ADC_IN13_PIN
#define MODA_IOUT2_PORT		ADC_10_13_PORT

#define MODA_LOWV			ADC_IN0_PIN
#define MODA_LOWV_PORT		ADC_IN0_PORT

/*end adc port*/


/*ref pin adc*/
#define ADC_IN1_PIN			GPIO_Pin_1
#define ADC_IN1_PORT		GPIOA

#define ADC_REF				ADC_IN1_PIN
#define ADC_REF_PORT		ADC_IN1_PORT
/*end*/

typedef struct _adc_obj_data{
	u_long LV;				/*dc-dc low port v*/
	u_long HV;				/*dc-dc high port v*/
	u_long iset;			/*dc-dc current set*/
	u_long lastiset;
	u_long iout1;			/*dc-dc phase1 current out*/
	u_long iout2;			/*dc_dc phase2 current out*/

	u_long currentv;				//pwm  filter limit currnt v
	u_long outv;					//fsu out  adc
	u_long vcc;						//power 
}ADC_OBJ_DATA;



void adcInit(void);
ADC_OBJ_DATA * adcGetObj(void);



#endif
