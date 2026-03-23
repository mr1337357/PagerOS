#include "hal.h"

SPIClass* hspi = new SPIClass(HSPI);

Adafruit_ST7796S tft = Adafruit_ST7796S(hspi, TFT_CS, TFT_DC, TFT_RST);

void hal_init()
{
  hspi->begin(SCLK,MISO,MOSI,-1);

  tft.init(222, 480, 0, 49);
  tft.setRotation(3);
  pinMode(42,OUTPUT);
  analogWrite(42,128);
}