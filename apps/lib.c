#include "lib.h"

void (*do_syscall)(int callnum, void *arg);

void main(int argc, char **argv);

void _start(void *syscall_handle)
{
   do_syscall = syscall_handle;
   main(0,0);
}

void print(char *message)
{
   do_syscall(1,message);
}
