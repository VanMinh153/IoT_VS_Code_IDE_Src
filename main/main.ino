#include "HX711-HEDSPI.h"
#include <LiquidCrystal_I2C.h>

#define DOUT 6
#define PD_SCK 7
#define TAREE 4
#define MODE 5
#define DOWN 8
#define UP 9
#define RECORD 3

#define SCALE 420
// #define SCALE 1030
#define MAX_LOAD 200
#define ERR_RANGE 0.2f
#define TIMEOUT 3000
#define RECORD_TIME 1500
#define DEBOUNT_TIME 200
#define FLICKER_DELAY 500
#define KG_MODE 0
#define LB_MODE 1
#define KG_TO_LB 2.204623f

HX711 sensor(DOUT, PD_SCK, GAIN_128, SCALE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float Scale = SCALE;
int32_t Zero = 0;
uint8_t Mode = KG_MODE;
float w = 0;
float w_prev = 0;

float w_data[3];
int p = 0;
float w_avg = 0;

int32_t sensor_error = 0;
unsigned long timer = millis();

float record[3];
int q = 0;
volatile char k = 0;
volatile bool state = false;
volatile bool interrupt = false;
unsigned long buttonTime = millis();

void recordInterrupt()
{
  if (millis() - buttonTime < DEBOUNT_TIME)
  {
    return;
  }
  buttonTime = millis();
  state = true;
  interrupt = true;
}

void sort_(int32_t arr[], uint8_t n, int32_t avg)
{
  for (int i = 1; i < n; i++)
  {
    int key = arr[i];
    int j = i - 1;
    while (j >= 0 && abs(arr[j] - avg) > abs(key - avg))
    {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

float getWeight(uint8_t K = 50, int32_t *sensor_error = NULL, uint8_t N = 5)
{
  int32_t w[N];
  int32_t w_avg = 0;
  int32_t w_worst = 0;
  int32_t temp = 0;
  uint8_t k = 0;

  for (uint8_t i = 0; i < N; i++)
  {
    temp = sensor.getData();
    if (abs(temp) < Scale * MAX_LOAD)
    {
      w[i] = temp;
      w_avg += temp;
    }
    else
    {
      i--;
    }
  }

  w_avg /= N;
  sort_(w, N, w_avg);
  w_worst = abs(w[N - 1] - w_avg);
  while (k < K && w_worst > (int)(Scale * (ERR_RANGE / 2)))
  {
    temp = sensor.getData();
    if (abs(temp - w_avg) < w_worst)
    {
      w_avg += (temp - w[N - 1]) / N;
      w[N - 1] = temp;
      sort_(w, N, w_avg);
      w_worst = abs(w[N - 1] - w_avg);
    }
    k++;
  }

  if (sensor_error != NULL)
  {
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
  lcd.setCursor(1, 0);
  lcd.print("Digital Scale");

  w = getWeight(50, &sensor_error);

  Serial.println(w);

  // display weighting results
  lcd_(w);

  // feature: Auto turn off the screen backlight
  // if the weighing result does not change by more than (ERR_RANGE)kg in 3s
  if (abs(w - w_prev) < ERR_RANGE)
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
      while (abs(w - sensor.getWeight()) < ERR_RANGE && interrupt == false)
      {
        delay(50);
      }
      if (interrupt == false)
      {
        lcd.backlight();
        timer = millis();
      }
    }
  }
  else
  {
    timer = millis();
  }

  // feature: Save the results of the last 3 weightings
  if (abs(w - record[q]) > ERR_RANGE && millis() - timer > RECORD_TIME)
  {
    q++;
    if (q == 3)
      q = 0;
    record[q] = w;
  }
  // feature: View the results of the last 3 weightings
  if (state == true)
  {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Record Weight: ");
    lcd.setCursor(1, 1);
    lcd.print(record[0], 2);
    state = false;
    interrupt = false;
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
  while (digitalRead(UP) == LOW)
  {
    Scale--;
    sensor.setScale(Scale);
    lcd_(getWeight(10));
    timer = millis();
  }

  // feature: Adjust weighting result down
  while (digitalRead(DOWN) == LOW)
  {
    Scale++;
    sensor.setScale(Scale);
    lcd_(getWeight(10));
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