#include <Arduino.h>

// pins
const int HEATER_PIN = 7;   // MOSFET gate

void setup() {
  Serial.begin(115200);

  Serial.println("System starting...");
  Serial.println("Send '0' to turn heater OFF");
  Serial.println("Send '1' to turn heater ON");
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);  // start off
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