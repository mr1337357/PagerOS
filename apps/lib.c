#include <stdint.h>

#include "lib.h"

typedef struct
{
  int op;
  int fd;
  int len;
  uint8_t *buffer;
} fileop;

int strlen(char *msg)
{
   int len;
   for(len=0;*msg++;len++);
   return len;
}

int (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(void *syscall_handle)
{
   struct { int callnum; void (*handler)(int, void *);} eventcall = { 0, event_handler};
   do_syscall = syscall_handle;
   do_syscall(0, &eventcall);
   main(0,0);
}

int write(int fd, char *buff, int len)
{
   fileop fop;
   fop.op = 1;
   fop.fd = fd;
   fop.buffer = buff;
   fop.len = len;
   return do_syscall(1,&fop);
}

int read(int fd, char *buff, int len)
{
   fileop fop;
   fop.op = 0;
   fop.fd = fd;
   fop.buffer = buff;
   fop.len = len;
   return do_syscall(1,&fop);
}

void print(char *message)
{
   write(0,message,strlen(message));
}

void delay(int ms)
{
   struct { int callnum; int ms; } delaycall = { 1, ms };
   do_syscall(0, &delaycall);
}

__attribute((weak)) void event_handler(int event, void *arg)
{
   (void) event;
   (void) arg;
}
