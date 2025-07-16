#ifndef DRAWHEAD_H
#define DRAWHEAD_H

#include "drawcmmn.h"
#include "sensorReadings.h"
extern SensorData sensorDataFinal;

// here we draw the header information for all the pages

// draw clock + city + date + battery % in landscape mode
void drawSensorAndClockFinal();
void drawClockPartialFinal(const DateTime& now);

// draw clock + city + date + battery % in portrait mode
void drawSensorAndClockBook();
void drawClockPartialBook(const DateTime& now);
#endif  // DRAWPAGE3_H
