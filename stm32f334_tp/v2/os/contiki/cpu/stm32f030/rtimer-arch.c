

#include "sys/rtimer.h"
#include "contiki-conf.h"

//#include "xprintf.h"
#include "rtimer-arch.h"
//#include "sysprintf.h"

//static uint32_t time_msb = 0;   /* Most significant bits of the current time. */
/* time of the next rtimer event. Initially is set to the max
    value. */
//static rtimer_clock_t next_rtimer_time = 0;

/*---------------------------------------------------------------------------*/

void rtimer_arch_init(void)
{

}

/*---------------------------------------------------------------------------*/
void rtimer_arch_disable_irq(void)
{
}




/*---------------------------------------------------------------------------*/
void rtimer_arch_enable_irq(void)
{
}


/*---------------------------------------------------------------------------*/
rtimer_clock_t rtimer_arch_now(void)
{
	return 0;
}


void rtimer_arch_set(rtimer_clock_t t)
{
//offset = t -  RTIMER_ARCH_TIMER_BASE->TC_CV;
	
}



/*---------------------------------------------------------------------------*/
void rtimer_arch_schedule(rtimer_clock_t t)
{
}

/** @} */

