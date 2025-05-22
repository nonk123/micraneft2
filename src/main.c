#include <windows.h>

#include "clock.h"
#include "g.h"
#include "screen.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// bruh.................
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    pxInit();
    clockInit();

    int fpsCounter = 0, second = 0;

    instant lastFrame = elapsed();
    while (!gExit) {
        for (size_t i = 0; i < SCREEN_MAX; i++)
            px(i)->txt = ' ';
        pxInput();

        gDummyY += kDown(KP_2) - kDown(KP_8);
        gDummyY = CLAMP(gDummyY, 0, gRows - 1);
        gDummyX += kDown(KP_6) - kDown(KP_4);
        gDummyX = CLAMP(gDummyX, 0, gCols - 1);

        pxAt(gDummyX, gDummyY)->txt = '@';
        pxAt(gDummyX, gDummyY)->fg = C_BRIGHT_WHITE;

        pxCommit();

        instant curFrame = elapsed(), delta = curFrame - lastFrame;
        const instant target = CLOCK_RES / gTargetFPS;
        if (delta < target) {
            mcSleep(((target - delta) * 1000) / CLOCK_RES);
            delta = target;
        }
        lastFrame = curFrame;
        fpsCounter++;

        second += delta;
        if (second >= CLOCK_RES) {
            second -= CLOCK_RES;
            gCurFPS = fpsCounter;
            fpsCounter = 0;
        }
    }

    pxCleanup();

    return 0;
}
