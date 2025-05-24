#pragma once

#include <stdint.h>

#define CHUNK_WIDTH (32)
#define WORLD_WIDTH (8 * 1024)
#define WORLD_HEIGHT (512)
#define VIEW_RADIUS (128)

#define FLAG(name) uint16_t f##name : 1
struct tile {
    FLAG(Solid);
};
#undef FLAG

void tInit();
void tUpdate();
void tDraw();
void tCleanup();

struct tile* tAt(int, int);
