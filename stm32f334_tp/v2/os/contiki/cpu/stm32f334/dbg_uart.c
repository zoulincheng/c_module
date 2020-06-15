#include "stm32f30x.h"
#include "basictype.h"
#include "contiki.h"
#include "lib/ringbuf.h"


#include "dbg_uart.h"

/**
  * @brief Configure the USART Device
  * @param  None
  * @retval None
  */
static void dbg_usart_cfg(void)
{ 
	USART_InitTypeDef USART_InitStructure;

	/* USARTx configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- Stop Bit = 1 Stop Bit
	- Parity = No Parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	//USART_InitStructure.USART_BaudRate = 230400;
	//USART_InitStructure.USART_BaudRate = 921600;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* USART configuration */
	USART_Init(USART1, &USART_InitStructure);

	//USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void dbg_uart_init(void)
{
	dbg_usart_cfg( );
}


int  dbg_uart_send_byte(int ch)
{
	/* Transmit Data */
	USART1->TDR = (ch & (uint16_t)0x01FF);
	/* Loop until transmit data register is empty */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
	//while (!(USART1->ISR & USART_FLAG_TXE)); // USART1 可换成你程序中通信的串口
	//USARTx->ISR & USART_FLAG) != (uint16_t)RESET
	//USART1->TDR = (ch & (uint16_t)0x01FF);
	return ch;
}


void dbg_putchar(int ch)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
	}
	USART1->TDR = (ch & (uint16_t)0x01FF);
}
void dbg_uart_send_bytes(u_char *pch, int len)
{
	int i = 0;
	for (i = 0; i < len ; i++)
	{
		dbg_uart_send_byte(pch[i]);
	}
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART1->ICR |= USART_ICR_TCCF;
}


static unsigned int dbg_send_bytes(const unsigned char *s, unsigned int len)
{
	unsigned int i=0;
	while(s && *s!=0) {
		if( i >= len) { break; }
		dbg_putchar(*s++); i++;
	}
	return i;
}

#if 0
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  //USART_SendData(USART1, (uint8_t) ch);
  

  /* Loop until transmit data register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  USART1->TDR = (ch & (uint16_t)0x01FF);

  return ch;
}
#endif



