#ifndef _ARCH_SPI_H
#define _ARCH_SPI_H

#define SOFTSPI

//#define HARDSPI

#ifdef SOFTSPI
void spi_cfg(void);
uint8_t spi_in_out( uint8_t outData);
void spi_write(u_char byte);
u_char spi_read(void);

#endif

#ifdef HARDSPI
void spiConfig(void);
u_char spiByteExchange(u_char byte);
#endif


void delay_us(int us);

#endif

