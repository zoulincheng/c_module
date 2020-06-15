#ifndef CONTIKI_CONF_H_CDBB4VIH3I__
#define CONTIKI_CONF_H_CDBB4VIH3I__

#include <stdint.h>

#define CCIF
#define CLIF

#define WITH_ASCII 1

#define CLOCK_CONF_SECOND 1000

/* These names are deprecated, use C99 names. */
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;

typedef unsigned int clock_time_t;
typedef unsigned int uip_stats_t;

typedef uint32_t rtimer_clock_t;

#define RTIMER_CLOCK_LT(a,b)     ((int32_t)((a)-(b)) < 0)

#ifndef BV
#define BV(x) (1<<(x))
#endif





/* Prefix for relocation sections in ELF files */
#define CC_BYTE_ALIGNED __attribute__ ((packed, aligned(1)))

//#define RAND_MAX 0x7fff
#endif /* CONTIKI_CONF_H_CDBB4VIH3I__ */




