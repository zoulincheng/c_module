#ifndef LV_CONF_H
#define LV_CONF_H
/* clang-format off */

#include <stdint.h>

/*=========================
   Memory manager settings
 *=========================*/

/* LittelvGL's internal memory manager's settings.
 * The graphical objects and other related data are stored here. */

/* 1: use custom malloc/free, 0: use the built-in `lv_mem_alloc` and `lv_mem_free` */
#define LV_MEM_CUSTOM      0

#if LV_MEM_CUSTOM == 0
/* Size of the memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#define LV_MEM_SIZE    (2U * 1024U)

/* Complier prefix for a big array declaration */
#define LV_MEM_ATTR

/* Set an address for the memory pool instead of allocating it as an array.
 * Can be in external SRAM too. */
#define LV_MEM_ADR          0

/* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
#define LV_MEM_AUTO_DEFRAG  1

#else       /*LV_MEM_CUSTOM*/
#  define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#  define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#  define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif     /*LV_MEM_CUSTOM*/


/*for log define */
#define LV_USE_LOG		1
#define LV_LOG_PRINTF	1
#define LV_LOG_LEVEL	0

#endif
