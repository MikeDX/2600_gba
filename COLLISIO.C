/*****************************************************************************

   This file is part of x2600, the Atari 2600 Emulator
   ===================================================
   
   Copyright 1996 Alex Hornby. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.
   
   See the file COPYING for Details.
   
   $Id: collision.c,v 1.2 1996/03/21 16:36:01 alex Exp $
******************************************************************************/

/* The 2600 collision detection code */

#include "types.h"
#include "address.h"
#include "vmachine.h"

/*
  There are 6 different objects on the screen. Each takes one bit of the 
  collision vector.
  Bit 0: Player 0
  Bit 1: Player 1
  Bit 2: Missile 0
  Bit 3: Missile 1
  Bit 4: Ball
  Bit 5: Playfield
*/

#define PL0_MASK 0x01
#define PL1_MASK 0x02
#define ML0_MASK 0x04
#define ML1_MASK 0x08
#define BL_MASK 0x10
#define PF_MASK 0x20

BYTE colvect[230];


/* Resets the collision registers of the tia */
void reset_collisions(void)
{
	int i;
	for(i=CXM0P; i<INPT0; i++)
	tiaRead[i]=0;
}


/* Does collision testing on the pixel b */
void set_collisions(BYTE b)
{
	/* unfortunately this needs to be run for every pixel! */
	/* C'est trez slow */
	if( (b & ML0_MASK) && (b & PL1_MASK) )
	tiaRead[CXM0P]|=0x80;
	if( (b & ML0_MASK) && (b & PL0_MASK) )
	tiaRead[CXM0P]|=0x40;
	if( (b & ML1_MASK) && (b & PL0_MASK) )
	tiaRead[CXM1P]|=0x80;
	if( (b & ML1_MASK) && (b & PL1_MASK) )
	tiaRead[CXM1P]|=0x40;
	if( (b & PL0_MASK) && (b & PF_MASK) )
	tiaRead[CXP0FB]|=0x80;
	if( (b & PL0_MASK) && (b & BL_MASK) )
	tiaRead[CXP0FB]|=0x40;
	if( (b & PL1_MASK) && (b & PF_MASK) )
	tiaRead[CXP1FB]|=0x80;
	if( (b & PL1_MASK) && (b & BL_MASK) )
	tiaRead[CXP1FB]|=0x40;
	if( (b & ML0_MASK) && (b & PF_MASK) )
	tiaRead[CXM0FB]|=0x80;
	if( (b & ML0_MASK) && (b & BL_MASK) )
	tiaRead[CXM0FB]|=0x40;
	if( (b & ML1_MASK) && (b & PF_MASK) )
	tiaRead[CXM1FB]|=0x80;
	if( (b & ML1_MASK) && (b & BL_MASK) )
	tiaRead[CXM1FB]|=0x40;
	if( (b & BL_MASK) && (b & PF_MASK) )
	tiaRead[CXBLPF]|=0x80;
	if( (b & PL0_MASK) && (b & PL1_MASK) )
	tiaRead[CXPPMM]|=0x80;
	if( (b & ML0_MASK) && (b & ML1_MASK) )
	tiaRead[CXPPMM]|=0x40;
}



