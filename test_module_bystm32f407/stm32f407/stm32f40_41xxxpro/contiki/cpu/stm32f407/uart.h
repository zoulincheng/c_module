#ifndef _UART_H
#define _UART_H


typedef int(*uart_handler_t)(unsigned char);
//#define UART1	1
//#define UART2	2

void uartHandlerFuncSet(int nport, uart_handler_t handler);

#endif