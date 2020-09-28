#include "contiki.h"
#include "sysprintf.h"
#include "basictype.h"


#include "lv_mem.h"

//Error[Li005]: no definition for "lv_log_add" [referenced from F:\bpms\program\stm32f407\stm32f40_41xxxpro\iar\Debug\ 

PROCESS(app_lv_mem_t, "lv_mem_t");



PROCESS_THREAD(app_lv_mem_t, ev, data)
{
	static struct etimer _et;
	static u_char *p[16] = {NULL};
	static int i = 0;
	static int j = 0;
	PROCESS_BEGIN();
	lv_mem_init();
	
	while(1)
	{
		etimer_set(&_et,500);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));

		{
			p[i%16] = lv_mem_alloc(16);
			memset(p[i], i, 16);
			XPRINTF((6, "P[%d]= %08x\r\n", i, p[i]));
			MEM_DUMP(6, "P[i]", p[i], 16);
			if (i %15 == 0)
			{
				for (j = 0; j < 16; j++)
				{
					lv_mem_free(p[j]);
				}
			}
		}
		i++;
	}
	PROCESS_END();
}



void app_lv_mem_start(void)
{
	process_start(&app_lv_mem_t, NULL);
}
