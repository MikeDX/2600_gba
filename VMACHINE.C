/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.
   Modified 1996 by Daniel Boris

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

******************************************************************************/

/*
   The virtual machine. Contains the RIOT timer code, and hardware
   initialisation.
*/

#include <stdio.h>
#include "types.h"
#include "address.h"
#include "resource.h"
#include "display.h"
#include "raster.h"
#include "cpu.h"
#include "misc.h"
#include "collisio.h"
#include "mouse.h"

/* The Rom define might need altering for paged carts */
BYTE cartram[256];
BYTE cart[16384];
BYTE theRom[4096];
BYTE theRam[128];
BYTE tiaRead[0x0e];
BYTE tiaWrite[0x2d];

/* These don't strictly need so much space */
BYTE riotRead[0x298];
BYTE riotWrite[0x298];

/* 
Hardware addresses not programmer accessible 
*/

/* Set if processor is reset */
int reset_flag=0;

/* The timer resolution, can be 1,8,64,1024 */
int timer_res=32;
int timer_count=0;
int timer_clks=0;
extern CLOCK clk;
extern int beamadj;

/* Electron beam position */
int ebeamx, ebeamy, sbeamx;

/* The state of the electron beam */
#define VSYNCSTATE 1
#define VBLANKSTATE 2
#define HSYNCSTATE 4
#define DRAWSTATE 8
#define OVERSTATE 16
int vbeam_state; /* 1 2 8 or 16 */
int hbeam_state; /* 4 8 or 16 */

/* The tv size, varies with PAL/NTSC */
int tv_width, tv_height, tv_vsync, tv_vblank,
    tv_overscan, tv_frame, tv_hertz, tv_hsync;

/* The PlayField structure */
struct PlayField {
	BYTE pf0,pf1,pf2,ref,col;
} pf[2];  

BYTE keypad[8];

struct Paddle {
	long pos;
	long val;
} paddle1;

/* The player variables */
struct Player {
    int x;
    BYTE grp;
	BYTE hmm;
	BYTE vdel;
	BYTE vdel_flag;
	BYTE col;
	BYTE nusize;
	BYTE reflect;
	BYTE mask;
} pl[2];

/* The element used in display lists */
struct RasterChange {
    int x;     /* Position at which change happened */
    int type;  /* Type of change */
	int val;   /* Value of change */
};

/* The various display lists */
struct RasterChange pl_change[2][80], pf_change[1][80], unified[80];

/* The display list counters */
int pl_change_count[2], pf_change_count[1], unified_count;

/* The missile an ball positions */
struct Missile {
    int x;
	BYTE hmm;
    BYTE enabled;
    BYTE width;
	BYTE vdel;
	BYTE vdel_flag;
    BYTE col;
	BYTE mask;
} ml[3];

/* Device independent screen initialisations */
void init_screen(void)
{
	/* Set the electron beam to the top left */
	ebeamx=-tv_hsync; ebeamy=0;
	vbeam_state=VSYNCSTATE;
	hbeam_state=OVERSTATE;

	tv_vsync=3;
	tv_hsync=68;

	tv_width=160; tv_height=192;
	tv_vblank=40; tv_overscan=30;
	tv_frame=262; tv_hertz=60;

}

/* Initialise the RIOT */
void init_riot(void)
{
	int i;
	/* Wipe the RAM */
	for(i=0;i<0x80;i++) theRam[i]=0;

	/* Set the timer to zero */
	riotRead[INTIM]=0;

	/* Set the joysticks and switches to input */
	riotWrite[SWACNT]=0;
	riotWrite[SWBCNT]=0;

	/* Centre the joysticks */
	riotRead[SWCHA]=0xff;
	riotRead[SWCHB]=0x0b;

	/* Set the counter resolution */
	timer_res=32;
	timer_count=0;
	timer_clks=0;
}

/* Initialise the television interface adaptor (TIA) */
void init_tia(void)
{
	int i;
	tiaWrite[CTRLPF]=0x00;
	for(i = 0; i < 2; i++){
		pl[i].col = 0xf2;
		pl[i].hmm = 0x0;
		pl[i].x = 0x0;
		pl[i].nusize=0;
		pl[i].grp=0;
		pl[i].vdel=0;
		pl[i].vdel_flag=0;
		pl_change_count[i]=0;
	}
	pl[0].mask=PL0_MASK;
	pl[1].mask=PL1_MASK;
	ml[0].mask=ML0_MASK;
	ml[1].mask=ML1_MASK;
	reset_collisions();

	pf_change_count[0]=0;
	unified_count=0;
	for(i=0;i<3;i++){
	ml[i].enabled=0;
	}

	pl[1].col=0xf4;
	tiaWrite[COLUPF]=0xf6;
	tiaWrite[COLUBK]=0x12;
	tiaWrite[VBLANK]=0;
	tiaRead[INPT4]=0x80;
	tiaRead[INPT5]=0x80;
}


/* Main hardware startup */
void init_hardware(void)
{
	int i;

	init_screen();
	init_riot();
	init_tia();
	for(i=0; i<8; i++) keypad[i]=0;
	if (app_data.left == PADDLE) {
		init_mouse();
		mouse_sensitivity(app_data.pad_sens*10);
	}

	init_cpu(0xfffc);
}


/* Do a raster change */
void do_raster_change(int i, int type, int val, struct RasterChange *rc)
{
	rc->x=ebeamx+beamadj;
	rc->type=type;
	rc->val=val;
}

/* Do a raster change on the unified list */
void do_unified_change(int type, int val)
{
	unified[unified_count].x=ebeamx+beamadj;
	unified[unified_count].type=type;
	unified[unified_count].val=val;
	unified_count++;
}

/* Do a player raster change */
void do_plraster_change(int i, int type, int val)
{
	int plc=pl_change_count[i];

	if (plc < 81) {
		do_raster_change(i, type, val, &pl_change[i][plc]);
		if (type == 1) pl_change[i][plc].x-=3;
		pl_change_count[i]++;
	}
}

/* Do a playfield raster change */
void do_pfraster_change(int i, int type, int val)
{
	int pfc=pf_change_count[i];
	/*
	if(ebeamy>=100) {
	printf("Raster change i=%d, x=%d, type=%d, val=%d\n",
	i, ebeamx+beamadj, type, val);
	show();
	}
	*/
	if (pfc < 81) {
		do_raster_change(i, type, val, &pf_change[i][pfc]);
		pf_change_count[i]++;
	}
}


/* Use a unified change */
void use_unified_change( struct RasterChange *rc)
{
	switch(rc->type) {
	case 0:
		/* PL0 colour */
		pl[0].col=rc->val;
		break;
	case 1:
		/* PL1 colour */
		pl[1].col=rc->val;
		break;
	case 2:
		/* PF colour */
		pf[0].col=rc->val;
		break;
	case 3:
		/* BK colour */
		tiaWrite[COLUBK]=rc->val;
		break;
	}
}

/* Use a playfield change */
void use_pfraster_change( struct PlayField *pl, struct RasterChange *rc)
{
	switch(rc->type) {
	case 0:
		/* PF0 */
		pl->pf0=rc->val;
		break;
	case 1:
		/* PF1 */
		pl->pf1=rc->val;
		break;
	case 2:
		/* PF2 */
		pl->pf2=rc->val;
		break;
	case 3:
		/* Reflection */
		pl->ref=rc->val;
		break;
	}
}

/* Use a player change */
void use_plraster_change( struct Player *pl, struct RasterChange *rc)
{
	switch(rc->type) {
	case 0:
		/* GRP */
		pl->grp=rc->val;
		break;
	case 1:
		pl->vdel=pl->grp;
		break;
	}
}

/*
   Called when the timer is set .
   Note that res is the bit shift, not absolute value.
   Assumes that any timer interval set will last longer than the instruction
   setting it.
*/
void set_timer(int res, int count, int clkadj)
{
	timer_count=count<<res;
	timer_clks=clk+clkadj;
	timer_res=res;
}

/* New timer code, now only called on a read of INTIM */
BYTE do_timer(int clkadj)
{
	BYTE result;
	int delta;
	int value;

	delta=clk-timer_clks;
	value=value-(delta >> timer_res);
	if(delta<=timer_count)	{
		/* Timer is still going down in res intervals */
		result=value;
	}
	else {
	if(value == 0)
		/* Timer is in holding period */
		result=0;
	else {
		/* Timer is descending from 0xff in clock intervals */
		set_timer( 0, 0xff, clkadj);
		result=0;
	}
	}

	/*  printf("Timer result=%d\n", result);*/
	return result;
}


/* Main screen logic */
void do_screen(int clks)
{
	switch (vbeam_state) {
		case VSYNCSTATE:
		case VBLANKSTATE:
		switch(hbeam_state) {
			case HSYNCSTATE:
				ebeamx+=clks*3;
				if(ebeamx>=0){
					hbeam_state=DRAWSTATE;
				}
				break;
			case DRAWSTATE:
				ebeamx+=clks*3;
				if( ebeamx >= tv_width ){
					ebeamx-=(tv_hsync+tv_width);
							tv_raster(ebeamy);

					/* Insert hsync stuff here */
					sbeamx=ebeamx;
					hbeam_state=HSYNCSTATE;
				}
				break;
			case OVERSTATE:
				break;
		}
		break;
		case DRAWSTATE:
			switch(hbeam_state) {
			case HSYNCSTATE:
				ebeamx+=clks*3;
				if(ebeamx>=0){
					hbeam_state=DRAWSTATE;
					/* Copy across left playfield mask to right playfield */
					pf[1].pf0=pf[0].pf0;
					pf[1].pf1=pf[0].pf1;
					pf[1].pf2=pf[0].pf2;
					pf[1].col=pf[0].col;
			}
			break;
			case DRAWSTATE:
				ebeamx+=clks*3;
				if( ebeamx >= tv_width ){
					/* Insert hsync stuff here */
					sbeamx=ebeamx;
					ebeamx-=(tv_hsync+tv_width);
					tv_raster(ebeamy);
					ebeamy++;
					hbeam_state=HSYNCSTATE;
				}
				if( ebeamy >= tv_height + tv_overscan){
					vbeam_state=OVERSTATE;
					ebeamy=0;
				}
				break;
			case OVERSTATE:
			break;
		}
		break;
	case OVERSTATE:
		break;
	}
}





