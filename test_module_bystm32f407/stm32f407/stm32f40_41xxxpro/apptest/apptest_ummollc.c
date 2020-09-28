#include "basictype.h"
#include "contiki.h"
#include "sysprintf.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umm_malloc.h"
#include "umm_malloc_cfg.h"
#include "message_queue.h"


char u_umm_heap[UMM_MALLOC_CFG_HEAP_SIZE]@0x20000100;//0x2001FFFF
static struct message_queue _msg;


typedef struct{
	u_char buf[124];
	u_long cnt;
}MSG_DATA;


PROCESS(message_in, "message_in");
PROCESS(message_out, "message_out");


PROCESS(umm_heap_t, "umm_heap_t");
PROCESS_THREAD(umm_heap_t, ev, data)
{
	static struct etimer _et;
	u_char *pch;
	PROCESS_BEGIN();
	while(1)
	{
		etimer_set(&_et, 500);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		pch = umm_malloc(16);

		XPRINTF((2, "pch = %08x, heap_start = %08x  heap_end = %08x\r\n", pch, &u_umm_heap[0], &u_umm_heap[UMM_MALLOC_CFG_HEAP_SIZE-1]));

		//umm_free(pch);
		
	}
	PROCESS_END();
}


void **test=NULL;

static void init_pointer(void)
{
	int i = 0;
	test = umm_malloc(sizeof(void *)*16);
	if (test)
	{
		for (i = 0; i < 16; i++)
		{
			test[i] = umm_malloc(16);
			XPRINTF((6, "TEST[%d]=%08x\r\n", i, test[i]));
		}
	}
}


static void print_pointer(void)
{
	int i = 0;
	XPRINTF((6, "test =%08x\r\n", test));
	for (i = 0; i < 16; i++)
	{
		XPRINTF((6, "test[%d]=%08x\r\n", i, test[i]));
	}
}

PROCESS_THREAD(message_in, ev, data)
{
	static struct etimer _et;
	//u_char *pch;
	static u_long  cnt = 0;
	static MSG_DATA *pmsg = NULL;

	PROCESS_BEGIN();
	XPRINTF((6, " %d %d\r\n", _msg.max_depth, _msg.allocator.free_blocks));
	umm_init( );
	message_queue_init(&_msg, sizeof(MSG_DATA), 64);
	//init_pointer();
	while(1)
	{
		cnt++;
		etimer_set(&_et, 10);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		
		pmsg = message_queue_message_alloc(&_msg);

		//print_pointer( );
		if (pmsg)
		{
			pmsg->cnt = cnt;
			memcpy(pmsg->buf, "abcd", 5);
			message_queue_write(&_msg, pmsg);
			process_poll(&message_out);
			XPRINTF((6, " <--- cnt = %d %08x\r\n", pmsg->cnt, pmsg));
		}
		
		//XPRINTF((6, "cnt = %d\r\n", cnt));
		pmsg = NULL;
		
		//umm_free(pch);
		
	}
	PROCESS_END();
}

PROCESS_THREAD(message_out, ev, data)
{
	//static struct etimer _et;
	//u_char *pch;
	MSG_DATA *pmsg;
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_YIELD();
		
		if (pmsg = message_queue_tryread(&_msg))
		{
			XPRINTF((6, "---> cnt = %d %08x\r\n", pmsg->cnt, pmsg));
			
			message_queue_message_free(&_msg, pmsg);
		}
		process_poll(&message_out);
		//umm_free(pch);
	}
	PROCESS_END();
}




void apptest_umm(void)
{
	//process_start(&umm_heap_t, NULL);
	process_start(&message_in, NULL);
	process_start(&message_out, NULL);
}



