// TODO: make cross-platform??????
#define _WIN32_WINNT 0x0500
#include <windows.h>

#include <stdio.h>

#include "g.h"
#include "screen.h"

static char title[TITLE_MAX];
static struct pixel fBuf[SCREEN_MAX], bBuf[SCREEN_MAX];

static int rows = 0, cols = 0;
static int lastX, lastY;

static HANDLE stdInput = NULL, stdOutput = NULL;

static void updateDimensions() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(stdOutput, &csbi);
    cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

static void cursorTo(int x, int y) {
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
        // deliberate junk
        bBuf[i].txt = 69;
        bBuf[i].fg = 4;
        bBuf[i].bg = 2;
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
}

void pxInput() {
    DWORD count = 0;
    GetNumberOfConsoleInputEvents(stdInput, &count);
    if (!count)
        return;

#define INPUT_MAX (16)
    INPUT_RECORD records[INPUT_MAX];
    ReadConsoleInput(stdInput, records, INPUT_MAX, &count);
#undef INPUT_MAX

    for (size_t i = 0; i < count; i++) {
        // TODO TODO TODO...
        if (records[i].EventType == KEY_EVENT) {
            KEY_EVENT_RECORD event = records[i].Event.KeyEvent;
            WORD scancode = event.wVirtualScanCode;
            if (scancode == 1) // exit on ESC - TODO: remove
                gExit = 1;
            if (scancode == 0x48)
                gDummyY--;
            if (scancode == 0x50)
                gDummyY++;
            if (scancode == 0x4b)
                gDummyX--;
            if (scancode == 0x4d)
                gDummyX++;
        }
    }
}

static int pxsSame(struct pixel a, struct pixel b) {
    return a.txt == b.txt && a.fg == b.fg && a.bg == b.bg;
}

static void displayFPS() {
#define BUF_MAX (512)
    static char buf[BUF_MAX] = {0};
    sprintf(buf, "%d (%dx%d)", gCurFPS, rows, cols);
    for (size_t i = 0; i < BUF_MAX; i++) {
        if (buf[i] && i < cols) {
            pxAt(i, 0)->txt = buf[i];
            pxAt(i, 0)->bg = C_BLACK;
            pxAt(i, 0)->fg = C_WHITE;
        }
        buf[i] = 0;
    }
#undef BUF_MAX
}

void pxCommit() {
    if (stdInput == NULL)
        printf("YOU FORGOT TO CALL `pxInit()` BITCHASS!!!");

    SetConsoleTitle(title);

    const size_t lastArea = rows * cols;
    updateDimensions();
    if (rows * cols != lastArea) {
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

    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++) {
            const size_t i = y * cols + x;
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
    return px(y * cols + x);
}

int pxRows() {
    return rows;
}

int pxCols() {
    return cols;
}
