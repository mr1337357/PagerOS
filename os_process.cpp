#include "FreeRTOS.h"
#include "hal.h"
#include "os_elf.h"
#include "os_process.h"
#include "psram_alloc.h"
#include "textbox.h"

process_t process_list[16];

int active_app;

void process_init()
{
  int i;
  for(i=0;i<16;i++)
  {
    process_list[i].freertos_handle = 0;
  }
  process_list[0].freertos_handle = xTaskGetCurrentTaskHandle();
  process_list[0].term = &tb;
  active_app = 0;
}

void process_is_gui()
{
  int pid = get_current_thread();
  delete process_list[pid].term;
  process_list[pid].term = 0;
  if(pid == active_app)
  {
    tft.fillScreen(0);
  }
}

void process_set_eventHandler(void (*handler)(int, void *))
{
  int pid = get_current_thread();
  process_list[pid].eventHandler = handler;
}

void process_switch_active_app(int app)
{
  int i;
  Serial.printf("update active was %d", active_app);
  if(process_list[active_app].freertos_handle != 0)
  {
    if(process_list[active_app].term)
    {
      process_list[active_app].term->set_visible(false);
    }
  }
  tft.fillScreen(0);
  if(app < 0)
  {
    for(i=0;i<16;i++)
    {
      active_app++;
      if(active_app>=16)
      {
        active_app = 0;
      }
      if(process_list[active_app].freertos_handle != 0)
      {
        break;
      }
    }
  }
  else
  {
    active_app = app;
  }
  if(process_list[active_app].freertos_handle != 0)
  {
    if(process_list[active_app].term)
    {
      process_list[active_app].term->set_visible(true);
    }
  }
  Serial.printf(" now is %d\n",active_app);
}

int get_current_thread()
{
  TaskHandle_t current = xTaskGetCurrentTaskHandle();
  int i;
  for(i=0;i<16;i++)
  {
    if(process_list[i].freertos_handle == current)
    {
      return i;
    }
  }
  return -1;
}

process_t *get_current_process()
{
  TaskHandle_t current = xTaskGetCurrentTaskHandle();
  int i;
  for(i=0;i<16;i++)
  {
    if(process_list[i].freertos_handle == current)
    {
      return &process_list[i];
    }
  }
  return 0;
}

int process_get_free()
{
  int i;
  for(i=0;i<16;i++)
  {
    if(process_list[i].freertos_handle == 0)
    {
      return i;
    }
  }
  return -1;
}

extern "C" void Cache_WriteBack_All(void);

int do_syscall(int callnum, void *arg);

void process_runner(void *args)
{
  char *filename = (char *)args;
  int pid;
  void (*entry)(void *);
  TaskHandle_t taskHandle;

  Serial.printf("Start app %s\n",filename);

  pid = get_current_thread();
  entry = (void (*)(void*))loadElf(SD,filename);
  if(entry == 0)
  {
    goto process_load_fail;
  }
  process_list[pid].term = new textbox();
  process_list[pid].term->begin(tft,0,0,480,222);
  //active_app = pid;
  Cache_WriteBack_All();
  Serial.printf("app start\n");
  process_switch_active_app(pid);
  if(entry != 0)
  {
    entry((void *)do_syscall);
  }
  Serial.printf("app end\n");
process_load_fail:
  if(process_list[pid].term != 0)
  {
    delete process_list[get_current_thread()].term;
    process_list[get_current_thread()].term = 0;
  }
  taskHandle = process_list[pid].freertos_handle;
  process_list[pid].freertos_handle = 0;
  psram_thread_killed(pid);
  if(pid == active_app)
  {
    process_switch_active_app();
  }
  vTaskDelete(taskHandle);
}

void process_launch(char *filename)
{
  uint32_t args[2];
  int pid;
  pid = process_get_free();
  strcpy(process_list[pid].name,filename);
  xTaskCreate( process_runner,
                         process_list[pid].name,
                         32768,
                         process_list[pid].name,
                         1,
                         &process_list[pid].freertos_handle
                       );
}