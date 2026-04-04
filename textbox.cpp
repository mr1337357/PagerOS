#include "Adafruit_GFX.h"
#include "textbox.h"

textbox::textbox()
{
}
void textbox::begin(Adafruit_GFX &tft, int x, int y, int w, int h)
{
  _tft = &tft;
  _x = x;
  _y = y;
  _w = w;
  _h = h;
  _cx = 0;
  _cy = 0;
}

size_t textbox::write(uint8_t c)
{
  _tft->setCursor(_cx + _x, _cy + _y);
  _tft->write(c);
  _cx += 6;
  if((_cx > _w - 6)|| c == '\n')
  {
    _cy += 8;
    _cx = 0;
  }
  return 1;
}

size_t textbox::write(const uint8_t *buffer, size_t size)
{
  int i;
  for(i=0;i<size;i++)
  {
    write(*buffer++);
  }
  return size;
}