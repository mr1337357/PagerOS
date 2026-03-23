#include "esp_intr_alloc.h"

#include "hal.h"

intr_handle_t syscall_handle;
int syscalled = 0;
void sw_intr(void *arg)
{
  int i;
  uint32_t *stack_climb = (uint32_t *)&i;
  Serial.printf("start stack dump\n");
  for(i=0;i<16;i++)
  {
    Serial.printf("stack %d %08X\n",i , stack_climb[i]);
  }
  syscalled += 1;
}

void do_syscall(int callnum, void *arg)
{
  int __interrupt = 1 << 7;
  volatile int call = callnum;
  volatile int aarg = (int)arg;
  volatile int a = 0;
  //__asm__ __volatile__("s32i %0 ")
  __asm__ __volatile__("wsr.intset %0" :: "a"(__interrupt):"memory");
  while(a == 0);
}

void setup() {
  esp_err_t status;
  Serial.begin(115200);
  // put your setup code here, to run once:
  hal_init();
  tft.fillScreen(0);
  Serial.printf("start\n");
  delay(1000);
  status = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, sw_intr, 0, &syscall_handle);
  Serial.printf("error %d\n",status);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.printf("loop %d\n", syscalled);
  tft.write("asdf");
  //xt_set_intset(1 << 7);
  delay(100);
  do_syscall(1,(void *)0x12345678);
  delay(100);
  do_syscall(2,0);
  delay(1000);
  while(1);
}
