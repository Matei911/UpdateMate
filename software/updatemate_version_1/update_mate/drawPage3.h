#ifndef DRAWPAGE3_H
#define DRAWPAGE3_H

#include "drawcmmn.h"
#include "sensorReadings.h"
#include "drawHead.h"

extern volatile bool selectBooks;
extern volatile int selectedBookIndex;
extern int maxBookIdx;
extern volatile bool listNeedsFullRefresh;

// scrolling state
extern int scrollLine;
extern int scrollStep;

void fullRefreshPage3();

#endif  // DRAWPAGE3_H
