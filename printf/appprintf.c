#include "contiki.h"
#include "basictype.h"
#include "printf.h"


PROCESS(appprintf, "printf");



void _putchar(char character)
{
	uart1_send_byte(character);
}


PROCESS_THREAD(appprintf, ev, data)
{
	static struct etimer _et;

	PROCESS_BEGIN();

	while(1)
	{
		etimer_set(&_et, 1000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));
	    printf(" first string: %s\r\n", "Hello world");
	    printf(" first digits: %d\r\n", 10);
	    printf(" first pointer: %p\r\n", &_et);	
		printf(" first float: %f\r\n", 10.2345);	
		printf(" first hex: %02x\r\n", 0x1678);			
	}
	PROCESS_END();
}

void appprintf_start(void)
{
	process_start(&appprintf,NULL);
}

