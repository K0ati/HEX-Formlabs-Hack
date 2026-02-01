#include "WeatherAPI.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= USER CONFIG =================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

const char* API_KEY = "YOUR_OPENWEATHER_API_KEY";
const char* CITY = "Toronto";
const char* COUNTRY = "CA";
// ==============================================

// Global variables
float snowProbability = 0.0;
float temperatureC = 5.0;
bool isRaining = false;

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
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
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
  temperatureC = doc["main"]["temp"];

  // Rain detection
  isRaining = doc.containsKey("rain");

  // Snow detection
  if (doc.containsKey("snow")) {
    snowProbability = 100.0;
  } else {
    snowProbability = 0.0;
  }

  Serial.println("Weather updated");
}
