#ifndef __OS_PROCESS_H__
#define __OS_PROCESS_H__
#include "FreeRTOS.h"

void process_init();

int get_current_thread();

int process_get_free();

void process_runner(void *args);

void process_launch(char *filename);

#endif