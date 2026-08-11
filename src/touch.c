#include "touch.h"
#include "lpc24xx.h"

#define CS_PIN            0x00100000        //P0.20

static unsigned char touch_read(unsigned char command);
void touch_read_xy(char *x, char *y, int *pressure);

void touch_init(void)
{
	//Implement this as you see fit
	//Remember to setup CS_TP as a GPIO output
	PINSEL0 = (PINSEL0 & ~(0x3 << 14)) | (0x2 << 14);
	PINSEL0 = (PINSEL0 & ~(0x3 << 18)) | (0x2 << 18);
	PINSEL0 = (PINSEL0 & ~(0x3 << 16)) | (0x2 << 16);
	PINSEL0 = (PINSEL0 & ~(0x3 << 20));
	
	S0SPCR = 0x093C;
	S0SPCCR = 0x24;
	FIO0DIR |= (0x1 << 20);
}

void touch_read_xy(char *x, char *y, int *pressure)
{
	char z1 = 0;
	char z2 = 0;
	//Read X co-ordinate from the touch screen controller
	*x = touch_read(0xD8);
	//Read Y co-ordinate from the touch screen controller
	*y = touch_read(0x98);
	
	z1 = touch_read(0xB8);
	z2 = touch_read(0xC8);
	if (z1 == 0) {
        *pressure = 0;       /* not touching */
    } else {
		// Pressure calculation as per the data sheet
        *pressure = (400 * (*x) * (z2 - z1)) / (256 * z1);
    }
}

static unsigned char touch_read(unsigned char command)
{
	unsigned short result;

	//Set CS_TP pin low to begin SPI transmission
	FIO0CLR = (0x1 <<  20);
	

	//Transmit command byte on MOSI, ignore MISO (full read write cycle)
	S0SPDR = command;
	while ((S0SPSR & 0x80) == 0) {}
	
	//Transmit 0x00 on MOSI, read in requested result on MISO (another full read write cycle)
	S0SPDR = 0x00;
	while ((S0SPSR & 0x80) == 0) {}
	result = S0SPDR;
		
	//Transmission complete, set CS_TP pin back to high
	FIO0SET = (0x1 << 20);
	
	
	//Return 8 bit result.
	return (unsigned char) result;
}	

