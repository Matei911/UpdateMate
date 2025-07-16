#include "drawcmmn.h"

SPIClass mySPI(FSPI);

#if defined(ESP32)
GxEPD2_BW<GxEPD2_750_GDEY075T7, 
          GxEPD2_750_GDEY075T7::HEIGHT> 
    display(GxEPD2_750_GDEY075T7(/*cs=*/10, /*dc=*/5, /*rst=*/23, /*busy=*/3));
#endif