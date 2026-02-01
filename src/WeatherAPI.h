#ifndef WEATHER_API_H
#define WEATHER_API_H

#include <Arduino.h>

// Public weather variables (accessible from main file)
extern float snowProbability;   // %
extern float temperatureC;      // °C
extern bool isRaining;

// Initialize WiFi
void initWeatherWiFi();

// Update weather data from API
void updateWeather();

#endif
