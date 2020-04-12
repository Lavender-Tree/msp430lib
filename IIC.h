
#ifndef IIC_H_
#define IIC_H_

#include <msp430f5529.h>
#include <msp430.h>


#define   SCL_HIGH    P6OUT |= BIT0
#define   SCL_LOW     P6OUT &= ~BIT0
#define   SDA_HIGH    P6OUT |= BIT5
#define   SDA_LOW     P6OUT &= ~BIT5

void IIC_ioinit(void);
void IIC_start(void);
void IIC_stop(void);
void IIC_writebyte(unsigned char IIC_byte);

#endif /* IIC_H_ */
