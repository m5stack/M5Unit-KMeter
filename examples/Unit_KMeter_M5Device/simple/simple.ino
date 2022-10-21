/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5Devices sample source code
*                          配套  M5Devices 示例源代码
* Visit for more information: https://docs.m5stack.com/en/unit/kmeter
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/kmeter
*
* Product: KMeter.  热电偶传感器
* Date: 2022/7/6
*******************************************************************************
  Thermocouple and internal temperature output on screen and serial port
  在屏幕和串口输出热电偶温度及内部温度
*/
#include <M5GFX.h>
#include <M5_KMeter.h>
#include <esp_log.h>

M5GFX display;
M5_KMeter sensor;

void setup(void) {
    display.begin();

    display.setTextSize(2);
    if (display.width() < display.height()) {
        display.setRotation(display.getRotation() ^ 1);
    }

    Wire.begin((int)SDA, (int)SCL, 400000L);
    // Wire.begin(21, 22, 400000L);

    sensor.begin();
    // sensor.begin(&Wire, 0x66);

    // set sleep time. (5 second)
    while (!sensor.setSleepTime(5)) {
        delay(100);
    }
}

void loop(void) {
    // Sensor deep sleep.
    // (Number of seconds set by the setSleepTime function)
    sensor.sleep();

    delay(5000);

    int fontHeight = display.fontHeight();
    int x          = display.width() / 2 - 64;
    int y          = (display.height() - fontHeight) / 2;

    if (sensor.update()) {
        float temperature = sensor.getTemperature();
        display.setCursor(x, y - fontHeight);
        display.printf("Tmp:%7.2f ", temperature);

        float internaltemp = sensor.getInternalTemp();
        display.setCursor(x, y);
        display.printf("In:%8.4f ", internaltemp);

        ESP_LOGI("loop", "%6.2f , %8.4f", temperature, internaltemp);
    }

    const char* err_msg;
    switch (sensor.getError()) {
        default:
        case M5_KMeter::error_code_t::err_unknown:
            err_msg = " UNKNOWN ";
            break;
        case M5_KMeter::error_code_t::err_ok:
            err_msg = " SUCCESS ";
            break;
        case M5_KMeter::error_code_t::err_i2c_fail:
            err_msg = "I2C ERROR";
            break;
        case M5_KMeter::error_code_t::err_open_circuit:
            err_msg = "NO SENSOR";
            break;
        case M5_KMeter::error_code_t::err_short_to_gnd:
            err_msg = "SHORT GND";
            break;
        case M5_KMeter::error_code_t::err_short_to_vcc:
            err_msg = "SHORT VCC";
            break;
    }
    display.setCursor(x, y + fontHeight);
    display.print(err_msg);
}
