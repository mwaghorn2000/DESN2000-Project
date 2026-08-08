#include "bubbles.h"
#include "touch.h"
#include "lcd/lcd_grph.h"
#include "delay.h"    
#include <stdlib.h>

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
	{0,0,0,0}, // Manual
};

static SystemEnable sys = {1,1,1,1};

// Screen state
typedef enum { SCREEN_HOME, SCREEN_MODE } Screen;
static Screen currentScreen = SCREEN_HOME;

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
// state parameter determines active mode
static Tile mode_tiles[5] = {
	{ 5, 5, 70, 25, "< Back", DARK_GRAY},
	{ 5, 60, 230, 50, "Away", DARK_GRAY},
	{ 5, 120, 230, 50, "Sleep", DARK_GRAY},
	{ 5, 180, 230, 50, "Normal", DARK_GRAY},
	{ 5, 240, 230, 50, "Manual", DARK_GRAY}
};

void draw_tile(Tile *t);
int poll_tile(Tile *t, int count, int x, int y);
void draw_menu(Screen s);
void draw_homeMenu(void);
void draw_modeMenu(void);
void apply_mode(SystemMode m, SystemEnable *se);
int handle_home(int hit);
int handle_mode(int hit);

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
	
	//Loop forever
	while (1) {
		//redraw
		if (redraw == 1) {
			draw_menu(currentScreen);
			redraw = 0;
		}
		
		touch_read_xy(&x,&y, &pressure);
		
		sprintf(buf, "x%3d y%3d h%2d s%d",
        (unsigned char)x, (unsigned char)y,
        poll_tile(mode_tiles, 5, x, y), currentScreen);
    lcd_fillRect(0, 305, 239, 319, BLACK);
    lcd_putString(2, 308, (unsigned char *)buf);
		
		switch(currentScreen) {
			case SCREEN_HOME:
				redraw = handle_home(poll_tile(home_tiles, 4, x, y));
				break;
			case SCREEN_MODE:
				redraw = handle_mode(poll_tile(mode_tiles, 5, x, y));
				break;
		}
	}
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
		case SCREEN_HOME: draw_homeMenu(); break;
		case SCREEN_MODE: draw_modeMenu(); break;
		//case SCREEN_BLIND: draw_blindsMenu(); break;
		//wcase SCREEN_COFFEE: draw_coffeeMenu(); break;
		//default: draw_error(); break;
	}
}

void apply_mode(SystemMode m) {
	if (m != MODE_MANUAL) {
		sys = mode_presets[m];
	}
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
			//currentScreen = SCREEN_COFFEE;
			return 1;
		default: return 0;
	}
}

// handles button presses for node menu
// return 1 if the press requires a redraw
int handle_mode(int hit) {
	SystemMode m;

	if (hit < 0) {
		return 0;
	}

	if (hit == 0) {            // Back button
		currentScreen = SCREEN_HOME;
		return 1;
	}

	if (hit > 4) {
		return 0;
	}

	m = (SystemMode)(hit - 1);   // tile 1..4 maps to mode 0..3

	if (m == currentMode) {
		return 0;
	}

	mode_tiles[currentMode + 1].colour = DARK_GRAY;
	currentMode = m;
	mode_tiles[currentMode + 1].colour = GREEN;
	return 1;
}
	
			
