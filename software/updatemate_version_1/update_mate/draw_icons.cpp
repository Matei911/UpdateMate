#include "draw_icons.h"

uint16_t bmpRead16(File &f) {
  uint16_t result;
  result = f.read();
  result |= f.read() << 8;
  return result;
}

uint32_t bmpRead32(File &f) {
  uint32_t result;
  result = f.read();
  result |= f.read() << 8;
  result |= f.read() << 16;
  result |= f.read() << 24;
  return result;
}

void drawBmp(const char *filename, int16_t x, int16_t y, GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> &display) {
  File bmpFile = SPIFFS.open(filename, "r");
  if (!bmpFile) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return;
  }

  // Check for the BMP signature ("BM")
  if (bmpRead16(bmpFile) != 0x4D42) {
    Serial.println("Not a valid BMP file");
    bmpFile.close();
    return;
  }

  // Skip file size and reserved fields
  bmpFile.seek(10);
  uint32_t bmpImageoffset = bmpRead32(bmpFile);

  // Read DIB header: header size, image width, height, planes, and bits per pixel
  uint32_t headerSize = bmpRead32(bmpFile);
  int bmpWidth = bmpRead32(bmpFile);
  int bmpHeight = bmpRead32(bmpFile);
  uint16_t planes = bmpRead16(bmpFile);
  uint16_t bpp = bmpRead16(bmpFile);

  if (bpp != 1) {
    Serial.println("BMP is not 1-bit");
    bmpFile.close();
    return;
  }

  // Seek to pixel data
  bmpFile.seek(bmpImageoffset);

  // Each BMP row is padded to a multiple of 4 bytes
  uint32_t rowSize = ((bmpWidth + 31) / 32) * 4;
  uint8_t rowBuffer[rowSize];

  // BMP files are stored bottom-up
  for (int row = 0; row < bmpHeight; row++) {
    // Calculate file position for current row (starting at bottom)
    uint32_t pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
    bmpFile.seek(pos);
    bmpFile.read(rowBuffer, rowSize);

    for (int col = 0; col < bmpWidth; col++) {
      int byteIndex = col / 8;
      int bitIndex = 7 - (col % 8);
      bool pixelBit = (rowBuffer[byteIndex] >> bitIndex) & 0x1;
      // Typical convention: 0 = black, 1 = white
      uint16_t color = pixelBit ? GxEPD_WHITE : GxEPD_BLACK;
      display.drawPixel(x + col, y + row, color);
    }
  }

  bmpFile.close();
}

