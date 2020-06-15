#ifndef _IODEF_H
#define _IODEF_H

#include "stm32f0xx.h"
#include "basictype.h"
#include "sysprintf.h"

#define _IO_SET(port, pin, a)			\
	if(a)								\
		{(port)->BSRR=(pin);}			\
	else								\
		{(port)->BRR=(pin);}

#define _IO_READ(GPIOx, GPIO_Pin)	GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)


#define PIN_MODE_OUT		1
#define PIN_MODE_IN			0

//define LED state pin test  net MA_DIR PC11 
#define LED_PIN			GPIO_Pin_11
#define LED_PORT		GPIOC
#define LED_SET(a)	_IO_SET(LED_PORT, LED_PIN, a)

//define LED state pin test  net MB_DIR PB1
#define LED_B_PIN		GPIO_Pin_1
#define LED_B_PORT		GPIOB
#define LED_B_SET(a)	_IO_SET(LED_B_PORT, LED_B_PIN, a)

//define PWM
#define PWM_PIN			GPIO_Pin_13	//TIM1_CH1N
#define PWM_PORT		GPIOB
#define PWM_SET(a)		_IO_SET(PWM_PORT, PWM_PIN, a)


#endif
