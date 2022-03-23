#ifndef COLLISION_H
#define COLLISION_H

//#include <btypes.h>

#define PL0_MASK 0x01
#define PL1_MASK 0x02
#define ML0_MASK 0x04
#define ML1_MASK 0x08
#define BL_MASK 0x10
#define PF_MASK 0x20

extern BYTE colvect[230];

extern void reset_collisions(void);

extern void set_collisions(BYTE b);

#endif






