#include "drawStocks.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Simple struct for holding one quote
struct Quote {
  float price;
  float change;
  float pctChange;
};

static bool fetchJson(const String& url, StaticJsonDocument<512>& doc) {
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    http.end();
    return false;
  }
  String body = http.getString();
  http.end();
  return !deserializeJson(doc, body);
}

// Fetch exchange rate for a symbol like "EUR/RON"
static bool fetchExchange(const char* symbol, Quote& out) {
  String url = String("https://api.twelvedata.com/exchange_rate?symbol=")
               + symbol + "&apikey=" + "API_KEY";
  StaticJsonDocument<512> doc;
  if (!fetchJson(url, doc)) return false;
  out.price = doc["rate"].as<float>();

  out.change = 0;
  out.pctChange = 0;
  return true;
}

// Fetch stock quote for a symbol like "AAPL"
static bool fetchStock(const char* symbol, Quote& out) {
  String url = String("https://api.twelvedata.com/quote?symbol=")
               + symbol + "&apikey=" + "API_KEY";
  StaticJsonDocument<512> doc;
  if (!fetchJson(url, doc)) return false;
  out.price = doc["close"].as<float>();
  out.change = doc["change"].as<float>();
  out.pctChange = doc["percent_change"].as<float>();
  return true;
}

void drawSensorAndClockStocks() {
  drawSensorAndClockFinal();
}

void drawStaticContentStocks() {
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(90, 100);
  display.setTextColor(GxEPD_BLACK);
  display.print("Exchange Rates");

  display.setCursor(490, 100);
  display.print("Stock Price");

  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(490, 460);
  display.print("Provided by");
  drawBmp("/twelvedata.bmp", 650, 440, display);

  // logos
  int x = 40, y = 130;
  drawBmp("/eur.bmp", x, y, display);
  drawBmp("/usd.bmp", x, y + 80, display);
  drawBmp("/btc.bmp", x, y + 160, display);
  drawBmp("/eth.bmp", x, y + 240, display);

  x = 440;
  y = 130;
  drawBmp("/apple.bmp", x, y, display);
  drawBmp("/google.bmp", x, y + 80, display);
  drawBmp("/meta.bmp", x, y + 160, display);
}

void fullRefreshPageStocks() {
  Quote ex[4], st[3];
  bool ok =
    fetchExchange("EUR/RON", ex[0]) && 
    fetchExchange("USD/RON", ex[1]) && 
    fetchExchange("BTC/EUR", ex[2]) && 
    fetchExchange("ETH/EUR", ex[3]) && 
    fetchStock("AAPL", st[0]) && 
    fetchStock("GOOGL", st[1]) && 
    fetchStock("META", st[2]);

  display.setRotation(0);
  display.setPartialWindow(0, 0, 800, 480);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawStaticContentStocks();
    drawSensorAndClockStocks();

    if (ok) {
      for (int i = 0; i < 4; i++) {
        int tx = 90, ty = 160 + i * 80;
        display.setFont(&FreeMonoBold12pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(tx, ty);

        display.print(ex[i].price, 2);
        display.print(" ");
        if (i < 2) display.print("RON");
        else display.print("EUR");
      }
      // Stock quotes: x=520.. y=130, spacing 80px
      for (int i = 0; i < 3; i++) {
        int tx = 490, ty = 160 + i * 80;
        display.setFont(&FreeMonoBold12pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(tx, ty);
        // price
        display.print(st[i].price, 1);
        display.print(" $  ");
        // change
        display.print(st[i].change, 1);
        display.print(" $  ");
        // pct
        display.print(st[i].pctChange, 1);
        display.print("%");
      }
    } else {
      // error message
      display.setFont(&FreeMonoBold12pt7b);
      display.setTextColor(GxEPD_BLACK);
      display.setCursor(100, 300);
      display.print("Failed to fetch data");
    }

  } while (display.nextPage());
}
