
#define RECORD 2
#define DEBOUNT_TIME 500

int count = 0;
unsigned long buttonTime = millis();

void recordInterrupt() {
  cli();
  if (millis() - buttonTime < DEBOUNT_TIME) {
    return;
  }
  buttonTime = millis();
  count++;
  Serial.print("Button pressed! ");
  Serial.println(count);
}

void setup() {
  Serial.begin(115200);
  pinMode(RECORD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RECORD), recordInterrupt, RISING);
}

void loop() {
  delay(100);
}
