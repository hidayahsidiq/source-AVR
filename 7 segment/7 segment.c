#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#define F_CPU 8000000L
#define F_SCL 100000L

unsigned int satuan,puluhan,ratusan,ribuan,ubah,nilai,data_temp,data_m,data_h; 

void tampil()
{
PORTD = satuan;
PORTB = 0b11110111;
_delay_ms(5);

PORTD = puluhan;
PORTB = 0b11111011;
_delay_ms(5);
 
PORTD = ratusan;
PORTB = 0b11111101;
_delay_ms(5);

PORTD = ribuan;
PORTB = 0b11111110;
_delay_ms(5);
}


void format()
{
	if (ubah==0){nilai=0b00111111;}
	if (ubah==1){nilai=0b10000110;}
	if (ubah==2){nilai=0b11011011;}
	if (ubah==3){nilai=0b11001111;}
	if (ubah==4){nilai=0b11100110;}
	if (ubah==5){nilai=0b11101101;}
	if (ubah==6){nilai=0b11111101;}
	if (ubah==7){nilai=0b00000111;}
	if (ubah==8){nilai=0b01111111;}
	if (ubah==9){nilai=0b11101111;}
}
// ---------------------------------------------------------------------------
// TYPEDEFS
typedef uint8_t byte; // I just like byte & sbyte better
typedef int8_t sbyte;
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// I2C (TWI) ROUTINES
//
// On the AVRmega series, PA4 is the data line (SDA) and PA5 is the clock (SCL
// The standard clock rate is 100 KHz, and set by I2C_Init. It depends on the AVR osc. freq.
#define F_SCL 100000L // I2C clock speed 100 KHz
#define READ 1
#define TW_START 0xA4 // send start condition (TWINT,TWSTA,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)
#define TW_ACK 0xC4 // return ACK to slave
#define TW_NACK 0x84 // don't return ACK to slave
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_READY (TWCR & 0x80) // ready when TWINT returns to logic 1.
#define TW_STATUS (TWSR & 0xF8) // returns value of status register
#define I2C_Stop() TWCR = TW_STOP // inline macro for stop condition
void I2C_Init()
// at 16 MHz, the SCL frequency will be 16/(16+2(TWBR)), assuming prescalar of 0.
// so for 100KHz SCL, TWBR = ((F_CPU/F_SCL)-16)/2 = ((16/0.1)-16)/2 = 144/2 = 72.
{
TWSR = 0; // set prescalar to zero
TWBR = ((F_CPU/F_SCL)-16)/2; // set SCL frequency in TWI bit register
}
byte I2C_Detect(byte addr)
// look for device at specified address; return 1=found, 0=not found
{
TWCR = TW_START; // send start condition
while (!TW_READY); // wait
TWDR = addr; // load device's bus address
TWCR = TW_SEND; // and send it
while (!TW_READY); // wait
return (TW_STATUS==0x18); // return 1 if found; 0 otherwise
}
byte I2C_FindDevice(byte start)
// returns with address of first device found; 0=not found
{
for (byte addr=start;addr<0xFF;addr++) // search all 256 addresses
{
if (I2C_Detect(addr)) // I2C detected?
return addr; // leave as soon as one is found
}
return 0; // none detected, so return 0.
}
void I2C_Start (byte slaveAddr)
{
I2C_Detect(slaveAddr);
}
byte I2C_Write (byte data) // sends a data byte to slave
{
TWDR = data; // load data to be sent
TWCR = TW_SEND; // and send it
while (!TW_READY); // wait
return (TW_STATUS!=0x28);
}
byte I2C_ReadACK () // reads a data byte from slave
{
TWCR = TW_ACK; // ack = will read more data
while (!TW_READY); // wait
return TWDR;
//return (TW_STATUS!=0x28);
}
byte I2C_ReadNACK () // reads a data byte from slave
{
TWCR = TW_NACK; // nack = not reading more data
while (!TW_READY); // wait
return TWDR;
//return (TW_STATUS!=0x28);
}
void I2C_WriteByte(byte busAddr, byte data)
{
I2C_Start(busAddr); // send bus address
I2C_Write(data); // then send the data byte
I2C_Stop();
}
void I2C_WriteRegister(byte busAddr, byte deviceRegister, byte data)
{
I2C_Start(busAddr); // send bus address
I2C_Write(deviceRegister); // first byte = device register address
I2C_Write(data); // second byte = data for device register
I2C_Stop();
}
byte I2C_ReadRegister(byte busAddr, byte deviceRegister)
{
byte data = 0;
I2C_Start(busAddr); // send device address
I2C_Write(deviceRegister); // set register pointer
I2C_Start(busAddr+READ); // restart as a read operation
data = I2C_ReadNACK(); // read the register data
I2C_Stop(); // stop
return data;
}
// ---------------------------------------------------------------------------
// DS1307 RTC ROUTINES
#define DS1307 0xD0 // I2C bus address of DS1307 RTC
#define SECONDS_REGISTER 0x00
#define MINUTES_REGISTER 0x01
#define HOURS_REGISTER 0x02
#define DAYOFWK_REGISTER 0x03
#define DAYS_REGISTER 0x04
#define MONTHS_REGISTER 0x05
#define YEARS_REGISTER 0x06
#define CONTROL_REGISTER 0x07
#define RAM_BEGIN 0x08
#define RAM_END 0x3F
void DS1307_GetTime(byte *hours, byte *minutes, byte *seconds)
// returns hours, minutes, and seconds in BCD format
{
*hours = I2C_ReadRegister(DS1307,HOURS_REGISTER);
*minutes = I2C_ReadRegister(DS1307,MINUTES_REGISTER);
*seconds = I2C_ReadRegister(DS1307,SECONDS_REGISTER);

if (*hours & 0x40) // 12hr mode:
{*hours &= 0x1F;} // use bottom 5 bits (pm bit = temp & 0x20)
else {*hours &= 0x3F;} // 24hr mode: use bottom 6 bits
}

void DS1307_GetDate(byte *months, byte *days, byte *years)
// returns months, days, and years in BCD format
{
*months = I2C_ReadRegister(DS1307,MONTHS_REGISTER);
*days = I2C_ReadRegister(DS1307,DAYS_REGISTER);
*years = I2C_ReadRegister(DS1307,YEARS_REGISTER);
}
void SetTimeDate()
// simple, hard-coded way to set the date.
{
I2C_WriteRegister(DS1307,MONTHS_REGISTER, 0x08);
I2C_WriteRegister(DS1307,DAYS_REGISTER, 0x31);
I2C_WriteRegister(DS1307,YEARS_REGISTER, 0x13);
I2C_WriteRegister(DS1307,HOURS_REGISTER, 0x04+0x40); // add 0x40 for PM 0x09=18.00
I2C_WriteRegister(DS1307,MINUTES_REGISTER, 0x24);
I2C_WriteRegister(DS1307,SECONDS_REGISTER, 0x00);
}

void WriteTime()
{
		byte hours, minutes, seconds;
		DS1307_GetTime(&hours,&minutes,&seconds);
		data_m = minutes;
		data_temp=data_m%10;
		ubah=data_temp;
		format();
		satuan=nilai;
		//tampil();
		
		data_temp=data_m/10;
		ubah=data_temp;
		format();
		puluhan=nilai;
		//tampil();
		
		data_h = hours;
		data_temp=data_h%10;
		ubah=data_temp;
		format();
		ratusan=nilai;
		//tampil();
		
		data_temp=data_h/10;
		ubah=data_temp;
		format();
		ribuan=nilai;
		tampil();
}
void MainLoop()
	{
	while(1)
		{
		WriteTime(); // put time & date on LCD
		}
	}
int main(void)
{
	DDRB = 0xFF;
	DDRD = 0xFF;
	I2C_Init(); // set I2C clock frequency
	_delay_ms(400);
	MainLoop(); // display time
	
    
			/*
		data = 65;
		data_temp=data%10;
		ubah=data_temp;
		format();
		satuan=nilai;
		//tampil();
		
		data_temp=data/10;
		ubah=data_temp;
		format();
		puluhan=nilai;
		//tampil();
		
		data = 21;
		data_temp=data%10;
		ubah=data_temp;
		format();
		ratusan=nilai;
		//tampil();
		
		data_temp=data/10;
		ubah=data_temp;
		format();
		ribuan=nilai;
		tampil();
		*/	
		
}