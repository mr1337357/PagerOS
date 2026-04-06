#include <Arduino.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "psram_alloc.h"
#include "esp32-hal-psram.h"

struct alloc_meta_struct
{
  struct alloc_meta_struct *next_alloc;
  uint32_t thread_id;
  uint8_t buffer[];
};
typedef struct alloc_meta_struct alloc_meta;

alloc_meta *alloc_head;

int get_current_thread();

void psram_init()
{
  alloc_head = (alloc_meta *)malloc(8);
  alloc_head->next_alloc = 0;
  alloc_head->thread_id = 0;
}

void *psram_malloc(int size)
{
  alloc_meta *list;
  alloc_meta *block;
  block = (alloc_meta *)ps_malloc(8 + size);
  Serial.printf("malloc size %d\n",size);
  block->next_alloc = 0;
  block->thread_id = get_current_thread();
  for(list = alloc_head; list->next_alloc; list = list->next_alloc)
  {

  }
  list->next_alloc = block;
  return block->buffer;
}

void psram_free(void *address)
{
  uint8_t *fudge_factor;
  alloc_meta *list;
  alloc_meta *block;
  fudge_factor = (uint8_t *)address;
  block = (alloc_meta *)&fudge_factor[-8];
  for(list = alloc_head; list->next_alloc; list = list->next_alloc)
  {
    if(list->next_alloc == block)
    {
      list->next_alloc = block->next_alloc;
      free(block);
      break;
    }
  }
}

void psram_thread_killed(int thread)
{
  int pid = get_current_thread();
  alloc_meta *list;
  alloc_meta *temp;
  for(list = alloc_head; list->next_alloc; list = list->next_alloc)
  {
    if(list->next_alloc)
    {
      if(list->next_alloc->thread_id == pid)
      {
        temp = list->next_alloc;
        list->next_alloc = list->next_alloc->next_alloc;
        free(temp);
      }
    }
  }
}