/*!
 * @brief An thermocouple sensor with I2C communication From M5Stack
 * @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
 *
 * @Links [Unit KMeter](https://docs.m5stack.com/en/unit/kmeter)
 * @version  V0.1.1
 * @date  2022-10-21
 */
#ifndef _M5_KMETER_H_
#define _M5_KMETER_H_

#include <Arduino.h>
#include <Wire.h>

class M5_KMeter {
   public:
    static constexpr const uint8_t reg_0x04_id_h = 0x31;
    static constexpr const uint8_t reg_0x05_id_l = 0x85;
    enum error_code_t {
        err_ok,
        err_unknown,
        err_i2c_fail,
        err_open_circuit,
        err_short_to_gnd,
        err_short_to_vcc,
    };

    void begin(TwoWire* wire = &Wire, uint8_t addr = 0x66);

    bool update(void);

    /*! @brief Read temperature data.
        @return temperature data. */
    float getTemperature(void) const {
        return _temperature;
    }

    /*! @brief Read internal temperature data.
        @return internal temperature data. */
    float getInternalTemp(void) const {
        return _internal_temp;
    }

    /*! @brief Get the error in the latest update run
        @return error code. */
    error_code_t getError(void) const {
        return _latest_error;
    }

    bool setSleepTime(uint16_t second);
    bool sleep(bool scl_low_wakeup = false);

    bool changeAddr(uint8_t new_i2c_addr);

    bool getRawData(uint8_t* result, size_t len = 4);

   private:
    TwoWire* _wire;
    float _temperature;
    float _internal_temp;
    uint8_t _addr;
    error_code_t _latest_error;
};

#endif
