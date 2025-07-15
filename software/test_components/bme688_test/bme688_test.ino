#include <Wire.h>
#include <Adafruit_BME680.h>

// Define I2C pins
#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_POWER_PIN 19  // Power control for I2C
#define INT_RTC 0

// I2C Address of BME688
#define BME688_ADDRESS 0x76

// Create BME688 instance
Adafruit_BME680 bme;  

void setup() {
    Serial.begin(115200);
    
    // Set up I2C power control
    pinMode(I2C_POWER_PIN, OUTPUT);
    digitalWrite(I2C_POWER_PIN, HIGH);  // Turn on I2C power

    // Initialize I2C with custom pins
    Wire.begin(SDA_PIN, SCL_PIN);

    // Initialize BME688 sensor
    if (!bme.begin(BME688_ADDRESS, &Wire)) {
        Serial.println("Could not find a valid BME688 sensor, check wiring!");
        while (1);
    }

    Serial.println("BME688 Sensor initialized!");

    // Set up oversampling and filter settings
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320°C for 150ms
}

void loop() {
    Serial.println("Measuring...");

    // Perform a measurement
    if (!bme.performReading()) {
        Serial.println("Failed to perform reading!");
        return;
    }

    // Display sensor data
    Serial.print("Temperature: "); 
    Serial.print(bme.temperature);
    Serial.println(" °C");

    Serial.print("Pressure: "); 
    Serial.print(bme.pressure / 100.0);  // Convert Pa to hPa
    Serial.println(" hPa");

    Serial.print("Humidity: "); 
    Serial.print(bme.humidity);
    Serial.println(" %");

    Serial.print("Gas Resistance: "); 
    Serial.print(bme.gas_resistance);
    Serial.println(" Ohms");

    Serial.println("----------------------------");

    delay(2000); // Wait before next measurement
}
