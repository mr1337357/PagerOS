#ifndef __OS_PROCESS_H__
#define __OS_PROCESS_H__
#include "FreeRTOS.h"

#include "hal.h"

#define EVENT_KB 0
#define EVENT_WHEEL 1
#define EVENT_LORA 2

typedef struct 
{
  TaskHandle_t freertos_handle;
  void (*eventHandler)(int, void *);
  char name[256];
  textbox *term;
} process_t;

void process_init();

void process_is_gui();

void process_set_eventHandler(void (*handler)(int, void *));

void process_send_event_active(int event, void *arg);

void process_send_event_all(int event, void *arg);

void process_switch_active_app(int app = -1);

int get_current_thread();

process_t *get_current_process();

int process_get_free();

void process_runner(void *args);

void process_launch(char *filename);

#endif
