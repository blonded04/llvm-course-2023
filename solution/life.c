#include "sim.h"
#define REPEAT_SIZE 1024

void drawGen(unsigned* gen) {
    unsigned x = 0;
    unsigned y = 0;
    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            simSetPixel(x, y, 0xFF000000 + 0xFF00 * gen[y * SIM_X_SIZE + x]);
        }
    }
    simFlush();
}

void calcGen(unsigned* nextGen, unsigned* prevGen) {
    // TBD
}

int main() {
    unsigned x = 0;
    unsigned y = 0;
    unsigned i = 0;
    unsigned gen1[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned gen2[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned* nextGen = gen1;
    unsigned* prevGen = gen2;

    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            prevGen[y * SIM_X_SIZE + x] = simRand() % 2;
        }
    }
    drawGen(prevGen);

    for (i = 0; i < REPEAT_SIZE; i++) {
        calcGen(nextGen, prevGen);
        drawGen(nextGen);
        unsigned* tmp = prevGen;
        prevGen = nextGen;
        nextGen = tmp;
    }
    return 0;
}