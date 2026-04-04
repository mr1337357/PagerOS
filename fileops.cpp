#include "fileops.h"

#include "hal.h"

typedef struct
{
  int op;
  int fd;
  int len;
  uint8_t buffer[];
} fileop;



int fileop_read(fileop *fop)
{
  switch(fop->fd)
  {
    case 0:
      return 0;
      break;
  }
  return -1;
}

int fileop_write(fileop *fop)
{
  switch(fop->fd)
  {
    case 0:
      fb.write(fop->buffer);
      break;
  }
}

int do_fileops(void *arg)
{
  fileop *fop = (fileop *)arg;
  switch(fop->op)
  {
    //read
    case 0:
      break;
    //write
    case 1:
      break;
    //open
    case 2:
      break;
    //close
    case 3:
      break;

  }
  return 0;
} 