#include "drawHead.h"


SensorData sensorDataFinal;

void drawClockPartialFinal(const DateTime& now) {
  // if the battery drops below 4 message apperas
  if (sensorDataFinal.batterySOC < 4) {
    drawBmp("/low_bat.bmp", 0, 0, display);
    return;
  }
  display.setRotation(0);
  display.setPartialWindow(0, 0, 100, 20);

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);



    char timeStr[6];  // "hh:mm"
    sprintf(timeStr, "%02d:%02d", now.hour(), now.minute());

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(20, 20);
    display.print(timeStr);
  } while (display.nextPage());

  display.setFullWindow();
}

void drawClockPartialBook(const DateTime& now) {
  // if the battery drops below 4 message apperas
  if (sensorDataFinal.batterySOC < 4) {
    drawBmp("/low_bat.bmp", 0, 0, display);
    return;
  }
  display.setRotation(1);  // Set to book orientation (portrait)
  display.setPartialWindow(0, 0, 100, 20);

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", now.hour(), now.minute());

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(20, 20);
    display.print(timeStr);
  } while (display.nextPage());

  display.setFullWindow();
  //display.hibernate();
}
void drawSensorAndClockBook() {
  // if the battery drops below 4 message apperas
  if (sensorDataFinal.batterySOC < 4) {
    drawBmp("/low_bat.bmp", 0, 0, display);
    return;
  }
  char sensorStr[30];
  // Read and clamp battery SOC at 100%
  int socRaw = sensorDataFinal.batterySOC;
  int soc = socRaw > 100 ? 100 : socRaw;
  sprintf(sensorStr, "%d%%", soc);

  // Default text position
  int textX = 405;
  int textY = 20;

  // shift left 5px if bat == 100
  if (socRaw > 100) {
    textX -= 15;
  }

  // Print battery percentage
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(textX, textY);
  display.setTextColor(GxEPD_BLACK);
  display.print(sensorStr);
  // Battery icon dimensions
  int batteryX = 440;
  int batteryY = 8;
  int batteryWidth = 24;
  int batteryHeight = 14;
  int batteryTipWidth = 2;

  // Draw the main battery rectangle outline
  display.drawRect(batteryX, batteryY, batteryWidth, batteryHeight, GxEPD_BLACK);

  // Draw the battery "tip" on the right
  int tipX = batteryX + batteryWidth;
  int tipY = batteryY + (batteryHeight / 4);
  int tipHeight = batteryHeight / 2;
  display.drawRect(tipX, tipY, batteryTipWidth, tipHeight, GxEPD_BLACK);

  // Compute the filled width from batterySOC (0–100%)
  int fillWidth = (sensorDataFinal.batterySOC * batteryWidth) / 100;
  if (fillWidth > batteryWidth) fillWidth = batteryWidth;

  // Fill the battery proportionally
  display.fillRect(batteryX, batteryY, fillWidth, batteryHeight, GxEPD_BLACK);

  // Check for battery charging
  if (sensorDataFinal.batteryChg > 0) {
    //Serial.println(sensorDataFinal.batteryChg);
    drawBmp("/charge.bmp", 396, 5, display);
  }



  // Clock & Date.
  char dateStr[40];
  char timeStr[10];
  const char* dayName = getDayName(sensorDataFinal.rtcTime.dayOfTheWeek());
  const char* monthName = getMonthName(sensorDataFinal.rtcTime.month());
  sprintf(dateStr, "%s %02d %s", dayName, sensorDataFinal.rtcTime.day(), monthName);
  sprintf(timeStr, "%02d:%02d", sensorDataFinal.rtcTime.hour(), sensorDataFinal.rtcTime.minute());
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(140, 20);
  display.print(dateStr);
  display.setCursor(20, 20);
  display.print(timeStr);
  display.setCursor(255, 20);
  display.print("Bucharest");
}

void drawSensorAndClockFinal() {
  // if the battery drops below 4 message apperas
  if (sensorDataFinal.batterySOC < 4) {
    drawBmp("/low_bat.bmp", 0, 0, display);
    return;
  }

  char sensorStr[30];
  // Read and clamp battery SOC at 100%
  int socRaw = sensorDataFinal.batterySOC;
  int soc = socRaw > 100 ? 100 : socRaw;
  sprintf(sensorStr, "%d%%", soc);

  // Default text position
  int textX = 725;
  int textY = 20;

  // shift left 5px if bat == 100
  if (socRaw > 100) {
    textX -= 15;
  }

  // Print battery percentage
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(textX, textY);
  display.setTextColor(GxEPD_BLACK);
  display.print(sensorStr);
  // Battery icon dimensions
  int batteryX = 760;
  int batteryY = 8;
  int batteryWidth = 24;
  int batteryHeight = 14;
  int batteryTipWidth = 2;

  // Draw the main battery rectangle outline
  display.drawRect(batteryX, batteryY, batteryWidth, batteryHeight, GxEPD_BLACK);

  // Draw the battery "tip" on the right
  int tipX = batteryX + batteryWidth;
  int tipY = batteryY + (batteryHeight / 4);
  int tipHeight = batteryHeight / 2;
  display.drawRect(tipX, tipY, batteryTipWidth, tipHeight, GxEPD_BLACK);

  // Compute the filled width from batterySOC (0–100%)
  int fillWidth = (sensorDataFinal.batterySOC * batteryWidth) / 100;
  if (fillWidth > batteryWidth) fillWidth = batteryWidth;

  // Fill the battery proportionally
  display.fillRect(batteryX, batteryY, fillWidth, batteryHeight, GxEPD_BLACK);

  // Check for battery charging
  if (sensorDataFinal.batteryChg > 0) {
    drawBmp("/charge.bmp", 696, 5, display);
  }

  // Clock & Date.
  char dateStr[40];
  char timeStr[10];
  const char* dayName = getDayName(sensorDataFinal.rtcTime.dayOfTheWeek());
  const char* monthName = getMonthName(sensorDataFinal.rtcTime.month());
  sprintf(dateStr, "%s %02d %s", dayName, sensorDataFinal.rtcTime.day(), monthName);
  sprintf(timeStr, "%02d:%02d", sensorDataFinal.rtcTime.hour(), sensorDataFinal.rtcTime.minute());
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(305, 20);
  display.print(dateStr);
  display.setCursor(20, 20);
  display.print(timeStr);
  display.setCursor(420, 20);
  display.print("Bucharest");
}