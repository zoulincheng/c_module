#ifndef _UART_H
#define _UART_H


typedef int(*uart_handler_t)(unsigned char);
#define UART1	1
#define UART2	2

void uart_handler_set(int nport, uart_handler_t handler);

#endif
