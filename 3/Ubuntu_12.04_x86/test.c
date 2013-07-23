// $ gcc -S test.c
#include <stdio.h>
#include <string.h>
unsigned long get_sp(void)
{
  __asm__("movl %esp, %eax");
}
int main(int argc, char *argv[])
{
  char buff[16];
  printf("0x%08lx", get_sp());
  getchar();
  strcpy(buff, argv[1]);
  return 0;
}
