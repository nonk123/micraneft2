#pragma once

#include "stdint.h"

#define CLOCK_RES (100000)
typedef int64_t instant;

void clockInit();
instant elapsed();

void mcSleep(int);
