#include "stm32f0xx.h"
#include "basictype.h"
#include "uart.h"
#include "iodef.h"
#include "stm32f030_uart.h"



/*
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
*/

void uartInit(int nPort)
{
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef *uartx = NULL;
	
	if (nPort == UART1)
	{
		uartx = USART1;
		/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- Stop Bit = 1 Stop Bit
		- Parity = No Parity
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
	}
	else if (nPort == UART2)
	{
		uartx = USART2;
		xprintf("usart2 init\r\n");
		/* USARTx configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- Stop Bit = 1 Stop Bit
		- Parity = No Parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
		*/
		//USART_InitStructure.USART_BaudRate = 460800;
		USART_InitStructure.USART_BaudRate = 230400;
		//USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	}

	/* USART configuration */
	USART_Init(uartx, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(uartx, ENABLE);
	USART_ITConfig(uartx, USART_IT_RXNE, ENABLE);	
}


#if 1
static int uartSendByte(int ch)
{
	//USART_SendData(USART1, (uint8_t) ch);
	/* Transmit Data */

	/* Loop until transmit data register is empty */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{
	}
	USART2->TDR = (ch & (uint16_t)0x01FF);

	//USARTx->ISR & USART_FLAG) != (uint16_t)RESET
	
	return ch;
}
#else
static int uartSendByte(int ch)
{
	//USART_SendData(USART1, (uint8_t) ch);
	/* Transmit Data */
	USART2->TDR = (ch & (uint16_t)0x01FF);
	/* Loop until transmit data register is empty */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
	{
	}
	/* Loop until USART1 DR register is empty */ 
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {}
	USART1->ICR |= USART_ICR_TCCF;

	//USARTx->ISR & USART_FLAG) != (uint16_t)RESET
	
	return ch;
}
#endif


void CON_TXRX_SET(u_short flag)
{
	if(flag)										
	{
		//GPIO_SetBits(CON_TXRX_PORT, CON_TXRX_PIN);
	}		
	else										
	{ 
		//GPIO_ResetBits(CON_TXRX_PORT, CON_TXRX_PIN);
	}	
}


void uartSendBytes(const void *pdata, int nLength)
{
	const u_char *pch = (const u_char *)pdata;
	int i = 0;
	//switch to tx
	CON_TXRX_SET(1);
	
	for (i = 0; i < nLength; i++)
	{
		uartSendByte(pch[i]);
	}

	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	//finish tx return rx
	CON_TXRX_SET(0);
}








