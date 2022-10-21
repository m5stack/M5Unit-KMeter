#include "M5_KMeter.h"

/*! @brief Initialize the Unit KMeter.*/
void M5_KMeter::begin(TwoWire *wire, uint8_t addr) {
    _wire          = wire;
    _addr          = addr;
    _temperature   = 0.0f;
    _internal_temp = 0.0f;
}

/*! @brief Read raw data.
    @return True if the read was successful, otherwise false.. */
bool M5_KMeter::getRawData(uint8_t *result, size_t len) {
    if (!_wire->requestFrom((int)_addr, len)) {
        return false;
    }

    _wire->readBytes(result, len);

    return true;
}

/*! @brief Read temperature data.
    @return temperature data. */
float M5_KMeter::getTemperature(void) {
    float result = _temperature;
    _wire->beginTransmission((int)_addr);
    _wire->write(0);
    if (0 == _wire->endTransmission(false)) {
        if (_wire->requestFrom((int)_addr, 2)) {
            int16_t t = (_wire->read() << 8);
            t |= _wire->read();
            if (0 == (t & 1)) {
                result       = 0.25f * (t >> 2);
                _temperature = result;
            }
        }
    }
    return result;
}

/*! @brief Read internal temperature data.
    @return internal temperature data. */
float M5_KMeter::getInternalTemp(void) {
    float result = _internal_temp;
    _wire->beginTransmission((int)_addr);
    _wire->write(0);
    if (0 == _wire->endTransmission(false)) {
        if (_wire->requestFrom((int)_addr, 4)) {
            int16_t t = (_wire->read() << 8);
            t |= _wire->read();
            if (0 == (t & 1)) {
                _temperature = 0.25f * (t >> 2);
            }

            t = (_wire->read() << 8);
            t |= _wire->read();
            if (0 == (t & 7)) {
                result         = 0.0625f * (t >> 4);
                _internal_temp = result;
            }
        }
    }
    return result;
}

/*! @brief Sets the number of sleep seconds. (Configure only, do not execute.)
    @brief The set values are stored in the unit's NVS and restored at wake-up.
    @param second Number of seconds between sleep execution and wake-up.
    @return true:success / false:failure
    @attention (Features added in firmware v0.0.3) */
bool M5_KMeter::setSleepTime(uint16_t second) {
    uint8_t buf[] = {0x0E, (uint8_t)(second >> 8), (uint8_t)second};
    _wire->beginTransmission((int)_addr);
    _wire->write(buf, sizeof(buf));
    return (0 == _wire->endTransmission());
}

/*! @brief Put the unit in deep sleep.
    @brief Specify the number of seconds before wake-up with setSleepTime.
    @param scl_low_wakeup true:Wake up when I2C SCL goes Low.
    @return true:success / false:failure
    @attention (Features added in firmware v0.0.3) */
bool M5_KMeter::sleep(bool scl_low_wakeup) {
    uint8_t buf[] = {0x10, (uint8_t)(scl_low_wakeup ? 0xEF : 0xEE)};
    _wire->beginTransmission((int)_addr);
    _wire->write(buf, sizeof(buf));
    return (0 == _wire->endTransmission());
}

/*! @brief Change the I2C address of the unit.
    @brief The address is stored even when the power is turned off.
    @param new_i2c_addr New I2C address (0x08 ~ 0x77)
    @return true:success / false:failure
    @attention When the power is turned back on,
    it will operate at the new address. */
bool M5_KMeter::changeAddr(uint8_t new_i2c_addr) {
    if (new_i2c_addr < 8 || new_i2c_addr > 0x77) {
        return false;
    }

    uint8_t buf[] = {0x08, new_i2c_addr, (uint8_t)~new_i2c_addr};
    _wire->beginTransmission((int)_addr);
    _wire->write(buf, sizeof(buf));
    return (0 == _wire->endTransmission());
}
