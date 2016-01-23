/*
 * RTC.c
 *
 * Created: 11/2/2015 9:09:13 PM
 *  Author: family
 */ 

#include <avr/io.h>
#include <util\delay.h>

#define DS1307_ID 0xD0     // DS1307 ID
 
#define SEC_ADDRESS   0x00 // Address to access Ds1307 SEC register
#define DATE_ADDRESS  0x04 // Address to access Ds1307 DATE register
#define CONTROL 0x07       // Address to access Ds1307 CONTROL register
int function(void)
{
    void DS1307_Init()
{
   	I2C_Init();             // Initilize the I2c module.
	I2C_Start();            // Start I2C communication
 
	I2C_Write(DS1307_ID);   // Connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(CONTROL);     // Select the Ds1307 ControlRegister to configure Ds1307
 
	I2C_Write(0x00);        // Write 0x00 to Control register to disable SQW-Out
 
	I2C_Stop();             // Stop I2C communication after initilizing DS1307
 }
 
 void DS1307_SetTime(unsigned char hh, unsigned char mm, unsigned char ss)
{
	I2C_Start();            // Start I2C communication
 
	I2C_Write(DS1307_ID);	// connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(SEC_ADDRESS); // Select the SEC RAM address
 
	I2C_Write(ss);			// Write sec on RAM address 00H
	I2C_Write(mm);			// Write min on RAM address 01H
	I2C_Write(hh);			// Write hour on RAM address 02H
 
	I2C_Stop();           	// Stop I2C communication after Setting the Time
}

void DS1307_SetDate(unsigned char dd, unsigned char mm, unsigned char yy)
{
 	I2C_Start();            // Start I2C communication
 
	I2C_Write(DS1307_ID);	// connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(DATE_ADDRESS);		// Request DAY RAM address at 04H
 
	I2C_Write(dd);			// Write date on RAM address 04H
	I2C_Write(mm);			// Write month on RAM address 05H
	I2C_Write(yy);			// Write year on RAM address 06h
 
	I2C_Stop();				// Stop I2C communication after Setting the Date
}

void DS1307_GetTime(unsigned char *h_ptr,unsigned char *m_ptr,unsigned char *s_ptr)
{
     I2C_Start();             // Start I2C communication
 
	I2C_Write(DS1307_ID);	 // connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(SEC_ADDRESS); // Request Sec RAM address at 00H
 
     I2C_Stop();			// Stop I2C communication after selecting Sec Register
 
    I2C_Start();		        // Start I2C communication
    I2C_Write(0xD1);	        // connect to DS1307( under Read mode)
                                //by sending its ID on I2c Bus
 
  *s_ptr = I2C_Read(1);      // read second and return Positive ACK
  *m_ptr = I2C_Read(1); 	// read minute and return Positive ACK
  *h_ptr = I2C_Read(0);      // read hour and return Negative/No ACK
 
  I2C_Stop();		       // Stop I2C communication after reading the Time
 }
 
 void DS1307_GetDate(unsigned char *d_ptr,unsigned char *m_ptr,unsigned char *y_ptr)
{
    I2C_Start();             // Start I2C communication
 
    I2C_Write(DS1307_ID);	// connect to DS1307 by sending its ID on I2c Bus
    I2C_Write(DATE_ADDRESS);		// Request DAY RAM address at 04H
 
    I2C_Stop();			 // Stop I2C communication after selecting DAY Register
 
 
   I2C_Start();		        // Start I2C communication
   I2C_Write(0xD1);	        // connect to DS1307( under Read mode)
                                // by sending its ID on I2c Bus
 
  *d_ptr = I2C_Read(1);       // read Day and return Positive ACK
  *m_ptr = I2C_Read(1);       // read Month and return Positive ACK
  *y_ptr = I2C_Read(0);       // read Year and return Negative/No ACK
 
  I2C_Stop();		       // Stop I2C communication after reading the Time
 }
}