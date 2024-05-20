/**
 * @brief       Arduino library for HX711
 *
 * @authors     @ soldered.com @ Robert Peric
 * @authors     Nguyen Van Minh - SOICT-HUST
 */

#include "HX711-HEDSPI.h"

// HX711::HX711(uint8_t DOUT, uint8_t PD_SCK, uint16_t max_load) {
//   PD_SCK = PD_SCK;
//   DOUT = DOUT;
//   MAX_LOAD = max_load;
//   native = 1;
// }

HX711::HX711(uint8_t DOUT, uint8_t PD_SCK) {
  PD_SCK = PD_SCK;
  DOUT = DOUT;
  native = 1;
}
HX711::HX711() {}

void HX711::initializeNative() {
  pinMode(PD_SCK, OUTPUT);
  pinMode(DOUT, INPUT);
}


void HX711::setGain(uint8_t gain) {
  if (gain >= 25 && gain <= 27) {
    Gain = gain;
  }
}


void HX711::setScale(float scale) { Scale = scale; }

void HX711::setZero() {
  uint32_t w = 0;
  for (int i = 0; i < 5; i++) {
    w += getData();
    delay(1);
  }
  w /= 5;
  _zero = w;
}


uint32_t HX711::getData() {
  uint32_t data = 0;

  if (native) {
    data = readDataNative();
  } else {
    uint8_t g[1];
    g[0] = Gain;
    sendData(g, 1);

    uint8_t buf[4];

    readData(buf, 4);
    data |= ((uint32_t)buf[0] << 24);
    data |= ((uint32_t)buf[1] << 16);
    data |= ((uint32_t)buf[2] << 8);
    data |= ((uint32_t)buf[3] << 0);
  }
  if (data != 8820) {
    Serial.print(data);
    Serial.print(" ");
  }
  return data;
}


float HX711::getWeight() {
  return (long)(getData() - _zerroConstant) / Scale;
}


float HX711::getWeight(uint8_t repeat) {
  unsigned int w = 0;
  unsigned int w_avg = 0;
  for (int i = 0; i < repeat; i++) {
    w = getData();
    if (w > 0 && w < )
  }
  return (long)(w - _zerroConstant) / Scale;
}


/**
 * @brief       readDataNative reads data from sensor using pins
 *
 * @return      data from device data register
 */
uint32_t HX711::readDataNative() {
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
  for (int i = 0; i < Gain; i++) {
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

  uint32_t result = 0;
  // read data
  for (int i = 0; i < 24; ++i) {
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

  return result;
}