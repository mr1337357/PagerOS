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
  psram_start = start;
  psram_size = size;
  for(i=0;i<64;i++)
  {
    psram_allocations[i] = 0xFF;
  }
  psram_sem = xSemaphoreCreateMutex();
}

void *psram_get_blocks(int numblocks)
{
  int i;
  int j;
  for(i=0;(i+numblocks)<2048;i++)
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
for(i=0;i<2048;i++)
  {
    if(psram_allocations[i] == thread)
    {
      psram_allocations[i] = 0;
    }
  }
}