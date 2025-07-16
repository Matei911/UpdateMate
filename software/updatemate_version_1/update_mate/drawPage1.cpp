#include "drawPage1.h"
#include <Arduino.h>


// API for Visual Crossing and URL for the script in Google console
// Personal Calendar
WeatherForecast forecast("API_KEY");
String scriptUrl = "GOOGLE_SCRIPT_URL";

// Daily Calendar Fetch Control
bool calendarFetchedToday = false;
const int CALENDAR_FETCH_HOUR = 0;     // 00 hours
const int CALENDAR_FETCH_MINUTE = 10;  // 10 minutes past midnight

// Google Calendar Data Storage
const int MAX_EVENTS = 7;
std::vector<EventInfo> calendarEvents;
bool noEventsFound = false;

// Calendar Drawing Configuration
const int eventStartX = 60;
const int eventStartY = 140;
const int eventLineHeight = 15;
const int maxEventLength = 20;
const int maxLocationLength = 20;




void storeEventData(const char* title, const char* startTime, const char* endTime, const char* location) {
  if (calendarEvents.size() < MAX_EVENTS) {
    EventInfo event;
    event.title = String(title);
    if (event.title.length() > maxEventLength) {
      event.title = event.title.substring(0, maxEventLength - 3) + "...";
    }
    event.startTime = String(startTime);
    event.endTime = String(endTime);
    event.location = String(location);
    calendarEvents.push_back(event);
  } else {
    Serial.println("Max event");
  }
}

void printStatus(const char* message) {
  Serial.println(message);
}

void markNoEvents() {
  noEventsFound = true;
}


// Function to draw static content on Page 1
void drawStaticContent() {
  // Bottom buttons
  drawBmp("/curr_page.bmp", 370, 445, display);
  drawBmp("/next_page.bmp", 410, 445, display);

  // Sensors
  drawBmp("/home.bmp", 440, 310, display);

  // Schedule
  drawBmp("/schedule.bmp", 40, 60, display);

  // Forecast
  drawBmp("/temperature.bmp", 440, 60, display);

  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(90, 100);
  display.print("Schedule");
  display.setCursor(490, 100);
  display.print("Forecast");
  display.setCursor(490, 350);
  display.print("Sensors");
}

//--- Dynamic Data Drawings ---

// Draw BME stats
void drawBME() {
  char sensorStr[30];
  // Temperature.
  sprintf(sensorStr, "T %.1f C", sensorDataFinal.temperature);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(450, 390);
  display.print(sensorStr);

  // Degree symbol.
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(540, 380);
  display.print("o");

  // Humidity.
  sprintf(sensorStr, "Hu %.1f%%", sensorDataFinal.humidity);
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(450, 420);
  display.print(sensorStr);

  // Pressure.
  sprintf(sensorStr, "P %d hPa", (int)sensorDataFinal.pressure);
  display.setCursor(590, 390);
  display.print(sensorStr);
  // Gas.
  sprintf(sensorStr, "Gas %d KOhm", (int)sensorDataFinal.gasResistance);
  display.setCursor(590, 420);
  display.print(sensorStr);
}
void drawSensorAndClock() {
  drawSensorAndClockFinal();
}

void drawForecast() {
  // Allocate variables for today's forecast and an array for the next 3 days
  ForecastToday todayForecast;
  ForecastDay forecastDays[3];

  // Make a single API call to get the entire forecast
  if (forecast.getForecast(todayForecast, forecastDays, 3)) {

    int todayX = 450;
    int todayY = 100;
    int bmpHeight = 60;  // BMP icon height.

    // Draw main temperature text
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(530, todayY + 40);
    display.print(todayForecast.temp);

    // Draw degree indicator
    display.setCursor(580, 140);
    display.print("C");
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(570, 122);
    display.print("o");

    // Draw the weather icon
    String todayIconFilename = "/" + todayForecast.icon + ".bmp";
    drawBmp(todayIconFilename.c_str(), todayX, todayY + 10, display);

    // Draw today's high and low
    char tempBuf[20];
    sprintf(tempBuf, "%d-%d", (int)todayForecast.low, (int)todayForecast.high);
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(todayX + 80, todayY + 70);
    display.print(tempBuf);

    // Draw degree indicator
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(598, 160);
    display.print("o");
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(608, 170);
    display.print("C");

    display.fillRect(450, 180, 320, 3, GxEPD_BLACK);

    // Draw three day forecast
    int startX = 450;
    int startY = 180;
    int spacingX = 120;

    for (int i = 0; i < 3; i++) {
      int x = startX + i * spacingX;
      int y = startY;

      display.setFont(&FreeMonoBold9pt7b);
      // Draw the formatted date
      display.setCursor(x, y + 24);
      display.print(forecastDays[i].date);

      // Draw forecast icon
      String iconFilename = "/" + forecastDays[i].icon + ".bmp";
      drawBmp(iconFilename.c_str(), x, y + 30, display);

      // Draw low-high temperature range.
      sprintf(tempBuf, "%d-%d", (int)forecastDays[i].low, (int)forecastDays[i].high);
      display.setFont(&FreeMonoBold9pt7b);
      display.setCursor(x, y + bmpHeight + 45);
      display.print(tempBuf);

      // // Draw degree indicator
      display.setFont(&FreeMonoBold9pt7b);
      display.setCursor(x + 54, y + bmpHeight + 37);
      display.print("o");
      display.setFont(&FreeMonoBold12pt7b);
      display.setCursor(x + 64, y + bmpHeight + 45);
      display.print("C");
    }
  } else {
    Serial.println("Failed to get forecast data.");
  }
}
// Draw Calendar Events
void drawCalendarEvents() {
  int currentY = eventStartY;

  if (noEventsFound) {
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(eventStartX, currentY);
    display.print("No events today.");
  } else {
    for (const auto& event : calendarEvents) {
      // Check available space
      if (currentY > display.height() - (2 * eventLineHeight + 10)) {
        display.setCursor(eventStartX, currentY);
        display.print("...");
        break;
      }

      // Prepare event title, if too long truncate
      String eventTitle = event.title;
      if (eventTitle.length() > maxEventLength) {
        eventTitle = eventTitle.substring(0, maxEventLength - 3) + "...";
      }

      String eventLine = event.startTime + " " + eventTitle;

      display.setFont(&FreeMonoBold12pt7b);
      display.setTextColor(GxEPD_BLACK);
      display.setCursor(eventStartX, currentY);
      display.print(eventLine);

      currentY += eventLineHeight;
      currentY += eventLineHeight + 20;
    }
  }
}
// Full Display Refresh for Page 1
void fullRefreshPage1() {
  display.setRotation(0);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawStaticContent();
    drawBME();
    drawForecast();
    drawCalendarEvents();
    drawSensorAndClock();
    //display.hibernate();
  } while (display.nextPage());
  //display.hibernate();
}

// Calendar fetch data, once a day
void fetchCalendarData() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Fetch calendar");
    calendarEvents.clear();
    noEventsFound = false;
    fetchAndProcessCalendarEvents(scriptUrl, storeEventData, printStatus, markNoEvents);
    calendarFetchedToday = true;
  } else {
    Serial.println("WiFi not connected calendar");
  }
}