#include "stm32f0xx.h"
#include "basictype.h"
#include "iodef.h"
#include "arch_spi.h"
#include "arch_time.h"

#define SPI_DATASIZE_8


/* Define data size and data masks */
#ifdef SPI_DATASIZE_8
 #define SPI_DATASIZE                     SPI_DataSize_8b
 #define SPI_DATAMASK                     (uint8_t)0xFF
#endif

#ifdef	HARDSPI
//hardware spi
//config interface between MCU and si4432
#define SPI_WAIT_TIME 	10000
void spiConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
  	/* Enable the SPI periph */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	

	GPIO_PinAFConfig(SPI_PORT, GPIO_PinSource5, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_PORT, GPIO_PinSource6, GPIO_AF_0);
	GPIO_PinAFConfig(SPI_PORT, GPIO_PinSource7, GPIO_AF_0);  
  	//GPIO_PinAFConfig(SPI_PORT, GPIO_PinSource4, GPIO_AF_0);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPI_MOSI_PIN;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);	

	/* SPI MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
	/* SPI NSS pin configuration */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = SPI_NSS_PIN;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Hard
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

  	/* Initialize the FIFO threshold */
  	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE); /* SD_SPI enable */
}

//send a byte data
/**
  * @brief  Sends a byte through the SPI interface and return the byte received 
  *         from the SPI bus.
  * @param  Data: byte send.
  * @retval The received byte value
  * @retval None
  */
u_char spiByteExchange(u_char ubData)
{
	uint8_t tmp = 0x00;

	/* Wait until the transmit buffer is empty */ 
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET)
	{
	}  
	/* Send the byte */
	SPI_SendData8(SPI1, ubData);

	/* Wait to receive a byte */ 
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET)
	{
	}
	/* Return the byte read from the SPI bus */    
	tmp = SPI_ReceiveData8(SPI1);

	/* Return read Data */
	return tmp;	
}
#endif


#ifdef	SOFTSPI

void spiConfig(void)
{
	GPIO_InitTypeDef stGpioInit;

	//spi pin init
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = SPI_NSS_PIN | SPI_SCK_PIN | SPI_MOSI_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(SPI_PORT, &stGpioInit);

	//TEST NSS PIN
	#if 0
	stGpioInit.GPIO_Mode = GPIO_Mode_IN;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = SPI_NSS_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_DOWN;
	stGpioInit.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(SPI_PORT, &stGpioInit);
	#endif
	stGpioInit.GPIO_Mode = GPIO_Mode_IN;
	stGpioInit.GPIO_Pin = SPI_MISO_PIN;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SPI_PORT, &stGpioInit);
	
}
static void rndelay(u_short us)
{
	while(us--);
};

//send a byte data
u_char spiByteExchange(u_char byte)
{
	u_char i = 0;  
	u_char bit_r = 0; 
	
	for(i=0;i<8;i++)   // output 8-bit  
	{   

		SPI_SCK_SET(1);
		SPI_SCK_SET(1);
		SPI_SCK_SET(1);
		rndelay(10);

		//MOSI_PIN=byte & 0x80;     //output 'byte' MSB to MOSI_PIN  
		if(byte & 0x80)
		{
			SPI_MOSI_SET(1);
			SPI_MOSI_SET(1);
		}
		else
		{
			SPI_MOSI_SET(0);
			SPI_MOSI_SET(0);
		}
		rndelay(10);

		byte <<= 1;                 // shift next bit into MSB..  

		//SPI_SCK_SET(1);
		//SPI_SCK_SET(1);
		//SPI_SCK_SET(1);
		
		bit_r<<=1;  
		if(SPI_MISO()) bit_r++;  

		SPI_SCK_SET(0);
		SPI_SCK_SET(0);
		SPI_SCK_SET(0);
		rndelay(10);
	}  

	//rndelay(10);
	return(bit_r);   	
}

void spiWriteByte(u8 Dat)
{
	u8 i;
	
	for(i=0;i<8;i++)
	{
		//PinWrite_SCK2(1);
		//PinWrite_SCK2(1);
		//fnDelay2us();
		//fnDelay2us();
		SPI_SCK_SET(1);
		SPI_SCK_SET(1);
		delayUs(2);
		delayUs(2);
		
		if(Dat & 0x80)
		{
			//PinWrite_MOSI2(1);
			//PinWrite_MOSI2(1);
			//fnDelay2us();
			//fnDelay2us();
			//fnDelay2us();
			SPI_MOSI_SET(1);
			SPI_MOSI_SET(1);
			delayUs(2);
			delayUs(2);
			delayUs(2);
		}
		else
		{
			//PinWrite_MOSI2(0);
			//PinWrite_MOSI2(0);
			//fnDelay2us();
			//fnDelay2us();
			//fnDelay2us();
			SPI_MOSI_SET(0);
			SPI_MOSI_SET(0);
			delayUs(2);
			delayUs(2);
			delayUs(2);			
		}
		//PinWrite_SCK2(0);
		//PinWrite_SCK2(0);
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		SPI_SCK_SET(0);
		SPI_SCK_SET(0);
		delayUs(2);
		delayUs(2);
		delayUs(2);
		Dat <<= 1;
	}
	//PinWrite_MOSI2(1);
	//PinWrite_MOSI2(1);
	//fnDelay2us();
	//fnDelay2us();
	//fnDelay2us();
	SPI_MOSI_SET(1);
	SPI_MOSI_SET(1);
	delayUs(2);
	delayUs(2);
	delayUs(2);
}

u8 spiReadByte(void)
{
    u8 i,B=0;
    for(i=0;i<8;i++)
    {
    	B<<=1;
    	//PinWrite_SCK2(1);
		//PinWrite_SCK2(1);
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		SPI_SCK_SET(1);
		SPI_SCK_SET(1);
		delayUs(2);
		delayUs(2);
		delayUs(2);
		delayUs(2);
		
		//PinWrite_SCK2(0);
		//PinWrite_SCK2(0);
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		SPI_SCK_SET(1);
		SPI_SCK_SET(1);
		delayUs(2);
		delayUs(2);
		delayUs(2);
		delayUs(2);
		
		//if (PinRead_MISO2())
		if (SPI_MISO())
		{
			B=B+0x1;
		}
		//fnDelay2us();
		//fnDelay2us();
		//fnDelay2us();
		delayUs(2);
		delayUs(2);
		delayUs(2);
    }
    return	B;
}




#endif

