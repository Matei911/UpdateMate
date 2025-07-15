#include <SPI.h>
#include <SD.h>

#define SD_SCK  6
#define SD_MISO 2
#define SD_MOSI 7
#define SD_CS   4

SPIClass spi = SPIClass(FSPI); // Use FSPI or HSPI as applicable

// Function to list files in a directory recursively
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (int i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize custom SPI bus
  spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Initialize the SD card using the custom SPI instance
  if (!SD.begin(SD_CS, spi)) {
    Serial.println("SD card initialization failed!");
    while (true) { /* Halt execution */ }
  }
  Serial.println("SD card initialized successfully.");

  // List files in the root directory
  Serial.println("Listing files in root directory:");
  File root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  // Write a test file
  File testFile = SD.open("/test.txt", FILE_WRITE);
  if (testFile) {
    Serial.println("Writing to /test.txt");
    testFile.println("Hello, SD card! This is a test file.");
    testFile.close();
    Serial.println("File written successfully.");
  } else {
    Serial.println("Error opening /test.txt for writing.");
  }

  // Read the test file back
  testFile = SD.open("/test.txt");
  if (testFile) {
    Serial.println("Reading /test.txt:");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();
    Serial.println("\nRead complete.");
  } else {
    Serial.println("Error opening /test.txt for reading.");
  }
}

void loop() {
  // No repeating code needed
}
