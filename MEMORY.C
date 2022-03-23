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
 * Holds the memory access routines to both memory and memory mapped
 * i/o, hence memory.c
 *
*/

#include <stdio.h>
#include "types.h"
#include "address.h"
#include "vmachine.h"
#include "misc.h"
#include "display.h"
#include "raster.h"
#include "collisio.h"
#include "resource.h"
#include "mouse.h"

extern CLOCK clkcount;
extern CLOCK clk;
extern int beamadj;

/* UnDecoded Read */
BYTE undecRead (ADDRESS a) {
	if(a & 0x1000)
		return theRom[a & 0xfff];
	else
		return theRam[a & 0x7f];
}

/* Decoded write */
void decWrite ( ADDRESS a, BYTE b) {

	int i;
	ADDRESS a1;

	if (a & 0x1000) {
		a=a & 0xfff;
		if (app_data.bank == 3) {
			if (a < 0x100) cartram[a]=b;
		}

		if (app_data.sc) {
			if (a < 0x7f) cartram[a] = b;
		}
		switch (app_data.bank) {
			case 1:
				if (a == 0xff6) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff7) memcpy(&theRom[0],&cart[4096],4096);
				if (a == 0xff8) memcpy(&theRom[0],&cart[8192],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[12288],4096);
				break;
			case 2:
				if (a == 0xff8) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[4096],4096);
				break;
			case 3:
				if (a == 0xff8) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[4096],4096);
				if (a == 0xffa) memcpy(&theRom[0],&cart[8192],4096);
				break;

			case 4:
				if (a > 0xfdf && a < 0xfe8) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0],&cart[a1],0x400);
				}
				if (a > 0xfe7 && a < 0xff0) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0x400],&cart[a1],0x400);
				}
				if (a > 0xfef && a < 0xff8) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0x800],&cart[a1],0x400);
				}
				break;

		}
		return;
	}

	/* RAM, mapped to page 0 and 1*/
	if ((a & 0x280) == 0x80) {
		theRam[a & 0x7f]=b;
		return;
	}

	if (!(a & 0x80)) {
	switch(a & 0x7f){
	/* TIA */
	case VSYNC:
	if(b & 0x02) {
		/* Start vertical sync */
		vbeam_state=VSYNCSTATE;
	}
	break;
	case VBLANK:
		if(b & 0x02) {
		if (!(tiaWrite[VBLANK] & 0x02)) {
			/* Start vertical blank */
			vbeam_state=VBLANKSTATE;
			/* Also means we can update screen */
			/* Insert vysnc stuff here */
			tv_display();
		}
	}
		else {
			/* End vblank, and start first hsync drawing */
			vbeam_state=DRAWSTATE;
			hbeam_state=HSYNCSTATE;
			/* Set up the screen */
			for( i=0; i<unified_count; i++) use_unified_change(&unified[i]);
				/* Hope for a WSYNC, but just in case */
				ebeamx=-tv_hsync;
				ebeamy=0;
		}
		/* Logic for dumped input ports */
		if(b & 0x40){
			/*Enable latches*/
			tiaRead[INPT4]=0x80;
			tiaRead[INPT5]=0x80;
		}
		if(b & 0x80) {
			tiaRead[INPT0]=0x00;
			paddle1.val=0;
		}
		tiaWrite[VBLANK]=b;

		break;
	case WSYNC:
	/* SKIP to HSYNC pulse */
	if( vbeam_state==DRAWSTATE && (ebeamx > -tv_hsync)) {
		tv_raster(ebeamy);
		/* Fix the clock value */
		clk+=(ebeamx-tv_width)/3;
		ebeamy++;
	}
		hbeam_state=HSYNCSTATE;
		ebeamx=-tv_hsync;
		sbeamx=0;
		break;
	case RSYNC:
		break;
	case NUSIZ0:
		pl[0].nusize= b & 0x07;
		ml[0].width= (b & 0x30) >> 4;
		break;
	case NUSIZ1:
		pl[1].nusize= b & 0x07;
		ml[1].width= (b & 0x30) >> 4;
		break;
	case COLUP0:
		do_unified_change( 0, b);
		break;
	case COLUP1:
		do_unified_change( 1, b);
		break;
	case COLUPF:
		do_unified_change( 2, b);
		break;
	case COLUBK:
		do_unified_change( 3, b);
		break;
	case CTRLPF:
		tiaWrite[CTRLPF]=b & 0x37; /* Bitmask 00110111 */
		do_pfraster_change(0,3,b & 0x01); /* Reflection */
		break;
	case REFP0:
		pl[0].reflect=(b &  0x08) >>3;
		break;
	case REFP1:
		pl[1].reflect=(b &  0x08) >>3;
		break;
	case PF0:
		do_pfraster_change(0,0,b & 0xf0);
		break;
	case PF1:
		do_pfraster_change(0,1,b);
		break;
	case PF2:
		do_pfraster_change(0,2,b);
		break;
	case RESP0:
		/* Ghost in pacman! */
		pl[0].x=ebeamx + beamadj;
		if (pl[0].x > 160) pl[0].x=160;
		break;
	case RESP1:
		pl[1].x=ebeamx + beamadj;
		if (pl[1].x > 160) pl[1].x=160;
		break;
	case RESM0:
		ml[0].x=ebeamx + beamadj;
		if (ml[0].x > 160) ml[0].x=160;
		break;
	case RESM1:
		ml[1].x=ebeamx + beamadj;
		if (ml[1].x > 160) ml[1].x=160;
		break;
	case RESBL:
		ml[2].x=ebeamx + beamadj;
		if (ml[2].x > 160) ml[2].x=160;
		break;
	case GRP0:
		do_plraster_change( 0, 0, b);
		do_plraster_change( 1, 1, b);
		break;
	case GRP1:
		do_plraster_change( 1, 0, b);
		do_plraster_change( 0, 1, b);
		ml[2].vdel = ml[2].enabled;
		break;
	case ENAM0:
		ml[0].enabled= b & 0x02;
		if (tiaWrite[RESMP0]) ml[0].enabled=0;
		break;
	case ENAM1:
		ml[1].enabled= b & 0x02;
		if (tiaWrite[RESMP1]) ml[1].enabled=0;
		break;
	case ENABL:
		ml[2].enabled= b & 0x02;
		break;
	case HMP0:
		pl[0].hmm=(b >> 4);
		break;
	case HMP1:
		pl[1].hmm=(b >> 4);
		break;
	case HMM0:
		ml[0].hmm=(b >> 4);
		break;
	case HMM1:
		ml[1].hmm=(b >> 4);
		break;
	case HMBL:
		ml[2].hmm=(b >> 4);
		break;
	case VDELP0:
		pl[0].vdel_flag=b & 0x01;
		break;
	case VDELP1:
		pl[1].vdel_flag=b & 0x01;
		break;
	case VDELBL:
		ml[2].vdel_flag=b & 0x01;
		break;
	case RESMP0:
		tiaWrite[RESMP0]=b & 0x02;
		if (b & 0x02) {
			ml[0].x=pl[0].x+4;
			ml[0].enabled=0;
			if (ml[0].x > 160) ml[0].x=160;
		}
		break;
	case RESMP1:
		tiaWrite[RESMP1]=b & 0x02;
		if (b & 0x02) {
			ml[1].x=pl[1].x+4;
			ml[1].enabled=0;
			if (ml[1].x > 160) ml[1].x=160;
		}
		break;
	case HMOVE:
		if(pl[0].hmm & 0x08)
			pl[0].x+= ((pl[0].hmm ^ 0x0f) +1);
		else
			pl[0].x-=pl[0].hmm;
		if (pl[0].x > 160) pl[0].x=-68;
		if (pl[0].x < -68) pl[0].x=160;

		if(pl[1].hmm & 0x08)
			pl[1].x+= ((pl[1].hmm ^ 0x0f) +1);
		else
			pl[1].x-=pl[1].hmm;
		if (pl[1].x > 160) pl[1].x=-68;
		if (pl[1].x < -68) pl[1].x=160;

		for(i=0; i<3; i++){
			if(ml[i].hmm & 0x08)
				ml[i].x+= ((ml[i].hmm ^ 0x0f)+1);
			else
				ml[i].x-=ml[i].hmm;
			if (ml[i].x > 160) ml[i].x=-68;
			if (ml[i].x < -68) ml[i].x=160;
		}

		break;
	case HMCLR:
		pl[0].hmm=0;
		pl[1].hmm=0;
		for(i=0;i<3;i++)
			ml[i].hmm=0;
		break;
	case CXCLR:
		reset_collisions();
		break;
   }
   }
   else
   {
   switch(a & 0x2ff) {
		/* RIOT I/O ports */
		case SWCHA:
			riotWrite[SWCHA]=b;
			break;
		case SWACNT:
			riotWrite[SWACNT]=b;
			break;
		case SWBCNT:
			break;

		/* Timer ports */
		case TIM1T:
			set_timer( 0, b, clkcount);
			break;
		case TIM8T:
			set_timer( 3, b, clkcount);
			break;
		case TIM64T:
			set_timer( 6, b, clkcount);
			break;
		case T1024T:
			set_timer( 10, b, clkcount);
			break;
		}
	}
}


/* Decoded Read */
BYTE decRead (ADDRESS a)
{
	BYTE res;
	ADDRESS a1;
	int x;

	if (a & 0x1000) {
		a=a & 0xfff;
		res=theRom[a & 0xfff];
		if (app_data.sc) {
			if (a > 0x7f && a < 0x100) {
				res=cartram[a & 0x7f];
				return res;
			}
		}
		if (app_data.bank == 3) {
			if (a > 0xFF && a < 0x200) {
				res=cartram[a & 0xFF];
				return res;
			}
		}
		if (app_data.bank == 0) return res;
		a=a & 0xfff;
		switch (app_data.bank) {
			case 1:
				if (a == 0xff6) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff7) memcpy(&theRom[0],&cart[4096],4096);
				if (a == 0xff8) memcpy(&theRom[0],&cart[8192],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[12288],4096);
				break;
			case 2:
				if (a == 0xff8) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[4096],4096);
				break;
			case 3:
				if (a == 0xff8) memcpy(&theRom[0],&cart[0],4096);
				if (a == 0xff9) memcpy(&theRom[0],&cart[4096],4096);
				if (a == 0xffa) memcpy(&theRom[0],&cart[8192],4096);
				break;

			case 4:
				if (a > 0xfdf && a < 0xfe8) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0],&cart[a1],0x400);
				}
				if (a > 0xfe7 && a < 0xff0) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0x400],&cart[a1],0x400);
				}
				if (a > 0xfef && a < 0xff8) {
					a1=(a&0x07)<<10;
					memcpy(&theRom[0x800],&cart[a1],0x400);
				}
				break;
		}
		return res;
	}

	if ((a & 0x280) == 0x80) {
		res = theRam[a & 0x7f];
		return res;
	}

	if (!(a & 0x80)) {
		switch(a & 0x0f){
		/* TIA */
		case CXM0P:
			res=tiaRead[CXM0P];
			break;
		case CXM1P:
			res=tiaRead[CXM1P];
			break;
		case CXP0FB:
			res=tiaRead[CXP0FB];
			break;
		case CXP1FB:
			res=tiaRead[CXP1FB];
			break;
		case CXM0FB:
			res=tiaRead[CXM0FB];
			break;
		case CXM1FB:
			res=tiaRead[CXM1FB];
			break;
		case CXBLPF:
			res=tiaRead[CXBLPF];
			break;
		case CXPPMM:
			res=tiaRead[CXPPMM];
			break;
		case INPT0:
			if (app_data.left == PADDLE) {
				if ((tiaWrite[VBLANK] & 0x80) == 0) {
					x=640-mouse_position();
					x=x*20;
					if (paddle1.val > x) {
						tiaRead[INPT0]=0x80;
					}
				}
			}
			if (app_data.left == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT0]=0x80;
				if (!(riotWrite[SWCHA] & 0x10)) tiaRead[INPT0]=(keypad[4] & 0x01) << 7;
				if (!(riotWrite[SWCHA] & 0x20)) tiaRead[INPT0]=(keypad[5] & 0x01) << 7;
				if (!(riotWrite[SWCHA] & 0x40)) tiaRead[INPT0]=(keypad[6] & 0x01) << 7;
				if (!(riotWrite[SWCHA] & 0x80)) tiaRead[INPT0]=(keypad[7] & 0x01) << 7;
			}
			res=tiaRead[INPT0];
			break;
		case INPT1:
			if (app_data.left == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT1]=0x80;
				if (!(riotWrite[SWCHA] & 0x10)) tiaRead[INPT1]=(keypad[4] & 0x02) << 6;
				if (!(riotWrite[SWCHA] & 0x20)) tiaRead[INPT1]=(keypad[5] & 0x02) << 6;
				if (!(riotWrite[SWCHA] & 0x40)) tiaRead[INPT1]=(keypad[6] & 0x02) << 6;
				if (!(riotWrite[SWCHA] & 0x80)) tiaRead[INPT1]=(keypad[7] & 0x02) << 6;
			}
			res=tiaRead[INPT1];
			break;
		case INPT2:
			if (app_data.right == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT2]=0x80;
				if (!(riotWrite[SWCHA] & 0x01)) tiaRead[INPT2]=((keypad[0] & 0x01) << 7);
				if (!(riotWrite[SWCHA] & 0x02)) tiaRead[INPT2]=((keypad[1] & 0x01) << 7);
				if (!(riotWrite[SWCHA] & 0x04)) tiaRead[INPT2]=((keypad[2] & 0x01) << 7);
				if (!(riotWrite[SWCHA] & 0x08)) tiaRead[INPT2]=((keypad[3] & 0x01) << 7);
			}

			res=tiaRead[INPT2];
			break;
		case INPT3:
			if (app_data.right == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT3]=0x80;
				if (!(riotWrite[SWCHA] & 0x01)) tiaRead[INPT3]=((keypad[0] & 0x02) << 6);
				if (!(riotWrite[SWCHA] & 0x02)) tiaRead[INPT3]=((keypad[1] & 0x02) << 6);
				if (!(riotWrite[SWCHA] & 0x04)) tiaRead[INPT3]=((keypad[2] & 0x02) << 6);
				if (!(riotWrite[SWCHA] & 0x08)) tiaRead[INPT3]=((keypad[3] & 0x02) << 6);

			}

			res=tiaRead[INPT3];
			break;
		case INPT4:
			if (app_data.left == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT4]=0x80;
				if (!(riotWrite[SWCHA] & 0x10)) tiaRead[INPT4]=(keypad[4] & 0x04) << 5;
				if (!(riotWrite[SWCHA] & 0x20)) tiaRead[INPT4]=(keypad[5] & 0x04) << 5;
				if (!(riotWrite[SWCHA] & 0x40)) tiaRead[INPT4]=(keypad[6] & 0x04) << 5;
				if (!(riotWrite[SWCHA] & 0x80)) tiaRead[INPT4]=(keypad[7] & 0x04) << 5;
			}
			else
			{
				keytrig();
			}
			res=tiaRead[INPT4];
			break;
		case INPT5:
			if (app_data.right == KEYPAD) {
				keyboard_keypad();
				tiaRead[INPT5]=0x80;
				if (!(riotWrite[SWCHA] & 0x01)) tiaRead[INPT5]=((keypad[0] & 0x04) << 5);
				if (!(riotWrite[SWCHA] & 0x02)) tiaRead[INPT5]=((keypad[1] & 0x04) << 5);
				if (!(riotWrite[SWCHA] & 0x04)) tiaRead[INPT5]=((keypad[2] & 0x04) << 5);
				if (!(riotWrite[SWCHA] & 0x08)) tiaRead[INPT5]=((keypad[3] & 0x04) << 5);

			}
			else
			{
				keytrig();
			}

			res=tiaRead[INPT5];
			break;
		case 0x0f:
			res=0x0f;
			break;
		}
	 }
	 else
	 {
		switch(a & 0x2ff) {
			/* Timer output */
			case INTIM:
			case 0x285:
			case 0x29D:
				res=do_timer(clkcount);
				break;
			case SWCHA:
				if (app_data.left == PADDLE) {
					x=mouse_button();
					if (x)
						riotRead[SWCHA] &= 0x7F;
					else
						riotRead[SWCHA] |= 0x80;
				}
				else
				{
					keyjoy();
				}
				res=riotRead[SWCHA];
				break;

			/* Switch B is hardwired to input */
			case SWCHB:
				keycons();
				res=riotRead[SWCHB];
				break;
			case TIM1T:
			case TIM8T:
			case TIM64T:
			case T1024T:
				res=do_timer(clkcount);
				break;
			default:
				res=65;
				break;
		}
	}
	return res;
}



/* Debug Read */
BYTE dbgRead (ADDRESS a){
	BYTE res;

	/* RAM, mapped to page 0 and 1*/

	if ((a>0x7f && a<0x100) || (a>0x17f && a<0x200)) {
		res=theRam[a & 0x7f];
		return res;
	}
	/* ROM Cartridge */
	if (a>0xFFF) {
		res=theRom[a & 0x0fff];
		return res;
	}

	switch(a){
		/* TIA */
		case COLUP0:
			res=pl[0].col;
			break;
		case COLUP1:
			res=pl[1].col;
			break;
		case COLUPF:
			res=pf[0].col;
			break;
		case COLUBK:
			res=tiaWrite[COLUBK];
			break;
		case CTRLPF:
			res=tiaWrite[CTRLPF]; /* Bitmask 00110111 */
			break;
		case REFP0:
			res=tiaWrite[REFP0];
			break;
		case REFP1:
			res=tiaWrite[REFP1];
			break;
		case PF0:
			res=pf[0].pf0;
			break;
		case PF1:
			res=pf[0].pf1;
			break;
		case PF2:
			res=pf[0].pf2;
			break;

		/* Timer output */
		case INTIM:
			res=riotRead[INTIM];
			break;
		case SWCHA:
			res=riotRead[SWCHA];
			break;
		/* Switch B is hardwired to input */
		case SWCHB:
			res=riotRead[SWCHB];
			break;

	default:
	res=0;
	break;
	}
	return res;
}





