#include "sim.h"

#include <SFML/Graphics.h>
#include <stdlib.h>

#define SIM_PIXEL_SIZE 10
#define SIM_INITIAL_DENSITY_MODULO 5

static int* simIsInitialized(void) {
    static int isInitialized = 0;
    return &isInitialized;
}

static sfRectangleShape* simGetBlock(void) {
    static sfRectangleShape* block = NULL;
    if (!block) {
        block = sfRectangleShape_create();
        sfVector2f size = {SIM_PIXEL_SIZE, SIM_PIXEL_SIZE};
        sfRectangleShape_setSize(block, size);
    }

    return block;
}

static void simInitialize(sfRenderWindow** window) {
    enum { ACTUAL_FRAMES_PER_SECOND = 24 };

    sfVideoMode mode = {SIM_X_SIZE * SIM_PIXEL_SIZE,
                        SIM_Y_SIZE * SIM_PIXEL_SIZE, 32};
    *window = sfRenderWindow_create(mode, "Game of life", sfClose, NULL);
    sfRenderWindow_setKeyRepeatEnabled(*window, sfFalse);
    sfRenderWindow_setFramerateLimit(*window, ACTUAL_FRAMES_PER_SECOND);

    simGetBlock();

    *(simIsInitialized()) = 1;
}

static sfRenderWindow* simGetWindow(void) {
    static sfRenderWindow* window = NULL;
    if (!(*(simIsInitialized()))) {
        simInitialize(&window);
    }

    return window;
}

static void simCleanup(void) {
    sfRectangleShape_destroy(simGetBlock());
    sfRenderWindow_destroy(simGetWindow());

    *(simIsInitialized()) = 0;
}

int simKeepRunning(void) {
    sfEvent event;
    sfClock* clock = sfClock_create();
    while (sfTime_asSeconds(sfClock_getElapsedTime(clock)) *
               SIM_FRAMES_PER_SECOND <
           1.0f) {
        if (!sfRenderWindow_isOpen(simGetWindow())) {
            sfClock_destroy(clock);
            simCleanup();

            return 0;
        }

        if (sfRenderWindow_pollEvent(simGetWindow(), &event) &&
            event.type == sfEvtClosed) {
            sfRenderWindow_close(simGetWindow());
            sfClock_destroy(clock);
            simCleanup();

            return 0;
        }
    }

    sfClock_restart(clock);

    return 1;
}

void simSetPixel(int x, int y, int isAlive) {
    sfVector2f blockPositions = {SIM_PIXEL_SIZE * x, SIM_PIXEL_SIZE * y};
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

void simFlush() { sfRenderWindow_display(simGetWindow()); }

int simRand(void) { return rand() % SIM_INITIAL_DENSITY_MODULO == 0; }
