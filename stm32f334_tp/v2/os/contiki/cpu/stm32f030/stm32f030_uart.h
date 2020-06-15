#ifndef STM32F030_UART_H
#define STM32F030_UART_H


void uartInit(int nPort);
void uartSendBytes(const void *pdata, int nLength);


#endif
