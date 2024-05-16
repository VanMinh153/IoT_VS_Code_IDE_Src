# 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
# 2 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino" 2
# 3 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino" 2
# 15 "C:\\Users\\Admin\\Downloads\\Wokwi\\Arduino_Scale\\Arduino_Scale.ino"
LiquidCrystal_I2C lcd(0x27, 16, 2);
HX711 sensor(6, 7);

void setup()
{
  Serial.begin(57600);
  pinMode(4, 0x2);
  pinMode(9, 0x2);
  pinMode(8, 0x2);

  sensor.begin();
  sensor.setToZerro();
  sensor.setGain(25);
  sensor.setScale(420);

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

int scale = 420;

void loop()
{
  float w = sensor.getAverageWeight(10, 20);

  lcd.setCursor(0, 1);
  lcd.print(w);
  lcd.print("kg   ");


  if (digitalRead(4) == 0x0)
  {
    sensor.setToZerro();
    lcd.setCursor(0, 1);
    lcd.print(" Taring         ");
  }

  while(digitalRead(9) == 0x0)
  {
    scale -= 5;
    sensor.setScale(scale);
    lcd.setCursor(0, 1);
    lcd.print(sensor.getWeight());
    lcd.print("kg   ");
    delay(200);
  }

  while(digitalRead(8) == 0x0)
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
