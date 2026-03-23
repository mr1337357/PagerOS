#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7796S.h>
#include <SPI.h>

#define SCLK          35
#define MOSI          34
#define MISO          33

extern SPIClass* hspi;

#define TFT_CS        38
#define TFT_RST       -1
#define TFT_DC        37

extern Adafruit_ST7796S tft;

#define SD_CS         21

void hal_init();