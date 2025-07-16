// sensorReadings.h

#ifndef SENSOR_READINGS_H
#define SENSOR_READINGS_H

#include <RTClib.h>
#include <esp_sleep.h>
#include <Adafruit_BME680.h>
#include <Wire.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h> 

struct SensorData {
 
  float temperature;
  float humidity;
  float pressure;
  float gasResistance;
  DateTime rtcTime;
  float batteryVoltage;
  float batterySOC; 
  float batteryChg;
};

bool initSensors(); 
void updateSensorData(SensorData &data);
const char* getDayName(uint8_t day);
const char* getMonthName(uint8_t month);

#endif /