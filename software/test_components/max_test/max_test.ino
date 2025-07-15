#include <Arduino.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <Wire.h>

SFE_MAX1704X lipo(MAX1704X_MAX17048);

double voltage = 0;  // Variable to keep track of LiPo voltage
double soc = 0;      // Variable to keep track of LiPo state-of-charge (SOC)
float change = 0;
#define SDA_PIN 21  // ESP32-C6 SDA pin
#define SCL_PIN 22  // ESP32-C6 SCL pin
#define I2C_PW 19   // GPIO19 controls I2C power

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Enable I2C power
  pinMode(I2C_PW, OUTPUT);
  digitalWrite(I2C_PW, HIGH);
  Serial.println("I2C power enabled");
  delay(1000);  // Allow power to stabilize

  while (!Serial)
    ;  // Wait for user to open terminal
  Serial.println(F("MAX17043 Example"));
  Wire.begin(SDA_PIN, SCL_PIN);
  lipo.enableDebugging();
  if (lipo.begin() == false) {
    Serial.println(
      F("MAX17043 not detected. Please check wiring. Freezing."));
    while (1)
      ;
  }

  //lipo.quickStart();
  lipo.setThreshold(20);
}

void loop() {
  // lipo.getVoltage() returns a voltage value (e.g. 3.93)
  voltage = lipo.getVoltage();
  // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
  soc = lipo.getSOC();

  change = lipo.getChangeRate();

  // Print the variables:
  Serial.print("Voltage: ");
  Serial.print(voltage);  // Print the battery voltage
  Serial.println(" V");

  Serial.print("Percentage: ");
  Serial.print(soc);  // Print the battery state of charge
  Serial.println(" %");

  if (change >= 0) {
    Serial.print("Charging: ");
    Serial.print(change);
    Serial.println();
  } else {
    Serial.print("Discharge: ");
    Serial.print(change);
    Serial.println();
  }
  delay(3000);
}