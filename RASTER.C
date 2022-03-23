/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.
   Modified 1996 by Daniel Boris

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

******************************************************************************/

/* Raster graphics procedures */

#include <stdio.h>
#include "types.h"
#include "address.h"
#include "vmachine.h"
#include "display.h"
#include "resource.h"
#include "debug.h"
#include "config.h"
#include "collisio.h"
typedef unsigned short u16;
extern u16 *screen;
extern BYTE squash;
/* Playfield screen position */
BYTE *pf_pos;
unsigned int line_ptr;
BYTE bkc;
//BYTE vline=0;
BYTE bscreen[62000];
extern int color[255];
void dodebug() {
		//Set_Old_Int9();
		//setmode(TEXT_MODE);
		app_data.debug=1; 
}

/* Place a playfield pixel */
static void tv_pfpixel1(void)
{
/*	*(pf_pos++)|=PF_MASK;
	*(pf_pos++)|=PF_MASK;
	*(pf_pos++)|=PF_MASK;
	*(pf_pos++)|=PF_MASK; */

}

/* Draw playfield register PF0 */
void draw_pf0(struct PlayField *pf, int dir) {
	int pfm; /* playfield mask */
			 /* 1=forward */

	if(dir) {
		for(pfm=0x10;pfm<0x100;pfm<<=1){
			if (pf->pf0 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
	}
	else
	{
		for(pfm=0x80;pfm >0x08; pfm>>=1){
			if(pf->pf0 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
	}
}

/* Draw playfield register PF1 */
void draw_pf1(struct PlayField *pf, int dir) {
	int pfm; /* playfield mask */
			 /* 1=forward */

	if(dir) {
		/* do PF1 */
		for(pfm=0x80; pfm>0; pfm>>=1){
			if(pf->pf1 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
		} else {
		/* do PF1 */
		for(pfm=0x01;pfm < 0x100;pfm<<=1){
			if(pf->pf1 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
	}
}

/* Draw playfield register PF2 */
void draw_pf2(struct PlayField *pf, int dir) {
	int pfm; /* playfield mask */
			 /* 1=forward */

	if(dir) {
		/* do PF2 */
		for(pfm=0x01;pfm<0x100; pfm<<=1){
			if (pf->pf2 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
	}
	else {
		for(pfm=0x80;pfm>0; pfm>>=1){
			if (pf->pf2 & pfm) {
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
				*(pf_pos++)|=PF_MASK;
			}
			else {
				pf_pos+=4;
			}
		}
	}
}

/* Update from the playfield display list */
void pf_update( int num, int nextx, int *pfc, int pf_max)
{
	for( ;(*pfc < pf_max) && (nextx+3 > pf_change[num][*pfc].x ); (*pfc)++) {
	use_pfraster_change( &pf[num], &pf_change[num][*pfc]);
	}
}

/* Draw the playfield */
void draw_playfield(void)
{
	const int num=0; /* Stick to one playfield */
	int pfc=0;
	int pf_max;

	pf_max=pf_change_count[num];
	pf_pos=colvect;
	/* First half of playfield */

	pf_update(num, 0, &pfc , pf_max);
	draw_pf0( &pf[0], 1);
	pf_update(num, 16, &pfc , pf_max);
	draw_pf1( &pf[0], 1);
	pf_update(num, 48, &pfc , pf_max);
	draw_pf2( &pf[0], 1);

	pf_update(num, 80, &pfc , pf_max);
	/* Second half of playfield */
	if(pf[0].ref) {
	draw_pf2( &pf[0], 0);
	pf_update(num, 112, &pfc , pf_max);
	draw_pf1( &pf[0], 0);
	pf_update(num, 144, &pfc , pf_max);
	draw_pf0( &pf[0], 0);
	} else {
	draw_pf0( &pf[0], 1);
	pf_update(num, 96, &pfc , pf_max);
	draw_pf1( &pf[0], 1);
	pf_update(num, 128, &pfc , pf_max);
	draw_pf2( &pf[0], 1);
	}
	/* Use last changes */
	for( ; pfc < pf_max; pfc++)
	use_pfraster_change( &pf[num], &pf_change[num][pfc]);

	pf_change_count[num]=0;
}

/* Draws a normal (8 clocks) sized player */
void pl_normal( struct Player *p, int x)
{
	/* Set pointer to start of player graphic */
	BYTE *ptr;
	BYTE mask;
	BYTE gr;

	if (p->vdel_flag)
		gr = p->vdel;
	else
		gr = p->grp;

	ptr=colvect+x;
	if(p->reflect) {
		/* Reflected: start with D0 of GRP on left */
		for(mask=0x01;mask>0;mask<<=1) {
			if(gr & mask) {
				*(ptr++)|=p->mask;
			} else
				ptr++;
		}
	}
	else {
		/* Unreflected: start with D7 of GRP on left */
		for(mask=0x80;mask>0;mask>>=1){
			if(gr & mask){
				*(ptr++)|=p->mask;
			} else
				ptr++;
		}
	}
}

/* Draws a double width ( 16 clocks ) player */
void pl_double(struct Player *p, int x)
{
	/* Set pointer to start of player graphic */
	BYTE *ptr;
	BYTE mask;
	BYTE gr;

	if (p->vdel_flag)
		gr = p->vdel;
	else
		gr = p->grp;


	ptr=colvect+(x);
	if(p->reflect) {
	for(mask=0x01;mask>0;mask<<=1){
		if(gr & mask){
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		} else
		ptr+=2;
	}
	}
	else {
	for(mask=0x80;mask>0;mask>>=1){
		if(gr & mask){
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		} else
		ptr+=2;
	}
	}
}

/* Draws a quad sized ( 32 clocks) player */
void pl_quad(struct Player *p, int x)
{
	/* Set pointer to start of player graphic */
	BYTE *ptr;
	BYTE mask;
	BYTE gr;

	if (p->vdel_flag)
		gr = p->vdel;
	else
		gr = p->grp;

	ptr=colvect+x;
	if(p->reflect) {
	for(mask=0x01;mask>0;mask<<=1){
		if(gr & mask){
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		} else
		ptr+=4;
	}
	}
	else {
	for(mask=0x80;mask>0;mask>>=1){
		if(gr & mask){
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		*(ptr++)|=p->mask;	  *(ptr++)|=p->mask;
		} else
		ptr+=4;
	}
	}
}



/* Consume the player display list */
void pl_update( int num, int nextx, int *plc, int pl_max)
{
	for( ;(*plc < pl_max) && (nextx > pl_change[num][*plc].x); (*plc)++) {
	use_plraster_change( &pl[num], &pl_change[num][*plc]);
	}
}

/* Draw a player */
void pl_draw1(int line,int num)
{
	int plc=0;
	int pl_max=pl_change_count[num];
	int nextx;

		pl_update(num, pl[num].x, &plc , pl_max);
	if(pl[num].x>=0 ) {

	/*if(pl_max > plc)
		use_plraster_change( &pl[num], &pl_change[num][plc++]);*/
	switch(pl[num].nusize){
	case 0:
		/* One copy */
		pl_normal(&pl[num], pl[num].x);
		break;
	case 1:
		/* Two copies close */
		pl_normal(&pl[num], pl[num].x);
		nextx=pl[num].x + 8 +8;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);
		break;
	case 2:
		/* Two copies medium */
		pl_normal(&pl[num], pl[num].x);
		nextx=pl[num].x + 8 +24;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);
		break;
	case 3:
		/* Three copies close */
		/* Pacman score line */
		pl_normal(&pl[num], pl[num].x);

		nextx=pl[num].x + 16;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);

		nextx=pl[num].x + 32;
		pl_update(num, nextx, &plc , pl_max);

		pl_normal(&pl[num], nextx);
		break;
	case 4:
		/* Two copies wide */
		pl_normal(&pl[num], pl[num].x);
		nextx=pl[num].x + 8 +56;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);
		break;
	case 5:
		/* Double sized player */
		pl_double(&pl[num], pl[num].x);
		break;
	case 6:
		/* Three copies medium */
		pl_normal(&pl[num], pl[num].x);
		nextx=pl[num].x + 8 +24;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);
		nextx=pl[num].x + 8 +56;
		pl_update(num, nextx, &plc , pl_max);
		pl_normal(&pl[num], nextx);
		break;
	case 7:
		/* Quad sized player */
		pl_quad(&pl[num], pl[num].x);
		break;
	}
	}

	/* Use last changes */
	for( ; plc < pl_max; plc++)
	use_plraster_change( &pl[num], &pl_change[num][plc]);
	pl_change_count[num]=0;
}


/* Draw the ball */
static void draw_ball(int line){
	int i;
	BYTE *blptr;
	BYTE e;

	if (ml[2].vdel_flag)
		e = ml[2].vdel;
	else
		e = ml[2].enabled;


	if(e && ml[2].x >= 0 ){
		blptr=colvect+(ml[2].x);
		switch(tiaWrite[CTRLPF] >> 4) {
		case 3:
			/* Eight clocks */
			for(i=0;i<8;i++)
			*(blptr++)|=BL_MASK;
			break;
		case 2:
			/* Four clocks */
			for(i=0;i<4;i++)
			*(blptr++)|=BL_MASK;
			break;
		case 1:
			/* Two clocks */
			for(i=0;i<2;i++)
			*(blptr++)|=BL_MASK;
			break;
		case 0:
			/* One clock */
			*(blptr++)|=BL_MASK;
			break;
		}
	}
}

void do_missile(int num, BYTE *misptr)
{
	int i;

	switch( ml[num].width) {
		case 0:
			/* one clock */
			*(misptr++)|=ml[num].mask;
			break;
		case 1:
			/* two clocks */
			for(i=0;i<2;i++)
			*(misptr++)|=ml[num].mask;
			break;
		case 2:
			/* four clocks */
			for(i=0;i<4;i++)
			*(misptr++)|=ml[num].mask;
			break;
		case 3:
			/* Eight clocks */
			for(i=0;i<8;i++)
			*(misptr++)|=ml[num].mask;
			break;
	}   /* switch */
}

/* Draw the missile */
static void draw_missile(int line, int num){
	int i;
	BYTE *misptr;

	if(ml[num].enabled && ml[num].x>=0){
		switch(pl[num].nusize){
		case 0:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			break;
		case 1:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			misptr=misptr+16;
			do_missile(num,misptr);
			break;
		case 2:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			misptr=misptr+32;
			do_missile(num,misptr);
			break;
		case 3:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			misptr=misptr+16;
			do_missile(num,misptr);
			misptr=misptr+16;
			do_missile(num,misptr);
			break;
		case 4:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			misptr=misptr+64;
			do_missile(num,misptr);
			break;
		case 5:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			break;
		case 6:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			misptr=misptr+32;
			do_missile(num,misptr);
			misptr=misptr+32;
			do_missile(num,misptr);
			break;
		case 7:
			misptr=colvect+(ml[num].x);
			do_missile(num,misptr);
			break;

		}

	} /* If */
}


/* Draw one tv raster line */
void tv_raster1( int line )
{
	if ( squash) line_ptr=35+line*vwidth/2;
	else line_ptr=vwidth*line;
	bkc=tiaWrite[COLUBK];

	if(tiaWrite[CTRLPF] & 0x02) {
	/* Alterate priority */
	draw_missile(line,1);
	pl_draw1(line,1);

	/* Do the missiles */
	draw_missile(line,0);
	pl_draw1(line,0);

	/* Do the ball */
	draw_ball(line);

	/* Do the playfield */
	draw_playfield();
	} else {
	/* Normal Priority */
	draw_playfield();

	/* Do the ball */
	 draw_ball(line);

	/* Do the player 1 graphics */
	draw_missile(line,1);
	pl_draw1(line,1);

	/* Do the player 0 graphics */
	draw_missile(line,0);
	pl_draw1(line,0);
}
}

/* Reset the collision vector */
void reset_vector(void)
{
	BYTE i=0;
	while(i<160) colvect[i++]=0;
}

/* draw the collision vector */

void draw_vector1(void)
{
	int i;
	int uct=0;
	BYTE pad=0;
	unsigned int tv_ptr;

	if ( squash) tv_ptr=line_ptr+240*25;
	else tv_ptr=line_ptr;
	
	/* Use starting changes */
	while(uct < unified_count && unified[uct].x<0)
	
	use_unified_change(&unified[uct++]);

	for(i=0; i< tv_width; i++) {

	if(uct < unified_count && unified[uct].x==i)
		use_unified_change(&unified[uct++]);
	if (coltable[colvect[i]]) set_collisions(colvect[i]);
	if(tiaWrite[CTRLPF] & 0x04) {
		if(colvect[i] & (BL_MASK | PF_MASK)) {
			if (tiaWrite[CTRLPF] & 0x02) {
				pad=pl[0].col;
				if (i > 79) pad=pl[1].col;
			}
			else
					{
				pad=pf[0].col;
			}
		}
		else if(colvect[i] & (PL0_MASK | ML0_MASK))
			pad=pl[0].col;
		else if(colvect[i] & (PL1_MASK | ML1_MASK))
			pad=pl[1].col;
		else
			pad=tiaWrite[COLUBK];
	} else {
		/* Normal Priority */
		if(colvect[i] & (PL0_MASK | ML0_MASK))
			pad=pl[0].col;
		else if(colvect[i] & (PL1_MASK | ML1_MASK))
			pad=pl[1].col;
		else if(colvect[i] & (BL_MASK | PF_MASK)) {
			if(colvect[i] & (BL_MASK | PF_MASK)) {
				if (tiaWrite[CTRLPF] & 0x02) {
					pad=pl[0].col;
					if (i > 79) pad=pl[1].col;
				}
				else
				{
					pad=pf[0].col;
				}
			}
		}
		else
			pad=tiaWrite[COLUBK];
	}

	if ( bscreen[tv_ptr]!=pad) {
		bscreen[tv_ptr]=pad;
		screen[tv_ptr]=color[pad];
		}
	tv_ptr++;
	}
	while(uct < unified_count)
	use_unified_change(&unified[uct++]);
	unified_count=0;
}

/* Used for when running in frame skipping mode */
static void update_registers(void)
{
	int i,num;

	/* Playfield */
	for(i=0 ; i < pf_change_count[0]; i++)
	use_pfraster_change( &pf[0], &pf_change[0][i]);
	pf_change_count[0]=0;

	/* Player graphics */
	for(num=0; num < 2; num ++) {
	for(i=0 ; i < pl_change_count[num]; i++)
		use_plraster_change( &pl[num], &pl_change[num][i]);
		pl_change_count[num]=0;
	}

	/* Unified */
	for(i=0; i < unified_count; i++)
	use_unified_change(&unified[i]);
	unified_count=0;
}



/* Main raster function, will have switches for different magsteps */
void tv_raster( int line ){

	int i;

	keyboard();
	if(tv_counter % app_data.rr != 0) {
		update_registers();
		return;
	}
	reset_vector();
	tv_raster1(line);
	
	
	if ( line < 200 ) {
		
	if (!(squash && (line %2))) draw_vector1();
		
	
	}
}






