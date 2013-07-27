#include <stdio.h>
#include <string.h>

unsigned long get_sp(void)
{
    __asm__("movl %esp, %eax");
}

int cpy(char *str)
{
    char buff[64];
    printf("0x%08lx", get_sp() + 0x10);
    getchar();
    strcpy(buff, str);
    return 0;
}

int main(int argc, char *argv[])
{
    cpy(argv[1]);
    return 0;
}
