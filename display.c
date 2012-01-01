#include "common.h"
#define pixel_scr_size_x 800
#define pixel_scr_size_y 600
void draw_bmp(void * img_buffer){
	ty_bitmapfileheader * bitmap_header = (ty_bitmapfileheader *)img_buffer;
	RGB * bitmap_data = (RGB *)((char *)0x00100000 + bitmap_header->data);
	RGB16 * gfx_lfb = (RGB16 *)0xe0000000;
	int i = 0;
	int j = 0;
	while(i < 600){
		j = 0;
		while(j < 800){
			gfx_lfb[i * 800 + j].b = bitmap_data[(599 - i) * 800 + j].b;
			gfx_lfb[i * 800 + j].g = bitmap_data[(599 - i) * 800 + j].g;
			gfx_lfb[i * 800 + j].r = bitmap_data[(599 - i) * 800 + j].r;
			++j;
		}
		++i;
	}
}
void gfx_solid(u64 dsp, u16 color){
	u16 * teste = (u16 *)(dsp);
	d32 i, j;
	for(i=0; i<pixel_scr_size_y; i++)
		for(j=0; j<pixel_scr_size_x; j++)
			teste[i*pixel_scr_size_x+j] = color;
}
void gfx_demo(u32 dsp){
	gfx_solid(dsp, 0x0000);
	gfx_solid(dsp, 0x1111);
	gfx_solid(dsp, 0x2222);
	gfx_solid(dsp, 0x3333);
	gfx_solid(dsp, 0x4444);
	gfx_solid(dsp, 0x5555);
	gfx_solid(dsp, 0x6666);
	gfx_solid(dsp, 0x7777);
	gfx_solid(dsp, 0x8888);
	gfx_solid(dsp, 0x9999);
	gfx_solid(dsp, 0xaaaa);
	gfx_solid(dsp, 0xbbbb);
	gfx_solid(dsp, 0xcccc);
	gfx_solid(dsp, 0xdddd);
	gfx_solid(dsp, 0xeeee);
}
/*
 * Funcoes para modo video de texto
 */
 #define ascii_scr_size_x 80
#define ascii_scr_size_y 25
typedef enum{
    BLACK, DBLUE, DGREEN, DCYAN, DRED, DMAGENTA, BROWN,  LGRAY,
    DGRAY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE
} Color;
typedef struct{
    unsigned value   : 8;
    unsigned fgcolor : 4;
    unsigned bgcolor : 3;
    unsigned blink   : 1;
} __attribute__((packed)) CharCell;
typedef CharCell PCScreen[ascii_scr_size_y][ascii_scr_size_x];
PCScreen * const screen = (PCScreen*)0xb8000;
#define SCREEN (*screen)
extern unsigned char bootdrv;
static d32 posx = 0;
static d32 posy = 0;
void scroll(){
	u64 x, y;
	for(y = 1; y<ascii_scr_size_y; y++)
		for(x = 0; x<ascii_scr_size_x; y++)
			SCREEN[y - 1][x] = SCREEN[y][x];
	for(x = 0; x<ascii_scr_size_x; y++)
		SCREEN[ascii_scr_size_y - 1][x].value = ' ';
	posy--;
}
void putchar(char c){
	if(c == 0)
		return;
	if(c == '\n'){
		posx = 0; // Carriage return automatico
		posy++;
		if(ascii_scr_size_y == posy)
			scroll();
		return;
	}
	if(c == '\r'){
		posx = 0;
		return;
	}
	if(c == '\t'){
		// Nao suportado ainda
		return;
	}
	SCREEN[posy][posx].fgcolor = LGRAY;
	SCREEN[posy][posx].bgcolor = DBLUE;
	SCREEN[posy][posx].blink   = false;
	SCREEN[posy][posx].value = c;
	posx++;
	if(ascii_scr_size_x == posx){
		posx = 0;
		posy++;
	}
	if(ascii_scr_size_y == posy)
		scroll();
}
void gotoXY(int x, int y){
	if(x < 0)
		x = 0;
	if(x >= ascii_scr_size_x)
		x = ascii_scr_size_x - 1;
	if(y < 0)
		y = 0;
	if(y >= ascii_scr_size_y)
		y = ascii_scr_size_y - 1;
	posx = x;
	posy = y;
}
void clrsrc(){
	int x, y;
	for(y = 0; y<ascii_scr_size_y; y++)
		for(x = 0; x<ascii_scr_size_x; x++){
			SCREEN[y][x].fgcolor = LGRAY;
			SCREEN[y][x].bgcolor = DBLUE;
			SCREEN[y][x].blink   = false;
			SCREEN[y][x].value = ' ';
		}
	gotoXY(0, 0);
}
