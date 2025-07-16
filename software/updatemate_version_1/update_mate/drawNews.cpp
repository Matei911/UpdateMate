#include "drawNews.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// same struct
struct NewsItem {
  String title;
  String content;
};

// Fetch headlines
static int fetchTopHeadlines(NewsItem out[], int maxCount) {
  HTTPClient http;
  // 4 articles
  String url = String("https://gnews.io/api/v4/top-headlines?token=")
               + "API_KEY"
               + "&lang=en&max=4";
  http.begin(url);
  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    http.end();
    return 0;
  }
  String body = http.getString();
  http.end();

  // parse
  StaticJsonDocument<4096> doc;
  if (deserializeJson(doc, body)) return 0;
  auto arr = doc["articles"].as<JsonArray>();
  int count = min((int)arr.size(), maxCount);
  for (int i = 0; i < count; i++) {
    auto a = arr[i].as<JsonObject>();
    out[i].title = a["title"].as<String>();
    out[i].content = a["content"].as<String>();
    // sanitize newlines
    out[i].title.replace('\n', ' ');
    out[i].content.replace('\n', ' ');
  }
  return count;
}

// draw the contents
static int drawWrapped(const String &txt, int x1, int x2, int y,
                       const GFXfont *font, int lineH, int maxLines) {
  display.setFont(font);
  display.setTextColor(GxEPD_BLACK);

  // Split into words
  std::vector<String> words;
  int len = txt.length();
  for (int i = 0, j; i < len; i = j + 1) {
    // find next space
    j = i;
    while (j < len && txt[j] != ' ') j++;
    words.push_back(txt.substring(i, j));
  }

  int cursorY = y;
  int pos = 0;
  int nWords = words.size();

  while (pos < nWords && maxLines--) {
    // build up a line word-by-word
    String line = words[pos];
    int lastPos = pos + 1;
    int16_t xx, yy; uint16_t w, h;

    // try to append more words
    while (lastPos < nWords) {
      String trial = line + " " + words[lastPos];
      display.getTextBounds(trial, 0, 0, &xx, &yy, &w, &h);
      if (w > (x2 - x1)) break;
      line = trial;
      lastPos++;
    }

    // if we never fit more than one word, and that word alone is too wide, split it
    if (lastPos == pos + 1) {
      display.getTextBounds(line, 0, 0, &xx, &yy, &w, &h);
      if (w > (x2 - x1)) {
        // character-by-character fallback
        String chunk;
        for (int c = 0; c < line.length(); c++) {
          String t = chunk + line[c];
          display.getTextBounds(t, 0, 0, &xx, &yy, &w, &h);
          if (w > (x2 - x1)) break;
          chunk = t;
        }
        // render the chunk
        display.setCursor(x1, cursorY);
        display.print(chunk);
        // leftover goes back into words
        words[pos] = line.substring(chunk.length());
      } else {
        // the single word fits by itself
        display.setCursor(x1, cursorY);
        display.print(line);
        pos++;
      }
    } else {
      // normal case: print line of words
      display.setCursor(x1, cursorY);
      display.print(line);
      pos = lastPos;
    }

    cursorY += lineH;
  }

  return cursorY;
}

void drawSensorAndClockNews() {
  drawSensorAndClockFinal();
}

void drawStaticContentNews() {
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(130, 100);
  display.setTextColor(GxEPD_BLACK);
  display.print("Headline");
  display.setCursor(555, 100);
  display.print("Other");
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(490, 460);
  display.print("Provided by");
  drawBmp("/gnews.bmp", 650, 440, display);
  display.fillRect(399, 100, 3, 330, GxEPD_BLACK);
}

void fullRefreshPageNews() {
  // fetch 4 headlines
  NewsItem news[4];
  int n = fetchTopHeadlines(news, 4);

  display.setRotation(0);
  display.setPartialWindow(0, 0, 800, 480);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawStaticContentNews();
    drawSensorAndClockNews();

    if (n > 0) {
      // Draw item 0 on left: full title+content
      int nextY = drawWrapped(news[0].title, 10, 390, 130, &FreeMonoBold12pt7b, 20, 4);
      drawWrapped(news[0].content, 10, 390, nextY + 15, &FreeMonoBold9pt7b, 16, 20);
    }
    // Draw items 1..3 on right: titles only
    for (int i = 1; i < n && i < 4; i++) {
      // pick Y positions evenly: 130, 200, 270
      int yPos = 130 + (i - 1) * 110;
      drawWrapped(news[i].title, 410, 790, yPos, &FreeMonoBold12pt7b, 20, 5);
    }
    // if no data
    if (n == 0) {
      display.setFont(&FreeMonoBold12pt7b);
      display.setCursor(40, 120);
      display.print("Failed to fetch news");
    }

  } while (display.nextPage());
}
