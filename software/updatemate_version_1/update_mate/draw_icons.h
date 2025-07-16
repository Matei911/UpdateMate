#ifndef DRAWICONS_H
#define DRAWICONS_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <GxEPD2_BW.h> 

/*
    In this file we are creating a function that takes icons and other
pictures from the SPIFF memory of the ESP and renders them on EPD screen 
*/

uint16_t bmpRead16(File &f);
uint32_t bmpRead32(File &f);

void drawBmp(const char *filename, int16_t x, int16_t y, GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> &display);


#endif
