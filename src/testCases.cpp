#define TEST_CASES

#ifdef  TEST_CASES

#include "WeatherAPI.h"
#include <Arduino.h>


// MOSFET pins
const int M1 = 20;
const int M2 = 21;
const int M3 = 47;
const int M4 = 48;

enum Mode { MODE_A, MODE_B, MODE_C };
Mode currentMode = MODE_C;

// thresholds
const float SNOW_THRESHOLD = 40.0;     
const float FREEZE_TEMP = 0.0;       
const int stepThreshold = 2000;  // Threshold for rush hour  
const int HOURS_IN_DAY = 24;

// Test case variables
bool isRushHour;
int simulatedHour; 

int footSteps[HOURS_IN_DAY] = {
  226,   120,   90,    60,    110,   150,   300,   2478,  
  3200,  2800,  2400,  2749,  2100,  1800,  1500,  3924,  
  2200,  1625,  1100,  950,   800,   400,   300,   200  
};

void setMOSFETs(bool m1, bool m2, bool m3, bool m4) {
  digitalWrite(M1, m1);
  digitalWrite(M2, m2);
  digitalWrite(M3, m3);
  digitalWrite(M4, m4);
}

// logic to decide mode
void decideMode() {
  bool snowOrIce =
    (snowProbability > SNOW_THRESHOLD) ||
    (rainLast12h && temperatureC <= FREEZE_TEMP); // recent rain + subfreezing


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

  setMOSFETs(HIGH, HIGH, HIGH, HIGH);

  Serial.println("Demo Mode: Press 1-4 to simulate test cases");
}

void loop() {
  if (Serial.available()) {
    char key = Serial.read();

    while(Serial.available()) Serial.read();

    switch (key) {
      case '1': // 9 am, SNOW=false, RUSH=false → MODE C
        Serial.println("\nTest Case 1");
        snowProbability = 0.0;
        rainLast12h = false;
        temperatureC = 5.0;
        isRushHour = false;
        simulatedHour = 9;
        break;
      case '2': // 12 pm, SNOW=true, RUSH=true → MODE A
        Serial.println("\nTest Case 2");
        snowProbability = 100.0;
        rainLast12h = false;
        temperatureC = -2.0;
        isRushHour = true;
        simulatedHour = 12;
        break;
      case '3': // 3 am, SNOW=true, RUSH=false → MODE B
        Serial.println("\nTest Case 3");
        snowProbability = 100.0;
        rainLast12h = false;
        temperatureC = -5.0;
        isRushHour = false;
        simulatedHour = 3;
        break;
      case '4': // 7 pm, rainLast12h=true, temp subfreezing, RUSH=false → MODE B
        Serial.println("\nTest Case 4");
        snowProbability = 0.0;
        rainLast12h = true;
        temperatureC = -1.0;
        isRushHour = false;
        simulatedHour = 19;
        break;
    }

    // Print conditions for audience
    Serial.println("----- CONDITIONS -----");
    Serial.print("Time: ");
    Serial.printf("%02d:00\n", simulatedHour); 
    Serial.print("Snow Probability: "); Serial.print(snowProbability); Serial.println("%");
    Serial.print("Rain in last 12h: "); Serial.println(rainLast12h ? "Yes" : "No");
    Serial.print("Temperature (°C): "); Serial.println(temperatureC);
    Serial.print("High traffic: "); Serial.println(isRushHour ? "Yes" : "No");
    Serial.println("---------------------");

    decideMode();
    applyMode();
  }
}

#endif
