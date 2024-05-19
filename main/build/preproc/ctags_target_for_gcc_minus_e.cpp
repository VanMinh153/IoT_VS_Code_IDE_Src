# 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
# 2 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 2
# 3 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 2
LiquidCrystal_I2C lcd(0x27, 16, 2);





int count = 0;
float record[3] = {1, 2, 3};
int q = 0;
volatile char k = 0;
unsigned long buttonTime = millis();

void recordInterrupt() {
  
# 16 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino" 3
 __asm__ __volatile__ ("cli" ::: "memory")
# 16 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\main.ino"
      ;
  // pinMode(RECORD, OUTPUT);
  // detachInterrupt(0);
  if (millis() - buttonTime < 500) {
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
  pinMode(2, 0x2);
  attachInterrupt(((2) == 2 ? 0 : ((2) == 3 ? 1 : -1)), recordInterrupt, 3);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Digital Scale");
}

void loop() {
  delay(100);
}
