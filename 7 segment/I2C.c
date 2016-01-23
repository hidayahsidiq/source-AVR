/*
 * I2C.c
 *
 * Created: 11/2/2015 9:02:39 PM
 *  Author: family
 */ 

#include <avr\io.h>
#include <util\delay.h>


int function(void)
{
    void I2C_Init()
{
  TWSR=0x00; //set presca1er bits to zero
  TWBR=0x46; //SCL frequency is 100K for XTAL = 7.3728M
  TWCR=0x04; //enab1e TWI module
}

void I2C_Start()
{
  TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
  while (!(TWCR & (1<<TWINT)));
}

void I2C_Stop(void)
{
  TWCR = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
   _delay_us(10) ; //wait for a short time
}

void I2C_Write(unsigned char dat)
{
  TWDR = dat ;
  TWCR = ((1<< TWINT) | (1<<TWEN));
  while (!(TWCR & (1 <<TWINT)));
}

unsigned char I2C_Read(unsigned char ack)
{
 TWCR = ((1<< TWINT) | (1<<TWEN) | (ack<<TWEA));
   while ( !(TWCR & (1 <<TWINT)));
   return TWDR;
}
	}