#include "basictype.h"
#include "contiki.h"
#include "sysprintf.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>
//#include <err.h>

#include <tlsf.h>

//#define _TLSF_EXTERN_SIZE()	 (tlsf_size()+tlsf_pool_overhead()+tlsf_alloc_overhead())

static u_char _buf_tlsf[10*1024]@0x20001000;


PROCESS(tlsf_test, "tlsf");


PROCESS_THREAD(tlsf_test, ev, data)
{
	static struct etimer _et;
	static tlsf_t tlsf;
	static u_char *p[32] = {NULL};
	static u_char i = 0;
	PROCESS_BEGIN();
	XPRINTF((0, "tlsf_test\r\n"));
	tlsf = tlsf_create_with_pool(_buf_tlsf, 10*1024);
	while(1)
	{
		etimer_set(&_et, 100);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		if (i < 32)
		{
			p[i] = tlsf_malloc( tlsf, 16);
			memset(p[i], i, 16);
		}

		if (i < 32)
		{
			XPRINTF((6, "P[%d]=%08x\r\n", i,p[i]));
			MEM_DUMP(6, "tlsf", p[i], 16);
			
			i++;
		}

		if (i >= 32)
		{	
			int j = 0;
			for(j = 0; j < i; j++)
			{
				tlsf_free(tlsf, p[j]);
			}
			i = 0;
		}
		//XPRINTF((10, "PRINT\r\n"));
	}
	PROCESS_END();
}


void app_test_tlsf(void)
{
	process_start(&tlsf_test, NULL);
}