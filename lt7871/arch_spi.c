#include "stm32f30x.h"

#include "basictype.h"
#include "iodef.h"
#include "arch_spi.h"
//#include "arch_time.h"

#include "dwt_delay.h"

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



/*
* 使用SysTick的CVR实现微秒级精确延时，一般SysTick周期设置为10MS，因此该方法适用于10MS以内的延时
*/
void delay_us(int us) 
{
    unsigned t1, t2, count, delta, sysclk;
    sysclk = 72;//假设为48MHz主频

    t1 = SysTick->VAL;
    while (1) {
        t2 = SysTick->VAL;
        delta = t2 < t1 ? (t1 - t2) : (SysTick->LOAD - t2 + t1) ;
        if (delta >= us * sysclk)
            break;
    }
}



#ifdef	SOFTSPI

void spi_cfg(void)
{
	GPIO_InitTypeDef stGpioInit;

	//spi pin init
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_Pin = SPI_NSS_PIN | SPI_CLK_PIN | SPI_MOSI_PIN;
	//stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_CLK_PORT, &stGpioInit);

	stGpioInit.GPIO_Mode = GPIO_Mode_IN;
	stGpioInit.GPIO_Pin = SPI_MISO_PIN;
	//stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SPI_MISO_PORT, &stGpioInit);
}


//delayUs(int nUs)
/*
//send a byte data
u_char spi_byte_exchange(u_char byte)
{
	u_char i = 0;  
	u_char bit_r = 0; 
	
	for(i=0;i<8;i++)   // output 8-bit  
	{   

		SPI_CLK_SET(1);

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
*/

/*
uint8_t spi_in_out( uint8_t outData )
{
	int i = 0;  
	for(i=0;i<8;i++)   // output 8-bit  
	{   
		SPI_MOSI_SET(outData & 0x80);   // output the bit7
		SPI_CLK_SET(1);
		outData <<= 1;              	// shift next bit into MSB..  
		if(SPI_MISO())                	// move the MOSI stat to bit0
    		outData|=0x01; 
		SPI_CLK_SET(0);
	}  
	return outData;
}
*/

u_char spi_in_out(u_char byte)
{
	u_char i = 0;  
	u_char bit_r = 0; 
	SPI_CLK_SET(0);
	//DWT_Delay(2);
	for(i=0;i<8;i++)   // output 8-bit  
	{   
		//MOSI_PIN=byte & 0x80;     //output 'byte' MSB to MOSI_PIN  
		if(byte & 0x80)
		{
			SPI_MOSI_SET(1);
			//SPI_MOSI_SET(1);
		}
		else
		{
			SPI_MOSI_SET(0);
			//SPI_MOSI_SET(0);
		}
		delay_us(2);//DWT_Delay(2);
		SPI_CLK_SET(1);
		byte <<= 1;                 // shift next bit into MSB..  

		delay_us(2);//DWT_Delay(2);
		bit_r<<=1;  
		if(SPI_MISO()) bit_r++;  

		SPI_CLK_SET(0);
	}  

	return(bit_r);   	
}



void spi_write(u_char byte)
{
	u_char i = 0;  
	SPI_CLK_SET(0);
	for(i=0;i<8;i++)   // output 8-bit  
	{   
		if(byte & 0x80)
		{
			SPI_MOSI_SET(1);
		}
		else
		{
			SPI_MOSI_SET(0);
		}
		delay_us(2);
		SPI_CLK_SET(1);
		byte <<= 1;                 // shift next bit into MSB..  

		delay_us(2);
		SPI_CLK_SET(0);
	}  
}

/*
void spi_write(u8 byte){
	int i;
	//xprintf("write data = %02x\r\n",byte);
	for(i=0;i<8;i++)
	{
		SPI_CLK_SET(0);
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
		//MASTAER_SDI((byte<<i)&0x80); 
		if((byte<<i) & 0x80)
		{
			SPI_MOSI_SET(1);
		}
		else
		{
			SPI_MOSI_SET(0);
		}
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
		SPI_CLK_SET(1);//MASTER_SCLK(1);
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
	}
    //MASTER_SCLK(0);
    SPI_CLK_SET(0);
}
*/

/*
u8 spi_read(void)
{
   	u8 data = 0x00;
   	int i;
    SPI_CLK_SET(0);//	MASTER_SCLK(0);
   	delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
   	for(i=0;i<8;i++)
	{
		SPI_CLK_SET(1);//MASTER_SCLK(1);
		data=data<<1;
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
		if(SPI_MISO()) data |=0x01;
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
		SPI_CLK_SET(0);//MASTER_SCLK(0);
		delay_us(2);//DWT_Delay(2);//delayMicroseconds(1);
	}
   	return data;
}
*/


u_char spi_read(void)
{
	u_char i;
	u_char data = 0;
	SPI_CLK_SET(0);
	for (i = 0; i < 8; i++)
	{
		data <<=1;
		SPI_CLK_SET(1);
		delay_us(1);
		if(SPI_MISO()) data |=0x01;
		delay_us(1);
		SPI_CLK_SET(0);
		delay_us(2);
	}

	return data;
}


#endif


