#ifndef SIM_H_INCLUDED_
#define SIM_H_INCLUDED_

#include "SFML/Graphics.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define SIM_X_SIZE 100
#define SIM_Y_SIZE 50
#define SIM_CELL_SIZE 10
#define SIM_FRAMES_PER_SECOND 2
#define SIM_INITIAL_DENSITY_MODULO 5

static inline _Bool* simIsInitialized() {
    static _Bool isInitialized = false;
    return &isInitialized;
}

static inline sfRectangleShape* simGetBlock() {
    static sfRectangleShape* block = NULL;
    if (!block) {
        block = sfRectangleShape_create();
        sfVector2f size = {SIM_CELL_SIZE, SIM_CELL_SIZE};
        sfRectangleShape_setSize(block, size);
    }

    return block;
}

static inline void simInitialize(sfRenderWindow** window) {
    assert(!(*(simIsInitialized())));
    assert(window);

    enum { ACTUAL_FRAMES_PER_SECOND = 24 };

    sfVideoMode mode = {SIM_X_SIZE * SIM_CELL_SIZE, SIM_Y_SIZE * SIM_CELL_SIZE,
                        32};
    *window = sfRenderWindow_create(mode, "Game of life", sfClose, NULL);
    sfRenderWindow_setKeyRepeatEnabled(*window, sfFalse);
    sfRenderWindow_setFramerateLimit(*window, ACTUAL_FRAMES_PER_SECOND);

    simGetBlock();

    *(simIsInitialized()) = true;
}

static inline sfRenderWindow* simGetWindow() {
    static sfRenderWindow* window = NULL;
    if (!(*(simIsInitialized()))) {
        simInitialize(&window);
    }

    return window;
}

static inline void simCleanup(_Bool shouldClose) {
    assert(*(simIsInitialized()));

    if (shouldClose) {
        sfRenderWindow_close(simGetWindow());
    }
    sfRectangleShape_destroy(simGetBlock());
    sfRenderWindow_destroy(simGetWindow());

    *(simIsInitialized()) = false;
}

static inline void simClearWindow() {
    sfRenderWindow_clear(simGetWindow(), sfBlack);
}

static inline void simSetPixel(int x, int y, _Bool isAlive) {
    assert(0 <= x && x < SIM_X_SIZE);
    assert(0 <= y && y < SIM_Y_SIZE);

    sfVector2f blockPositions = {SIM_CELL_SIZE * x, SIM_CELL_SIZE * y};
    sfRectangleShape_setPosition(simGetBlock(), blockPositions);

    if (isAlive) {
        sfRectangleShape_setFillColor(simGetBlock(),
                                      sfColor_fromRGB(0, 120, 215));
    } else {
        sfRectangleShape_setFillColor(simGetBlock(),
                                      sfColor_fromRGB(40, 40, 40));
    }

    sfRenderWindow_drawRectangleShape(simGetWindow(), simGetBlock(), NULL);
}

static inline void simFlush() {
    assert(*(simIsInitialized()));

    sfRenderWindow_display(simGetWindow());
}

static inline int simRand() { return rand(); }

#endif // SIM_H_INCLUDED_
