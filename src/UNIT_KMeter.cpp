#include "UNIT_KMeter.h"

void UNIT_KMeter::begin(TwoWire* wire, uint8_t addr) {
  _wire = wire;
  _addr = addr;
  _temperature = 0.0f;
}

float UNIT_KMeter::getTemperature(void) {
  float result = _temperature;
  if (_wire->requestFrom((int)_addr, 2))
  {
    int16_t thermocouple = (_wire->read() << 8);
    thermocouple |= _wire->read();
    result = 0.25f * (thermocouple >> 2);
    _temperature = result;
  }
  return _temperature;
}

