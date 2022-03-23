/*****************************************************************************

   This file is part of Virtual VCS, the Atari 2600 Emulator
   ===================================================

   Copyright 1996 Daniel Boris. For contributions see the file CREDITS.

   This software is distributed under the terms of the GNU General Public
   License. This is free software with ABSOLUTELY NO WARRANTY.

   See the file COPYING for Details.

******************************************************************************/

/*
   Used to load cartridge images into memory.
   This module has been completely re-written since X2600
*/

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "vmachine.h"
#include "config.h"
#include "resource.h"
#include "roms/pitfall.h"
#include "roms/pacman.h"
#include "roms/combat.h"
#include "roms/spcinvad.h"
#include "roms/asteroid.h"
#include "roms/Spchack3.h"
#include "roms/air_sea.h"
#include "roms/centiped.h"
#include "roms/carnival.h"
#include "roms/qboofly.h"
#include "roms/lasrblst.h"
#include "roms/HAUNTHSE.H"
#include "roms/FISHDRBY.H"
#define FILE_LEN FISHDRBY_LEN
//extern int FILE_LEN;
#define cartfile fishdrby
long foff=0;
long filesize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);
   return length;
}

void show_config()
{
	/* printf("Left Controller: ");
	switch(app_data.left) {
		case STICK:
			printf("Stick\n");
			break;
		case PADDLE:
			printf("Paddle\n");
			break;
		case KEYPAD:
			printf("Keypad\n");
			break;
	}
	printf("Right Controller: ");
	switch(app_data.right) {
		case STICK:
			printf("Stick\n");
			break;
		case PADDLE:
			printf("Paddle\n");
			break;
		case KEYPAD:
			printf("Keypad\n");
			break;
	}
	printf("Frame Rate: %d\n",app_data.rr);
	printf("Bank Switch: ");
	switch (app_data.bank) {
		case 0: printf("None\n"); break;
		case 1: printf("F6\n"); break;
		case 2: printf("F8\n"); break;
		case 3: printf("FA\n"); break;
		case 4: printf("E0\n"); break;
		case 7: printf("F6SC\n"); break;
	}*/
}

void autoconfig(unsigned long sum)
{
	FILE *fn;
	int done;
	char line[100];
	char attr[50],val[50];
	char *p;
	char ch[10];
	int i;

	sprintf(ch,"%lx",sum);
	//strupr(ch);
	fn=fopen("vcscfg.dat","r");
	done=0;
	while (!done) {
		fgets(line,100,fn);
		if (feof(fn)) {
			printf("  Configuration not found\n");
			return;
		}
		p=strtok(line,"=");
		strcpy(attr,p);
		p=strtok(NULL,"=");
		strcpy(val,p);
		val[strlen(val)-1]=0;

		 if (!strcmp(attr,"CHECKSUM")) {
			if (!strcmp(val,ch)) {
				done=1;
			}
		} 
	}
	printf("  Configuration Found...\n\n");
	done=0;
	while(!done) {
		fgets(line,100,fn);
		if (feof(fn)) return;
		p=strtok(line,"=");
		strcpy(attr,p);
		p=strtok(NULL,"=");
		strcpy(val,p);
		val[strlen(val)-1]=0;
		if (!strcmp(attr,"CHECKSUM")) return;
		if (!strcmp(attr,"NAME")) printf("Name: %s\n",val);
		if (!strcmp(attr,"MAKER")) printf("Made by: %s\n",val);
		if (!strcmp(attr,"EMULATION")) printf("Emulation: %s\n",val);
		if (!strcmp(attr,"SENSITIVITY")) {
			sscanf(val,"%d",&i);
			app_data.pad_sens=i;
		}
		if (!strcmp(attr,"FRAMERATE")) {
			sscanf(val,"%d",&i);
			if (i == 0) i=1;
			app_data.rr=i;
		}
		if (!strcmp(attr,"TYPE")) {
			if (!strcmp(val,"2K")) app_data.bank=0;
			if (!strcmp(val,"4K")) app_data.bank=0;
			if (!strcmp(val,"F6")) app_data.bank=1;
			if (!strcmp(val,"F8")) app_data.bank=2;
			if (!strcmp(val,"E0")) app_data.bank=4;
			if (!strcmp(val,"FA")) app_data.bank=3;
			if (!strcmp(val,"F6SC")) {
				app_data.bank=1;
				app_data.sc=1;
			}
		}
		if (!strcmp(attr,"RIGHT")) {
			if (!strcmp(val,"STICK")) {
				app_data.right=STICK;
			}
			if (!strcmp(val,"KEYPAD")) {
				app_data.right=KEYPAD;
			}
			if (!strcmp(val,"PADDLE")) {
				app_data.right=PADDLE;
			}
		}

		if (!strcmp(attr,"LEFT")) {
			if (!strcmp(val,"STICK")) {
				app_data.left=STICK;
			}
			if (!strcmp(val,"KEYPAD")) {
				app_data.left=KEYPAD;
			}
			if (!strcmp(val,"PADDLE")) {
				app_data.left=PADDLE;
			}
		}

	}

}


/* Loads a cart image. Returns -1 on error, 0 otherwise */
int loadCart( char *name) {
/*	FILE *fp; */
	unsigned char buffer; 
	long len; 
	int i; 
	unsigned long checksum;
//memcpy(&theRom[0],&pitfall[0],4096);
//return 0;

	//if(name == NULL) return -1;
/*
	printf("Load Cartridge: %s\n",name);
	fp=fopen(name, "rb");
	if (!fp) {
		printf("Error loading cartridge\n");
		exit(0);
	} */
	len=FILE_LEN;

	//fread(&cart[0],1,len,fp);
	//fclose(fp);
	checksum=0;
	for(i=0; i<len; i++) {
		cart[i]=cartfile[i+foff];
		checksum+=cart[i+foff];
	}
	foff+=len;
	//printf("Checksum: %lx\n",checksum);
	if (app_data.autoconfig) {
		//printf("\nAttempting to autoconfigure... \n");
	//	autoconfig(checksum);
	}
	//show_config();
	switch (app_data.bank) {
	case 0:
	case 1:
	case 2:
		switch (len) {
			case 2048:
				memcpy(&theRom[0],&cart[0],2048);
				memcpy(&theRom[2048],&cart[0],2048);
				break;
			case 4096:
				memcpy(&theRom[0],&cart[0],4096);
				break;
			case 8192:
				memcpy(&theRom[0],&cart[4096],4096);
				break;
			case 16384:
				memcpy(&theRom[0],&cart[12288],4096);
				break;
		}
		break;
	case 3:
		memcpy(&theRom[0],&cart[8192],4096);
		break;
	case 4:
		memcpy(&theRom[0],&cart[0],3072);
		memcpy(&theRom[0xC00],&cart[0x1C00],1024);
		break;
	}
	return 0;
}








