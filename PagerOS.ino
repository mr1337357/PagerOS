#include "hal.h"
#include "psram_alloc.h"
#include "os_elf.h"

uint8_t *psram_ptr = (uint8_t *)0x3C060000;
char app_name_buffer[128];
void do_syscall(int callnum, void *arg)
{
  switch(callnum)
  {
    case 1:
      Serial.print((char *)arg);
      break;
  }
}

TaskHandle_t threads[16];

int get_current_thread()
{
  TaskHandle_t current = xTaskGetCurrentTaskHandle();
  int i;
  for(i=0;i<16;i++)
  {
    if(threads[i] == current)
    {
      return i;
    }
  }
  return -1;
}

int get_free_thread()
{
  int i;
  for(i=0;i<16;i++)
  {
    if(threads[i] == 0)
    {
      return i;
    }
  }
  return -1;
}

void thread_runner(void *args)
{
  char *filename = (char *)args;
  Serial.printf("Start app %s\n",filename);
  int pid;
  void (*entry)(void *);
  TaskHandle_t taskHandle;
  pid = get_current_thread();
  entry = (void (*)(void*))loadElf(SD,filename);
  Serial.printf("entry %08X\n",entry);
  Serial.printf("app start\n");
  if(entry != 0)
  {
    entry((void *)do_syscall);
  }
  Serial.printf("app end\n");
  taskHandle = threads[pid];
  threads[pid] = 0;
  psram_thread_killed(pid);
  vTaskDelete(taskHandle);
}

void app_launch(char *filename)
{
  uint32_t args[2];
  int pid;
  pid = get_free_thread();
  strcpy(app_name_buffer,filename);
  xTaskCreate( thread_runner,
                         app_name_buffer,
                         8192,
                         app_name_buffer,
                         1,
                         &threads[pid]
                       );
}

void setup() {
  esp_err_t status;
  Serial.begin(115200);
  //tft.fillScreen(ST77XX_BLACK);
  hal_init();
  psramInit();
  psram_init((uint32_t)psram_ptr, ESP.getPsramSize());
  int i;
  for(i=0;i<16;i++)
  {
    threads[i] = 0;
  }
  //fake a thread for the kernel ;)
  threads[get_free_thread()] = xTaskGetCurrentTaskHandle();
  app_launch("/apps/hello.stripped.elf");
}

void loop()
{
  delay(1000);
}
