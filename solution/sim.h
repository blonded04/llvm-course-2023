#ifndef SIM_H_INCLUDED_
#define SIM_H_INCLUDED_

#include "stdlib.h"

#define SIM_X_SIZE 256
#define SIM_Y_SIZE 128
#define SIM_DISPLAY_MEM_ADDR 128 * 256

inline void simSetPixel(int x, int y, int argb);

inline void simFlush();

inline int simRand() { return rand() & 1; }

#endif // SIM_H_INCLUDED_
