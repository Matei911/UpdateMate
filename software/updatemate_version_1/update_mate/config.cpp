#include "config.h"

volatile int page = 1;
volatile bool pageChanged = false;
volatile unsigned long lastDebounceRight = 0;
volatile unsigned long lastDebounceLeft = 0;
volatile unsigned long lastDebounceEnter = 0;
const unsigned long debounceDelay = 300;  // milliseconds

DateTime lastRefreshTime;
DateTime lastClockUpdate;
DateTime lastRefreshTimeNews;
DateTime lastClockUpdateNews;               
const uint32_t rtcRefreshIntervalSeconds = 300;  // 5 minutes
const uint32_t rtcClockUpdate = 60;  // 1 minutes

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASS";
 
