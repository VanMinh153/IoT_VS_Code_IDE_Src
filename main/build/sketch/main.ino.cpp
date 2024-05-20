#include <Arduino.h>
#line 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
#include "HX711-HEDSPI.h"
#include <LiquidCrystal_I2C.h>

#define DOUT 6
#define PD_SCK 7
#define TAREE 4
#define MODE 5
#define DOWN 8
#define UP 9
#define RECORD 3
#define DEBOUNT_TIME 200

#define SCALE 420
// #define SCALE 1030
#define MAX_LOAD 200
#define ERR_RANGE 0.2f
#define TIMEOUT 10000
#define FLICKER_DELAY 500
#define KG_MODE 0
#define LB_MODE 1
#define KG_TO_LB 2.204623f

HX711 sensor(DOUT, PD_SCK, GAIN_128, SCALE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float Scale = SCALE;
int32_t Zero = 0;
uint8_t Mode = KG_MODE;
int32_t Max_load = Scale*MAX_LOAD;
float w = 0;
float w_data[5];
int p = 0;
float w_avg = 0;
int32_t sensor_error = 0;
unsigned long timer = millis();

float record[3];
int q = 0;
volatile char k = 0;
volatile bool status = false;
unsigned long buttonTime = millis();

#line 43 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void recordInterrupt();
#line 52 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
float getWeight(uint8_t repeat);
#line 65 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void sort_(int32_t arr[], uint8_t n, int32_t avg);
#line 114 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void setup();
#line 136 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void loop();
#line 278 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void lcd_(float w);
#line 43 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
void recordInterrupt()
{
  if (millis() - buttonTime < DEBOUNT_TIME)
  {
    return;
  }
  buttonTime = millis();
  status = true;
}
inline float getWeight(uint8_t repeat) {
  int32_t w = 0;
  for (uint8_t i = 0; i < repeat; i++) {
    w += sensor.getData();
    if (abs(w) < Max_load) {
      w_avg += w;
      i--;
    }
  }

  return (long)(w/repeat - Zero) / Scale;
}

void sort_(int32_t arr[], uint8_t n, int32_t avg) {
  for (int i = 1; i < n; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= 0 && abs(arr[j] - avg) > abs(key - avg)) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

float getWeight_(int32_t *sensor_error = NULL, uint8_t K = 50, uint8_t N = 5) {
  int32_t w[N];
  int32_t w_avg = 0;
  int32_t w_worst = 0;
  int32_t temp = 0;
  uint8_t k = 0;

  for (uint8_t i = 0; i < N; i++) {
    temp = sensor.getData();
    if (abs(temp) < Max_load) {
      w[i] = temp;
      w_avg += temp;
    } else {
      i--;
    }
  }

  w_avg /= N;
  sort_(w, N, w_avg);
  w_worst = abs(w[N-1] - w_avg);
  while (k < K && w_worst > (int) (Scale*(ERR_RANGE/2)) ) {
    temp = sensor.getData();
    if (abs(temp - w_avg) < w_worst) {
      w_avg += (temp - w[N-1])/N;
      w[N-1] = temp;
      sort_(w, N, w_avg);
      w_worst = abs(w[N-1] - w_avg);
    }
    k++;
  }

  if (sensor_error != NULL) {
    *sensor_error = w_worst;
  }
  return (long)(w_avg - Zero) / Scale;
}

void setup()
{
  Serial.begin(57600);
  pinMode(TAREE, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(RECORD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RECORD), recordInterrupt, RISING);

  sensor.init();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Digital Scale");
  lcd.setCursor(0, 1);
  lcd.print(" SOICT - HUST");
  delay(1000);
}


void loop()
{
  lcd.setCursor(0, 0);
  lcd.print(" Digital Scale");

  delay(1000);
  Serial.println("\r\ngetWeight()*3");
  for (int i = 0; i < 20; i++)
  {
    sensor.getWeight();
    sensor.getWeight();
    sensor.getWeight();
  }
  delay(1000);
  Serial.println("\r\ngetWeight(3)");
  for (int i = 0; i < 20; i++)
  {
    getWeight(3);
  }
  delay(1000);
  Serial.println("\r\ngetWeight_()");
  for (int i = 0; i < 20; i++)
  {
    w = getWeight_(&sensor_error);
    Serial.print("\t Result: ");
    Serial.print(w);
    Serial.print("\t");
    Serial.println(sensor_error);
  }
  Serial.println("\r\nEND!!!");


  do
  {
    w = getWeight(3);
  } while (abs(w) > MAX_LOAD);

  w_avg += (w - w_data[p]) / 5;
  w_data[p] = w;
  p++;
  if (p == 5)
    p = 0;
  Serial.println(w);

  // display weighting results
  lcd_(w);

  // feature: Auto turn off the screen backlight
  // if the weighing result does not change by more than (ERR_RANGE)kg in 3s
  if (abs(w - w_avg) < ERR_RANGE)
  {
    if (millis() - timer > TIMEOUT)
    {
      lcd.noBacklight();
      delay(FLICKER_DELAY);
      lcd.backlight();
      delay(FLICKER_DELAY);
      lcd.noBacklight();
      delay(FLICKER_DELAY);
      lcd.backlight();
      delay(1200);
      lcd.noBacklight();
      lcd.clear();
      while (abs(w - sensor.getWeight()) < ERR_RANGE)
      {
        delay(500);
      }
      lcd.backlight();
      timer = millis();
    }
  }
  else
  {
    timer = millis();
  }

  // feature: Save and View the results of the last 3 weightings
  if (w_avg > 1 && abs(w - w_avg) < ERR_RANGE && abs(w_avg - record[q]) > ERR_RANGE)
  {
    int flag = 0;
    for (int i = 0; i < 5; i++)
    {
      if (abs(w_data[i] - w) > ERR_RANGE)
      {
        flag = 1;
        break;
      }
      if (flag == 0)
      {
        q++;
        if (q == 3)
          q = 0;
        record[q] = w;
      }
    }
  }

  // feature: Adjust the scale to 0 kg
  if (digitalRead(TAREE) == LOW)
  {
    Zero = sensor.setZero();
    lcd.setCursor(0, 1);
    lcd.print(" Taring...       ");
    delay(100);
    timer = millis();
  }

  // feature: Change weight unit from kilogram to pound
  if (digitalRead(MODE) == LOW)
  {
    Mode = (Mode == KG_MODE) ? LB_MODE : KG_MODE;
    lcd_(w);
    delay(100);
    timer = millis();
  }

  // feature: Adjust weighting result up
  // by decreasing weight coefficient (scale)
  while (digitalRead(UP) == LOW)
  {
    Scale -= 1;
    Max_load -= MAX_LOAD;
    sensor.setScale(Scale);
    lcd_(getWeight(3));
    timer = millis();
  }

  // feature: Adjust weighting result down
  // by increasing weight coefficient (scale)
  while (digitalRead(DOWN) == LOW)
  {
    Scale += 1;
    Max_load += MAX_LOAD;
    sensor.setScale(Scale);
    lcd_(getWeight(3));
    timer = millis();
  }

  delay(20);
}

// display weighting results
void lcd_(float w)
{
  lcd.setCursor(1, 1);
  if (Mode == LB_MODE)
  {
    lcd.print(w * KG_TO_LB);
    lcd.print(" lb    ");
  }
  else
  {
    lcd.print(w);
    lcd.print(" kg    ");
  }
}


