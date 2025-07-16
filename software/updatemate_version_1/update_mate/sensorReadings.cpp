#include "sensorReadings.h"


#define BME688_ADDRESS 0x76

Adafruit_BME680 bme;
RTC_DS3231 rtc;
SFE_MAX1704X lipo(MAX1704X_MAX17048);


const char* getDayName(uint8_t day) {
  const char* days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  if (day < 7) return days[day];
  return "";
}

const char* getMonthName(uint8_t month) {
  const char* months[] = {
    "",
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  if (month >= 1 && month <= 12) return months[month];
  return "";
}


bool initSensors() {
  bool success = true;

  Serial.println("Init BME688");
  if (!bme.begin(BME688_ADDRESS, &Wire)) {
    Serial.println("No BME");
    success = false;
  } else {
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
    Serial.println("BME688 success");
  }

  Serial.println("Init RTC");
  if (!rtc.begin()) {
    Serial.println("No RTC");
  } else {
    if (rtc.lostPower()) {
      Serial.println("Reinit RTC");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2025, 7, 15, 16, 18, 0));
    Serial.println("RTC init");
  }

  Serial.println("Init MAX");

  if (!lipo.begin()) {
    Serial.println(F("No MAX"));
    success = false;
  } else {
    Serial.println("MAX sucess");
    //lipo.quickStart();
  }


  return success;
}


void updateSensorData(SensorData& data) {
  if (!bme.performReading()) {
    Serial.println("Fail BME read");
  }
  data.temperature = bme.temperature;
  data.humidity = bme.humidity;
  data.pressure = bme.pressure / 100.0;
  data.gasResistance = bme.gas_resistance / 1000.0;

  // Read RTC
  data.rtcTime = rtc.now();

  // Read MAX1704x
  data.batteryVoltage = lipo.getVoltage();
  data.batterySOC = lipo.getSOC();
  data.batteryChg = lipo.getChangeRate();
}
