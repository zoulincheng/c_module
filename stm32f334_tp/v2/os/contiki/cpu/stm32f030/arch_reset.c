#include "stm32f0xx.h"
#include "arch_reset.h"


void sysReset(void)
{
	NVIC_SystemReset( );
}