#ifndef DISPLAY_H
#define DISPLAY_H

#define VGA256		0x13
#define TEXT_MODE	0x03


extern void create_cmap(void);
extern void put_image(void);
void setmode(int);
void fselLoad(void);
void rate_update(void);
void tv_off(void);
int tv_on(void);
void tv_event(void);
void tv_display(void);
void tv_putpixel(int x, int y, BYTE value);
extern BYTE tv_color(BYTE b);
extern int redraw_flag;


extern BYTE *vscreen;
extern int magstep;
extern int vwidth,vheight;
extern int tv_counter;
extern unsigned int linetab[256];
extern char coltable[256];
#endif

