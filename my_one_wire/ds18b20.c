#include "stm32f30x.h"
#include "iodef.h"
#include "ds18b20.h"

static void _set_high(const DS18B20_DEV* pdev);
static void _set_low(const DS18B20_DEV* pdev);
static uint8_t _read_data(const DS18B20_DEV *pdev);

static const DS18B20_DEV tgdev ={
	DS18B20_DQ_PORT,
	DS18B20_DQ_PIN,
	_set_high,
	_set_low,
	_read_data,
};


static void _set_high(const DS18B20_DEV* pdev)
{
	_IO_SET(pdev->port, pdev->pin, 1);
}

static void _set_low(const DS18B20_DEV* pdev)
{
	_IO_SET(pdev->port, pdev->pin, 0);
}

static uint8_t _read_data(const DS18B20_DEV *pdev)
{
	return _IO_READ(pdev->port, pdev->pin);
}

void ds18b20_dev_init(const DS18B20_DEV * pdev) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = pdev->pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(pdev->port, &GPIO_InitStructure);
}


/*
	return true, has ds18b20 dev
		   false no ds18b20 dev
*/
bool ds18b20_reset_pulse(const DS18B20_DEV * pdev) 
{
	BitAction bit;
	// Pull bus down for 500 us (min. 480 us)
	pdev->set_low(pdev);	//GPIO_ResetBits(gpio, pin);
	delay_us(500);			//delay_us(timer, 500);
	pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

	// Wait 70 us, bus should be pulled up by resistor and then
	// pulled down by slave (15-60 us after detecting rising edge)
	delay_us(70);//delay_us(timer, 70);
	bit = pdev->read(pdev);//BitAction bit = GPIO_ReadInputDataBit(gpio, pin);
	
	if (bit == Bit_RESET) {
	} else {
	    return false;
	}

	// Wait additional 430 us until slave keeps bus down (total 500 us, min. 480 us)
	delay_us(430);//delay_us(timer, 430);
	return true;
}

void ds18b20_write_1(const DS18B20_DEV * pdev) {
    // Pull bus down for 15 us
    pdev->set_low(pdev);//GPIO_ResetBits(gpio, pin);
    delay_us(15);//delay_us(timer, 15);
    pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    delay_us(50);//delay_us(timer, 50);
}


void ds18b20_write_0(const DS18B20_DEV * pdev) {
    // Pull bus down for 60 us
    pdev->set_low(pdev);//GPIO_ResetBits(gpio, pin);
    delay_us(60);//delay_us(timer, 60);
    pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    delay_us(5);//delay_us(timer, 5);
}

void ds18b20_write_bit(uint8_t bit, const DS18B20_DEV * pdev) {
    if (bit) {
        ds18b20_write_1(pdev);
    } else {
        ds18b20_write_0(pdev);
    }
}


uint8_t ds18b20_read_bit(const DS18B20_DEV * pdev) {
    // Pull bus down for 5 us
    BitAction bit;
    pdev->set_low(pdev);//GPIO_ResetBits(gpio, pin);
    delay_us(5);//delay_us(timer, 5);
    pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

    // Wait 5 us and check bus state
    delay_us(5);//delay_us(timer, 5);
    bit = pdev->read(pdev);//bit = GPIO_ReadInputDataBit(gpio, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    delay_us(55);//delay_us(timer, 55);

    if (bit == Bit_SET) {
        return true;
    } else {
        return false;
    }
}


void ds18b20_write_byte(uint8_t data, const DS18B20_DEV * pdev) {
    uint8_t i;
    for (i = 0; i < 8; ++i) {
        if ((data >> i) & 1) {
            ds18b20_write_1(pdev);
        } else {
            ds18b20_write_0(pdev);
        }
    }
}

uint8_t ds18b20_read_byte(const DS18B20_DEV * pdev) {
    uint8_t i;
    uint8_t data = 0;
    uint8_t bit;
    for (i = 0; i < 8; ++i) {
        bit = ds18b20_read_bit(pdev);
        data |= bit << i;
    }
    return data;
}


void one_wire_write_bit(uint8_t bit) {
	const DS18B20_DEV * pdev = &tgdev;
    if (bit) {
        ds18b20_write_1(pdev);
    } else {
        ds18b20_write_0(pdev);
    }
}


uint8_t one_wire_read_bit(void) {
	const DS18B20_DEV * pdev = &tgdev;
    // Pull bus down for 5 us
    BitAction bit;
    pdev->set_low(pdev);//GPIO_ResetBits(gpio, pin);
    delay_us(5);//delay_us(timer, 5);
    pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

    // Wait 5 us and check bus state
    delay_us(5);//delay_us(timer, 5);
    bit = pdev->read(pdev);//bit = GPIO_ReadInputDataBit(gpio, pin);

    // Wait until end of timeslot (60 us) + 5 us for recovery
    delay_us(55);//delay_us(timer, 55);

    if (bit == Bit_SET) {
        return true;
    } else {
        return false;
    }
}


void one_wire_write_byte(uint8_t data) {
    uint8_t i;
    const DS18B20_DEV * pdev = &tgdev;
    for (i = 0; i < 8; ++i) {
        if ((data >> i) & 1) {
            ds18b20_write_1(pdev);
        } else {
            ds18b20_write_0(pdev);
        }
    }
}

uint8_t one_wire_read_byte(void) {
    uint8_t i;
    uint8_t data = 0;
    uint8_t bit;
    const DS18B20_DEV * pdev = &tgdev;
    for (i = 0; i < 8; ++i) {
        bit = ds18b20_read_bit(pdev);
        data |= bit << i;
    }
    return data;
}



/*
	return true, has ds18b20 dev
		   false no ds18b20 dev
*/
bool one_wire_reset_pulse(void) 
{
	const DS18B20_DEV * pdev = &tgdev;
	BitAction bit;
	// Pull bus down for 500 us (min. 480 us)
	pdev->set_low(pdev);	//GPIO_ResetBits(gpio, pin);
	delay_us(500);			//delay_us(timer, 500);
	pdev->set_high(pdev);//GPIO_SetBits(gpio, pin);

	// Wait 70 us, bus should be pulled up by resistor and then
	// pulled down by slave (15-60 us after detecting rising edge)
	delay_us(70);//delay_us(timer, 70);
	bit = pdev->read(pdev);//BitAction bit = GPIO_ReadInputDataBit(gpio, pin);
	
	if (bit == Bit_RESET) {
	} else {
	    return false;
	}

	// Wait additional 430 us until slave keeps bus down (total 500 us, min. 480 us)
	delay_us(430);//delay_us(timer, 430);
	return true;
}



void ds18b20_init(void)
{
	ds18b20_dev_init(&tgdev);
}


