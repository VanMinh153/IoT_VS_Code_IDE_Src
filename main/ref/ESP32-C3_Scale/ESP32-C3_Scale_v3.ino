#include "HX711-HEDSPI.h"
#include <LiquidCrystal_I2C.h>

#define DOUT 6
#define PD_SCK 7
#define SDA 2
#define SCL 3
#define TAREE 4
#define MODE 5
#define DOWN 8
#define UP 9
#define RECORD 18

#define SCALE 420
// #define SCALE 1030
#define MAX_LOAD 200
#define ERR_RANGE 0.2f
#define TIMEOUT 6600
#define RECORD_TIME 1200
#define DEBOUNT_TIME 200
#define FLICKER_DELAY 540
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

int32_t sensor_error = 0;
unsigned long timer = millis();

float record[3];
uint8_t q = 0;
uint8_t k = 0;
volatile bool state = false;
volatile bool interrupt = false;
unsigned long buttonTime = millis();
uint8_t i = 0;
uint8_t flag = 0;
bool _sleep = false;

//-----------------------------------------------------------------------------------------------
void IRAM_ATTR recordInterrupt();
void lcd_(float w);
void sort_(int32_t arr[], uint8_t n, int32_t avg);
float getWeight(uint8_t K = 50, int32_t *sensor_error = NULL, uint8_t N = 5);
void sleep_();

//-----------------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(57600);
  // Serial.println("Starting");
  pinMode(TARE, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(RECORD, INPUT_PULLUP);
  attachInterrupt(RECORD, recordInterrupt, RISING);
  Wire.begin(2, 3);

  sensor.init();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Digital Scale");
  lcd.setCursor(0, 1);
  lcd.print(" SOICT - HUST");
  delay(1000);
  lcd.clear();
}
//-----------------------------------------------------------------------------------------------
void loop()
{
  lcd.setCursor(1, 0);
  lcd.print("Digital Scale");

  w = getWeight();

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
      delay_(2400);
      _sleep = true;
      sleep_();
      // Serial.println(_sleep);
      if (_sleep == false)
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
    Serial.println(String("Record: " + String(w)));
  }
  // feature: View the results of the last 3 weightings
  while (state == true)
  {
    // flag = 1;
    k++;
    if (k == 4)
    {
      k = 0;
      state = false;
      interrupt = false;
      if (_sleep == true)
      {
        sleep_();
        if (_sleep == true)
        {
          continue;
        }
      }
      break;
    }
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(1, 0);
    lcd.print("Record Weight: ");
    lcd_(record[(q - k + 4) % 3]);
    state = false;
    interrupt = false;
    if (delay_(2000) == false)
      continue;

    k = 0;
    if (_sleep == true)
      sleep_();
  }

  if (flag == 1)
  {
    lcd.clear();
    timer = millis();
    flag = 0;
  }

  // feature: Adjust the scale to 0 kg
  if (digitalRead(TARE) == LOW)
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

  w_prev = w;
  delay(20);
}

//-----------------------------------------------------------------------------------------------
void IRAM_ATTR recordInterrupt()
{
  if (millis() - buttonTime < DEBOUNT_TIME)
  {
    return;
  }
  buttonTime = millis();
  state = true;
  interrupt = true;
}

void lcd_(float w)
{
  if (w < 0 && w > -ERR_RANGE)
    return;
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

void sleep_()
{
  lcd.clear();
  lcd.noBacklight();
  uint8_t i = 0;
  uint8_t flag = 0;
  while (interrupt == false)
  {
    float temp = getWeight(10);
    
    if (flag == 1)
      if (abs(temp) > ERR_RANGE)
        break;
      else
        continue;

    if (abs(temp) < ERR_RANGE)
    {
      flag = 1;
      continue;
    }

    if (abs(w - temp) > ERR_RANGE)
    {
      i++;
      if (i == 3)
        break;
    }
    else
      i = 0;

    delay(50);
  }
  lcd.backlight();
  _sleep = (i == 3) ? false : true;
}
bool delay_(uint8_t timeout)
{
  int i = timeout / 20;
  while (interrupt == false && i > 0)
  {
    i--;
    delay(20);
  }
  return (i == 0) ? true : false;
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

float getWeight(uint8_t K, int32_t *sensor_error, uint8_t N)
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
