#include "main.h"
#include "contiki.h"
#include "dbg_uart.h"
#include "sysprintf.h"
#include "dev/serial-line.h"

PROCINIT(NULL);
PROCESS(blink_led_process, "blink_led");

//const u_char *Data2 = (const u_char *)(0X1FFFF7BB);//����
//const u_char *Data1 = (const u_char *)(0X1FFFF7BA);//

PROCESS_THREAD(blink_led_process, ev, data)
{
	static struct etimer et_led;
	//u_short data12, data21;
	//u_short v;
	PROCESS_BEGIN();
	etimer_set(&et_led, 1000);
	while(1)
	{
		PROCESS_YIELD_UNTIL(etimer_expired(&et_led));
		//LED_SET(1);
		LED_B_SET(1);
		etimer_reset(&et_led);
		PROCESS_YIELD_UNTIL(etimer_expired(&et_led));
		//LED_SET(0);
		LED_B_SET(0);
		etimer_reset(&et_led);
	}
	PROCESS_END();
}


static void sys_init(void)
{
	//board_init( );
	clock_init( );
	dbg_uart_init( );
	xdev_out(dbg_uart_send_byte);
	uart_handler_set(1,serial_line_input_byte);

	process_init( );
	process_start(&etimer_process, NULL);
	ctimer_init( );

	serial_line_init( );//For shell
	serial_shell_init( );
	shell_init( );

	//watch dog init
	//watch_dog_init( );
	//ctimer_set(&ctWatch, 200, feedWatchdog, NULL);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	clock_init( );
	/* Infinite loop */
	xprintf("system run-------------------\r\n");
	while (1)
	{
		do 
		{
		} while(process_run() > 0);	
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
