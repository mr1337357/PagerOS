#include "Adafruit_GFX.h"
#include "textbox.h"

textbox::textbox()
{
}

void textbox::begin(Adafruit_GFX &tft, int x, int y, int w, int h)
{
  int i;
  _tft = &tft;
  _x = x;
  _y = y;
  _w = w / 6;
  _h = h / 8;
  _cx = 0;
  _cy = 0;
  vis = true;
  buffer = (uint8_t *)malloc(_w * _h);
  for(i=0;i<_w * _h;i++)
  {
    buffer[i] = 0;
  }
}

void textbox::redraw()
{
  int x,y;
  _tft->fillScreen(0); //erases screen instead of textbox lol
  for(y=0;y<_h;y++)
  {
    for(x=0;x<_w;x++)
    {
      _tft->setCursor((x * 6) + _x, (y * 8) + _y);
      _tft->write(buffer[x + (y * _w)]);
    }
  }
}

void textbox::set_visible(bool visible)
{
  vis = visible;
  if(vis)
  {
    redraw();
  }
}

size_t textbox::write(uint8_t c)
{
  buffer[_cx+(_cy*_w)] = c;
  if(vis)
  {
    _tft->setCursor((_cx * 6) + _x, (_cy * 8) + _y);
    _tft->write(c);
  }
  _cx += 1;
  if((_cx >= _w)|| c == '\n')
  {
    if(_cy < _h-1)
    {
      _cy += 1;
      _cx = 0;
      //redraw();
    }
    else
    {
      int x,y;
      for(y=0;y<(_h-1);y++)
      {
        for(x=0;x<_w;x++)
        {
          buffer[x + (y * _w)] = buffer[x + ((y+1)*_w)];
        }
      }
      for(x=0;x<_w;x++)
      {
        buffer[x + ((_h-1)*_w)] = 0;
      }
      _cx = 0;
      if(vis)
      {
        redraw();
      }
    }
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