#pragma once

#include "defines.h"

#if TIME_TOOLS

void initStopwatch();
void loopStopwatch();

void drawEmptyCircle();
void exitStopwatch();
void drawSingleSegment(int segment, bool fill);

#endif
