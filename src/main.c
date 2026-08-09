#include "bubbles.h"
#include "doorbell.h"
#include "touch.h"
#include "lcd/lcd_grph.h"
#include "delay.h"    
#include <stdlib.h>
#include <stdio.h>
#include "clock.h"
#include "plug.h"

#define SCREEN_W 240
#define SCREEN_H 320
#define TILE_W 110
#define TILE_H 90

// Tiles are buttons
typedef struct {
	int x, y, w, h;
	char *label;
	int colour;
} Tile;

// Used to enable or disable different subsystems
typedef struct {
	unsigned char blind_w_open;
	unsigned char plug_enable;
	unsigned char doorbell_enable;
	unsigned char thermo_enable;
} SystemEnable;

// Default System states
static SystemEnable mode_presets[4] = {
	{0,0,0,0}, // Away
	{0,1,0,1}, // Sleep
	{1,1,1,1}, // Normal
	{1,1,1,1}, // Manual
};

// Coffee globals
int coffee_hour = 7;
int coffee_min = 0;
int coffee_enabled = 0;
int coffee_time = 60*7;
int plug_on = 0;

static SystemEnable sys = {1,1,1,1};

// Screen state
typedef enum { SCREEN_TIME, SCREEN_HOME, SCREEN_MODE, SCREEN_COFFEE} Screen;
static Screen currentScreen = SCREEN_TIME;

// Device Mode
typedef enum { MODE_AWAY, MODE_SLEEP, MODE_NORMAL, MODE_MANUAL } SystemMode;
static SystemMode currentMode = MODE_NORMAL;

// Home tiles. 2x2 grid
static Tile home_tiles[4] = {
  { 5, 40, TILE_W, TILE_H, "Mode", DARK_GRAY },
	{ 125, 40, TILE_W, TILE_H, "Blinds", DARK_GRAY },
  { 5, 140, TILE_W, TILE_H, "Climate Ctrl", DARK_GRAY },
  { 125,140, TILE_W, TILE_H, "Coffee", DARK_GRAY }
};

// mode tiles. 4x1 grid
static Tile mode_tiles[5] = {
	{ 5, 5, 70, 25, "< Back", DARK_GRAY},
	{ 5, 60, 230, 50, "Away", DARK_GRAY},
	{ 5, 120, 230, 50, "Sleep", DARK_GRAY},
	{ 5, 180, 230, 50, "Normal", GREEN},
	{ 5, 240, 230, 50, "Manual", DARK_GRAY}
};

// time tiles
static Tile time_tiles[5] = {
	{  30,  70, 60, 45, "  +",   DARK_GRAY },  
	{ 150,  70, 60, 45, "  +",   DARK_GRAY },  
	{  30, 160, 60, 45, "  -",   DARK_GRAY },   
	{ 150, 160, 60, 45, "  -",   DARK_GRAY },   
	{  30, 250, 180, 45, "  Confirm", GREEN }
};

// coffee tiles
static Tile coffee_tiles[7] = {
	{   5,   5,  60, 25, "< Back",   DARK_GRAY },
	{  75,  45,  90, 90, "",         BLACK },      /* mug hit region */
	{  30, 165,  50, 35, "  +",      DARK_GRAY },
	{ 150, 165,  50, 35, "  +",      DARK_GRAY },
	{  30, 235,  50, 35, "  -",      DARK_GRAY },
	{ 150, 235,  50, 35, "  -",      DARK_GRAY },
	{   5, 285, 230, 30, "  Confirm", GREEN }
};

// Time globals
static int set_hour = 12;
static int set_min = 0;
int sim_minutes = 0;
unsigned int last_tick = 0;

void draw_tile(Tile *t);
int poll_tile(Tile *t, int count, int x, int y);
void draw_menu(Screen s);
void draw_homeMenu(void);
void draw_modeMenu(void);
void draw_timeMenu(void);
void apply_mode(SystemMode m);
int handle_home(int hit);
int handle_mode(int hit);
int handle_time(int hit);
int handle_coffee(int hit);
void draw_mug(int x, int y, int on);
void draw_coffeeMenu(void);
void draw_statusbar(void);

int main(void) {
	char x=0, y=0;
	int pressure = 0;
	// Default to normal mode
	char redraw = 1;
	char buf[40];
	
	//Setup LCD
	lcd_init();
	//Setup touchscreen
	touch_init();
	// Setup timer0 and doorbell
	doorbell_init();
	timer0_init();
	
	//Loop forever
	while (1) {
		//redraw
		doorbell_poll(sys.doorbell_enable);
		if (redraw == 1) {
			draw_menu(currentScreen);
			redraw = 0;
		}
		
		touch_read_xy(&x,&y, &pressure);
		
		switch(currentScreen) {
			case SCREEN_TIME:
				redraw = handle_time(poll_tile(time_tiles, 5, x, y));
				break;
			case SCREEN_HOME:
				redraw = handle_home(poll_tile(home_tiles, 4, x, y));
				break;
			case SCREEN_MODE:
				redraw = handle_mode(poll_tile(mode_tiles, 5, x, y));
				break;
			case SCREEN_COFFEE:
				redraw = handle_coffee(poll_tile(coffee_tiles, 7, x, y));
				break;
		}
		
		if (clock_service()) {
			int scheduled = plug_service(sim_minutes, coffee_time, coffee_enabled);
			int want = scheduled && sys.plug_enable;
			if (want != plug_on) {
				plug_on = want;
				plug_set(want);
			}
			draw_statusbar();
		}
	}
}

void draw_statusbar(void) {
	char buf[40];
	sprintf(buf, "%02d:%02d   Plug:%s",
	        sim_minutes / 60, sim_minutes % 60,
	        plug_on ? "ON " : "OFF");
	lcd_fillRect(0, 305, 239, 319, BLACK);
	lcd_putString(2, 308, (unsigned char *)buf);
}

// Draws a rectangle on the screen with the text.
void draw_tile(Tile *t) {
	int ty = t->y + (t->h / 2)-4;
	lcd_fillRect(t->x, t->y, t->x + t->w, t->y + t->h, t->colour);
	lcd_putString(t->x + 10, ty, (unsigned char *)t->label);
}

// Draws our home menu
void draw_homeMenu(void) {
	int i;
	lcd_fillScreen(BLACK);
	lcd_putString(10, 10, (unsigned char*)"HOME CONTROL");
	for (i = 0; i < 4; i++) { draw_tile(&home_tiles[i]); };
}

void draw_modeMenu(void) {
	int i;
	lcd_fillScreen(BLACK);
	lcd_putString(100, 12, (unsigned char*)"MODE MENU");
	for (i = 0; i < 5; i++) {draw_tile(&mode_tiles[i]); };
}

void draw_timeMenu(void) {
	char t[16];
	int i;
	lcd_fillScreen(BLACK);
	lcd_putString(60, 15, (unsigned char*)"SET TIME");
	
	sprintf(t, "%02d : %02d", set_hour, set_min);
	lcd_putString(80, 125, (unsigned char*)t);
	
	for (i = 0; i < 5; i++) { draw_tile(&time_tiles[i]); }
}

// Takes in a list of tiles and checks if the x - y position lands on a tile.
// Return the tile in the array which is pressed.
int poll_tile(Tile *t, int count, int x, int y) {
	int i;
	int sx = ((unsigned char)x * SCREEN_W / 255);
	int sy = ((unsigned char)y * SCREEN_H / 255);
	for (i = 0; i < count; i++) {
		if (sx >= t[i].x && sx < t[i].x + t[i].w &&
				sy >= t[i].y && sy < t[i].y + t[i].h) {
			return i;
		}
	}
	return -1;
}

void draw_menu(Screen s) {
	switch (s) {
		case SCREEN_TIME: draw_timeMenu(); break;
		case SCREEN_HOME: draw_homeMenu(); break;
		case SCREEN_MODE: draw_modeMenu(); break;
		//case SCREEN_BLIND: draw_blindsMenu(); break;
		case SCREEN_COFFEE: draw_coffeeMenu(); break;
		//default: draw_error(); break;
	}
}

void draw_coffeeMenu(void) {
	char t[16];
	int i;
	lcd_fillScreen(BLACK);

	for (i = 0; i < 7; i++) draw_tile(&coffee_tiles[i]);

	draw_mug(120, 90, coffee_enabled); 

	sprintf(t, "%02d : %02d", coffee_hour, coffee_min);
	lcd_putString(85, 212, (unsigned char*)t);
}
void apply_mode(SystemMode m) {
	sys = mode_presets[m];
	currentMode = m;
}

// handles button presses for home menu
// return 1 if the press requires a redraw
int handle_home(int hit) {
	switch (hit) {
		case 0: 
			currentScreen = SCREEN_MODE; 
			return 1;
		case 1:
			//currentScreen = SCREEN_BLINDS;
			return 1;
		case 2:
			//currentScreen = SCREEN_CLIMATE;
			return 1;
		case 3:
			currentScreen = SCREEN_COFFEE;
			return 1;
		default: return 0;
	}
}

// handles button presses for mode menu
// return 1 if the press requires a redraw
int handle_mode(int hit) {
	SystemMode m;

	if (hit < 0) {
		return 0;
	}
	// Back button
	if (hit == 0) {            
		currentScreen = SCREEN_HOME;
		return 1;
	}

	if (hit > 4) {
		return 0;
	}
	// tile 1-4 maps to modes 0-3
	m = (SystemMode)(hit - 1);   

	if (m == currentMode) {
		return 0;
	}

	mode_tiles[currentMode + 1].colour = DARK_GRAY;
	apply_mode(m);
	mode_tiles[currentMode + 1].colour = GREEN;
	return 1;
}
	
// handles button presses for the time menu
// return 1 is the press requires a redraw
int handle_time(int hit) {
	if  (hit < 0) {
		return 0;
	}
	// increment hour
	if (hit == 0) {
		set_hour = (set_hour + 1) % 24;
		return 1;
	}
	// increment minutes by 5. Dont want to have to press it too much for testing
	if (hit == 1) {
		set_min = (set_min + 5) % 60;
		return 1;
	}
	if (hit == 2) {
		set_hour = (set_hour + 23) % 24;
		return 1;
	}
	if (hit == 3) {
		set_min = (set_min + 55) % 60;
		return 1;
	}
	if (hit == 4) {
		sim_minutes = set_hour * 60 + set_min;
		currentScreen = SCREEN_HOME;
		return 1;
	}
	return 0;
}

// handles button presses for the time menu
int handle_coffee(int hit) {
	if (hit < 0) {
		return 0;
	}
	if (hit == 0) {
		currentScreen = SCREEN_HOME;
		return 1;
	}
	if (hit == 1) {
		coffee_enabled = !coffee_enabled;
		if (!coffee_enabled && sys.plug_enable) {
			sys.plug_enable = 0;
			plug_set(0);
		}
		return 1;
	}
	if (hit == 2) { 
		coffee_hour = (coffee_hour + 1) % 24; return 1;
	}
	if (hit == 3) {
		coffee_min  = (coffee_min + 5) % 60;  return 1;
	}
	if (hit == 4) {
		coffee_hour = (coffee_hour + 23) % 24; return 1;
	}
	if (hit == 5) {
		coffee_min  = (coffee_min + 55) % 60;  return 1;
	}
	if (hit == 6)  {
		coffee_time = coffee_hour * 60 + coffee_min;
		currentScreen = SCREEN_HOME;
		return 1;
	}
	return 0; 
}
// rough mug drawing for coffee display
void draw_mug(int x, int y, int on) {
	lcd_fillRect(x - 25, y - 20, x + 25, y + 25, WHITE);
	lcd_fillcircle(x + 38, y, 14, WHITE);
	lcd_fillcircle(x + 38, y, 8,  BLACK);
	lcd_fillRect(x - 21, y - 17, x + 21, y - 8, on ? MAROON : DARK_GRAY);
	if (on) {
		lcd_line(x - 12, y - 38, x - 12, y - 26, LIGHT_GRAY);
		lcd_line(x,      y - 42, x,      y - 26, LIGHT_GRAY);
		lcd_line(x + 12, y - 38, x + 12, y - 26, LIGHT_GRAY);
	}
}
	
			
