#include <stdint.h>

#define PSRAM_BLOCKSIZE 4096

void psram_init(uint32_t start, uint32_t size);
void *psram_get_blocks(int numblocks);
void psram_thread_killed(int thread);