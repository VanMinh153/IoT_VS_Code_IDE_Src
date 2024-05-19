#include <util/atomic.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


#define RECORD 2
#define DEBOUNT_TIME 500

int count = 0;
float record[3] = {1, 2, 3};
int q = 0;
volatile char k = 0;
unsigned long buttonTime = millis();

void recordInterrupt() {
  cli();
  // pinMode(RECORD, OUTPUT);
  // detachInterrupt(0);
  if (millis() - buttonTime < DEBOUNT_TIME) {
    return;
  }
  buttonTime = millis();
  count++;
  Serial.print("Button pressed! ");
  Serial.println(count);
  // lcd.clear();
  // sei();
  // pinMode(RECORD, INPUT_PULLUP);
  // attachInterrupt(0, recordInterrupt, RISING);
}

void setup() {
  Serial.begin(115200);
  pinMode(RECORD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RECORD), recordInterrupt, RISING);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Digital Scale");
}

void loop() {
  delay(100);
}
