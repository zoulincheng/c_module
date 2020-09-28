#ifndef _ARCH_CAN_H
#define _ARCH_CAN_H
#include "arch_io.h"



#define CAN_RX_PIN		GPIO_Pin_5
#define CAN_RX_PORT		GPIOB

#define CAN_TX_PIN		GPIO_Pin_6
#define CAN_GPIO_PORT	CAN_RX_PORT

#define CAN_STB_PIN		GPIO_Pin_7
#define CAN_STB_PORT	CAN_RX_PORT
#define CAN_STB_SET(a)	IO_SET(CAN_STB_PORT, CAN_STB_PIN, a)

#define CAN_GPIO_CLK 		RCC_AHB1Periph_GPIOB

#define CAN_AF_PORT        GPIO_AF_CAN2
#define CAN_RX_SOURCE      GPIO_PinSource5
#define CAN_TX_SOURCE      GPIO_PinSource6 
#define CAN_CLK            RCC_APB1Periph_CAN2

#define CANx			   CAN2


#define CAN_StdId			0x321
#define CAN_ExtId			0x01

#define CAN_RINGINDEX_NUM	256

#endif
