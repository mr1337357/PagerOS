#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7796S.h>
#include <IoExpanderXL9555.hpp>
#include <SPI.h>
#include <SD.h>

extern SPIClass hspi;

extern Adafruit_ST7796S tft;

extern IoExpanderXL9555 io;

const uint8_t GPIOs[] = 
{
  DISP_CS,
  DISP_DC,
  SD_CS,

};

void hal_init();