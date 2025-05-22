#pragma once

#include <stdint.h>

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

int kUp(int);
int kDown(int);
int kPressed(int);

// Straight outta <https://github.com/nonk123/conge>:

enum {
    C_BLACK,
    C_BLUE,
    C_GREEN,
    C_AQUA,
    C_RED,
    C_PURPLE,
    C_YELLOW,
    C_WHITE,
    C_GRAY,
    C_BRIGHT_BLUE,
    C_BRIGHT_GREEN,
    C_BRIGHT_AQUA,
    C_BRIGHT_RED,
    C_BRIGHT_PURPLE,
    C_BRIGHT_YELLOW,
    C_BRIGHT_WHITE,
};

enum {
    K_EMPTY,
    K_ESC,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_0,
    K_HYPHEN,
    K_EQUALS,
    K_BACKSPACE,
    K_TAB,
    K_Q,
    K_W,
    K_E,
    K_R,
    K_T,
    K_Y,
    K_U,
    K_I,
    K_O,
    K_P,
    K_LEFT_BRACKET,
    K_RIGHT_BRACKET,
    K_ENTER,
    K_LCTRL,
    K_A,
    K_S,
    K_D,
    K_F,
    K_G,
    K_H,
    K_J,
    K_K,
    K_L,
    K_SEMICOLON,
    K_QUOTE,
    K_GRAVE, // also known as tilde
    K_LSHIFT,
    K_BACKSLASH,
    K_Z,
    K_X,
    K_C,
    K_V,
    K_B,
    K_N,
    K_M,
    K_COMMA,
    K_FULL_STOP,
    K_SLASH,
    K_RSHIFT,
    K_PRT_SCR,
    K_LALT,
    K_SPACEBAR,
    K_CAPS_LOCK,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_NUMLOCK,
    K_SCROLL_LOCK,
    KP_7,
    KP_8,
    KP_9,
    KP_MINUS,
    KP_4,
    KP_5,
    KP_6,
    KP_PLUS,
    KP_1,
    KP_2,
    KP_3,
    KP_0,
    KP_DOT,
};
