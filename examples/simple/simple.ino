
#include <UNIT_KMeter.h>
#include <M5GFX.h>

M5GFX display;

UNIT_KMeter sensor;

void setup(void)
{
  display.begin();
  display.setTextSize(2);
  if (display.width() < display.height())
  {
    display.setRotation(display.getRotation() ^ 1);
  }

  Wire.begin(SDA, SCL, 400000);

  sensor.begin();
}

void loop(void)
{
  delay(100);
  float temperature = sensor.getTemperature();

  display.setCursor(display.width()/2 - 64, display.height()/2);
  display.printf("Temp:%4.2f ", temperature);
}
