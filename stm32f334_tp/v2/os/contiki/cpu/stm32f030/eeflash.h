#ifndef _EEFLASH_H
#define _EEFLASH_H


#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x0800F800)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x0800FC00)   /* End @ of user Flash area */

#define FLASH_PAGE_OFFSET_MASK  (FLASH_PAGE_SIZE-1)
#define FLASH_PAGE_MASK  		(0xffffffff-FLASH_PAGE_OFFSET_MASK)


#define DATA_32                 ((uint32_t)0x12345678)


int eeWriteMultiWord(u_long dwiAddr, const u_long *pciSrc, u_short wiDataLen);
u_long eeBlockRead(u_long dwAbsAddr,void*poBuf,u_long dwiSize);

#endif
