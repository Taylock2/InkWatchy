#ifndef WMANAGE_ALL_H
#define WMANAGE_ALL_H

#include "../../../defines/defines.h"

#define WATCHFACE_COUNT 1

extern uint8_t watchfaceSelected;

// This is a forward declaration
struct watchfaceDefOne;
extern const watchfaceDef *watchfacesList[];

const watchfaceDef *getCurrentWatchface();
const watchfaceDefOne *getwatchfaceDefOne();
void initWatchfaceManage();
void loopWatchfaceManage();

#endif
