#ifndef _IIC_H
#define _IIC_H

#include "stm32f30x.h" //#include "stm32f0xx.h"


/*
#define PCT2075_SCL_PIN		GPIO_Pin_6
#define PCT2075_SCL_PORT	GPIOB
#define PCT2075_SDA_PIN		GPIO_Pin_7
#define PCT2075_SDA_PORT	GPIOB

#define M24C02W_SCL_PIN		GPIO_Pin_15
#define M24C02W_SCL_PORT	GPIOB
#define M24C02W_SDA_PIN		GPIO_Pin_14
#define M24C02W_SDA_PORT	GPIOB
*/


typedef struct _iic_dev{
	GPIO_TypeDef* SCL_PORT;
	GPIO_TypeDef* SDA_PORT;
	uint16_t 	  SCL_PIN;
	uint16_t 	  SDA_PIN;
}IIC_DEV;

#endif
