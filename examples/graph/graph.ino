
#include <Arduino.h>

#include <UNIT_KMeter.h>
#include <M5GFX.h>
#include <stdlib.h>
#include <alloca.h>

#include <esp_log.h>

static float* rawdata;
static size_t rawdata_len;
static int rawdata_idx = 0;
static int graph_height;
static int draw_y_offset;
static float min_temp;
static float max_temp;

M5GFX display;
UNIT_KMeter sensor;

M5Canvas canvas[2];

void setup(void)
{
  display.begin();

  Wire.begin(SDA, SCL, 400000);

  sensor.begin();

  display.clear();
  display.setEpdMode(epd_mode_t::epd_fast);
  if (display.width() > display.height())
  {
    display.setRotation(display.getRotation() ^ 1);
  }
  display.setFont(&fonts::Font4);
  display.setTextColor((uint32_t)~display.getBaseColor(), display.getBaseColor());
  display.setTextDatum(textdatum_t::top_right);
  display.setTextPadding(display.textWidth("1888.88", &fonts::Font4));
  draw_y_offset = display.fontHeight(&fonts::Font4);

  graph_height = display.height() - draw_y_offset;

  for (int i = 0; i < 2; ++i)
  {
    canvas[i].setColorDepth(display.getColorDepth());
    canvas[i].createSprite(1, graph_height);
    canvas[i].setTextDatum(textdatum_t::bottom_right);
    canvas[i].setTextColor(TFT_LIGHTGRAY);
  }

  rawdata_len = display.width();
  rawdata = (float*)malloc(rawdata_len * sizeof(float));
//  memset(rawdata, 0, rawdata_len * sizeof(float));

  float temperature = sensor.getTemperature();
  min_temp = max_temp = temperature;
  for (int i = 0; i < rawdata_len; ++i)
  {
    rawdata[i] = temperature;
  }
}

void drawGraph(void)
{
  float min_t = INT16_MAX;
  float max_t = INT16_MIN;
  for (int i = 0; i < rawdata_len; ++i)
  {
    float value = rawdata[i];
    if (min_t > value) { min_t = value; }
    if (max_t < value) { max_t = value; }
  }
  float diff = (max_t - min_t) + 1;
  min_temp = (min_temp + min_t - (diff / 20)) / 2;
  max_temp = (max_temp + max_t + (diff / 20)) / 2;

  int k16 = (graph_height << 16) / (max_temp - min_temp);

  static constexpr int steps[] = { 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000 };
  int step_index = 0;
  while (((k16 * steps[step_index]) >> 16) < 10) { ++step_index; }
  int step = steps[step_index];
  bool flip = 0;

  canvas[flip].clear(display.getBaseColor());

  int v = ((int)min_temp / step) * step;
  do
  {
    canvas[flip].drawPixel(0, graph_height - ((int)((v - min_temp ) * k16) >> 16), TFT_DARKGRAY);
  } while (max_temp > (v += step));

  size_t buffer_len = canvas[flip].bufferLength();
  auto buffer = (uint8_t*)alloca(buffer_len);
  memcpy(buffer, canvas[flip].getBuffer(), buffer_len);

  int push_x = 2;
  int x_next = rawdata_idx;

  int y0 = graph_height - ((int)((rawdata[x_next] - min_temp) * k16) >> 16);
  display.startWrite();
  do
  {
    memcpy(canvas[flip].getBuffer(), buffer, buffer_len);
    int x = x_next;
    x_next = (x + 1) % rawdata_len;

    int y1 = y0;
    y0 = graph_height - ((int)((rawdata[x_next] - min_temp) * k16) >> 16);
    if (display.width() - push_x < 24)
    {
      v = ((int)min_temp / step) * step;
      do
      {
        canvas[flip].drawNumber((int)v, display.width() - push_x, graph_height - ((int)((v - min_temp ) * k16) >> 16));
      } while (max_temp > (v += step));
    }

    int y_min = y0, y_max = y1;
    if (y_min > y_max) std::swap(y_min, y_max);
    canvas[flip].drawFastVLine(0, y_min, y_max - y_min + 1, ~display.getBaseColor());
    canvas[flip].pushSprite(&display, push_x, draw_y_offset);
    flip = !flip;
  } while (++push_x < rawdata_len);
  display.endWrite();
}

void loop(void)
{
  float temperature = sensor.getTemperature();

  rawdata[rawdata_idx] = temperature;
  if (++rawdata_idx >= rawdata_len)
  {
    rawdata_idx = 0;
  }

  drawGraph();
  display.drawFloat( temperature, 2, display.width(), 0 );

  static uint32_t prev_msec;
  uint32_t msec = millis();
  if (msec - prev_msec < 100)
  {
    ESP_LOGI("main", "%2.2f", temperature);
    m5gfx::delay(100 - (msec - prev_msec));
  }
  prev_msec = msec;
//*/
}
