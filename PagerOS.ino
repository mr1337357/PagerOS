#include "hal.h"
#include "psram_alloc.h"
#include "os_elf.h"
#include "os_process.h"
#include "fileops.h"

uint8_t *psram_ptr = (uint8_t *)0x3C060000;

typedef struct
{
  int op;

} os_op;

int do_os_ops(void *arg)
{
  int *op = (int *)arg;
  arg = (void *)&op[1];
  switch(*op)
  {
    case 0:
      {
        void *argp = *(void **)arg;
        void (*handler)(int, void *) = (void (*)(int, void *))argp;
        process_set_eventHandler(handler);
        return 0;
        break;
      }
    case 1:
      delay(*(int *)arg);
      break;
  }
  return -1;
}

int do_syscall(int callnum, void *arg)
{
  //Serial.printf("(%d)syscall(%d,%p)\n",get_current_thread(),callnum,arg);
  switch(callnum)
  {
    //OS ops
    case 0:
      return do_os_ops(arg);
      break;
    //file ops
    case 1:
      return do_fileops(arg);
      break;
    //gfx ops
    case 2:
      //return do_gfxops(arg);
      break;
  }
  return -1;
}



void setup() {
  esp_err_t status;
  Serial.begin(115200);
  //tft.fillScreen(ST77XX_BLACK);
  hal_init();
  uint8_t *psram_ptr = (uint8_t *)ps_malloc(7 * 1024 * 1024);
  psramInit();
  psram_init((uint32_t)psram_ptr, 7 * 1024 * 1024);
  process_init();
  tb.set_visible(false);
  tft.fillScreen(0);
  tb.printf("launching /apps/launcher.elf\n");
  process_launch("/apps/launcher.elf");
}

int count = 0;

void loop()
{
  delay(500);
}
