#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
 
uint16_t adc_value;            //Variable used to store the value read from the ADC
char buffer[5];                //Output of the itoa function
uint8_t i=0;                //Variable for the for() loop
 
void adc_init(void);            //Function to initialize/configure the ADC
uint16_t read_adc(uint8_t channel);    //Function to read an arbitrary analogic channel/pin
void USART_init(void);            //Function to initialize and configure the USART/serial
void USART_send( unsigned char data);    //Function that sends a char over the serial port
void USART_putstring(char* StringPtr);    //Function that sends a string over the serial port
 
int main(void){
adc_init();        //Setup the ADC
USART_init();        //Setup the USART
 
for(;;){        //Our infinite loop
 
 //USART_putstring("Reading channel ");
 //USART_putstring(" : ");            //Just to keep things pretty
 adc_value = read_adc(0);        //Read one ADC channel
 itoa(adc_value, buffer, 10);        //Convert the read value to an ascii string
 USART_putstring(buffer);        //Send the converted value to the terminal
 //USART_putstring("  ");            //Some more formatting
 _delay_ms(500);                //You can tweak this value to have slower or faster readings or for max speed remove this line
 
 USART_send('\r');
 USART_send('\n');                //This two lines are to tell to the terminal to change line
}
 
return 0;
}
 
void adc_init(void){
 ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));    //16Mhz/128 = 125Khz the ADC reference clock
 ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
 ADCSRA |= (1<<ADEN);                //Turn on ADC
 ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}
 
uint16_t read_adc(uint8_t channel){
 ADMUX &= 0xF0;                    //Clear the older channel that was read
 ADMUX |= channel;                //Defines the new ADC channel to be read
 ADCSRA |= (1<<ADSC);                //Starts a new conversion
 while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
 return ADCW;                    //Returns the ADC value of the chosen channel
}
 
void USART_init(void){
 
 UBRRH = (uint8_t)(BAUD_PRESCALLER>>8);
 UBRRL = (uint8_t)(BAUD_PRESCALLER);
 UCSRB = (1<<RXEN)|(1<<TXEN);
 UCSRC = (3<<UCSZ0);
}
 
void USART_send( unsigned char data){
 
 while(!(UCSRA & (1<<UDRE)));
 UDR = data;
 
}
 
void USART_putstring(char* StringPtr){
 
while(*StringPtr != 0x00){
 USART_send(*StringPtr);
 StringPtr++;}
 
}