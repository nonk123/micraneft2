#include <windows.h>

#include "clock.h"
#include "g.h"
#include "screen.h"

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
        if (gDummyX >= 0 && gDummyY >= 0 && gDummyX < pxCols() && gDummyY < pxRows())
            pxAt(gDummyX, gDummyY)->txt = '@';

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
