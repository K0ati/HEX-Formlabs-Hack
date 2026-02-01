#ifndef WEATHER_API_H
#define WEATHER_API_H

// Public weather variables (accessible from main file)
extern float snowProbability;   // %
extern float temperatureC;      // °C
extern bool rainLast12h;
extern const char* CITY;
extern const char* COUNTRY;

// Initialize WiFi
void initWeatherWiFi();

// Update weather data from API
void updateWeather();

#endif
