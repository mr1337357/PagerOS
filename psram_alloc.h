#ifndef __PSRAM_ALLOC_H__
#define __PSRAM_ALLOC_H__

#include <stdint.h>

void psram_init();

void *psram_malloc(int size);

void psram_free(void *address);

void psram_thread_killed(int thread);

#endif