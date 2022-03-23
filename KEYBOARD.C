/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Daniel Boris. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

******************************************************************************/
/*
  This module has been completely re-written since X2600
*/
#define KEYS KEYZ
#include <stdio.h>
#include "key.h"
typedef unsigned long u32;
typedef unsigned short u16;
#include "keypad.h"
#include "types.h"

#include "address.h"
#include "vmachine.h"
#include "extern.h"
#include "memory.h"
#include "display.h"
#include "resource.h"
extern BYTE squash;
extern u16 *screen;
void keyjoy(void) {
	BYTE val;
	BYTE v1,v2;

	v1=v2=0x0f;
	if(!((*KEYS) & KEY_UP)) v1&=0x0E;
	if(!((*KEYS) & KEY_DOWN)) v1&=0x0D;
	//if (keys[kDARROW]) v1&=0x0D;
	if(!((*KEYS) & KEY_LEFT)) v1&=0x0B;
	if(!((*KEYS) & KEY_RIGHT)) v1&=0x07;
	//if (keys[kRARROW]) v1&=0x07;
	//if (keys[kW]) v2&=0x0E;
	//if (keys[kZ]) v2&=0x0D;
	//if (keys[kA]) v2&=0x0B;
	//if (keys[kS]) v2&=0x07;
	if (app_data.swap)
		riotRead[0x280]=(v2 << 4) |v1;
	else
		riotRead[0x280]=(v1 << 4) | v2; 
}

void keycons(void) {
	BYTE val;
	int x=0;
	riotRead[0x282] |= 0x03;
//	if (keys[kF7]) riotRead[0x282] &= 0xF7;		/* BW */
//	if (keys[kF8]) riotRead[0x282] |= 0x08;		/* Color */
if(!((*KEYS) & KEY_START)) riotRead[0x282] &=0xFE;
if(!((*KEYS) & KEY_SELECT)) riotRead[0x282] &=0xFD;
if(!((*KEYS) & KEY_R)) {
	squash=1-squash;
	while ( x<240*160) screen[x++]=0;
	}
//	if (keys[kF1]) riotRead[0x282] &= 0xFE;		/* Reset */
//	if (keys[kF2]) riotRead[0x282] &= 0xFD;     /* Select */
//	if (keys[kF9]) riotRead[0x282] &= 0xBF; 	/* P0 amateur */
//	if (keys[kF10]) riotRead[0x282] |= 0x40;    /* P0 pro */
//	if (keys[kF11]) riotRead[0x282] &= 0x7f;    /* P1 amateur */
//	if (keys[kF12]) riotRead[0x282] |= 0x80;	/* P1 pro */ 
}

void keytrig(void) {
	int kr;
	int kl;

	if (app_data.swap) {
		kl=!((*KEYS) & KEY_A);
		kr=0;
		//kr=keys[kLEFTALT];
		//kl=keys[kRIGHTALT];
		
	}
	else
	{
		kr=!((*KEYS) & KEY_A);
		kl=0;
		//kl=keys[kLEFTALT];
		//kr=keys[kRIGHTALT];
	}

	if (!(tiaWrite[VBLANK] & 0x40)) {
		if (kr)
			tiaRead[INPT4]=0x00;
		else
			tiaRead[INPT4]=0x80;
		if (kl)
			tiaRead[INPT5]=0x00;
		else
			tiaRead[INPT5]=0x80;

	}
	else
	{
		if (kr)  tiaRead[INPT4]=0x00;
		if (kl) tiaRead[INPT5]=0x00;

	} 
}

void keyboard_keypad(void) {
/*	int i;

	for(i=0; i<8; i++) keypad[i]=0xFF;
	if (keys[k1]) keypad[4]=0xFE;
	if (keys[k2]) keypad[4]=0xFD;
	if (keys[k3]) keypad[4]=0xFB;
	if (keys[kQ]) keypad[5]=0xFE;
	if (keys[kW]) keypad[5]=0xFD;
	if (keys[kE]) keypad[5]=0xFB;
	if (keys[kA]) keypad[6]=0xFE;
	if (keys[kS]) keypad[6]=0xFD;
	if (keys[kD]) keypad[6]=0xFB;
	if (keys[kZ]) keypad[7]=0xFE;
	if (keys[kX]) keypad[7]=0xFD;
	if (keys[kC]) keypad[7]=0xFB;
	if (app_data.right == KEYPAD) {
		if (keys[k4]) keypad[0]=0xFE;
		if (keys[k5]) keypad[0]=0xFD;
		if (keys[k6]) keypad[0]=0xFB;
		if (keys[kR]) keypad[1]=0xFE;
		if (keys[kT]) keypad[1]=0xFD;
		if (keys[kY]) keypad[1]=0xFB;
		if (keys[kF]) keypad[2]=0xFE;
		if (keys[kG]) keypad[2]=0xFD;
		if (keys[kH]) keypad[2]=0xFB;
		if (keys[kV]) keypad[3]=0xFE;
		if (keys[kB]) keypad[3]=0xFD;
		if (keys[kN]) keypad[3]=0xFB;
	} */
}

void keyboard(void) {

	/* if (keys[kF5]) {
		Set_Old_Int9();
		setmode(TEXT_MODE);
		app_data.debug=1;
	} 

	if (keys[kESC]) {
		Set_Old_Int9();
		tv_off();
		exit(0);
	} */
}


/* Initilises the keyboard */
void init_keyboard(void)
{
	//Set_New_Int9();
}




