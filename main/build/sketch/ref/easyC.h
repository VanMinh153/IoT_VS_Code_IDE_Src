#line 1 "C:\\Users\\Admin\\Downloads\\Wokwi\\main\\ref\\easyC.h"
/**
 **************************************************
 *
 * @file        Generic-easyC-SOLDERED.h
 * @brief       Basic funtions for easyC libraries
 *
 *
 *
 * @authors     @ soldered.com
 ***************************************************/

#ifndef __EASYC_LIBRARY__
#define __EASYC_LIBRARY__

#include "Arduino.h"
#include "Wire.h"

#define ANALOG_READ_REG  0
#define DIGITAL_READ_REG 1

class EasyC
{
  public:
    EasyC();

    void begin();
    void begin(uint8_t _address);

  protected:
    int native = 0;
    bool beginDone = 0;

    virtual void initializeNative() = 0;

    int err;

    char address;
    const char defaultAddress = 0x30;

    int sendAddress(char regAddr);
    int readData(uint8_t a[], int n);
    int sendData(uint8_t a[], int n);
    int readRegister(char regAddr, uint8_t a[], size_t n);
};

#endif
