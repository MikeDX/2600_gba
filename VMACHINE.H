#ifndef VMACHINE_H
#define VMACHINE_H

/*
   $Id: vmachine.h,v 2.5 1996/03/12 16:15:24 alex Exp alex $
   External definitions of the vmachine.c stuff
*/

//#include <btypes.h>

extern BYTE cartram[256];
extern BYTE cart[16384];
extern BYTE theRom[4096];
extern BYTE theRam[128];
extern BYTE riotRead[0x298];
extern BYTE riotWrite[0x298];
extern BYTE tiaWrite[0x2d];
extern BYTE tiaRead[0x0e];

extern int reset_flag;

extern int ebeamx, ebeamy, sbeamx;

extern BYTE keypad[8];


#define VSYNCSTATE 1
#define VBLANKSTATE 2
#define HSYNCSTATE 4
#define DRAWSTATE 8
/* #define OVERSTATE 16 removed as is only a special case of HSYNC/DRAW*/

extern int vbeam_state, hbeam_state;

extern int tv_width, tv_height, tv_vsync, tv_vblank,
    tv_overscan, tv_frame, tv_hertz, tv_hsync;

extern int timer_res, timer_count, timer_clks; 

extern struct PlayField {
    BYTE pf0,pf1,pf2;
    BYTE ref, col;
} pf[2];  

extern struct Paddle {
	long pos;
	long val;
} paddle1;


extern struct Player {
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

struct RasterChange {
  int x;     /* Position at which change happened */
  int type;  /* Type of change */
  int val;   /* Value of change */
} ;

extern struct RasterChange pl_change[2][80], pf_change[1][80], unified[80];

extern int pl_change_count[2], pf_change_count[1], unified_count;

/* The missile and ball positions */
extern struct Missile {
  int x;
  BYTE hmm;
  BYTE enabled;
  BYTE width;
  BYTE vdel;
  BYTE vdel_flag;
  BYTE col;
  BYTE mask;
} ml[3];

extern  void do_plraster_change(int i, int type, int val);

extern void do_unified_change(int type, int val);

extern void use_unified_change( struct RasterChange *rc);

extern void use_plraster_change( struct Player *pl, struct RasterChange *rc);

extern void do_pfraster_change(int i, int type, int val);
extern void use_pfraster_change( struct PlayField *pf, struct RasterChange *rc);

extern void init_hardware(void);
extern void set_timer(int res, int count, int clkadj);
extern BYTE do_timer(int clkadj);
extern void do_screen(int clks);

#endif






