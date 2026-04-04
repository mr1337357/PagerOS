#include "lib.h"

int i;
int j;
void print_loop()
{
   for(i = 0; i < 10; i ++)
   {
      print("hello world\n");
   }
}

int main(int argc, char **argv)
{
   print("hello world\n");
   print_loop();
   return 0;

}
