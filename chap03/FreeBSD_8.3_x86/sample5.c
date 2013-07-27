#include <unistd.h>

int main(void)
{
    char *data[2];
    char sh[] = "/bin/sh";

    data[0] = sh;
    data[1] = NULL;

    execve(sh, data, NULL);
    return 0;
}
