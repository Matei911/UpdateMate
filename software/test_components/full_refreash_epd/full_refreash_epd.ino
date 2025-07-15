#define ENABLE_GxEPD2_GFX 0
#define SD_SCK 6
#define SD_MISO 2
#define SD_MOSI 7
#define SD_CS 4

#include <GxEPD2_BW.h>
#include <SPI.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "bitmaps/Bitmaps800x480.h"

SPIClass mySPI(FSPI);

#if defined(ESP32)
GxEPD2_BW<GxEPD2_750_GDEY075T7,
          GxEPD2_750_GDEY075T7::HEIGHT>
  display(GxEPD2_750_GDEY075T7(/*cs=*/10, /*dc=*/5, /*rst=*/23, /*busy=*/3));
#endif

void initDisplay() {
  Serial.begin(115200);
  Serial.println("SET");
  pinMode(20, OUTPUT);
  digitalWrite(20, LOW);
  delay(1000);

  mySPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  display.epd2.selectSPI(mySPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  display.init(115200, true, 2, false);
}

void setup() {
  Serial.println("Init Display");
  initDisplay();
  display.setRotation(0);
}

void loop() {
  // full refreash every 5 seconds
  display.setFullWindow();           
  display.firstPage();               
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBold12pt7b);
    display.setCursor(200, 200);
    display.setTextColor(GxEPD_BLACK);
    display.print("Hello World");
  } while (display.nextPage());
  
  Serial.println("Screen updated");
  delay(5000);  
}
