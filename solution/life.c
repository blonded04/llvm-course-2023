#include "sim.h"
#define REPEAT_SIZE 1024

void drawGeneration(unsigned* generation) {
    unsigned x = 0;
    unsigned y = 0;
    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            simSetPixel(x, y,
                        0xFF000000 + 0xFF00 * generation[y * SIM_X_SIZE + x]);
        }
    }
    simFlush();
}

void calculateNextGeneration(unsigned* currentGeneration,
                             unsigned* nextGeneration) {
    // TODO
}

int main() {
    unsigned x = 0;
    unsigned y = 0;
    unsigned i = 0;
    unsigned generation1[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned generation2[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned* nextGeneration = generation1;
    unsigned* prevGeneration = generation2;

    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            prevGeneration[y * SIM_X_SIZE + x] = simRand() % 2;
        }
    }
    drawGeneration(prevGeneration);

    for (i = 0; i < REPEAT_SIZE; i++) {
        calculateNextGeneration(nextGeneration, prevGeneration);
        drawGeneration(nextGeneration);

        unsigned* tmp = prevGeneration;
        prevGeneration = nextGeneration;
        nextGeneration = tmp;
    }

    return 0;
}