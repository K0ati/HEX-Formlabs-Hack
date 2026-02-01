#ifdef MAIN_LOGIC

#include "WeatherAPI.h"
#include <TimeLib.h> 
#include <Arduino.h>

// pins
const int M1 = 20;
const int M2 = 21;
const int M3 = 47;
const int M4 = 48;

// modes
enum Mode { MODE_A, MODE_B, MODE_C };
Mode currentMode = MODE_C;

// thresholds
const float SNOW_THRESHOLD = 40.0;     
const float FREEZE_TEMP = 0.0;       
const int stepThreshold = 2000;  // Threshold for rush hour  
const int HOURS_IN_DAY = 24;

// sensor variables
bool isRushHour = false;

float voltageA0 = 0.0;

int footSteps[HOURS_IN_DAY] = {
  226,   120,   90,    60,    110,   150,   300,   2478,  
  3200,  2800,  2400,  2749,  2100,  1800,  1500,  3924,  
  2200,  1625,  1100,  950,   800,   400,   300,   200  
};

// timing
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 20000; // 20 sec

void setMOSFETs(bool m1, bool m2, bool m3, bool m4) {
  digitalWrite(M1, m1);
  digitalWrite(M2, m2);
  digitalWrite(M3, m3);
  digitalWrite(M4, m4);
}

// Returns voltage on A0
float readVoltageA0() {
  int raw = analogRead(A0);      
  Serial.print("Raw: ");  
  Serial.println(raw);
  float voltage = (raw / 4095.0) * 3.3; 
  return voltage;
}

void printVoltage() {
  float v = readVoltageA0();
  Serial.print("A0 Voltage: ");
  Serial.print(v, 3); 
  Serial.println(" V");
}

// Check if rush hour based on current hour
bool checkRushHour(int currentHour) {
  int stepsNow = footSteps[currentHour];
  Serial.print("Hour: "); Serial.print(currentHour);
  Serial.print(" | Steps: "); Serial.println(stepsNow);

  return stepsNow >= stepThreshold;
}

// logic to decide mode
void decideMode() {
  bool snowOrIce =
    (snowProbability > SNOW_THRESHOLD) ||
    (rainLast12h && temperatureC <= FREEZE_TEMP); // recent rain + subfreezing

  bool isRushHour = checkRushHour();

  if (snowOrIce && isRushHour) {
    currentMode = MODE_A;
  }
  else if (snowOrIce && !isRushHour) {
    currentMode = MODE_B;
  }
  else {
    currentMode = MODE_C;
  }
}

// apply mode
void applyMode() {
  switch (currentMode) {
    case MODE_A:
      Serial.println("MODE A: Energy → Heating");
      setMOSFETs(HIGH, HIGH, LOW, LOW);
      break;

    case MODE_B:
      Serial.println("MODE B: Battery → Heating");
      setMOSFETs(HIGH, LOW, HIGH, LOW);
      break;

    case MODE_C:
      Serial.println("MODE C: Energy → Battery");
      setMOSFETs(LOW, HIGH, LOW, HIGH);
      break;
  }
}

void printModeDescription() {
  switch (currentMode) {
    case MODE_A:
      Serial.println("MODE A");
      Serial.println("Energy → Heating");
      break;

    case MODE_B:
      Serial.println("MODE B");
      Serial.println("Battery → Heating");
      break;

    case MODE_C:
      Serial.println("MODE C");
      Serial.println("Energy → Battery");
      break;
  }
}


void setup() {
  Serial.begin(115200);

  setTime(12, 38, 0, 1, 2, 2026);  // 12:35:00, Feb 1, 2026

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);

  pinMode(A0, INPUT);

  setMOSFETs(HIGH, HIGH, HIGH, HIGH);
  Serial.println("Heating Tile System Started");

  initWeatherWiFi();

  lastUpdate = millis() - UPDATE_INTERVAL;

}

void loop() {
  
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();

    updateWeather();

    // Get current time once
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Time: ");
        Serial.printf("%02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        
        // Rush hour based on current hour
        isRushHour = checkRushHour(timeinfo.tm_hour);
    } else {
        Serial.println("Failed to obtain time");
        isRushHour = false;
    }

    decideMode(isRushHour);
    applyMode();

    Serial.println("----- STATUS -----");

    Serial.print("City: "); Serial.println(CITY);

    Serial.print("Country: "); Serial.println(COUNTRY);

    // TIME
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.print("Time: ");
        Serial.printf("%02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    } else {
        Serial.println("Failed to obtain time");
    }

    // WEATHER
    Serial.println("\n[ WEATHER ]");
    Serial.print("Snow: ");
    Serial.print((int)snowProbability);
    Serial.println("%");

    Serial.print("Temp: ");
    Serial.print(temperatureC);
    Serial.println("°C");

    Serial.print("Rain in last 12h: "); Serial.println(rainLast12h ? "Yes" : "No");


    Serial.print("High traffic: "); Serial.println(isRushHour ? "YES" : "NO");

    // MODE
    Serial.println("\n[ SYSTEM MODE ]");
    printModeDescription();
    Serial.println("=================================\n");

    // test A0 voltage
    printVoltage();
  }
}

#endif
