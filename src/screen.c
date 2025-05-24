// TODO: make cross-platform??????
#define _WIN32_WINNT 0x0500
#include <windows.h>

#include <stdio.h>

#include "g.h"
#include "log.h"
#include "screen.h"

#define KBD_SIZE (32 / sizeof(uint32_t))

static char title[TITLE_MAX];
static struct pixel fBuf[SCREEN_MAX], bBuf[SCREEN_MAX];
static int lastX, lastY;
static uint32_t kbd[KBD_SIZE], lastKbd[KBD_SIZE];

static HANDLE stdInput = NULL, stdOutput = NULL;

static void updateDimensions() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(stdOutput, &csbi);
    gCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    gRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

static void cursorTo(int x, int y) {
    y = gRows - y - 1; // +Y up because reasons...
    if (lastY != y || lastX != x - 1)
        SetConsoleCursorPosition(stdOutput, (COORD){.X = x, .Y = y});
    lastX = x;
    lastY = y;
}

static void clearFbuf() {
    for (size_t i = 0; i < SCREEN_MAX; i++) {
        fBuf[i].txt = ' ';
        fBuf[i].fg = C_GRAY;
        fBuf[i].bg = C_BLACK;
    }
}

static void trashBbuf() {
    for (size_t i = 0; i < SCREEN_MAX; i++) {
        bBuf[i].txt = 0;
        bBuf[i].fg = C_GRAY;
        bBuf[i].bg = C_BLACK;
    }
}

void pxInit() {
    strncpy(title, "micraneft 2!!!", TITLE_MAX);
    clearFbuf();
    trashBbuf();

    AllocConsole();
    stdInput = GetStdHandle(STD_INPUT_HANDLE);
    stdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleMode(stdInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(stdOutput, 0);

    updateDimensions();

    for (size_t i = 0; i < KBD_SIZE; i++)
        kbd[i] = lastKbd[i] = 0;
}

void pxInput() {
    for (size_t i = 0; i < KBD_SIZE; i++)
        lastKbd[i] = kbd[i];

    DWORD count = 0;
    GetNumberOfConsoleInputEvents(stdInput, &count);
    if (!count)
        return;

#define INPUT_MAX (16)
    INPUT_RECORD records[INPUT_MAX];
    ReadConsoleInput(stdInput, records, INPUT_MAX, &count);
#undef INPUT_MAX

    for (size_t i = 0; i < count; i++) {
        if (records[i].EventType == KEY_EVENT) {
            KEY_EVENT_RECORD event = records[i].Event.KeyEvent;
            WORD scancode = event.wVirtualScanCode;
            if (gExitOnEsc && scancode == K_ESC)
                gExit = 1;

            if (scancode >= 0 && scancode <= 256) {
                const int kIdx = scancode / 32, bIdx = scancode % 32;
                if (event.bKeyDown)
                    kbd[kIdx] |= 1 << bIdx;
                else
                    kbd[kIdx] &= ~(1 << bIdx);
            }
        }

        // TODO: mouse events.
    }
}

static int pxsSame(struct pixel a, struct pixel b) {
    return a.txt == b.txt && a.fg == b.fg && a.bg == b.bg;
}

static void displayFPS() {
#define BUF_MAX (512)
    static char buf[BUF_MAX] = {0};
    sprintf(buf, "FPS: %d (%dx%d)", gCurFPS, gRows, gCols);
    const int top = gRows - 1;
    for (size_t i = 0; i < BUF_MAX; i++) {
        if (buf[i] && i < gCols) {
            pxAt(i, top)->txt = buf[i];
            pxAt(i, top)->bg = C_BLACK;
            pxAt(i, top)->fg = C_WHITE;
        }
        buf[i] = 0;
    }
#undef BUF_MAX
}

void pxCommit() {
    if (stdInput == NULL)
        Fatal("YOU FORGOT TO CALL `pxInit()` BITCHASS!!!");

    SetConsoleTitle(title);

    const size_t lastArea = gRows * gCols;
    updateDimensions();
    if (gRows * gCols != lastArea) {
        clearFbuf(); // TODO: work around?
        trashBbuf();
    }

    if (gDisplayFPS)
        displayFPS();

    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 100;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(stdOutput, &cursorInfo);

    int lastColor = -1;
    lastX = -2;
    lastY = 0;
    DWORD dummy;

    for (int y = 0; y < gRows; y++)
        for (int x = 0; x < gCols; x++) {
            const size_t i = y * gCols + x;
            if (pxsSame(fBuf[i], bBuf[i]))
                continue;
            cursorTo(x, y);

            const int curColor = 16 * fBuf[i].bg + fBuf[i].fg;
            if (lastColor < 0 || lastColor != curColor)
                SetConsoleTextAttribute(stdOutput, curColor);
            lastColor = curColor;

            WriteFile(stdOutput, &fBuf[i].txt, 1, &dummy, NULL);
            bBuf[i] = fBuf[i];
        }
    clearFbuf();
    FlushFileBuffers(stdOutput);

    cursorTo(0, 0);
    SetConsoleTextAttribute(stdOutput, C_GRAY);
}

void pxCleanup() {
    FreeConsole();
}

struct pixel* px(size_t idx) {
    return &fBuf[idx];
}

struct pixel* pxAt(int x, int y) {
    return px(y * gCols + x);
}

// Intentional `return 1`/`return 0`s:

int kUp(int code) {
    if (kbd[code / 32] & (1 << (code % 32)))
        return 0;
    return 1;
}

int kDown(int code) {
    if (kbd[code / 32] & (1 << (code % 32)))
        return 1;
    return 0;
}

int kPressed(int code) {
    if (lastKbd[code / 32] & (1 << (code % 32)))
        return 0;
    return kDown(code);
}
