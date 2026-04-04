#include "FreeRTOS.h"
#include "hal.h"
#include "os_elf.h"
#include "psram_alloc.h"
#include "textbox.h"

typedef struct 
{
  TaskHandle_t freertos_handle;
  void (*eventHandler)(void *);
  char name[256];
  textbox *term;
} process_t;

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
}

void process_is_gui()
{
  delete process_list[get_current_thread()].term;
  process_list[get_current_thread()].term = 0;
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

  tb.printf("Loading app %s\n", filename);
  Serial.printf("Start app %s\n",filename);

  pid = get_current_thread();
  entry = (void (*)(void*))loadElf(SD,filename);
  if(entry == 0)
  {
    goto process_load_fail;
  }
  process_list[pid].mode = 0;
  Cache_WriteBack_All();
  Serial.printf("app start\n");
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
                         8192,
                         process_list[pid].name,
                         1,
                         &process_list[pid].freertos_handle
                       );
}