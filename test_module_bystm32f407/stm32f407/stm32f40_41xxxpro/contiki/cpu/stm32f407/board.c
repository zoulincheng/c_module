#include "stm32f4xx.h"


void boardInit(void)
{
	GPIO_InitTypeDef		stGpioInit;
	NVIC_InitTypeDef		stNvicInit;

	//gpio clock enable
  	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);	
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE);

  	/* Enable USARTs Clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure pins as AF pushpull */
	//usart nvic config
	/* USART1 IRQ Channel configuration */
	stNvicInit.NVIC_IRQChannel = USART1_IRQn;
	stNvicInit.NVIC_IRQChannelPreemptionPriority = 0x01;
	stNvicInit.NVIC_IRQChannelSubPriority = 0x01;
	stNvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNvicInit);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_USART1);

	/* Configure USART Tx as alternate function  */
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Mode = GPIO_Mode_AF;

	stGpioInit.GPIO_Pin = GPIO_Pin_9;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &stGpioInit);

	/* Configure USART Rx as alternate function  */
	stGpioInit.GPIO_Mode = GPIO_Mode_AF;
	stGpioInit.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &stGpioInit);
}
