#include "stm32f4xx.h"
#include "basictype.h"
#include "uart.h"
#include "sysprintf.h"

static uart_handler_t uart_std = NULL;
static uart_handler_t uart_485_handler = NULL;

void uartHandlerFuncSet(int nport, uart_handler_t handler)
{
	/* store the setting */
	if (nport == 1)
	{
		uart_std = handler;
	}
	else if (nport == 2)
	{
		uart_485_handler = handler;
	}
}


void USART1_IRQHandler(void)
{
	uart_handler_t handler = uart_std;
	static u_char ubrxdata;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ubrxdata = (uint16_t)(USART1->DR & (uint16_t)0x01FF);
		//xprintf("irq data = %02x\r\n", ubrxdata);
		if (handler != NULL)
		{
			handler(ubrxdata);
			//serial_line_input_byte((unsigned char)ubrxdata);
			//xprintf("irq data = %02x\r\n", ubrxdata);
		}
	}
}


void USART2_IRQHandler(void)
{
	uart_handler_t handler = uart_485_handler;
	u_char ubrxdata;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ubrxdata = (uint16_t)(USART2->DR & (uint16_t)0x01FF);
		//
		if (handler != NULL)
		{
			handler(ubrxdata);
		}
	}	
}

