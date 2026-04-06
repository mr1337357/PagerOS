#include <stdint.h>
#include "FreeRTOS.h"
#include "psram_alloc.h"
//#include "semphr.h"

int get_current_thread();

uint32_t psram_start = 0;
uint32_t psram_size = 0;

uint8_t psram_allocations[2048];

SemaphoreHandle_t psram_sem;

void psram_init(uint32_t start, uint32_t size)
{
  int i;
  uint8_t *psram_ptr = (uint8_t *)start;
  psram_start = start;
  psram_size = size / 4096;
  for(i=0;i<64;i++)
  {
    psram_allocations[i] = 0xFF;
  }
  for(i=0;i<size;i++)
  {
    psram_ptr[i] = 0x55;
  }
  psram_sem = xSemaphoreCreateMutex();
}

void *psram_get_blocks(int numblocks)
{
  int i;
  int j;
  for(i=0;(i+numblocks)<psram_size;i++)
  {
    if(psram_allocations[i] == 0xFF)
    {
      for(j=1;j<numblocks;j++)
      {
        if(psram_allocations[i+j] != 0xFF)
        {
          break;
        }
      }
      if(j == numblocks)
      {
        for(j=0;j<numblocks;j++)
        {
          psram_allocations[i+j] = get_current_thread();
        }
        return (void *)(psram_start + (i * PSRAM_BLOCKSIZE));
      }
    }
  }
  return 0;
}

void psram_free_blocks(void *address)
{

}

void psram_thread_killed(int thread)
{
int i;
for(i=0;i<psram_size;i++)
  {
    if(psram_allocations[i] == thread)
    {
      psram_allocations[i] = 0;
    }
  }
}