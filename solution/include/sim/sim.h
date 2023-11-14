#ifndef SIM_H_INCLUDED_
#define SIM_H_INCLUDED_

#define SIM_X_SIZE 100
#define SIM_Y_SIZE 50
#define SIM_FRAMES_PER_SECOND 2

int simKeepRunning(void);

void simSetPixel(int x, int y, int isAlive);

void simFlush(void);

int simRand(void);

#endif // SIM_H_INCLUDED_
