#include "stm32f30x.h"

#include "basictype.h"


#define _IO_SET(port, pin, a)			\
	if(a)								\
		{(port)->BSRR=(pin);}			\
	else								\
		{(port)->BRR=(pin);}

#define _IO_READ(GPIOx, GPIO_Pin)	GPIO_ReadInputDataBit(GPIOx, GPIO_Pin)






