/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Daniel Boris

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

   $Id: xdebug.c,v 2.5 1996/03/21 16:36:01 alex Exp $
******************************************************************************/

/*
 * This code has been almost complete re-written since X2600
 *
 */



#include <stdio.h>

#include "cpu.h"
#include "macro.h"
#include "vmachine.h"	/* I_SPECX */
#include "extern.h"
#include "misc.h"
#include "memory.h"
#include "display.h"
#include "config.h"
#include "address.h"

/*
 * Variables
 */

#define VGA256		0x13
#define TEXT_MODE	0x03


int debugf_halt=0, debugf_trace=0, debugf_raster=0;

static ADDRESS  current_point;
static char     buf[256];
static ADDRESS  brk=0;
static ADDRESS  dPC;

enum Register {
	R_AC, R_XR, R_YR, R_SP, R_PC, R_SR, R_EFF, R_DL, R_CLK, NO_REGISTERS
};

static char *registerLabels[] = {
	"AC:", "XR:", "YR:", "SP:", "PC:", "SR:", "EA>", "DL>", "Clk"
};


/*
 * Functions
 */

extern void  debug_main ( void );


static void UpdateRegisters ( void );
static void UpdateHardware ( void ); /* New */
static void UpdateStatus ( enum DState );
static void UpdateButtons ( void );

static void  set_asm ( ADDRESS );
static void  draw_asm ( ADDRESS );

/* ------------------------------------------------------------------------- */

void set_single(void)
{
	debugf_halt=1;
}

void  x_loop(void)
{
	char c,cc;
	int done;
	unsigned int d;

   if (PC == brk) {
		brk=0;
	}
	else
	{
		gotoxy(30,6);
		printf ("PC: $%04X", PC);
		done=1;
	}

	dPC=PC;
	if (!brk) {
		clrscr();
		UpdateRegisters();
		UpdateHardware();
		set_asm(PC);
		done=0;
	}
	do {
		if (kbhit()) {
			set_asm(dPC);
			c=getch();
			switch (c) {
				case 0:
					cc=getch();
					switch (cc) {
						case 72:
							dPC--;
							break;
						case 80:
							dPC+=clength[lookup[DLOAD(dPC)&0xff].addr_mode];
							break;
					}
					break;
				case 'B':
				case 'b':
					cc=1;
					break;
				case 'D':
				case 'd':
					setmode(VGA256);
					create_cmap();
					put_image();
					while(!kbhit()){};
					setmode(TEXT_MODE);
					UpdateRegisters();
					UpdateHardware();
					set_asm(PC);
					getch();
					break;
				case 'Q':
				case 'q':
					tv_off();
					exit(0);
				case 's':
				case 'S':
					done=1;
					break;
				case 'm':
				case 'M':
					gotoxy(3,23);
					printf("Address: ");
					scanf("%x",&d);
					gotoxy(18,23);
					printf("= %x   ",dbgRead(d));
					break;
				case 'g':
				case 'G':
					gotoxy(1,23);
					printf("Address to run to: ");
					scanf("%x",&brk);
					gotoxy(1,23);
					printf("                  ");
					done=1;
					break;
			}
		}
	} while (!done);

}

static void UpdateHardware  (void)
{
	int temp;

	gotoxy(50,1);
	printf ("ebeamx: %04d", ebeamx);
	gotoxy(50,2);
	printf ("ebeamy: %04d", ebeamy);
	gotoxy(50,3);
	printf ("CTRLPF: %04d", tiaWrite[CTRLPF]);
	gotoxy(50,4);
	printf ("timer: %04d", timer_count);
	gotoxy(50,5);
	printf ("INTIM: %06d", riotRead[INTIM]);
	gotoxy(50,6);
	printf ("timer_res: %06d", timer_res);
	gotoxy(50,7);
	printf ("timer_clks: %06d", timer_clks);
	gotoxy(50,8);
	temp= pl[0].vdel;
	printf ("Pl0 vdel: %02x", temp);
	gotoxy(50,9);
	temp= pl[1].vdel;
	printf ("Pl1 vdel: %02x", temp);
	gotoxy(50,10);
	temp= pl[0].grp;
	printf ("Pl0 grp: %02x", temp);
	gotoxy(50,11);
	temp= pl[1].grp;
	printf ("Pl1 grp: %02x", temp);
	gotoxy(50,12);
	temp= pl[0].x;
	printf ("Pl0 x: %03d", temp);
	gotoxy(50,13);
	temp= pl[1].x;
	printf ("Pl1 x: %03d", temp);
	gotoxy(50,14);
	temp= ml[0].x;
	printf ("Ml0 x: %03d", temp);
	gotoxy(50,15);
	temp= ml[1].x;
	printf ("Ml1 x: %03d", temp);

}

static void UpdateRegisters (void)
{
	int eff;

	gotoxy(30,2);
	printf ("AC: $%02X", AC);
	gotoxy(30,3);
	printf ("XR: $%02X", XR);
	gotoxy(30,4);
	printf ("YR: $%02X", YR);
	gotoxy(30,5);
	printf ("SP: $%02X", SP);
	gotoxy(30,6);
	printf ("PC: $%04X", PC);
	gotoxy(30,7);
	printf ("SR: $%02X", GET_SR());
	gotoxy(30,8);
	/* Memory */

	eff = eff_address(PC, 1);
	if (eff >= 0)
	printf ("EA: $%04X", eff);
	else
	printf ("EA: ----");

	gotoxy(30,9);
	if (eff >= 0)
	printf ("DL: $%02X", DLOAD(eff));
	else
	printf ("DL: --");

	/* Clock */
	gotoxy(30,10);
	printf ("CL: %ld", clk);
}


/* Debugger  Status */

static void UpdateStatus (enum DState dst)
{
	static int s = -1;
	if (s == (int)dst)
	return;

	s = dst;
}


/* ------------------------------------------------------------------------- */

/*
 * Assembly Window
 */


void set_asm (ADDRESS p)
{
	draw_asm (p);
}


void draw_asm (ADDRESS p)
{
	char    buf2[8192];
	short   lines, height;  /* NOT ints */
	int i,l;
	BYTE j,j1,j2;

	buf2[0] = '\0';
	current_point = p;
	lines=20;
	for (i = 0; i <= lines; i++) {
		p &= 0xffff;
		sprintf (buf, "%04X %s  %-s", p, sprint_ophex (p), sprint_opcode(p, 1));
		l=strlen(buf);
		if (l < 28) {
			for(j=0; j < (28-l); j++) strcat(buf," ");
		}
		gotoxy(1,i+1);
		printf("%s",buf);
		p+=clength[lookup[DLOAD(p)&0xff].addr_mode];

	}

}













