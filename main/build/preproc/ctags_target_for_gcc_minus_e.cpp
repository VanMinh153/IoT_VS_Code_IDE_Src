# 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
# 2 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 2
# 3 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 2
# 14 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
// #define SCALE 1030
# 23 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
HX711 sensor(6, 7, 25, 420);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float Scale = 420;
int32_t Zero = 0;
uint8_t Mode = 0;
int32_t Max_load = Scale*200;
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

void recordInterrupt()
{
  if (millis() - buttonTime < 200)
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
    if (((w)>0?(w):-(w)) < Max_load) {
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
    while (j >= 0 && ((arr[j] - avg)>0?(arr[j] - avg):-(arr[j] - avg)) > ((key - avg)>0?(key - avg):-(key - avg))) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

float getWeight_(int32_t *sensor_error = 
# 77 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 3 4
                                        __null
# 77 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
                                            , uint8_t K = 50, uint8_t N = 5) {
  int32_t w[N];
  int32_t w_avg = 0;
  int32_t w_worst = 0;
  int32_t temp = 0;
  uint8_t k = 0;

  for (uint8_t i = 0; i < N; i++) {
    temp = sensor.getData();
    if (((temp)>0?(temp):-(temp)) < Max_load) {
      w[i] = temp;
      w_avg += temp;
    } else {
      i--;
    }
  }

  w_avg /= N;
  sort_(w, N, w_avg);
  w_worst = ((w[N-1] - w_avg)>0?(w[N-1] - w_avg):-(w[N-1] - w_avg));
  while (k < K && w_worst > (int) (Scale*(0.2f/2)) ) {
    temp = sensor.getData();
    if (((temp - w_avg)>0?(temp - w_avg):-(temp - w_avg)) < w_worst) {
      w_avg += (temp - w[N-1])/N;
      w[N-1] = temp;
      sort_(w, N, w_avg);
      w_worst = ((w[N-1] - w_avg)>0?(w[N-1] - w_avg):-(w[N-1] - w_avg));
    }
    k++;
  }

  if (sensor_error != 
# 108 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 3 4
                     __null
# 108 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
                         ) {
    *sensor_error = w_worst;
  }
  return (long)(w_avg - Zero) / Scale;
}

void setup()
{
  Serial.begin(57600);
  pinMode(4, 0x2);
  pinMode(5, 0x2);
  pinMode(9, 0x2);
  pinMode(8, 0x2);
  pinMode(3, 0x2);
  attachInterrupt(((3) == 2 ? 0 : ((3) == 3 ? 1 : -1)), recordInterrupt, 3);

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
  } while (((w)>0?(w):-(w)) > 200);

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
  if (((w - w_avg)>0?(w - w_avg):-(w - w_avg)) < 0.2f)
  {
    if (millis() - timer > 10000)
    {
      lcd.noBacklight();
      delay(500);
      lcd.backlight();
      delay(500);
      lcd.noBacklight();
      delay(500);
      lcd.backlight();
      delay(1200);
      lcd.noBacklight();
      lcd.clear();
      while (((w - sensor.getWeight())>0?(w - sensor.getWeight()):-(w - sensor.getWeight())) < 0.2f)
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
  if (w_avg > 1 && ((w - w_avg)>0?(w - w_avg):-(w - w_avg)) < 0.2f && ((w_avg - record[q])>0?(w_avg - record[q]):-(w_avg - record[q])) > 0.2f)
  {
    int flag = 0;
    for (int i = 0; i < 5; i++)
    {
      if (((w_data[i] - w)>0?(w_data[i] - w):-(w_data[i] - w)) > 0.2f)
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
  if (digitalRead(4) == 0x0)
  {
    Zero = sensor.setZero();
    lcd.setCursor(0, 1);
    lcd.print(" Taring...       ");
    delay(100);
    timer = millis();
  }

  // feature: Change weight unit from kilogram to pound
  if (digitalRead(5) == 0x0)
  {
    Mode = (Mode == 0) ? 1 : 0;
    lcd_(w);
    delay(100);
    timer = millis();
  }

  // feature: Adjust weighting result up
  // by decreasing weight coefficient (scale)
  while (digitalRead(9) == 0x0)
  {
    Scale -= 1;
    Max_load -= 200;
    sensor.setScale(Scale);
    lcd_(getWeight(3));
    timer = millis();
  }

  // feature: Adjust weighting result down
  // by increasing weight coefficient (scale)
  while (digitalRead(8) == 0x0)
  {
    Scale += 1;
    Max_load += 200;
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
  if (Mode == 1)
  {
    lcd.print(w * 2.204623f);
    lcd.print(" lb    ");
  }
  else
  {
    lcd.print(w);
    lcd.print(" kg    ");
  }
}
