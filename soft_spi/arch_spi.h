#ifndef _ARCH_SPI_H
#define _ARCH_SPI_H

#define SOFTSPI

//#define HARDSPI

#ifdef SOFTSPI
void spiConfig(void);
u_char spiByteExchange(u_char byte);
void spiWriteByte(u8 Dat);
u8 spiReadByte(void);
#endif

#ifdef HARDSPI
void spiConfig(void);
u_char spiByteExchange(u_char byte);
#endif

#endif

