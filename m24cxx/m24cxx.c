
#include "iic.h"
#include "contiki.h"
#include "basictype.h"
#include "sysprintf.h"
#include "iodef.h"
#include "m24cxx.h"


static const IIC_DEV m24c02w={
/*
	PCT2075_SCL_PORT, 
	PCT2075_SDA_PORT, 
	PCT2075_SCL_PIN, 
	PCT2075_SDA_PIN
*/
	IIC_SCL_PORT,//M24C02W_SCL_PORT,
	IIC_SDA_PORT,//M24C02W_SDA_PORT, 
	IIC_SCL_PIN,//M24C02W_SCL_PIN, 
	IIC_SDA_PIN,//M24C02W_SDA_PIN

	//.SCL_PORT = PCT2075_SCL_PORT,
	//.SCL_PIN = PCT2075_SCL_PIN,
	//.SDA_PORT = PCT2075_SDA_PORT,
	//.SDA_PIN = PCT2075_SDA_PIN,
};



#define PERFORMANCE_ANALYSIS 0
void iicCheckBusy(u_char nOpt)
{
	const IIC_DEV *pdev = &m24c02w;
#if 0
    static u_long dwLastWritingTime = -10;
    u_long dwCurrTime  = sysGetSysTick(); // mil-second
    if(nOpt){
        dwLastWritingTime = dwCurrTime;
    }else if( (dwCurrTime-dwLastWritingTime) < 5 ){
        NutSleep(5-(dwCurrTime-dwLastWritingTime));
    }
#else
    if(nOpt){
        u_char slaw;
#if(PERFORMANCE_ANALYSIS)        
        u_long dwTst = sysGetSysTick();
#endif
        while(1){
            //I2C_Start(bModNum);
            iicStart(pdev);
            slaw = EEPROM_SLAW | EEPROM_PA;
            //I2C_Send8Bits(bModNum,(u_char)(slaw));
            iicSend8Bits(pdev, slaw);
            //if(I2C_GetACK(bModNum)) 
            if(iicGetAck(pdev))
                break;
            //NutSleep(0);
        }
        //I2C_Start(bModNum);
        //I2C_Stop(bModNum);
        iicStart(pdev);
        iicStop(pdev);
#if(PERFORMANCE_ANALYSIS)        
        PRINTF((10,"time consumption of writting cycle%u ms\n",sysGetSysTick()-dwTst));
#endif
    }
#endif
}



/*
 *\brief Write a page of 24CXX
 *
 *\param bModNum, module object index
 *\param addr, internal starting address to be accessed
 *\param pData, data pointer to be written
 *\param wLength, byte size of data
 *
 * return  byte count of writting
 */
//static u_char at24cxx_writepage(u_char bModNum,u_short addr,u_char const*pData,u_short wLength)
static u_char at24cxx_writepage(u_short addr,u_char const*pData,u_short wLength)
{
	u_char slaw;
	u_char bResult = 0;
	const IIC_DEV *pdev = &m24c02w;

    if(pData==NULL || wLength == 0)
        return 0;
        
    //I2C_Start(bModNum);
    iicStart(pdev);
    do{
        //;--SLAW=1010'0000B
#if(EEPROM_DEVICE <= 1)//AT24C01/02
        slaw = EEPROM_SLAW | EEPROM_PA;
#elif    (EEPROM_DEVICE <= 4)//AT24C04/08/16
        slaw = ((addr>>7)&(EEPROM_PAGE_MASK<<1)) | EEPROM_SLAW | EEPROM_PA;
#else//AT24C32
        slaw = EEPROM_SLAW | EEPROM_PA;//24c32 A1 is high
        //I2C_SENDBYTE_ACK(slaw);
        iicSendByteAck(pdev, slaw);
        slaw = (addr>>8)&EEPROM_PAGE_MASK;
#endif 
        //I2C_SENDBYTE_ACK(slaw);
        //I2C_SENDBYTE_ACK(addr);//SEND INTERNAL ADDRESS
        iicSendByteAck(pdev, slaw);
        iicSendByteAck(pdev, addr);

        while(wLength){
            //I2C_SENDBYTE_ACK(*pData++);//SEND DATA
            iicSendByteAck(pdev, *pData++);
            wLength--;
            //watch_dog_feed( );
        }
        //watch_dog_feed( );
        bResult = (wLength==0);
    }while(0);
    
    //I2C_Stop(bModNum);
    iicStop(pdev);
    
    if(bResult){
        //I2C_CheckBusy(bModNum,5);
        iicCheckBusy( 5);
    }
    return bResult;
}



/*
 *\brief Initialize the I2C port
 *
 *\param none
 *
 * return  none
 */
void at24cxx_PortInit(void)
{
	const IIC_DEV *pdev = &m24c02w;
	iicInit(pdev);
}



/*
 *\brief Write a block to 24CXX
 *
 *\param bModNum, module object index
 *\param addr, internal starting address to be accessed
 *\param pData, data pointer to be written
 *\param wLength, byte size of data
 *
 * return  byte count of writting
 */
//u_short at24cxx_write(u_char bModNum,u_short addr,const void *pData,u_short wLength)
u_short at24cxx_write(u_short addr,const void *pData,u_short wLength)
{
	u_short wLen = 0;
	u_short endAddr;
	const IIC_DEV *pdev = &m24c02w;
    const u_char *pDat = (u_char*)pData;
    
#if(PERFORMANCE_ANALYSIS)        
    u_long dwTst = sysGetSysTick();
#endif    
    if(pData == NULL || wLength == 0 || addr>=MAX_EEPROM_SIZE)
        return 0;
    
    endAddr = addr + wLength;
    if(endAddr > MAX_EEPROM_SIZE)
        endAddr = MAX_EEPROM_SIZE;

    for(; addr < endAddr; addr += wLen){
		u_short wLenLeft;

        wLenLeft = endAddr - addr;
        wLen = addr & (EEPROM_PAGE_SIZE-1);
        
        if(wLen){
            wLen = EEPROM_PAGE_SIZE - wLen;
            if(wLenLeft < wLen) 
                wLen = wLenLeft;
        }else{
            wLen = (wLenLeft >= EEPROM_PAGE_SIZE) ? EEPROM_PAGE_SIZE : wLenLeft;
        }
        
        //I2C_CheckBusy(bModNum,0);
        iicCheckBusy(0);
        //watch_dog_feed( );
        if(!at24cxx_writepage(addr, pDat, wLen))
            break;
        pDat += wLen;
    }
    wLen = (u_short)((u_long)pDat-(u_long)pData);
#if(PERFORMANCE_ANALYSIS)        
    XPRINTF((10,"wirtes %u bytes, time consumpted %u ms\n",wLen,sysGetSysTick()-dwTst));
#endif
    return wLen;
}


/*
 *\brief Read  a block from 24CXX
 *
 *\param bModNum, module object index
 *\param addr, internal starting address to be accessed
 *\param pData, data pointer to be written
 *\param wLength, byte size of data
 *
 * return  byte count of received
 */
//u_short at24cxx_read(u_char bModNum,u_short addr,void *pData,u_short wLength)
u_short at24cxx_read(u_short addr,void *pData,u_short wLength)
{
    u_char slaw;
    u_char *pOut = (u_char*)pData;
    const IIC_DEV *pdev = &m24c02w;

    if(pData == NULL || wLength == 0 || addr>=MAX_EEPROM_SIZE)
        return 0;

    if((addr+wLength)>MAX_EEPROM_SIZE){
        wLength = MAX_EEPROM_SIZE-addr;
    }
    
    //I2C_CheckBusy(bModNum,0);
    iicCheckBusy(0);

    //////////////////////////////////////////////////////////
    //I2C_Start(bModNum);
    iicStart(pdev);
    do{
        //;--SLAW=1010'0000B
#if(EEPROM_DEVICE <= 1)//AT24C01/02
        slaw = EEPROM_SLAW | EEPROM_PA;
        //I2C_SENDBYTE_ACK(slaw);
        //I2C_SENDBYTE_ACK(addr);
        iicSendByteAck(pdev, slaw);
        iicSendByteAck(pdev, addr);
#elif (EEPROM_DEVICE <= 4)//AT24C04/08/16
        slaw = ((addr>>7)&(EEPROM_PAGE_MASK<<1)) | EEPROM_SLAW | EEPROM_PA;
        //I2C_SENDBYTE_ACK(slaw);
        //I2C_SENDBYTE_ACK(addr);
        iicSendByteAck(pdev, slaw);
        iicSendByteAck(pdev, addr);
#else//AT24C32
        slaw = EEPROM_SLAW | EEPROM_PA;//24c32 A2..0 connect with VCC
        //I2C_SENDBYTE_ACK(slaw);
        //I2C_SENDBYTE_ACK(addr>>8);
        //I2C_SENDBYTE_ACK(addr);
        iicSendByteAck(pdev, slaw);
        iicSendByteAck(pdev, addr>>8);
        iicSendByteAck(pdev, addr);
#endif 

        //I2C_Start(bModNum);
        iicStart(pdev);

        slaw |= 0x01;//READ FLAG
        //I2C_SENDBYTE_ACK(slaw);
        iicSendByteAck(pdev, slaw);
        
        while(--wLength){
            //*pOut++ = I2C_Get8Bits(bModNum);
            //I2C_SendACK(bModNum);//ACK
            *pOut++ = iicGet8Bits(pdev);
            iicSendAck(pdev);//ACK
        }
        //*pOut++ = I2C_Get8Bits(bModNum);
        //I2C_SendNAK(bModNum);//NACK
        *pOut++ = iicGet8Bits(pdev);
        iicSendNak(pdev);//NACK
        
    }while(0);

    //I2C_Stop(bModNum);
    iicStop(pdev);
    
    wLength = (u_short)((u_long)pOut-(u_long)pData);
    
    return wLength;
}



/*
 *\brief Write a byte to 24CXX
 *
 *\param bModNum, module object index
 *\param addr, internal starting address to be accessed
 *\param dat,  byte to be written
 *
 * return  1 for succeds
 */
//BOOL at24cxx_write1byte(u_char bModNum,u_short addr,u_char dat)
bool at24cxx_write1byte(u_short addr,u_char dat)
{
	//const IIC_DEV *pdev = &m24c02w;

      return at24cxx_write(addr,&dat,1);
}


/*
 *\brief Read a byte from 24CXX
 *
 *\param bModNum, module object index
 *\param addr, internal starting address to be accessed
 *\param dat,  byte to be written
 *
 * return  byte of reading
 */
//u_char at24cxx_read1byte(u_char bModNum,u_short addr)
u_char at24cxx_read1byte(u_short addr)
{
    u_char dat = 0xff;
//	const IIC_DEV *pdev = &m24c02w;
    
    at24cxx_read(addr,&dat,1);
    return dat;
}


//test data  and code
const u_char test1[]={0x11, 0x00, 0x12, 0xaa, 0xbb, 0xcc,0xdd, 0x33, 0xee, 0xff,0x99};
const u_char test2[]={0x12, 0x01, 0x13, 0xa1, 0xb1, 0xc1,0xd1,0x31, 0xe1, 0xf1,0x91};
void at24xtest1(void)
{
	u_char buf[16] = {0x00};
	at24cxx_write(0, test1, sizeof(test1));
	MEM_DUMP(0, "TESW", test1, sizeof(test1));
	at24cxx_read(0x00,buf,sizeof(test1));
	MEM_DUMP(0, "TESR", buf, sizeof(test1));
}
/*
void at24xtest2(void)
{
	u_char buf[16] = {0x00};
	at24cxx_write(0, test2, sizeof(test2));
	MEM_DUMP(0, "TESW", test2, sizeof(test2));
	at24cxx_read(0x00,buf,sizeof(test2));
	MEM_DUMP(0, "TESR", buf, sizeof(test2));
}
*/

