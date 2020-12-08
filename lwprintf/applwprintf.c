#include "contiki.h"
#include "basictype.h"
#include "lwprintf/lwprintf.h"

PROCESS(applwprintf, "lwprintf");

/**
 * \brief           Output function to handle all characters for print operation
 * \param[in]       ch: Character to output
 * \param[in]       p: \ref lwprintf_t handle
 * \return          ch on success, 0 on failure
 */
static int lwprintf_my_out_func(int ch, lwprintf_t* p) {
    uint8_t c = (uint8_t)ch;

    /* Don't print zero */
    if (c == '\0') {
        return ch;
    }
    //HAL_UART_Transmit(&huart2, &c, 1, 100);
    uart1_send_byte(c);
    return ch;
}



PROCESS_THREAD(applwprintf, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	lwprintf_init(lwprintf_my_out_func);
	while(1)
	{
		etimer_set(&_et, 1000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));
	    /* Print formatted data */
	    lwprintf_printf("My first string: %s\r\n", "Hello world");
	    lwprintf_printf("My first digits: %d\r\n", 10);
	    lwprintf_printf("My first pointer: %p\r\n", &_et);	
		lwprintf_printf("My first float: %f\r\n", 10.2345);	
		lwprintf_printf("My first hex: %02x\r\n", 0x1678);	
	 
	}
	PROCESS_END();
}


void applwprintf_start(void)
{
	process_start(&applwprintf, NULL);
}

