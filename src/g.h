#pragma once

#ifdef G_IMPL
#define VAR(type, name, val) type g##name = (val)
#else
#define VAR(type, name, val) extern type g##name
#endif

VAR(int, Exit, 0);
VAR(int, TargetFPS, 60);
VAR(int, CurFPS, 60);
VAR(int, DisplayFPS, 1);
VAR(int, ExitOnEsc, 1);

VAR(int, DummyX, 8);
VAR(int, DummyY, 8);

VAR(int, Rows, 0);
VAR(int, Cols, 0);
