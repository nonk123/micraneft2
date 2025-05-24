#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "clock.h"
#include "g.h"
#include "log.h"
#include "screen.h"
#include "terrain.h"

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

// bruh.................
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    srand(time(NULL));

    logInit();
    clockInit();
    pxInit();
    tInit();

    Info("Ready to rock!");

    int fpsCounter = 0, second = 0;
    instant lastFrame = elapsed();

    while (!gExit) {
        for (size_t i = 0; i < SCREEN_MAX; i++)
            px(i)->txt = ' ';
        pxInput();

        gDummyY += kDown(KP_8) - kDown(KP_2);
        gDummyX += kDown(KP_6) - kDown(KP_4);
        tUpdate();

        tDraw();
        pxCommit();

        instant curFrame = elapsed(), delta = curFrame - lastFrame;
        const instant target = CLOCK_RES / gTargetFPS;
        if (delta < target) {
            mcSleep(((target - delta) * 1000) / CLOCK_RES);
            delta = target;
        }
        lastFrame += delta;
        fpsCounter++;

        second += delta;
        if (second >= CLOCK_RES) {
            second -= CLOCK_RES;
            gCurFPS = fpsCounter;
            fpsCounter = 0;
        }
    }

    Info("Goodbye!");
    tCleanup();
    pxCleanup();
    logCleanup();

    return 0;
}
