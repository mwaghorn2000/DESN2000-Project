#include "bubbles.h"
#include "touch.h"

int main(void) {
	char x=0, y=0, z1=0, z2=0;
	int pressure = 0;

	//Setup LCD and 'blow' first bubble
	blow_bubbles();

	//Setup touchscreen
	touch_init();
	
	//Loop forever
	while (1) {
		//Read in X and Y coordinates
		touch_read_xy(&x,&y,&z1,&z2);
		
		pressure = 400 * x/4096*((z1/z2)-1);
		
		//Pop the bubble
		pop_bubble(x, y, pressure);
	}
}
