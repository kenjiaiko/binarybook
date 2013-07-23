// $ gcc test00.c -o test00
#include <stdio.h>
#include <stdlib.h>
unsigned long get_sp(void)
{
  __asm__("movl %esp, %eax");
}
int main(void)
{
  printf("malloc: %p\n", malloc(16));
  printf(" stack: 0x%lx\n", get_sp());
  return 0;
}

