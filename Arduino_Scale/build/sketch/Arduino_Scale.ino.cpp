#include <Arduino.h>
#line 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
#include "HX711-SOLDERED.h"
#include "LiquidCrystal_I2C.h"

#define DOUT 6
#define PD_SCLK 7

#define TAREE 4
#define UP 9
#define DOWN 8

#define SCALE 420
#define MAX_LOAD 150
#define AVG 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 sensor(DOUT, PD_SCLK);

#line 18 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
void setup();
#line 43 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
void loop();
#line 18 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
void setup()
{
  Serial.begin(57600);
  pinMode(TAREE, INPUT_PULLUP);
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

int  scale = SCALE;

void loop()
{
  float w = sensor.getAverageWeight(10, 20);

  lcd.setCursor(0, 1);
  lcd.print(w);
  lcd.print("kg   ");


  if (digitalRead(TAREE) == LOW)
  {
    sensor.setToZerro();
    lcd.setCursor(0, 1);
    lcd.print(" Taring         ");
  }

  while(digitalRead(UP) == LOW)
  {
    scale -= 5;
    sensor.setScale(scale);
    lcd.setCursor(0, 1);
    lcd.print(sensor.getWeight());
    lcd.print("kg   ");
    delay(200);
  }
  
  while(digitalRead(DOWN) == LOW)
  {
    scale += 5;
    sensor.setScale(scale);
    lcd.setCursor(0, 1);
    lcd.print(sensor.getWeight());
    lcd.print("kg   ");
    delay(200);
  }
  delay(50);
}
