#ifndef __UNIT_KMETER_H
#define __UNIT_KMETER_H

#include <Arduino.h>
#include <Wire.h>

class UNIT_KMeter 
{
private:
  TwoWire* _wire;
  uint8_t _addr;
  float _temperature;
  float _internal_temp;

public:

  void begin(TwoWire* wire = &Wire, uint8_t addr = 0x66);

  bool getRawData(uint8_t* result, size_t len = 4);

  float getTemperature(void);
  float getInternalTemp(void);
};

#endif
