#ifndef _DS18B20_H
#define _DS18B20_H

#include "stm32f30x.h"


#define DS18B20_MAX_NUM		2


typedef struct _ds18b20_dev DS18B20_DEV;

struct _ds18b20_dev
{
	GPIO_TypeDef* port;
	uint16_t 	  pin;
	void (*set_high)(const DS18B20_DEV *pdev);
	void (*set_low)(const DS18B20_DEV *pdev);
	uint8_t (*read)(const DS18B20_DEV *pdev);
};

typedef struct ds18b20_device
{
	u_char id[DS18B20_MAX_NUM][8];
	u_char cnt;
	short ptemp;
	short btemp;
}DS18B20_DEVICE;


void one_wire_write_bit(uint8_t bit);
uint8_t one_wire_read_bit(void);
void one_wire_write_byte(uint8_t data);
uint8_t one_wire_read_byte(void);
void ds18b20_init(void);
bool one_wire_reset_pulse(void) ;

#endif
