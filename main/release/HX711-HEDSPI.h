/**
 **************************************************
 *
 * @file        HX711.h
 * @brief       Arduino library for hx711 breakout
 *
 *
 * @authors     @ soldered.com @Robert Peric
 * @authors     Nguyen Van Minh - SOICT-HUST
 ***************************************************/
#ifndef __HX711_HEDSPI__
#define __HX711_HEDSPI__

#include "Arduino.h"
#include "easyC.h"

// Gain 32 is for B+-, while others are for A+-
#define GAIN_128 25
#define GAIN_64 27
#define GAIN_32 26

class HX711 : public EasyC {
public:
  // HX711(uint8_t DOUT, uint8_t PD_SCK, uint8_t Gain, uint16_t MAX_LOAD);
  // HX711(uint8_t DOUT, uint8_t PD_SCK, uint8_t Gain);
  HX711(uint8_t DOUT, uint8_t PD_SCK);
  HX711();

  void setGain(uint8_t Gain);
  void setScale(float Scale);
  void setZero();
  uint32_t getData();
  uint32_t readDataNative();
  float getWeight();
  float getWeight(uint8_t repeat);

protected:
  void initializeNative();

private:
  uint8_t DOUT;
  uint8_t PD_SCK;
  uint16_t MAX_LOAD;
  uint8_t Gain = GAIN_128;
  uint32_t Zero = 0;
  float Scale = 1;
};

#endif
