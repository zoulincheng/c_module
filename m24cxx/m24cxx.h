#ifndef _M24CXX_H
#define _M24CXX_H

//------------------------------------------------------------
//Configurations
#define EEPROM_DEVICE 1
				// 0 AT24C01
				// 1 AT24C02
				// 2 AT24C04
				// 3 AT24C08
				// 4 AT24C16
				// 5 AT24C32

//------------------------------------------------------------

#define MAX_EEPROM_SIZE     (128*(1<<EEPROM_DEVICE))
#define EEPROM_PAGE_SIZE   ((EEPROM_DEVICE<=1) ? 8 : ((EEPROM_DEVICE<=4) ? 16 :32))

#define EEPROM_SLAW 0xa0
#define I2C_PA      0x00

#if (EEPROM_DEVICE<=1)
	#define EEPROM_PAGE_MASK  0x00
	#define EEPROM_PA  (I2C_PA<<1)  //pin address
#elif (EEPROM_DEVICE==2)
	#define EEPROM_PAGE_MASK  0x01
	#define EEPROM_PA  (I2C_PA<<1)  //pin address
#elif (EEPROM_DEVICE==3)
	#define EEPROM_PAGE_MASK  0x03
	#define EEPROM_PA  (I2C_PA<<1)  //pin address
#elif (EEPROM_DEVICE==4)
	#define EEPROM_PAGE_MASK  0x07
	#define EEPROM_PA  (I2C_PA<<1)  //pin address
#elif (EEPROM_DEVICE==5)
	#define EEPROM_PAGE_MASK  0x0f
	#define EEPROM_PA  (I2C_PA<<1)  //pin address
#else
	#error "DEVICE defined is not supported!"
#endif



u_short at24cxx_write(u_short addr,const void*pData,u_short wLength);
u_short at24cxx_read(u_short addr,void *pData,u_short wLength);

bool at24cxx_write1byte(u_short addr,u_char dat);

u_char at24cxx_read1byte(u_short addr);



#endif
