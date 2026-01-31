// MOSFET pins
const int M1 = 4;
const int M2 = 5;
const int M3 = 6;
const int M4 = 7;

// timing 
unsigned long timer = 1000;
unsigned long pause = 1000;

// mode selection
enum Mode { MODE_A, MODE_B, MODE_C };
Mode currentMode = MODE_A;

void setMOSFETs(bool m1, bool m2, bool m3, bool m4) {
  digitalWrite(M1, m1);
  digitalWrite(M2, m2);
  digitalWrite(M3, m3);
  digitalWrite(M4, m4);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);

  setMOSFETs(HIGH, HIGH, HIGH, HIGH);

  Serial.println("System ready");
  Serial.println("A = Energy → Heating");
  Serial.println("B = Battery → Heating");
  Serial.println("C = Energy → Battery");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'A') currentMode = MODE_A;
    if (cmd == 'B') currentMode = MODE_B;
    if (cmd == 'C') currentMode = MODE_C;
  }

 // logic
  switch (currentMode) {

    case MODE_A:
      Serial.println("MODE A: ON");
      setMOSFETs(HIGH, HIGH, LOW, LOW);
      delay(timer);

      Serial.println("MODE A: OFF");
      setMOSFETs(HIGH, HIGH, HIGH, HIGH);
      delay(pause);i 
      break;

    case MODE_B:
      Serial.println("MODE B: ON");
      setMOSFETs(HIGH, LOW, HIGH, LOW);
      delay(timer);

      Serial.println("MODE B: OFF");
      setMOSFETs(HIGH, HIGH, HIGH, HIGH);
      delay(pause);
      break;

    case MODE_C:
      Serial.println("MODE C");
      setMOSFETs(LOW, HIGH, LOW, HIGH);
      delay(500);
      break;
  }
}
