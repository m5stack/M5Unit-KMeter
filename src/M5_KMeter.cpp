#include "M5_KMeter.h"

/*! @brief Initialize the Unit KMeter.*/
void M5_KMeter::begin(TwoWire *wire, uint8_t addr) {
    _wire          = wire;
    _addr          = addr;
    _temperature   = 0.0f;
    _internal_temp = 0.0f;
    _latest_error  = err_unknown;
}

/*! @brief Get values from the unit and update temperature data.
    @return True if the read was successful, otherwise false.. */
bool M5_KMeter::update(void) {
    uint8_t read_data[6];
    int retry = 3;
    while ((!getRawData(read_data, sizeof(read_data)) ||
            read_data[4] != reg_0x04_id_h || read_data[5] != reg_0x05_id_l) &&
           --retry)
        ;
    error_code_t err = err_i2c_fail;
    if (retry != 0) {
        int16_t tmp    = read_data[2] << 8 | read_data[3];
        _internal_temp = 0.0625f * (tmp >> 4);

        switch (read_data[3] & 7) {
            case 0:
                err = (read_data[1] & 1) ? err_unknown : err_ok;
                break;
            case 1:
                err = err_open_circuit;
                break;
            case 2:
                err = err_short_to_gnd;
                break;
            case 4:
                err = err_short_to_vcc;
                break;
            default:
                err = err_unknown;
                break;
        }
        if (err == err_ok) {
            tmp          = read_data[0] << 8 | read_data[1];
            _temperature = 0.25f * (tmp >> 2);
        }
    }
    _latest_error = err;
    return (err == err_ok);
}

/*! @brief Read raw data.
    @return True if the read was successful, otherwise false. */
bool M5_KMeter::getRawData(uint8_t *result, size_t len) {
    _wire->beginTransmission((int)_addr);
    _wire->write(0);
    if (0 == _wire->endTransmission(false) &&
        _wire->requestFrom((int)_addr, len) &&
        (len == _wire->readBytes(result, len))) {
        return true;
    }
    return false;
}

/*! @brief Sets the number of sleep seconds. (Configure only, do not execute.)
    @brief The set values are stored in the unit's NVS and restored at wake-up.
    @param second Number of seconds between sleep execution and wake-up.
    @return true:success / false:failure
    @attention (Features added in firmware v1.0) */
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
    @attention (Features added in firmware v1.0) */
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
