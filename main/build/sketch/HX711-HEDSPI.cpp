#line 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\HX711-HEDSPI.cpp"
/**
 * @brief       Arduino library for HX711
 *
 * @authors     @ soldered.com @ Robert Peric
 * @authors     Nguyen Van Minh - SOICT-HUST
 */

#include "HX711-HEDSPI.h"

HX711::HX711(uint8_t dout, uint8_t pd_sck, uint8_t gain, float scale) {
  DOUT = dout;
  PD_SCK = pd_sck;
  Gain = gain;
  Scale = scale;
}

void HX711::init() {
  pinMode(PD_SCK, OUTPUT);
  pinMode(DOUT, INPUT);
}


void HX711::setGain(uint8_t gain) {
  if (gain >= 25 && gain <= 27) {
    Gain = gain;
  }
}

void HX711::setScale(float scale) { Scale = scale; }

int32_t HX711::setZero() {
  Zero = getData();
  return Zero;
}


int32_t HX711::getData() {
  int32_t data = 0;
  data = readDataNative();
  if (data != 8820) {
    Serial.print(data);
    Serial.print(" ");
  }
  
  return data;
}


float HX711::getWeight() {
  return (long)(getData() - Zero) / Scale;
}

/**
 * @brief       readDataNative reads data from sensor using pins
 *
 * @return      data from device data register
 */
int32_t HX711::readDataNative() {
  // begin reading
  digitalWrite(PD_SCK, HIGH);
  delayMicroseconds(70);
  digitalWrite(PD_SCK, LOW);
  delayMicroseconds(100);

  // wait for DOUT pin to go LOW and signal data available
  unsigned long timer = millis();
  while (digitalRead(DOUT) == HIGH) {
    if (millis() - timer > 550) {
      return 0;
    }
  }

  // Set gain for next reading of data
  for (uint8_t i = 0; i < Gain; i++) {
    digitalWrite(PD_SCK, HIGH);
    delayMicroseconds(4);
    digitalWrite(PD_SCK, LOW);
    delayMicroseconds(4);
  }

  // Wait aggain for DOUT pin to signal available data
  timer = millis();
  while (digitalRead(DOUT) == HIGH) {
    if (millis() - timer > 550) {
      return 0;
    }
  }

  int32_t result = 0;
  // read data
  for (uint8_t i = 0; i < 24; ++i) {
    digitalWrite(PD_SCK, HIGH);
    delayMicroseconds(4);

    result = ((result << 1) | digitalRead(DOUT));

    digitalWrite(PD_SCK, LOW);
    delayMicroseconds(4);
  }

  // One last pulse to set DOUT to high
  digitalWrite(PD_SCK, HIGH);
  delayMicroseconds(4);
  digitalWrite(PD_SCK, LOW);
  delayMicroseconds(4);

  // turn chip off
  digitalWrite(PD_SCK, HIGH);
  delayMicroseconds(70);
  
  if (bitRead(result, 23) == 1) {
    result |= 0xFF000000;
  }
  return result;
}