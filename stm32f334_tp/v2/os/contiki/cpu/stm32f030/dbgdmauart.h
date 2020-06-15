#ifndef _DBGDMAUART_H
#define _DBGDMAUART_H

void dbguart_DMA_Configuration(void);
void dbguart_DMA_NVIC_Config(void);
//unsigned int dbg_send_bytes(const unsigned char *seq, unsigned int len);
void uart_dma_putchar(u_char ch);
#endif

