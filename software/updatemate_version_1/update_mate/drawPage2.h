#ifndef DRAWPAGE2_H
#define DRAWPAGE2_H

#include "drawcmmn.h"
#include "sensorReadings.h"
#include "drawHead.h"
extern volatile bool selectionMode;
extern volatile int selectedCategory; 
extern volatile bool inCategoryPage;
extern volatile int fromCategoryPage;

void fullRefreshPage2();

#endif  // DRAWPAGE2_H
