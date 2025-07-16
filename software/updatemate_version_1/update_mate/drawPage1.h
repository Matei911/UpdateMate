#ifndef DRAWPAGE1_H
#define DRAWPAGE1_H

#define ENABLE_GxEPD2_GFX 0

#include "drawcmmn.h"
#include "sensorReadings.h"
#include "unified_forecast_calendar.h"
#include "drawHead.h"

extern WeatherForecast forecast;
extern String scriptUrl;


extern bool calendarFetchedToday;
extern const int CALENDAR_FETCH_HOUR;
extern const int CALENDAR_FETCH_MINUTE;

// Google Calendar Data Storage 
struct EventInfo {
  String title;
  String startTime;
  String endTime;
  String location;
};

extern const int MAX_EVENTS;
extern std::vector<EventInfo> calendarEvents;
extern bool noEventsFound;

// Calendar drawing informations
extern const int eventStartX;
extern const int eventStartY;
extern const int eventLineHeight;
extern const int maxEventLength;


// Function declarations for storing events and drawing for the main page
void storeEventData(const char* title, const char* startTime, const char* endTime, const char* location);
void printStatus(const char* message);
void markNoEvents();
void fullRefreshPage1();
void fetchCalendarData();

#endif 
