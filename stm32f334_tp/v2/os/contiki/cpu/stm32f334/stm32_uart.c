#include "stm32f30x.h"

#include "basictype.h"
#include "uart.h"
#include "sysprintf.h"


static uart_handler_t uart_std = NULL;
static uart_handler_t uart_485_handler = NULL;

void uart_handler_set(int nport, uart_handler_t handler)
{
	/* store the setting */
	if (nport == UART1)
	{
		uart_std = handler;
	}
	else if (nport == UART2)
	{
		uart_485_handler = handler;
	}
}


void USART1_IRQHandler(void)
{
	uart_handler_t handler = uart_std;
	u_char ubrxdata;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ubrxdata = (uint16_t)(USART1->RDR & (uint16_t)0x01FF);
		//xprintf("irq data = %02x\r\n", ubrxdata);
		if (handler != NULL)
		{
			handler(ubrxdata);
		}
	}
}


void USART2_IRQHandler(void)
{
	uart_handler_t handler = uart_485_handler;
	u_char ubrxdata;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ubrxdata = (uint16_t)(USART2->RDR & (uint16_t)0x01FF);
		//
		if (handler != NULL)
		{
			handler(ubrxdata);
		}
	}	
}

