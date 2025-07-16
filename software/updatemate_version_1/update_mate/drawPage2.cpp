#include "drawPage2.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

volatile bool selectionMode = false;
volatile int selectedCategory = 0;
volatile bool inCategoryPage = false;
volatile int fromCategoryPage = 0;


void drawSensorAndClock2() {
  drawSensorAndClockFinal();
}



void drawStaticContent2() {
  // Bottom buttons
  drawBmp("/next_page.bmp", 370, 445, display);
  drawBmp("/curr_page.bmp", 410, 445, display);

  // Apps header
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(65, 90);
  display.setTextColor(GxEPD_BLACK);
  display.print("APPS");
  display.fillRect(50, 100, 95, 3, GxEPD_BLACK);

  // Icon bitmaps & labels
  const char* icons[] = { "/ebook.bmp", "/news.bmp", "/stocks.bmp", "/exit.bmp" };
  const char* labels[] = { "eBook", "News", "Stocks", "Exit" };
  const int iconSize = 40;
  const int frameSize = 60;
  const int spacing = 70;
  const int startX = 100;
  const int startY = 130;

  // 1) Draw all 4 icons
  for (int i = 0; i < 4; i++) {
    int x = startX + i * spacing;
    drawBmp(icons[i], x, startY, display);
  }

  // 2) If in selection mode, draw frame + redraw icon + draw label
  if (selectionMode) {
    int i = selectedCategory;
    int sx = startX + i * spacing;
    int sy = startY;

    // draw the 60×60 frame centered behind the 40×40 icon
    int fx = sx - (frameSize - iconSize) / 2;  
    int fy = sy - (frameSize - iconSize) / 2;  
    drawBmp("/select_frame.bmp", fx, fy, display);

    // redraw the icon on top
    drawBmp(icons[i], sx, sy, display);

    // now draw the label centered under the icon
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);

    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(labels[i], 0, 0, &x1, &y1, &w, &h);
    // center under icon
    int tx = sx + (iconSize - w) / 2;
    int ty = sy + iconSize + h + 15; 
    display.setCursor(tx, ty);
    display.print(labels[i]);
  }
}

void fullRefreshPage2() {
  display.setRotation(0);
  display.setPartialWindow(0, 0, 800, 480);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawStaticContent2();
    drawSensorAndClock2();
  } while (display.nextPage());
}