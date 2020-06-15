#include "stm32f0xx.h"
#include "basictype.h"
#include "iodef.h"
#include "boardInit.h"
#include "arch_spi.h"


void board_init(void)
{
	GPIO_InitTypeDef		stGpioInit;
	NVIC_InitTypeDef		stNvicInit;

	//gpio clock enable
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);	

  	/* Enable USARTs Clock */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	{PWM_PORT->BRR=PWM_PIN;}

	//led pin init
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = LED_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT, &stGpioInit);

	stGpioInit.GPIO_Pin = LED_B_PIN;
	GPIO_Init(LED_B_PORT, &stGpioInit);

	//config usart1 pins
	/* USART1 Pins configuration ************************************************/
	/* Connect pin to Periph */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
	/* Configure pins as AF pushpull */
	//cfg tx
	stGpioInit.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
	stGpioInit.GPIO_Mode = GPIO_Mode_AF;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &stGpioInit);

	//usart nvic config
	/* USART1 IRQ Channel configuration */
	stNvicInit.NVIC_IRQChannel = USART1_IRQn;
	stNvicInit.NVIC_IRQChannelPriority = 0x01;
	stNvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNvicInit);

	//config 5V,2.5V, EXTERN logic output  pin init
	//5V 2.5V
	//led pin init
	/*
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = CON_2V5_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CON_2V5_PORT, &stGpioInit);
	CON_2V5_SET(0);


	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = CON_5V_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CON_5V_PORT, &stGpioInit);
	CON_5V_SET(0);
	
	stGpioInit.GPIO_Pin = EXTERN_SER_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(EXTERN_SER_PORT, &stGpioInit);
	EXTERN_SER_SET(0);	

	//T
	stGpioInit.GPIO_Pin = CURRENT_MOS_SW_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CURRENT_MOS_SW_PORT, &stGpioInit);
	CURRENT_MOS_SW_SET(1);
	*/
}

