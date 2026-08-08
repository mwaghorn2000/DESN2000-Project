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
	int state;
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

// Screen state
typedef enum { SCREEN_HOME, SCREEN_MODE } Screen;
static Screen currentScreen = SCREEN_HOME;

// Device Mode
typedef enum { MODE_AWAY, MODE_SLEEP, MODE_NORMAL, MODE_MANUAL } SystemMode;
static SystemMode currentMode = MODE_NORMAL;

// Home tiles. 2x2 grid
static Tile home_tiles[4] = {
  { 5, 40, TILE_W, TILE_H, "Mode", 0 },
	{ 125, 40, TILE_W, TILE_H, "Blinds", 0 },
  { 5, 140, TILE_W, TILE_H, "Climate Ctrl", 0 },
  { 125,140, TILE_W, TILE_H, "Coffee", 0 }
};

// mode tiles. 4x1 grid
// state parameter determines active mode
static Tile mode_tiles[5] = {
	{ 5, 5, 70, 25, "< Back", 0},
	{ 5, 60, 230, 50, "Away", 0},
	{ 5, 120, 230, 50, "Sleep", 0},
	{ 5, 180, 230, 50, "Normal", 1},
	{ 5, 240, 230, 50, "Manual", 0}
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
	SystemEnable se = mode_presets[2];
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
		
		sprintf(buf, "x%3d y%3d p%6d s%d",
            (unsigned char)x, (unsigned char)y);
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
	int fill = t->state ? GREEN : DARK_GRAY;
	int ty = t->y + (t->h / 2)-4;
	lcd_fillRect(t->x, t->y, t->x + t->w, t->y + t->h, fill);
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

void apply_mode(SystemMode m, SystemEnable *se) {
	switch (m) {
		case MODE_AWAY:
			*se = mode_presets[0];
			break;
		case MODE_SLEEP:
			*se = mode_presets[1];
			break;
		case MODE_NORMAL:
			*se = mode_presets[2];
			break;
		case MODE_MANUAL:
			*se = mode_presets[3];
			break;
	}
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
	switch (hit) {
		case 0:
			currentScreen = SCREEN_HOME;
			return 1;
		default: return 0;
	}
}
	
			
