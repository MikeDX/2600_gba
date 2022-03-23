/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.
   Modified 1996 Daniel Boris

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

   $Id: main.c,v 1.13 1996/03/21 16:36:01 alex Exp $
******************************************************************************/

/*
   The main program body.
*/

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "display.h"
#include "keyboard.h"
#include "files.h"
#include "config.h"
#include "vmachine.h"
#include "string.h"
#include "gba.h"
//#include "keypad.h"     // dovoto's keypad defines
#include "screenmode.h" // dovoto's screen mode defines


#define STICK 	0x01
#define PADDLE 	0x02
#define KEYPAD  0x03
int FILE_LEN=0;
BYTE squash=1;
struct resource {
	int rr;
	int debug;
	char bank;
	char pad_sens;
	char sc;
	char autoconfig;
	char left;
	char right;
	char swap;
} app_data;


/* The mainloop from cpu.c */
extern void mainloop (void);

/* The main entry point */
void C_Entry(void)
{
//	int i;
	char file[50];
	SetMode(MODE_3 | BG2_ENABLE | OBJ_ENABLE | OBJ_MAP_1D); // Sets the screen to mode 3 [a 240*160 array of 15-bit colors [5 bits per R,G,B = 0-31 each]], turns on background 2 [the only one accessible in bitmap mode], turns on sprites, sets sprites access mode to 1d 
	
//	char attr[50],val[50];
//	char *p;

//	clrscr();
/*	printf("                         Virtural VCS  ver 0.60\n");
	printf("                      copyright 1996 by Daniel Boris\n");
	printf("                ported from X2600 copyright 1996 by Alex Hornby\n");
	printf("      This software is released under the terms of the GNU Public License\n");
	printf("\nVirtual VCS starting...\n");
	if (argc < 2) {
		printf("\nUse: vcs [file] [options]");
		printf("  -bank=#: Set bank select mode\n");
		printf("  -debug:  Start in debug mode\n");
		printf("  -r=#: Set frame rate\n");
		printf("  -left=(STICK,PADDLE,KEYPAD): Set left controller\n");
		printf("  -right=(STICK,PADDLE,KEYPAD): Set right controller\n");
		printf("  -sens=#: Set mouse sensitivity for paddle emulation\n");
		printf("  -auto: Autoconfigure from config file\n");
		printf("  -swap: Swap left and right joystick keys\n");
		exit(0);
	} */

	//app_data.rr=1;
	app_data.debug=0;
	app_data.bank=0;
	app_data.pad_sens=5;
	app_data.sc=0;
	app_data.left=STICK;
	app_data.right=STICK;
	paddle1.pos=0;
	paddle1.val=0;
	app_data.autoconfig=0;
	app_data.swap=0;
	//app_data.autoconfig=1;
	app_data.rr=5;
	//app_data.left=KEYPAD;
	/* if (argc > 1) {
		for(i=1; i<argc; i++) {
			if (argv[i][0] != 45) 	{
				strcpy(file,argv[i]);
			}
			else
			{
				p=strtok(argv[i],"=");
				strcpy(attr,p);
				p=strtok(NULL,"=");
				strcpy(val,p);
				if (!strcmpi(attr,"-swap")) {
					app_data.swap=1;
				}
				if (!strcmpi(attr,"-auto")) {
					app_data.autoconfig=1;
				}
				if (!strcmpi(attr,"-sens")) {
					sscanf(val,"%d",&app_data.pad_sens);
				}
				if (!strcmpi(attr,"-left")) {
					if (!strcmpi(val,"stick")) app_data.left=STICK;
					if (!strcmpi(val,"paddle")) app_data.left=PADDLE;
					if (!strcmpi(val,"keypad")) app_data.left=KEYPAD;
				}
				if (!strcmpi(attr,"-right")) {
					if (!strcmpi(val,"stick")) app_data.right=STICK;
					if (!strcmpi(val,"paddle")) app_data.right=PADDLE;
					if (!strcmpi(val,"keypad")) app_data.right=KEYPAD;
				}
				if (!strcmpi(attr,"-bank")) {
					if (!strcmpi(val,"F6")) app_data.bank=1;
					if (!strcmpi(val,"F8")) app_data.bank=2;
					if (!strcmpi(val,"E0")) app_data.bank=4;
					if (!strcmpi(val,"FA")) app_data.bank=3;
					if (!strcmpi(val,"F6SC")) {
						app_data.bank=1;
						app_data.sc=1;
					}
				}
				if (!strcmpi(attr,"-debug")) {
					app_data.debug=1;
				}
				if (!strcmpi(attr,"-r")) {
					sscanf(val,"%d",&app_data.rr);
				}
			}
		}
	} */

	/* load image */
	/*while(FILE_LEN==0)
	{
		if(!((*KEYS) & KEY_A)) {
			FILE_LEN=PACMAN_LEN;
			
		
	}*/
	while(1){
	
	loadCart(file);

//	printf("\n\nPress any key to start.\n");
//	while(!kbhit()){};
//	getch();

	/* start cpu */
	mainloop();
}
	/* Thats it folk's */
	exit(0);
}
void * memcpy(void * dest,const void *src,size_t count)
 {
 	char *tmp = (char *) dest, *s = (char *) src;
 	while (count--)
 		*tmp++ = *s++;
 	return dest;
 }