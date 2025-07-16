#ifndef UNIFIED_FORECAST_CALENDAR_H
#define UNIFIED_FORECAST_CALENDAR_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


// Structure for an upcoming day's forecast
struct ForecastDay {
  String date;
  int low;
  int high;
  String icon;
};

// Structure for today's forecast
struct ForecastToday {
  float low;
  float high;
  String icon;
  String temp;
  String sunrise;
  String sunset;
};

class WeatherForecast {
public:
  WeatherForecast(const char* apiKey);
  bool getForecast(ForecastToday& today, ForecastDay forecast[], size_t forecastCount);

private:
  String _apiKey;
  String _url;
};


// Google Calendar Fetching Functionality
typedef void (*EventDataCallback)(const char* title, const char* startTime, const char* endTime, const char* location);
typedef void (*StatusCallback)(const char* message);
typedef void (*NoEventsCallback)();
// Fetches calendar events from the specified Google Apps Script URL and processes the JSON
void fetchAndProcessCalendarEvents(const String& scriptUrl, EventDataCallback eventCb, StatusCallback statusCb, NoEventsCallback noEventsCb);

#endif