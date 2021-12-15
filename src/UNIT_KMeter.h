#ifndef __UNIT_KMETER_H
#define __UNIT_KMETER_H

#include <Arduino.h>
#include <Wire.h>

class UNIT_KMeter 
{
private:
  uint8_t _addr;
  TwoWire* _wire;
  float _temperature;
public:
  void begin(TwoWire* wire = &Wire, uint8_t addr = 0x66);
  float getTemperature(void);
};

#endif
