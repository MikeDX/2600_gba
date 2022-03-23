// COMPILED

//#include <dos.h>
//#include <bios.h>
#include <stdio.h>
#include <math.h>


int init_mouse() {
/*	union REGS inregs,outregs;

	inregs.x.ax = 0x00; // subfunction 0: reset
	int86(0x33, &inregs, &outregs);
	return(outregs.x.ax);    // return overall success/failure */
	return 1;
}

int	mouse_position() {
	/* union REGS inregs,outregs;


	inregs.x.ax = 0x03;
	int86(0x33, &inregs, &outregs);
	return(outregs.x.cx); */
	return 1;
}

int mouse_button() {

	/* union REGS inregs,outregs;


	inregs.x.ax = 0x03;
	int86(0x33, &inregs, &outregs);
	return(outregs.x.bx); */
	return 0;
}

void mouse_sensitivity(int sens) {
	/* union REGS inregs,outregs;

	inregs.x.bx = sens;
	inregs.x.cx = sens;
	inregs.x.dx = 1;
	inregs.x.ax = 0x1A;
	int86(0x33, &inregs, &outregs); */
}

