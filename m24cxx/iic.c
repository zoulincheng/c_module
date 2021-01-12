#include "iic.h"
#include "basictype.h"
#include "iodef.h"
#include "sysprintf.h"



#define IO_SET_PIN(port, pin, a)			_IO_SET(port, pin, a)
#define IO_READ_PIN(GPIOx, GPIO_Pin)		_IO_READ(GPIOx, GPIO_Pin)

#if 1
void iicSdaPinOut(const IIC_DEV *pdev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = pdev->SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(pdev->SDA_PORT, &GPIO_InitStructure);
}

void iicSdaPinIn(const IIC_DEV *pdev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = pdev->SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(pdev->SDA_PORT, &GPIO_InitStructure);
}

void iicSclPinOut(const IIC_DEV *pdev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = pdev->SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(pdev->SDA_PORT, &GPIO_InitStructure);
}

void iicSdaH(const IIC_DEV *pdev)
{
	IO_SET_PIN((pdev->SDA_PORT), (pdev->SDA_PIN), 1);
}

/*
v must be 1 or 0
*/
void iicSdaSet(const IIC_DEV *pdev, u_char v)
{
	IO_SET_PIN((pdev->SDA_PORT), (pdev->SDA_PIN), v);
}

void iicSdaL(const IIC_DEV *pdev)
{
	IO_SET_PIN((pdev->SDA_PORT), (pdev->SDA_PIN), 0);
}


void iicSclH(const IIC_DEV *pdev)
{
	IO_SET_PIN((pdev->SCL_PORT), (pdev->SCL_PIN), 1);
}

void iicSclL(const IIC_DEV *pdev)
{
	IO_SET_PIN((pdev->SCL_PORT), (pdev->SCL_PIN), 0);
}


void iicSclSet(const IIC_DEV *pdev, u_char v)
{
	IO_SET_PIN((pdev->SCL_PORT), (pdev->SCL_PIN), v);
}

uint8_t iicSdaRead(const IIC_DEV *pdev)
{
	uint8_t data;

	data = IO_READ_PIN(pdev->SDA_PORT, pdev->SDA_PIN);

	return data;
}

void iicInit(const IIC_DEV *pdev)
{
	iicSdaPinOut(pdev);
	iicSclPinOut(pdev);
	//iicSdaH(pdev);
	iicSdaSet(pdev, 1);
	delay_us(2);
	//iicSclH(pdev);
	iicSclSet(pdev, 1);
	delay_us(2);
}

void iicStart(const IIC_DEV *pdev)
{
	iicSdaPinOut(pdev);
	delay_us(1);
	iicSdaSet(pdev, 1);
	iicSclSet(pdev, 1);
	delay_us(2);
	iicSdaSet(pdev, 0);
	delay_us(2);
	iicSclSet(pdev, 0);
	delay_us(2);
}

void iicStop(const IIC_DEV *pdev)
{
	iicSclSet(pdev, 0);
	delay_us(1);
	iicSdaPinOut(pdev);
	iicSdaSet(pdev, 0);
	delay_us(2);
	iicSclSet(pdev, 1);
	delay_us(2);
	iicSdaSet(pdev, 1);
	delay_us(2);
}

u_char iicGetAck(const IIC_DEV *pdev)
{
	u_long dwDelay = 0;
	u_char bAck = 0;

	iicSdaSet(pdev, 1);
	iicSdaPinIn(pdev);
	delay_us(1);
	iicSclSet(pdev, 1);
	delay_us(2);
	while(iicSdaRead(pdev) && dwDelay < 10)
	{
		delay_us(2);
		dwDelay++;
	}

	bAck = dwDelay<10;

	if (dwDelay < 1)
		delay_us(2);
	iicSclSet(pdev,0);
	delay_us(2);
	iicSdaPinOut(pdev);
	return bAck;
}


void iicSendAck(const IIC_DEV *pdev)
{
	iicSdaSet(pdev, 0);
	delay_us(1);
	iicSclSet(pdev, 1);
	delay_us(2);
	iicSclSet(pdev, 0);
	delay_us(1);
	iicSdaSet(pdev, 1);
	delay_us(2);
}

void iicSendNak(const IIC_DEV *pdev)
{
	iicSdaSet(pdev, 1);
	delay_us(1);
	iicSclSet(pdev, 1);
	delay_us(2);
	iicSclSet(pdev, 0);
	delay_us(2);
}


void iicSend8Bits(const IIC_DEV *pdev,u_char dat)
{
	u_char i;

	for (i=0; i<8; i++,dat<<=1)
	{
		iicSdaSet(pdev,dat&0x80);
		delay_us(1);
		iicSclSet(pdev, 1);
		delay_us(2);
		iicSclSet(pdev, 0);
		delay_us(2);
	}

	iicSdaSet(pdev, 1);//release bus line
}

//I2C_SENDBYTE_ACK(dat)                
void iicSendByteAck(const IIC_DEV *pdev, u_char dat)
{                                        
        //I2C_Send8Bits(bModNum,(u_char)(dat));
        //if(!I2C_GetACK(bModNum))             
        //    break;
	iicSend8Bits(pdev, dat);
	if (!iicGetAck(pdev))
		return;
}

u_char iicGet8Bits(const IIC_DEV *pdev)
{
	u_char i;
	u_char dat;

	iicSdaPinIn(pdev);
	iicSdaSet(pdev, 1);

	for (dat = 0, i = 0; i < 8; i++)
	{
		dat <<=1;
		delay_us(2);
		iicSclSet(pdev, 1);
		delay_us(2);
		if (iicSdaRead(pdev))
			dat |= 0x01;
		delay_us(2);
		iicSclSet(pdev, 0);
		delay_us(2);
	}
	delay_us(2);
	iicSdaPinOut(pdev);

	return dat;
}
#else


#endif



