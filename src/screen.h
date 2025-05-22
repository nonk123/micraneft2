#pragma once

#include <stdint.h>

#define C_BLACK (0)
#define C_RED (1)
#define C_GRAY (7)
#define C_WHITE (15)

#define SCREEN_MAX (400 * 300)
#define TITLE_MAX (256)

struct pixel {
    uint8_t txt;
    uint8_t fg : 4, bg : 4;
};

void pxInit();
void pxInput();
void pxCommit();
void pxCleanup();

struct pixel* px(size_t);
struct pixel* pxAt(int, int);

int pxRows();
int pxCols();
