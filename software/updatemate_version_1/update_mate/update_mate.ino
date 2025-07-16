#include "drawPage1.h"
#include "drawPage2.h"
#include "drawPage3.h"
#include "drawNews.h"
#include "drawStocks.h"
#include <SPIFFS.h>
#include <Wire.h>
#include "config.h"
#include <SD.h>

SPIClass spi = SPIClass(FSPI);
volatile bool bookTime = false;

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;  // no more
    for (int i = 0; i < numTabs; i++) Serial.print('\t');
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

IRAM_ATTR void rightButtonISR() {
  unsigned long now = millis();
  if (now - lastDebounceRight < debounceDelay) return;
  lastDebounceRight = now;

  if (page == 3 && !selectBooks) {
    // if in book view, scroll in books content
    scrollLine += scrollStep;
    pageChanged = true;
    return;
  }
  if (page == 3 && selectBooks) {
    // select book
    selectedBookIndex = (selectedBookIndex + maxBookIdx) % (maxBookIdx + 1);
    pageChanged = true;
    return;
  }
  // 1) block left/right while in an app page
  if (page >= 3) return;

  // 2) if we’re on page 2 in selection mode, move through icons
  if (page == 2 && selectionMode) {
    selectedCategory--;
    if (selectedCategory < 0) selectedCategory = 3;
    pageChanged = true;
    return;
  }

  // 3) otherwise, move between page 1 and 2
  if (page > 1) {
    page--;
    pageChanged = true;
  }
}

IRAM_ATTR void leftButtonISR() {
  unsigned long now = millis();
  if (now - lastDebounceLeft < debounceDelay) return;
  lastDebounceLeft = now;

  if (page == 3 && selectBooks) {
    selectedBookIndex = (selectedBookIndex + 1) % (maxBookIdx + 1);
    pageChanged = true;
    return;
  }
  if (page == 3 && !selectBooks) {
    scrollLine = max(0, scrollLine - scrollStep);
    pageChanged = true;
    return;
  }
  if (page >= 3) return;
  if (page == 2 && selectionMode) {
    selectedCategory++;
    if (selectedCategory > 3) selectedCategory = 0;
    pageChanged = true;
    return;
  }

  if (page < 2) {
    page++;
    pageChanged = true;
  }
}
IRAM_ATTR void enterButtonISR() {
  unsigned long now = millis();
  if (now - lastDebounceEnter < debounceDelay) return;
  lastDebounceEnter = now;

  // --- Page 3: either start selecting, show book, or go back to list/exit ---
  if (page == 3) {
    // If you're currently on the list, and you press Enter on a valid book:
    if (selectBooks && selectedBookIndex < maxBookIdx) {
      // switch into "show book" mode
      selectBooks = false;
      scrollLine = 0;
      bookTime = true;
    } else if (!selectBooks) {
      // already showing the book, pressing Enter returns to the list
      selectBooks = true;
      listNeedsFullRefresh = true;
      bookTime = false;
    } else {
      // selectBooks && selectedBookIndex == maxBookIdx → Exit entry
      page = 2;
      selectionMode = true;
      selectBooks = false;  // reset for next time
    }

    pageChanged = true;
    return;
  }

  // --- Page 2: app menu selection / launch ---
  if (page == 2) {
    if (!selectionMode) {
      selectionMode = true;
      selectedCategory = 0;
    } else {
      switch (selectedCategory) {
        case 0:  // eBook
          page = 3;
          selectBooks = true;
          selectedBookIndex = 0;
          scrollLine = 0;
          listNeedsFullRefresh = true;
          break;
        case 1: page = 4; break;  // News
        case 2: page = 5; break;  // Stocks
        case 3: page = 1; break;  // Exit
      }
      selectionMode = false;
    }
    pageChanged = true;
    return;
  }
  // Enter and exit the stocks and news app
  if (page == 4 || page == 5) {
    page = 2;
    selectionMode = true;
    pageChanged = true;
  }
}
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

void initSDcard() {
  Serial.println("Mounting SD card…");
  if (!SD.begin(SD_CS, mySPI)) {
    Serial.println("SD init failed!");
  } else {
    Serial.println("SD initialized");
    // List files:
    Serial.println("Files on SD:");
    File root = SD.open("/");
    printDirectory(root, 0);
    root.close();
  }
}

void setup() {
  initDisplay();
  initSDcard();
  // Setup buttons and attach interrupts
  pinMode(RIGHT_BUT, INPUT_PULLUP);
  pinMode(LEFT_BUT, INPUT_PULLUP);
  pinMode(ENTER_BUT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUT), rightButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUT), leftButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENTER_BUT), enterButtonISR, FALLING);

  // Connect to WiFi.
  WiFi.begin(ssid, password);
  Serial.println("WiFi..");
  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiF Err!");
  } else {
    Serial.println("\nWiF OK");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  // Initialize SPIFFS
  Serial.println("SPI..");
  if (!SPIFFS.begin(true)) {
    Serial.println("SPI Err!");
  } else {
    Serial.println("SPI OK");
  }

  // Power on I2C
  pinMode(I2C_POWER_PIN, OUTPUT);
  digitalWrite(I2C_POWER_PIN, HIGH);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize sensors
  if (!initSensors()) {
    Serial.println("Sens Err!");
    while (1)
      ;
  } else {
    Serial.println("Sens OK");
  }
  updateSensorData(sensorDataFinal);
  // Fetch calendar data once on boot
  fetchCalendarData();
  if (sensorDataFinal.rtcTime.hour() > CALENDAR_FETCH_HOUR || (sensorDataFinal.rtcTime.hour() == CALENDAR_FETCH_HOUR && sensorDataFinal.rtcTime.minute() >= CALENDAR_FETCH_MINUTE)) {
    calendarFetchedToday = true;
    Serial.println("Cal set");
  } else {
    calendarFetchedToday = false;
    Serial.println("Cal clr");
  }

  // Draw initial display (Page 1).
  fullRefreshPage1();

  lastRefreshTime = sensorDataFinal.rtcTime;
  Serial.print("R: ");
  Serial.println(lastRefreshTime.timestamp(DateTime::TIMESTAMP_FULL));
}
void ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.println("WiFi lost! Reconnecting...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? "\nWiFi OK" : "\nWiFi Err!");
}
void handlePage(int pg,
                DateTime& lastRef,
                DateTime& lastClk,
                void (*fullRefresh)(),
                void (*partialClock)(const DateTime&)) {
  if (page != pg) return;
  DateTime now = sensorDataFinal.rtcTime;
  // do a full refresh for the content
  if ((now - lastRef).totalseconds() >= rtcRefreshIntervalSeconds) {
    ensureWiFi();
    Serial.printf("RTC ref (page %d)\n", pg);
    fullRefresh();
    lastRef = now;
    lastClk = now;
    Serial.print("Ref");
    Serial.println(lastRef.timestamp(DateTime::TIMESTAMP_FULL));
  }
  // do a partial refreash for the clock
  if ((now - lastClk).totalseconds() >= rtcClockUpdate) {
    partialClock(now);
    lastClk = now;
  }
}

void loop() {
  if (pageChanged) {
    pageChanged = false;
    switch (page) {
      case 1:
        fullRefreshPage1();
        lastRefreshTime = sensorDataFinal.rtcTime;
        break;
      case 2:
        fullRefreshPage2();
        lastRefreshTime = sensorDataFinal.rtcTime;
        break;
      case 3:
        fullRefreshPage3();
        lastRefreshTime = sensorDataFinal.rtcTime;
        break;
      case 4:
        fullRefreshPageNews();
        lastRefreshTime = sensorDataFinal.rtcTime;
        break;
      case 5:
        fullRefreshPageStocks();
        lastRefreshTime = sensorDataFinal.rtcTime;
        break;
    }
  }

  updateSensorData(sensorDataFinal);
  // Daily Calendar Fetch Check at 00:10
  if (sensorDataFinal.rtcTime.hour() == CALENDAR_FETCH_HOUR && sensorDataFinal.rtcTime.minute() < CALENDAR_FETCH_MINUTE) {
    if (calendarFetchedToday) {
      Serial.println("Cal reset");
      calendarFetchedToday = false;
    }
  }
  if (sensorDataFinal.rtcTime.hour() < CALENDAR_FETCH_HOUR) {
    if (calendarFetchedToday) {
      Serial.println("Cal reset");
      calendarFetchedToday = false;
    }
  }
  if (sensorDataFinal.rtcTime.hour() == CALENDAR_FETCH_HOUR && sensorDataFinal.rtcTime.minute() == CALENDAR_FETCH_MINUTE && !calendarFetchedToday) {
    Serial.println("Cal fetch");
    fetchCalendarData();
    fullRefreshPage1();
    lastRefreshTime = sensorDataFinal.rtcTime;
  }

  handlePage(1, lastRefreshTime, lastClockUpdate, fullRefreshPage1, drawClockPartialFinal);
  handlePage(2, lastRefreshTime, lastClockUpdate, fullRefreshPage2, drawClockPartialFinal);
  if (bookTime == false) {
    handlePage(3, lastRefreshTime, lastClockUpdate, fullRefreshPage3, drawClockPartialFinal);
  } else {
    handlePage(3, lastRefreshTime, lastClockUpdate, fullRefreshPage3, drawClockPartialBook);
  }
  handlePage(4, lastRefreshTime, lastClockUpdate, fullRefreshPageNews, drawClockPartialFinal);
  handlePage(5, lastRefreshTime, lastClockUpdate, fullRefreshPageStocks, drawClockPartialFinal);
  delay(200);
}