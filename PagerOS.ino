#include "hal.h"
#include "psram_alloc.h"
#include "os_elf.h"
#include "os_process.h"
#include "fileops.h"

uint8_t *psram_ptr = (uint8_t *)0x3C060000;

int do_syscall(int callnum, void *arg)
{
  switch(callnum)
  {
    //OS ops
    case 0:
      break;
    //file ops
    case 1:
      do_fileops(arg);
      break;
  }
  return -1;
}



void setup() {
  esp_err_t status;
  Serial.begin(115200);
  //tft.fillScreen(ST77XX_BLACK);
  hal_init();
  psramInit();
  psram_init((uint32_t)psram_ptr, ESP.getPsramSize());
  process_init();

  tft.fillRect(0, 0, 20, 222, tft.color565(5,5,5));
  process_launch("/apps/launcher.elf");
}

void loop()
{
  delay(1000);
}
