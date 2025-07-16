#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <RTClib.h>   
 
// Pin Definitions
#define SD_SCK 6
#define SD_MISO 2
#define SD_MOSI 7
#define SD_CS 4
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_POWER_PIN 19
#define BME688_ADDRESS 0x76
#define RIGHT_BUT 9
#define LEFT_BUT 8
#define ENTER_BUT 15

extern const char* ssid;
extern const char* password;

extern volatile int page;
extern volatile bool pageChanged;
extern volatile unsigned long lastDebounceRight;
extern volatile unsigned long lastDebounceLeft;
extern volatile unsigned long lastDebounceEnter;
extern const unsigned long debounceDelay; 

extern DateTime lastRefreshTime;
extern DateTime lastClockUpdate;  
extern DateTime lastRefreshTime2;
extern DateTime lastClockUpdate2;
extern DateTime lastRefreshTime3;
extern DateTime lastClockUpdate3;    
extern DateTime lastRefreshTimeNews;
extern DateTime lastClockUpdateNews;    
extern const uint32_t rtcRefreshIntervalSeconds;  
extern const uint32_t rtcClockUpdate;



#endif 