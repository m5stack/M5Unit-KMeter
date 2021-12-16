
#include <UNIT_KMeter.h>
#include <M5GFX.h>
#include <esp_log.h>

M5GFX display;
UNIT_KMeter sensor;

void setup(void)
{
  display.begin();

  Wire.begin(SDA, SCL, 400000L);
  //Wire.begin(21, 22, 400000L);

  sensor.begin();

  display.setTextSize(2);
  if (display.width() < display.height())
  {
    display.setRotation(display.getRotation() ^ 1);
  }
}

void loop(void)
{
  delay(100);
  float temperature = sensor.getTemperature();

  display.setCursor(display.width()/2 - 64, display.height()/2);
  display.printf("Temp:%4.2f ", temperature);

  ESP_LOGI("loop", "%4.2f", temperature);
}
