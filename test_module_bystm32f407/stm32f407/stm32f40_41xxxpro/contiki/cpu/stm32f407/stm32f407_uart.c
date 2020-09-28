#include "stm32f4xx.h"
#include "stm32f407_uart.h"


void uart_init(int port)
{
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef *UARTx;
	if (port == 1)
	{
		UARTx = USART1;
		/* USARTx configured as follows:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
		*/
		//USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_BaudRate = 921600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		/* USART configuration */
		USART_Init(UARTx, &USART_InitStructure);

		/* Enable USART */
		USART_Cmd(UARTx, ENABLE);

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}
}

void uart1_send_byte(uint16_t Data)
{
	USART1->DR = (Data & (uint16_t)0x01FF);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
}


void uart1_dbg_byte(uint8_t Data)
{
	USART1->DR = (Data & (uint16_t)0x01FF);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}
}



