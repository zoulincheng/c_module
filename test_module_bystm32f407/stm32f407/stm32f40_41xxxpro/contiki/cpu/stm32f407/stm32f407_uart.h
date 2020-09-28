#ifndef _STM32F407_UART_H
#define _STM32F407_UART_H

void uart_init(int port);
void uart1_send_byte(uint16_t Data);
void uart1_dbg_byte(uint8_t Data);
#endif
