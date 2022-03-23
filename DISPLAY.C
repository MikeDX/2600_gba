/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.
   Modified 1996 by Daniel Boris

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

   $Id: display.c,v 1.23 1996/03/21 15:52:38 alex Exp $
******************************************************************************/

/*
   Display handling code.
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "config.h"
#include "vmachine.h"
#include "address.h"
#include "files.h"
#include "colours.h"
#include "keyboard.h"
#include "resource.h"
#include "misc.h"
//#include "dos.h"

#define VGA256		0x13
#define TEXT_MODE	0x03
#define PALETTE_MASK      0x3c6
#define PALETTE_REGISTER  0x3c8
#define PALETTE_DATA	  0x3c9
typedef unsigned short u16;
//unsigned char *screen=(char *)0xA0000000L;
u16* screen 		=(u16*)0x6000000;
int screen_num;
unsigned long white,black;
#define RGB(r,g,b) ((r >> 3) << 10) | ((g >> 3) << 5) | ((b >> 3));
/* The pointer to the graphics buffer */
BYTE vscreen[64000];

int color[255];

/* The width and height of the buffer */
int vwidth,vheight;

char coltable[256];

/* The refresh skipping counter */
int tv_counter=0;
int redraw_flag=1;

void set_palette(int index,BYTE red,BYTE green,BYTE blue)
{
//color[index]=colortable[index];
color[index]=RGB(red, green, blue);
//color[index]=red<<10 | blue<<5 | green;
/*	outportb(0x3c6,0xff);
	outportb(0x3c8,index);
	outportb(0x3c9,red);
	outportb(0x3c9,green);
	outportb(0x3c9,blue); */
}

void setmode(int vmode)
{
/*	union REGS regs;

	regs.h.ah = 0;
	regs.h.al = vmode;
	int86(0x10, &regs, &regs); */
}


/** PUTIMAGE: macro copying image buffer into a window ********/
void put_image(void)
{
//		memcpy(&screen[0],&vscreen[0],64000);
}

/* Set up the colormap */
void create_cmap(void)
{
	int i;
	BYTE red,green,blue;

	/* Initialise parts of the colors array */
	for(i=0; i< 256; i++)
	{
		blue  = (BYTE)((colortable[i] & 0xff0000)>> 16);
		green = (BYTE)((colortable[i] & 0x00ff00)>> 8);
		red = (BYTE)(colortable[i] & 0x0000ff);
		/*red=red;
		green=green;
		blue=blue; */
		set_palette(i,red,green,blue);
	}
}

/* Create the main application shell */
static void create_window()
{
	int i;
		create_cmap();
		/*
	vwidth=tv_width;
	vheight=tv_height+tv_overscan;
	if (!app_data.debug) {
		init_keyboard();
		create_cmap();
	}
*/	for(i=0; i<256; i++) coltable[i]=1;
	coltable[0]=coltable[1]=coltable[2]=coltable[4]=0;
	coltable[8]=coltable[16]=coltable[32]=coltable[64]=0;
	coltable[128]=0; 
	vwidth=240;
	vheight=160;
}

void tv_off(void)
{
	/* #ifdef VERBOSE
	  printf("Switching off...\n");
	#endif
	if (!app_data.debug) {
//		Set_Old_Int9();
	}
	free(vscreen);
	setmode(TEXT_MODE); */
}

/* The main initialiser for the X stuff */
int tv_on()
{
	/* long i;

   // Get the basic colors 
	white=0;
	black=15;
	if (!app_data.debug) {
		setmode(VGA256);
	} */
	create_window();
	/* #ifdef VERBOSE
	   printf("OK\n  Allocating screen buffer...");
	#endif
	vscreen=(BYTE *)malloc(64000);
	//memset(vscreen,0,64000);
	if(!vscreen) {
		#ifdef VERBOSE
			printf("FAILED\n");
		#endif
	return(0);
	} */
	return(1); 
}

/* The graceful shut down for the X stuff */


/* Translates a 2600 color into is X pixel value */
/* BYTE tv_color(BYTE b)
{
	if(app_data.private)
	return (b >> 1);
	else
	return colors[(b >> 1)].pixel;
}  */

/* Displays the 2600 screen display */
void tv_display(void)
{

	if (!app_data.debug) {
		put_image();
	}
	tv_counter++;
}


/* Put one pixel to the image buffer. Used mainly for debugging */
void tv_putpixel(int x, int y,BYTE value)
{
	//BYTE *p;

	//x=x<<1;
//	screen[x+y*240]=color[value];
	//*(p)=512;
	//*(p+1)=value;
}








