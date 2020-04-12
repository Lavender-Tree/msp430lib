#include "IIC.h"

/* IIC init */
void IIC_ioinit()
{
	P6DIR |= BIT0 | BIT5; // config to output
	P6REN |= BIT0 | BIT5; // config IIC IO enable pull-up/down resistor
}

/* start a IIC channel */
void IIC_start()
{
	SCL_HIGH;
	SDA_HIGH;
	SDA_LOW;
	SCL_LOW;
}

/* stop a IIC channel */
void IIC_stop()
{
	SCL_LOW;
	SDA_LOW;
	SCL_HIGH;
	SDA_HIGH;
}

/* IIC send one byte */
void IIC_writebyte(unsigned char IIC_byte)
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		if (IIC_byte & 0x80)
			SDA_HIGH;
		else
			SDA_LOW;
		SCL_HIGH;
		SCL_LOW;
		IIC_byte <<= 1; // loop
	}
	SDA_HIGH;
	SCL_HIGH;
	SCL_LOW;
}
