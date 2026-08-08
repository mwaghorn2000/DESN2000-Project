#include "bubbles.h"
#include "touch.h"
#include "lcd/lcd_grph.h"
#include "delay.h"    

#define SCREEN_W 240
#define SCREEN_H 320
#define TILE_W 110
#define TILE_H 90

typedef struct {
	int x, y, w, h;
	char *label;
	int state;
} Tile;

typdef enum { SCREEN_HOME, SCREEN_MODE } Screen;
static Screen currentScreen = SCREEN_HOME;

typedef enum { MODE_AWAY, MODE_SLEEP, MODE_NORMAL, MODE_MANUAL } SystemMode;
static SystemMode currentMode = MODE_NORMAL;

static Tile home_tiles[4] = {
    {  5,  40, TILE_W, TILE_H, "Mode",   0 },
    { 125, 40, TILE_W, TILE_H, "Blinds",  0 },
    {  5, 140, TILE_W, TILE_H, "Climate Ctrl", 0 },
    { 125,140, TILE_W, TILE_H, "Coffee",   0 }
};

void draw_tile(Tile *t);
int poll_tile(Tile *t, int count, int x, int y);
void draw_homemenu(void);

int main(void) {
	char x=0, y=0;
	int pressure = 0;
	//Setup LCD
	lcd_init();
	
	//Setup touchscreen
	touch_init();
	
	draw_homemenu();
	
	//Loop forever
	while (1) {
		//Read in X and Y coordinates
		touch_read_xy(&x,&y, &pressure);
	}
}

// Draws a rectangle on the screen with the text.
void draw_tile(Tile *t) {
	int fill = t->state ? GREEN : DARK_GRAY;
	lcd_fillRect(t->x, t->y, t->x + t->w, t->y + t->h, fill);
	lcd_putString(t->x + 10, t->y + 40, (unsigned char *)t->label);
}

// Draws our home menu
void draw_homemenu(void) {
	int i;
	lcd_fillScreen(BLACK);
	lcd_putString(10, 10, (unsigned char*)"HOME CONTROL");
	for (i = 0; i < 4; i++) { draw_tile(&home_tiles[i]); };
}

// Takes in a list of tiles and checks if the x - y position lands on a tile.
// Return the tile in the array which is pressed.
int poll_tile(Tile *t, int count, int x, int y) {
	int i;
	for (i = 0; i < count; i++) {
		if (x >= t[i].x && x < t[i].x + t[i].w &&
				y >= t[i].y && y < t[i].y + t[i].h) {
			return i;
		}
	}
	return -1;
}
