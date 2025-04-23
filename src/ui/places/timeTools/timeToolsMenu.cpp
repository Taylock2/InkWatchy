#include "timeToolsMenu.h"

#if TIME_TOOLS

void initTimeTools(){
    entryMenu buttons[2];
    buttons[0] = {"Timer", getImg("timetools/timer"), switchTimer};
    buttons[1] = {"Stopwatch", getImg("timetools/stopwatch"), switchStopwatch};

    initMenu(buttons, 2, "Time Tools", 1);
}

#endif