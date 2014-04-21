#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

int main(int argc, char *argv[], char *argp[])
{
	int status, pid;
	int step_num = 0;
	struct user_regs_struct regs;
	
	switch(pid = fork())
	{
	case 0:
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1){
			fprintf(stderr, "err: ptrace,TRACEME\n");
			return -1;
		}
		__asm("int3          ");
		__asm("nop           ");
		__asm("mov %esp, %edx");
		__asm("int3          ");
		return 0;
	case -1:
		fprintf(stderr, "err: fork\n");
		return -1;
	default:
		break;
	}
	
	while(step_num < 3){
		unsigned long data = 0;
		waitpid(pid, &status, 0);
		if(WIFEXITED(status))
			break;
		ptrace(PTRACE_GETREGS, pid, 0, &regs);
		data = ptrace(PTRACE_PEEKDATA, pid, regs.eip, NULL);
		printf("%08lx: ", regs.eip);
		if(step_num == 0)
			printf("%08lx\n", data & 0xFF);
		if(step_num == 1)
			printf("%08lx\n", data & 0xFFFF);
		if(step_num == 2)
			printf("%08lx\n", data & 0xFF);
		ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
		step_num++;
	}
	
	return 0;
}
