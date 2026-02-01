#include "WeatherAPI.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= USER CONFIG =================
const char* WIFI_SSID = "Formlabs Guest";
const char* WIFI_PASS = "formguest";

const char* API_KEY = "5364ceecdb297c2e7416b12ea854798e";
const char* CITY = "Minneapolis";
const char* COUNTRY = "US";


// ==============================================

// Global variables
float snowProbability = 0.0;
float temperatureC = 5.0;
bool rainLast12h = false;

// Build API URL
String weatherURL() {
  return String("http://api.openweathermap.org/data/2.5/weather?q=") +
         CITY + "," + COUNTRY +
         "&units=metric&appid=" + API_KEY;
}

// WiFi initialization
void initWeatherWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    // -------------------- NTP TIME SYNC --------------------
    // Set your UTC offset in seconds (e.g., Boston EST: -5*3600)
    const long utcOffsetSeconds = -5 * 3600; 
    configTime(utcOffsetSeconds, 0, "pool.ntp.org", "time.nist.gov"); // NTP servers
    Serial.println("Synchronizing time...");

    struct tm timeinfo;
    int attempts = 0;
    while (!getLocalTime(&timeinfo) && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (getLocalTime(&timeinfo)) {
      Serial.println("\nTime synchronized:");
      Serial.printf("Current time: %02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    } else {
      Serial.println("\nFailed to synchronize time.");
    }
    // --------------------------------------------------------
  } else {
    Serial.println("\nWiFi connection failed");
  }
}

// Weather update
void updateWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Weather update skipped (no WiFi)");
    return;
  }

  HTTPClient http;
  http.begin(weatherURL());

  int httpCode = http.GET();
  if (httpCode != 200) {
    Serial.print("Weather API error: ");
    Serial.println(httpCode);
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.println("Weather JSON parse failed");
    return;
  }

  // Temperature
  temperatureC = doc["main"]["temp"].as<float>();

 
  // Check snow
  for (JsonObject item : doc["weather"].as<JsonArray>()) {
      String w = item["main"].as<String>();

      if (w.equalsIgnoreCase("Snow")) {
          snowProbability = 100.0;   // still 100% if snow present
      } 
  }

  // Check if it rained in the last 12 hours
  if (doc.containsKey("rain")) {
      if (doc["rain"].containsKey("12h")) {
          rainLast12h = doc["rain"]["12h"] > 0.0;
      } else {
          rainLast12h = false;
      }
  } else {
      rainLast12h = false;
  }


  Serial.println("Weather updated");
}
