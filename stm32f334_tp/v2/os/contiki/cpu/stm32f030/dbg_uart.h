#ifndef _DBG_UART_H
#define _DBG_UART_H
#include "uart.h"
#include <stdio.h>


int  dbgUartSendByte(int ch);
void dbgUartInit(void);
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif
