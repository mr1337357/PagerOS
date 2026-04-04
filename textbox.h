#ifndef __TEXTBOX_H__
#define __TEXTBOX_H__
#include "Adafruit_GFX.h"
class textbox : public Print
{
  public:
    textbox();
    void begin(Adafruit_GFX &tft, int x, int y, int w, int h);
    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);
  private:
    Adafruit_GFX *_tft;
    int _x;
    int _y;
    int _w;
    int _h;
    int _cx;
    int _cy;
};

#endif