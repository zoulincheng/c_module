#include "basictype.h"
#include "contiki.h"
#include "main.h"
#include "sysprintf.h"

#include "stm32f407_uart.h"
#include "dev/serial-line.h"

#include "arch_io.h"



#define LED1_PIN	GPIO_Pin_7
#define LED1_PORT	GPIOA
#define LED1_SET(a)	IO_SET(LED1_PORT, LED1_PIN, a)


#define LED2_PIN	GPIO_Pin_4
#define LED2_PORT	GPIOC
#define LED2_SET(a)	IO_SET(LED2_PORT, LED2_PIN, a)


#define LED3_PIN	GPIO_Pin_5
#define LED3_PORT	GPIOC
#define LED3_SET(a)	IO_SET(LED3_PORT, LED3_PIN, a)

PROCINIT(NULL);

PROCESS(test_process, "test");




static void led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LED3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED3_PORT, &GPIO_InitStructure);	
}

static void led_set(void)
{
	LED1_SET(1);
}

static void led_reset(void)
{
	LED1_SET(0);
}

static void led2_set(void)
{
	LED2_SET(1);
}

static void led2_reset(void)
{
	LED2_SET(0);
}


static void led3_set(void)
{
	LED3_SET(1);
}

static void led3_reset(void)
{
	LED3_SET(0);
}

PROCESS_THREAD(test_process, ev, data)
{
	static 	struct etimer _et;
	PROCESS_BEGIN();
	
	while(1)
	{
		led_set( );
		led2_set( );
		led3_set( );
		etimer_set(&_et, 100);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));

		led_reset( );
		led2_reset( );
		led3_reset( );	

		etimer_set(&_et, 100);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));		
		//uart1_send_byte('a');
		//uart1_send_byte('b');
		//XPRINTF((10, "test_process\r\n"));
	}
	PROCESS_END();
}


static void _sysinit(void)
{
	/*board base hardware init*/
	boardInit( );
    led_init( );
	/*dbg uart init*/
	uart_init(1);
	xdev_out(uart1_send_byte);
	uartHandlerFuncSet(1, serial_line_input_byte);
	
	/*os init*/
	clock_init();
	process_init( );
	process_start(&etimer_process, NULL);
	ctimer_init( );	

	/*test process*/
	process_start(&test_process, NULL);

	/*shell init*/
	serial_line_init( );
	serial_shell_init( );	
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	_sysinit( );

	//canInit( );
	//can_test();
	apptest_umm( );
	//app_test_tlsf( );
	//app_lv_mem_start( );
	//app_test_qmem( );
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
	//clock_init();

	
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
