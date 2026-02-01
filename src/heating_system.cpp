#include <Arduino.h>
#include "WeatherAPI.h"
#include <TimeLib.h> 

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
float snowProbability = 0.0;
float temperatureC = 5.0;
bool isRaining = false;
bool isRushHour = false;

float voltageA0 = 0.0;

int footSteps[HOURS_IN_DAY] = {
  226,   120,   90,    60,    110,   150,   300,   2478,  
  3200,  2800,  2400,  2749,  2100,  1800,  1500,  3924,  
  2200,  1625,  1100,  950,   800,   400,   300,   200  
};

// timing
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 10UL * 60UL * 1000UL; // 10 minutes

void setMOSFETs(bool m1, bool m2, bool m3, bool m4) {
  digitalWrite(M1, m1);
  digitalWrite(M2, m2);
  digitalWrite(M3, m3);
  digitalWrite(M4, m4);
}

// Returns voltage on A0
float readVoltageA0() {
  int raw = analogRead(A0);             
  float voltage = (raw / 4095.0) * 3.3; 
  return voltage;
}

void printVoltage() {
  float v = readVoltageA0();
  Serial.print("A0 Voltage: ");
  Serial.print(v, 3); 
  Serial.println(" V");
}


bool checkRushHour() {
  int currentHour = hour();  // (0-23)
  
  int stepsNow = footSteps[currentHour];
  Serial.print("Hour: "); Serial.print(currentHour);
  Serial.print(" | Steps: "); Serial.println(stepsNow);

  if (stepsNow >= stepThreshold) {
    return true;  // rush hour/lots of people walking
  } else {
    return false; // not rush hour
  }
}

// logic to decide mode
void decideMode() {
  bool snowOrIce =
    (snowProbability > SNOW_THRESHOLD) ||
    (isRaining && temperatureC <= FREEZE_TEMP);

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

void setup() {
  Serial.begin(115200);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);

  pinMode(A0, INPUT);

  setMOSFETs(HIGH, HIGH, HIGH, HIGH);
  Serial.println("Heating Tile System Started");

  initWeatherWiFi();

}

void loop() {
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    lastUpdate = millis();

    updateWeather();
    isRushHour = checkRushHour();

    decideMode();
    applyMode();

    Serial.println("----- STATUS -----");
    Serial.print("Snow %: "); Serial.println(snowProbability);
    Serial.print("Temp C: "); Serial.println(temperatureC);
    Serial.println("------------------");

    // test A0 voltage
    printVoltage();
  }
}
