#include "stm32f0xx.h"
#include "basictype.h"
#include "eeflash.h"
#include "sysprintf.h"
#include <string.h>


/*
* \ brief  check a block flash is needed to erase.
*
* \ param dwiAddr The specify address in the flash between
*         BANK_WRITE_END_ADDR to BANK_WRITE_START_ADDR
* \ param wiSize
*
* \ return 
*          -2 the block flash need to erase,if we write data to the flash,
*           0 can write data to the block flash.
*/
static int eeCheckBlockFlash(u_long dwiAddr, u_short wiSize)
{
//    u_int nResult;
    u_short i = 0;

    for (i = 0; i < wiSize; i++)
    {
        if (((u_long*)dwiAddr)[i] != 0xffffffff)
            return -2;/*Flag This block in the flash is needed to erase.*/
    }

    return 0;
}




#if 0
/*
* \ brief  write multi  word data to the flash block
*
* \ param dwiAddr save the data in the flash,The specify address in the flash between,
*         BANK_WRITE_END_ADDR to BANK_WRITE_START_ADDR.
*
* \ param pciSrc pointer to the data need to save.
* \ param wiDataLen read number of data.
*
* \ return -1 addr error, 
*          -2 addr num is odd.
*          0 write success.
*/
int eeWriteMultiWord(u_long dwiAddr, const u_long *pciSrc, u_short wiDataLen)
{
    u_short i = 0;
    u_long *pDestAddr = (u_long*)dwiAddr;
    int nResult;

    /*The addr is over the specify page.*/
    if ((dwiAddr > FLASH_USER_END_ADDR) || (dwiAddr< FLASH_USER_START_ADDR))
    {
        XPRINTF((8,"eeWriteMultiWord:Out of range\r\n"));
        return -1;
    }
    
    /*The addr(dwiAddr) is a odd num, write data here will be made a fault.*/
    if (dwiAddr % 2 != 0)
    {
        return -2;   
    }
    
    /*Check block Flash is needed to erase.*/
    nResult = eeCheckBlockFlash(dwiAddr, wiDataLen);
    FLASH_Unlock( );
      /* Clear pending flags (if any) */  
 	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    if (nResult == -2)
    {
        FLASH_ErasePage((u_long)pDestAddr);
    }
    
    for (i = 0; i < wiDataLen; i++)
    {
        FLASH_ProgramWord((u_long)pDestAddr, *pciSrc);
        pDestAddr++;
        pciSrc++;
    }
    FLASH_Lock( );
    return 0;
}
#else

/*
* \ brief  write multi  word data to the flash block
*
* \ param dwiAddr save the data in the flash,The specify address in the flash between,
*         BANK_WRITE_END_ADDR to BANK_WRITE_START_ADDR.
*
* \ param pciSrc pointer to the data need to save.
* \ param wiDataLen read number of data.
*
* \ return -1 addr error, 
*          -2 addr num is odd.
*          0 write success.
*/
int eeWriteMultiWord(u_long dwiAddr, const u_long *pciSrc, u_short wiDataLen)
{
    u_short i = 0;
    int nResult;

    /*The addr is over the specify page.*/
    if ((dwiAddr > FLASH_USER_END_ADDR) || (dwiAddr< FLASH_USER_START_ADDR))
    {
        XPRINTF((8,"eeWriteMultiWord:Out of range\r\n"));
        return -1;
    }
    
    /*The addr(dwiAddr) is a odd num, write data here will be made a fault.*/
    if (dwiAddr % 2 != 0)
    {
        return -2;   
    }
    
    /*Check block Flash is needed to erase.*/
    FLASH_Unlock( );
    /* Clear pending flags (if any) */  
 	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 
    FLASH_ErasePage(dwiAddr);
    
    for (i = 0; i < wiDataLen; i++)
    {
        FLASH_ProgramWord((u_long)dwiAddr, *pciSrc);
        dwiAddr += 4;
        pciSrc++;
    }
    FLASH_Lock( );
    return 0;
}
#endif


u_long eeBlockRead(u_long dwAbsAddr,void*poBuf,u_long dwiSize)
{
	u_long dwLength = 0;
	if(poBuf)
	{
		memcpy(poBuf,(void*)dwAbsAddr,dwiSize);
		dwLength = dwiSize;
	}
	return dwLength;
}


static u_char ubaPbuf[FLASH_PAGE_SIZE];//For copy data
u_long eeBlockWrite(u_long dwAbsAddr,const void*pciBuf,u_long dwiSize)
{
	u_long dwPageStart;
	u_long dwPageEnd;
	u_long dwPageOffsetStart;
	u_long dwPageOffsetEnd;
	u_long dwWriteSize;
	u_char *pbkBuf = NULL;
	
	//pbkBuf = (u_char*)malloc(FLASH_PAGE_SIZE);
	pbkBuf = (u_char*)&ubaPbuf[0];
	if(pbkBuf == NULL)
		return 0;

	//NutFeedWatchdog();
	
	dwPageStart	= dwAbsAddr&FLASH_PAGE_MASK;
	dwPageEnd	= (dwAbsAddr+dwiSize)&FLASH_PAGE_MASK;
	
	dwWriteSize = 0;
	
	for(;dwPageStart <= dwPageEnd; dwPageStart += FLASH_PAGE_SIZE)
	{
		u_long dwPageWriteSize;
		
		if(dwPageStart == dwPageEnd)
			dwPageOffsetEnd = (dwAbsAddr+dwiSize)&FLASH_PAGE_OFFSET_MASK;
		else
			dwPageOffsetEnd =  FLASH_PAGE_SIZE;

		if(dwPageStart == (dwAbsAddr&FLASH_PAGE_MASK))
			dwPageOffsetStart = dwAbsAddr&FLASH_PAGE_OFFSET_MASK;
		else
			dwPageOffsetStart = 0;

		eeBlockRead(dwPageStart,pbkBuf,FLASH_PAGE_SIZE);
		
		dwPageWriteSize = dwPageOffsetEnd - dwPageOffsetStart;
		
		if(memcmp(&pbkBuf[dwPageOffsetStart],&((u_char*)pciBuf)[dwWriteSize],dwPageWriteSize) != 0)
		{
			memcpy(&pbkBuf[dwPageOffsetStart],&((u_char*)pciBuf)[dwWriteSize],dwPageWriteSize);
			if(eeWriteMultiWord(dwPageStart,(u_long *) pbkBuf, FLASH_PAGE_SIZE/sizeof(u_long)))
				break;
		}

		dwWriteSize += dwPageOffsetEnd - dwPageOffsetStart;
	}
	
	//free(pbkBuf);
	pbkBuf = NULL;
	
	return dwWriteSize;
}

/*

                        FLASH_Unlock();

                        FLASH_OB_Unlock();

                        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

                        FLASH_OB_RDPConfig(OB_RDP_Level_1);
                        FLASH_OB_Lock();
*/
void eeRDPSet(void)
{
	if ( (u_char)OB->RDP != OB_RDP_Level_1)
	{
		//FLASH_OB_Lock();
		FLASH_Status status;
		FLASH_Unlock();
		FLASH_OB_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
		status = FLASH_OB_RDPConfig(OB_RDP_Level_1);
		XPRINTF((0, "status=%d\r\n", status));
		FLASH_OB_Lock();
	}
}

void eeRDBReset(void)
{
	FLASH_Unlock();
	FLASH_OB_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	FLASH_OB_RDPConfig(OB_RDP_Level_0);
	FLASH_OB_Lock();
}

void eePrintObrdp(void)
{
	XPRINTF((0, "rdp=%04x\r\n", OB->RDP));
}


