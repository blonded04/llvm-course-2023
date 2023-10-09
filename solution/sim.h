#ifndef SIM_H_INCLUDED_
#define SIM_H_INCLUDED_

#include "SFML/Graphics.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define SIM_X_SIZE 256
#define SIM_Y_SIZE 128
#define SIM_CELL_SIZE 4
#define SIM_SPACING 1

static inline _Bool* simIsInitialized() {
    static _Bool isInitialized = false;
    return &isInitialized;
}

static inline sfRectangleShape* simGetBlock() {
    static sfRectangleShape* block = NULL;
    if (!block) {
        block = sfRectangleShape_create();
        sfVector2f size = {SIM_CELL_SIZE - SIM_SPACING,
                           SIM_CELL_SIZE - SIM_SPACING};
        sfRectangleShape_setSize(block, size);
        sfVector2f offset = {-SIM_SPACING, -SIM_SPACING};
        sfRectangleShape_setOrigin(block, offset);
    }

    return block;
}

static inline void simInitialize(sfRenderWindow** window) {
    assert(!(*(simIsInitialized())));
    assert(window);

    sfVideoMode mode = {SIM_X_SIZE * SIM_CELL_SIZE, SIM_Y_SIZE * SIM_CELL_SIZE,
                        32};
    *window = sfRenderWindow_create(mode, "Game of life", sfClose, NULL);
    sfRenderWindow_setKeyRepeatEnabled(*window, sfFalse);
    sfRenderWindow_setFramerateLimit(*window, 24);

    simGetBlock();
}

static inline sfRenderWindow* simGetWindow() {
    static sfRenderWindow* window = NULL;
    if (!(*(simIsInitialized()))) {
        simInitialize(&window);
    }

    return window;
}

static inline void simCleanup() {
    assert(*(simIsInitialized()));

    sfRenderWindow_close(simGetWindow());
    sfRectangleShape_destroy(simGetBlock());
    sfRenderWindow_destroy(simGetWindow());
}

static inline void simSetPixel(int x, int y, _Bool isAlive) {
    assert(0 <= x && x < SIM_X_SIZE);
    assert(0 <= y && y < SIM_Y_SIZE);

    sfVector2f blockPositions = {SIM_CELL_SIZE * (x - SIM_SPACING),
                                 SIM_CELL_SIZE * (y - SIM_SPACING)};
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

static inline void simClearWindow() {
    sfRenderWindow_clear(simGetWindow(), sfBlack);
}

static inline int simRand() { return rand() & 1; }

#endif // SIM_H_INCLUDED_
