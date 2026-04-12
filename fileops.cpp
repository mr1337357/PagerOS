#include <string.h>
#include "SD.h"
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

File files[16];
bool filevalid[16];

int console_read(uint8_t *buffer, int len)
{
  //todo
  return 0;
}

int console_write(uint8_t *buffer, int len)
{
  process_t *current;
  current = get_current_process();
  Serial.write(buffer,len);
  if(current && current->term)
  {
    current->term->write(buffer,len);
  }
  return len;
}

void fileop_init()
{
  int i;
  for(i=0;i<16;i++)
  {
    filevalid[i] = false;
  }
}

int fileop_write(fileop *fop)
{
  int index;
  if(fop->fd == 0)
  {
    return -1;
  }
  if(fop->fd == 1)
  {
    return console_write(fop->buffer,fop->len);
  }
  index = fop->fd - 2;
  if(filevalid[index])
  {
    return files[index].write(fop->buffer,fop->len);
  }
  return -1;
}

int fileop_read(fileop *fop)
{
  int index;
  if(fop->fd == 0)
  {
    return console_read(fop->buffer, fop->len);
  }
  if(fop->fd == 1)
  {
    return -1;
  }
  index = fop->fd - 2;
  if(filevalid[index])
  {
    if(!files[index].isDirectory())
    {
      return files[index].read(fop->buffer,fop->len);
    }
    else
    {
      String fname = files[index].getNextFileName();
      strcpy((char *)fop->buffer,fname.c_str()); //buffer overflow possible. eek.
      return strlen(fname.c_str());
    }
  }
  return 0;
}

int fileop_open(fileop *fop)
{
  char *mode = "r";
  int i;
  for(i=0;i<16;i++)
  {
    if(!filevalid[i])
    {
      break;
    }
  }
  if(i==16)
  {
    return -1;
  }
  if(fop->fd == 1)
  {
    mode[0] = 'w';
  }
  files[i] = SD.open((char *)fop->buffer,mode);
  if(!files[i])
  {
    return -1;
  }
  filevalid[i] = true;
  return i+2;
}

int fileop_close(fileop *fop)
{
  if(filevalid[fop->fd])
  {
    files[fop->fd].close();
    filevalid[fop->fd]=false;
  }
  return 0;
}

typedef struct
{
  uint32_t isdir;
  uint32_t len;
} fileinfo;
int fileop_info(fileop *fop)
{
  fileinfo *finfo;
  if(fop->len < sizeof(fileinfo) || !filevalid[fop->fd])
  {
    return -1;
  }
  finfo = (fileinfo *)fop->buffer;
  finfo->isdir = files[fop->fd].isDirectory();
  finfo->len = files[fop->fd].size();
  return sizeof(fileinfo);
}

int do_fileops(void *arg)
{
  fileop *fop = (fileop *)arg;
  switch(fop->op)
  {
    //read
    case 0:
      return fileop_read(fop);
      break;
    //write
    case 1:
      return fileop_write(fop);
      break;
    //open
    case 2:
      return fileop_open(fop);
      break;
    //close
    case 3:
      return fileop_close(fop);
      break;
    case 4:
      return fileop_info(fop);
      break;
  }
  return -1;
} 