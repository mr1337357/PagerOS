#include <string.h>
#include "fileops.h"
#include "hal.h"
#include "os_process.h"

typedef struct
{
  int op;
  int fd;
  int len;
  uint8_t *buffer;
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
  process_t *current;
  switch(fop->fd)
  {
    case 0:
      current = get_current_process();
      Serial.write(fop->buffer, fop->len);
      if(current)
      {
        current->term->write(fop->buffer, fop->len); //todo: make this thread specific
      }
      return fop->len;
      break;
  }
  return -1;
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
      return fileop_write(fop);
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