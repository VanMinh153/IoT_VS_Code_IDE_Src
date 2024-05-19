#include "HX711-SOLDERED.h"
#include <LiquidCrystal_I2C.h>

#define DOUT 6
#define PD_SCLK 7

#define TAREE 4
#define MODE 5
#define DOWN 8
#define UP 9

#define SCALE 420
// #define SCALE 1030
#define MAX_LOAD 150
#define ERR_RANGE 0.3
#define TIMEOUT 5000
#define FLICKER_DELAY 500
#define KG_MODE 0
#define LB_MODE 1
#define KG_TO_LB 2.204623

HX711 sensor(DOUT, PD_SCLK);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(57600);
  pinMode(TAREE, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);

  sensor.begin();
  sensor.setToZerro();
  sensor.setGain(GAIN_128);
  sensor.setScale(SCALE);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Digital Scale");
  lcd.setCursor(1, 1);
  lcd.print("SOICT - HUST");
  delay(1000);
  lcd.setCursor(1, 1);
  lcd.print("            ");
}

int scale = SCALE;
float w = 0;
float w_data[5];
int p = 0;
float w_avg = 0;
unsigned long time = millis();
uint8_t mode = KG_MODE;

void loop() {
  w = sensor.getAverageWeight(8, 50);

  w_data[p] = w;
  p++;
  if (p == 5)
    p = 0;

  if (millis() - time > TIMEOUT) {
    w_avg = 0;
    for (int i = 0; i < 5; i++)
      w_avg += w_data[i];

    w_avg /= 5;
    if (abs(w_avg - w) < ERR_RANGE) {
      lcd.noBacklight();
      delay(FLICKER_DELAY);
      lcd.backlight();
      delay(FLICKER_DELAY);
      lcd.noBacklight();
      delay(FLICKER_DELAY);
      lcd.backlight();
      delay(1500);
      lcd.noBacklight();
      lcd.clear();
      while (abs(w - sensor.getAverageWeight(4, 50)) < 3 * ERR_RANGE)
        delay(500);
      lcd.setCursor(1, 0);
      lcd.print("Digital Scale");
      lcd.backlight();
    }
    time = millis();
  }

  lcd_(w);

  if (digitalRead(TAREE) == LOW) {
    sensor.setToZerro();
    lcd.setCursor(0, 1);
    lcd.print(" Taring...       ");
    delay(200);
  }

  if (digitalRead(MODE) == LOW) {
    if (mode == KG_MODE) {
      mode = LB_MODE;
    } else {
      mode = KG_MODE;
    }
    Serial.println(mode);
    lcd_(w);
    delay(200);
  }

  while (digitalRead(UP) == LOW) {
    scale -= 2;
    sensor.setScale(scale);
    lcd_(sensor.getWeight());
    delay(50);
  }

  while (digitalRead(DOWN) == LOW) {
    scale += 2;
    sensor.setScale(scale);
    lcd_(sensor.getWeight());
    delay(50);
  }
  delay(50);
}

void lcd_(float w) {
  lcd.setCursor(1, 1);
  if (mode == LB_MODE) {
    lcd.print(w*KG_TO_LB);
    lcd.print(" lb    ");
  } else {
    lcd.print(w);
    lcd.print(" kg    ");
  }
}