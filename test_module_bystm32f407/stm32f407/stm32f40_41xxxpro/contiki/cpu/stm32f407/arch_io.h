#ifndef _ARCH_IO_H

#define _ARCH_IO_H

#include "stm32f4xx.h"

#define IO_SET(portx, pin, a)		\
{									\
	if(a){							\
		(portx)->BSRRL = (pin);		\
	}else{							\
		(portx)->BSRRH = (pin);		\
	}								\
}

#define IO_READ(portx, pin)	GPIO_ReadInputDataBit((portx), (pin))

#endif
