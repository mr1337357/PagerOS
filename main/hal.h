#ifndef __HAL_H__
#define __HAL_H__

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7796S.h>
#include "IoExpanderXL9555.hpp"
#include <SPI.h>
#include <SD.h>

//PINDEFS
#define SDA 3
#define SCL 2

#define MOSI 34
#define MISO 33
#define SCK  35

#define DISP_RST  (-1)
#define DISP_CS   (38)
#define DISP_DC   (37)
#define DISP_BL   (42)

#define SD_CS       (21)

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

#endif
