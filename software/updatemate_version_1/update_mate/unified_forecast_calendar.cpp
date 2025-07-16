#include "unified_forecast_calendar.h"

// Google Calendar Event Fetching
void fetchAndProcessCalendarEvents(const String& scriptUrl, EventDataCallback eventCb, StatusCallback statusCb, NoEventsCallback noEventsCb) {
  if (!eventCb || !statusCb || !noEventsCb) {
#ifdef CORE_DEBUG_LEVEL
    if (Serial) Serial.println("Err: CB bad");
#endif
    return;
  }

  statusCb("CalF");  

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  String urlWithCacheBuster = scriptUrl + "?nocache=" + String(random(0, 10000));
  statusCb(("Req:" + urlWithCacheBuster).c_str());

  http.setTimeout(15000);  // 15 sec timeout

  if (http.begin(urlWithCacheBuster)) {
    int httpCode = http.GET();
    statusCb(("HTTP:" + String(httpCode)).c_str());

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();

        const size_t JSON_DOC_SIZE = 1000; 
        DynamicJsonDocument doc(JSON_DOC_SIZE);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          String errorMsg = "dJsonErr: ";
          errorMsg += error.c_str();
          statusCb(errorMsg.c_str());
        } else {
          if (doc.containsKey("error")) {
            String errorMsg = "AppErr: ";
            errorMsg += doc["error"].as<const char*>();
            statusCb(errorMsg.c_str());
          } else if (doc.containsKey("events") && doc["events"].is<JsonArray>()) {
            JsonArray events = doc["events"].as<JsonArray>();
            statusCb(("Evs:" + String(events.size())).c_str());

            if (events.size() == 0) {
              noEventsCb();
            } else {
              for (JsonObject event : events) {
                const char* title = event["title"] | "No Title";
                const char* startTime = event["startTime"] | "??:??";
                const char* endTime = event["endTime"] | "??:??";
                const char* location = event["location"];  // may be null
                eventCb(title, startTime, endTime, location);
              }
            }
          } else {
            statusCb("Bad JSON");
            statusCb("Raw:");
            statusCb(payload.c_str());
          }
        }
      } else {
        String errorMsg = "BadHTTP:" + String(httpCode);
        statusCb(errorMsg.c_str());
        String payload = http.getString();
        if (payload.length() > 0) {
          statusCb("Resp:");
          statusCb(payload.c_str());
        }
      }
    } else {
      String errorMsg = "GETErr:" + String(http.errorToString(httpCode).c_str());
      statusCb(errorMsg.c_str());
    }
    http.end();
  } else {
    String errorMsg = "ConnErr:" + scriptUrl;
    statusCb(errorMsg.c_str());
  }
}

// Weather Forecast Implementation
WeatherForecast::WeatherForecast(const char* apiKey) {
  _apiKey = String(apiKey);
  _url = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/Bucharest?unitGroup=metric&include=days&key="+ _apiKey +"&contentType=json";
}

bool WeatherForecast::getForecast(ForecastToday& today, ForecastDay forecast[], size_t forecastCount) {
  HTTPClient http;
  http.begin(_url);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    const size_t capacity = 35 * JSON_OBJECT_SIZE(20);
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print("JErr:");
      Serial.println(error.c_str());
      http.end();
      return false;
    }

    JsonArray days = doc["days"];
    size_t daysAvailable = days.size();
    if (daysAvailable < 1) {
      Serial.println("No data");
      http.end();
      return false;
    }

    // Process today's forecast
    {
      JsonObject day = days[0];
      today.low = day["tempmin"].as<float>();
      today.high = day["tempmax"].as<float>();
      today.icon = day["icon"].as<String>();

      float rawTemp = day["temp"].as<float>();
      today.temp = String((int)rawTemp);

      today.sunrise = "Sunrise " + day["sunrise"].as<String>();
      today.sunset = "Sunset " + day["sunset"].as<String>();
    }

    // Process upcoming forecast days 
    size_t daysToReturn = min(forecastCount, (daysAvailable > 1 ? daysAvailable - 1 : 0));
    const char* monthAbbrs[13] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    for (size_t i = 0; i < daysToReturn; i++) {
      JsonObject day = days[i + 1];
      String datetime = day["datetime"].as<String>();
      String dayStr = datetime.substring(8, 10);
      String monthStr = datetime.substring(5, 7);
      int monthIndex = monthStr.toInt();
      String monthAbbr = (monthIndex >= 1 && monthIndex <= 12) ? monthAbbrs[monthIndex] : "";

      forecast[i].date = dayStr + " " + monthAbbr;
      forecast[i].low = (int)day["tempmin"].as<float>();
      forecast[i].high = (int)day["tempmax"].as<float>();
      forecast[i].icon = day["icon"].as<String>();
    }

    http.end();
    return true;
  } else {
    Serial.print("GETErr:");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}
