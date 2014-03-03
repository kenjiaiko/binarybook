
// ------------------------------------------------------------------------
// sandbox.c
// $ gcc -Wall sandbox.c -o sb.cgi -ludis86
// ------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <errno.h>
#include <sys/mman.h>

#include <udis86.h>

int read_data(int pid, unsigned long addr, unsigned char *mem, int size)
{
	int i, n;
	unsigned long *out = (unsigned long *)mem;

	n = size / 4;
	
	for(i=0; i < n; i++){
		errno = 0;
		unsigned long data = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		addr += 4;
		if(errno != 0)
			return -1;
		*out++ = data;
	}

	if((size % 4) > 0){
		errno = 0;
		unsigned long data = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		unsigned char *ptr = (unsigned char *)&data;
		unsigned char *buff = (unsigned char *)out;
		if(errno != 0)
			return -1;
		switch(size % 4){
			case 3: buff[2] = ptr[2];
			case 2: buff[1] = ptr[1];
			case 1: buff[0] = ptr[0];
		}
	}
	
	return 0;
}

int write_data(int pid, unsigned long addr, unsigned char *mem, int size)
{
	int i, n;
	unsigned long *out = (unsigned long *)mem;

	n = size / 4;

	for(i=0; i < n; i++){
		errno = 0;
		unsigned long data = *out++;
		ptrace(PTRACE_POKEDATA, pid, addr, data);
		addr += 4;
		if(errno != 0)
			return -1;
	}

	if((size % 4) > 0){
		unsigned char buff[4];
		unsigned char *data = (unsigned char *)out;
		if(read_data(pid, addr, buff, 4) == -1)
			return -1;
		switch(size % 4){
			case 3: buff[2] = data[2];
			case 2: buff[1] = data[1];
			case 1: buff[0] = data[0];
		}
		write_data(pid, addr, buff, 4);
	}
	
	return 0;
}

unsigned char * get_alignmem(int size)
{
	int r;
	unsigned char *mem = NULL;

	r = posix_memalign((void **)&mem, sysconf(_SC_PAGE_SIZE), size);
	if(r != 0)
		return NULL;

	mprotect(mem, size, PROT_READ | PROT_WRITE | PROT_EXEC);
	
	return mem;
}

int str2bin(char *s, unsigned char *p, int p_size)
{
	int i, j;
	int slen = strlen(s);

	if((slen % 3) != 0)
		return -1;

	for(i=0, j=0; s[i] == '%'; i+=3, j++){
		char x[4];
		if(j >= p_size)
			break;
		x[0] = *(s+i+1);
		x[1] = *(s+i+2);
		x[2] = '\0';
		p[j] = (unsigned char)strtoul(x, NULL, 16);
	}

	return 0;
}

void target(char *s)
{
	unsigned char *p;
	
	if(s == NULL)
		return;

	p = get_alignmem(1024);
	if(p == NULL)
		return;
	memset(p, 0xcc, 1024);

	if(str2bin(s, p+1, 64) == -1)
		return;

	if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		return;
	
	// initialize
	__asm("xorl %eax, %eax");
	__asm("xorl %ecx, %ecx");
	__asm("xorl %edx, %edx");
	__asm("xorl %ebx, %ebx");
	__asm("xorl %esi, %esi");
	__asm("xorl %edi, %edi");
	
	// goto there
	((void (*)())p)();
}

int check_prefix(unsigned char *d)
{
	int i, j;
	unsigned char prefix[] = {
		0x2e, 0x36, 0x3e, 0x26, 0x64, 0x65,
		0x66,
		0x67,
		0xf3, 0xf2,
		0xf0,
	};
	for(i=0; i < 16; i++){
		int fflg = 0;
		for(j=0; j < sizeof(prefix); j++){
			if(d[i] == prefix[j]){
				fflg = 1;
				break;
			}
		}
		if(fflg == 0)
			break;
	}
	return i;
}

int check_sysenter(unsigned char *d)
{
	int pos = 0;
	// for deleting prefix
	//pos = check_prefix(d);
	// sysenter
	if(d[pos+0] == 0x0f && d[pos+1] == 0x34)
		return 1;
	return 0;
}

int check_int80h(unsigned char *d)
{
	int pos = 0;
	// for deleting prefix
	//pos = check_prefix(d);
	// int 80h
	if(d[pos+0] == 0xcd && d[pos+1] == 0x80)
		return 1;
	return 0;
}

int check_int3(unsigned char *d)
{
	int pos = 0;
	// for deleting prefix
	//pos = check_prefix(d);
	// int3
	if(d[pos+0] == 0xcc)
		return 1;
	return 0;
}

int check_sys_write(int pid, struct user_regs_struct *pr, char *buff, int size)
{
	// write syscall num is 4
	if(pr->eax != 0x04)
		return 0;

	// stdout
	if(pr->ebx != 1)
		return 0;

	// copying
	if(pr->edx < size)
		size = pr->edx;
	else
		size = size - 1;
	read_data(pid, pr->ecx, (unsigned char *)buff, size);
	buff[size] = '\0';
	
	return 1;
}

int check_00(unsigned char *d, int len)
{
	int i;
	for(i=0; i < len; i++){
		if(d[i] == 0x00)
			return 1;
	}
	return 0;
}

int check_status(int pid)
{
	static int eip_range_size = 1024 - 1;
	static unsigned long eip_range = 0;

	ud_t ud_obj;
	unsigned char buff[16];
	struct user_regs_struct regs;
	int i, len;

	ptrace(PTRACE_GETREGS, pid, 0, &regs);

	if(eip_range == 0)
		eip_range = regs.eip - 1;

	read_data(pid, regs.eip, buff, 16);

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buff, sizeof(buff));
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	if(!ud_disassemble(&ud_obj))
		return -1;

	len = ud_insn_len(&ud_obj);

	printf("eax=%08lx, ecx=%08lx, edx=%08lx, ebx=%08lx\n",
		regs.eax, regs.ecx, regs.edx, regs.ebx);
	printf("esi=%08lx, edi=%08lx, esp=%08lx, ebp=%08lx\n",
		regs.esi, regs.edi, regs.esp, regs.ebp);
	printf("eip=%08lx, eflags=%08lx\n", regs.eip, regs.eflags);
	
	printf("\n");

	printf("eip: ");
	for(i=0; i < len; i++)
		printf("%02x ", buff[i]);
	printf("( %s )\n", ud_insn_asm(&ud_obj));

	printf("\n");

	// check system call
	if(check_int80h(buff) || check_sysenter(buff)){
		char str[64];
		if(check_sys_write(pid, &regs, str, sizeof(str))){
			printf("output:\n");
			printf("%s", str);
			if(strcmp(str, "HelloASM") == 0){
				printf("\nPassword: MagicaMadoca\n");
			}
		}
		return -1;
	}

	// found int3
	if(check_int3(buff))
		return -1;
	
	// out of mem range
	if(regs.eip < eip_range || (eip_range + eip_range_size) < regs.eip)
		return -1;

	// found 0x00
	if(check_00(buff, len))
		return -1;

	return 0;
}

void control(int pid)
{
	int status;
	int step_num = 0;
	
	while(step_num < 256){
		waitpid(pid, &status, 0);
		if(WIFEXITED(status))
			break;
		if(check_status(pid))
			break;
		ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
		step_num++;
	}
	
	ptrace(PTRACE_KILL, pid, 0, NULL);
	return;
}

int exec_prog(char *s)
{
	int pid;
	switch(pid = fork())
	{
	case 0:
		target(s);
		break;
	case -1:
		fprintf(stderr, "err: fork\n");
		break;
	default:
		control(pid);
		break;
	}
	return 0;
}

int main(int argc, char *argv[], char *argp[])
{
	char *qs = getenv("QUERY_STRING");
	
	if(qs != NULL){
		// cgi
		printf("Content-Type: text/plain\r\n");
		printf("\r\n");
		exec_prog(getenv("QUERY_STRING"));
	}else{
		// cmd
		exec_prog(argv[1]);
	}

	return 0;
}

