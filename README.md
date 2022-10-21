# M5Unit-KMeter

## Overview

Contains case programs of M5Stack UNIT KMeter. 

## Related Link

- [Document & Datasheet - M5Unit-KMeter](https://docs.m5stack.com/en/unit/kmeter)

## How To Use

```
#include <M5_KMeter.h>

M5_KMeter sensor;

void setup(void) {

    // prepare I2C.
    Wire.begin((int)SDA, (int)SCL, 400000L);
//  Wire.begin(21, 22, 400000L);

    // prepare sensor. ( default I2C addr : 0x66 )
    sensor.begin();
//  sensor.begin(&Wire, 0x66);

    // change I2C addr.
    // ( When the unit is turned back on, it will operate with the new I2C address. )
//  sensor.changeAddr(0x60);

    // set sleep time. (5 second)
    sensor.setSleepTime(5);
}

void loop(void) {

    // Sensor deep sleep.
    // (Number of seconds set by the setSleepTime function)
    sensor.sleep();

    delay(5000);

    // data read from unit.
    if (sensor.update()) {

        // get sensor temperature.
        float temperature = sensor.getTemperature();

        // get unit internal temperature.
        float internaltemp = sensor.getInternalTemp();

        printf("%3.2f  /  %3.2f \n", temperature, internaltemp);
    } else {
        printf("error %d \n", sensor.getError());
    }
}
```

## License

- [M5Unit-KMeter - MIT](LICENSE)
