#include <Arduino.h>

// 4 pins (MOSFET gate)
const int HEATER_PIN1 = 7;
const int HEATER_PIN2 = 8;
const int HEATER_PIN3 = 9;
const int HEATER_PIN4 = 10;


void setup() {
  Serial.begin(115200);

  Serial.println("System starting...");
  Serial.println("Send '0' to turn heater OFF");
  Serial.println("Send '1' to turn heater ON");
  pinMode(HEATER_PIN1, OUTPUT);
  pinMode(HEATER_PIN2, OUTPUT);
  pinMode(HEATER_PIN3, OUTPUT);
  pinMode(HEATER_PIN4, OUTPUT);
  digitalWrite(HEATER_PIN1, LOW);  // start off
  digitalWrite(HEATER_PIN2, LOW);
  digitalWrite(HEATER_PIN3, LOW);
  digitalWrite(HEATER_PIN4, LOW);

}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1') {
      Serial.println("Heater ON");
      digitalWrite(HEATER_PIN, HIGH);
    }
    else if (cmd == '0') {
      Serial.println("Heater OFF");
      digitalWrite(HEATER_PIN, LOW);
    }
  }
}