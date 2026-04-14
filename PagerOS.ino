#include "hal.h"
#include "psram_alloc.h"
#include "os_elf.h"
#include "os_process.h"
#include "fileops.h"
#include "os_keyboard.h"

typedef struct
{
  int op;

} os_op;

SemaphoreHandle_t kEventSem;

uint32_t os_event_mask = 0;

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
    case 2:
      {
        void *buffer = psram_malloc(*(int *)arg);
        return int(buffer);
        break;
      }
    case 3:
      process_launch(*(char **)arg);
      return 0;
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

void os_wait_for_event()
{
  xSemaphoreTake(kEventSem, portMAX_DELAY);
}

void os_event_send(uint8_t event = 0)
{
  BaseType_t task;
  os_event_mask |= (1<<event);
  xSemaphoreGiveFromISR( kEventSem, &task);
}

void setup() {
  esp_err_t status;
  Serial.begin(115200);
  //tft.fillScreen(ST77XX_BLACK);
  kEventSem = xSemaphoreCreateCounting(32, 0);
  hal_init();
  keyboard_init();
  //psramInit();
  psram_init();
  process_init();
  Serial.printf("%d\n",__LINE__);
  keypad.readRegister(TCA8418_REG_INT_STAT);
  tb.set_visible(false);
    Serial.printf("%d\n",__LINE__);
  keypad.readRegister(TCA8418_REG_INT_STAT);
    Serial.printf("%d\n",__LINE__);
  tft.fillScreen(0);
    Serial.printf("%d\n",__LINE__);
  keypad.readRegister(TCA8418_REG_INT_STAT);
  tb.printf("launching /apps/launcher.elf\n");
    Serial.printf("%d\n",__LINE__);
  keypad.readRegister(TCA8418_REG_INT_STAT);
  //process_launch("/apps/launcher.elf");
}

int count = 0;

void loop()
{
  os_wait_for_event();
  if(os_event_mask & 1)
  {
    keyboard_handle_event();
    os_event_mask &= ~1;
  }
  if(os_event_mask & 2)
  {
    wheel_handle_event();
    os_event_mask &= ~2;
  }
}
