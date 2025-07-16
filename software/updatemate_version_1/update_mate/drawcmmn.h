#ifndef DRAWCMMON_H
#define DRAWCMMON_H

#define ENABLE_GxEPD2_GFX 0

#include <SPI.h>
#include <GxEPD2_BW.h>

#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include "draw_icons.h"
#include "bitmaps/Bitmaps800x480.h"
#include <vector>
#include "config.h"

extern SPIClass mySPI;
#if defined(ESP32)
extern GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display;
#endif


#endif // DRAWCMMON_H
