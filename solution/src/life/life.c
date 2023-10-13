#include "life.h"

#include "sim/sim.h"

static void drawGeneration(unsigned* generation) {
    unsigned x = 0;
    unsigned y = 0;
    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            simSetPixel(x, y, (int)generation[y * SIM_X_SIZE + x]);
        }
    }

    simFlush();
}

static void calculateNextGeneration(unsigned* currentGeneration,
                                    unsigned* nextGeneration) {
    int x = 0;
    int y = 0;
    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            int dx = -1;
            int dy = -1;
            unsigned aliveNeighbours = 0;
            for (dx = -1; dx <= 1; dx++) {
                for (dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }

                    int newX = x + dx;
                    int newY = y + dy;
                    if (newX < 0 || SIM_X_SIZE <= newX) {
                        continue;
                    }
                    if (newY < 0 || SIM_Y_SIZE <= newY) {
                        continue;
                    }

                    aliveNeighbours +=
                        currentGeneration[newY * SIM_X_SIZE + newX];
                }
            }

            if (currentGeneration[y * SIM_X_SIZE + x] == 0) {
                if (aliveNeighbours == 3) {
                    nextGeneration[y * SIM_X_SIZE + x] = 1;
                } else {
                    nextGeneration[y * SIM_X_SIZE + x] = 0;
                }
            } else {
                if (aliveNeighbours < 2 || 3 < aliveNeighbours) {
                    nextGeneration[y * SIM_X_SIZE + x] = 0;
                } else {
                    nextGeneration[y * SIM_X_SIZE + x] = 1;
                }
            }
        }
    }
}

static void initializeGeneration(unsigned* generation) {
    unsigned x = 0;
    unsigned y = 0;
    for (y = 0; y < SIM_Y_SIZE; y++) {
        for (x = 0; x < SIM_X_SIZE; x++) {
            generation[y * SIM_X_SIZE + x] = simRand();
        }
    }
}

void app(void) {
    unsigned generation1[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned generation2[SIM_Y_SIZE * SIM_X_SIZE];
    unsigned* nextGeneration = generation1;
    unsigned* prevGeneration = generation2;

    initializeGeneration(nextGeneration);

    while (simKeepRunning()) {
        calculateNextGeneration(nextGeneration, prevGeneration);
        drawGeneration(nextGeneration);

        unsigned* tmp = prevGeneration;
        prevGeneration = nextGeneration;
        nextGeneration = tmp;
    }
}
