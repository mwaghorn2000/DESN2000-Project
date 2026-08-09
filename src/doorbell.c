#include "doorbell.h"
#include "lpc24xx.h" 
#include "delay_asm.h"

#define DAC_MASK (0x3 << 20)
#define DAC_AOUT (0x2 << 20)
#define DAC_VALUE (0x3FF << 6)

#define TONE1 784
#define TONE2 659
#define DURATION1 300000
#define DURATION2 600000
#define VOLUME 0x300

void doorbell_init(void) {
	// button is GPIO already. Already input
	
	// Setup DAC
	timer0_init();
	PINSEL1 = (PINSEL1 & ~DAC_MASK) | DAC_AOUT;
	PCONP |= (1<<22);
}

// check if button is pressed. Dont if enable is 0.
void doorbell_poll(int enable) {
	if (enable == 0) {
		return;
	}
	
	if ((FIO0PIN & (1 << 11))) {
		doorbell_play();
	}
}

void doorbell_play() {
	int period1 = 1000000/TONE1;
	int period2 = 1000000/TONE2;
	
	int h_period1 = period1/2;
	int h_period2 = period2/2;
	
	unsigned int cycles1 = DURATION1 / period1;
	unsigned int cycles2 = DURATION2 / period2;
	
	
	unsigned int i = 0;
	while (i < cycles1) {
		DACR = (DACR & ~DAC_VALUE) | (VOLUME << 6);
		delay_us(h_period1);
		DACR = (DACR & ~DAC_VALUE);
		delay_us(h_period1);
		i++;
	}
	
	delay_us(500000);
	
	i = 0;
	while (i < cycles2) {
		DACR = (DACR & ~DAC_VALUE) | (VOLUME << 6);
		delay_us(h_period2);
		DACR = (DACR & ~DAC_VALUE);
		delay_us(h_period2);
		i++;
	}
}
