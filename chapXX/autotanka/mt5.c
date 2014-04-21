
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
#include <sys/stat.h>
#include <time.h>
#include <udis86.h>

int check_eax_0(int pid, struct user_regs_struct *pregs)
{
	if(pregs->eax == 0)
		return 1;
	return 0;
}

int check_eax_1(int pid, struct user_regs_struct *pregs)
{
	if(pregs->eax == 4)
		return 1;
	return 0;
}

int check_ebx_0(int pid, struct user_regs_struct *pregs)
{
	if(check_eax_1(pid, pregs) && pregs->ebx == 0)
		return 1;
	return 0;
}

int check_ebx_1(int pid, struct user_regs_struct *pregs)
{
	if(check_eax_1(pid, pregs) && pregs->ebx == 1)
		return 1;
	return 0;
}

int check_edx_0(int pid, struct user_regs_struct *pregs)
{
	if(check_ebx_1(pid, pregs) && pregs->edx == 0)
		return 1;
	return 0;
}

int check_edx_1(int pid, struct user_regs_struct *pregs)
{
	if(check_ebx_1(pid, pregs) && pregs->edx == 4)
		return 1;
	return 0;
}

unsigned long g_check_string = 0;
int read_data(int pid, unsigned long addr, unsigned char *mem, int size);

int check_ecx_0(int pid, struct user_regs_struct *pregs)
{
	unsigned long buff = 0;
	read_data(pid, pregs->ecx, (unsigned char *)&buff, 4);
	if(check_edx_1(pid, pregs) && buff == g_check_string)
		return 1;
	return 0;
}

unsigned long get_randvalue(unsigned char *rnd, int size)
{
	static int first = 1;
	
	if(first){
		srand(time(NULL));
		first = 0;
	}
	
	while(size--)
		rnd[size] = rand() % 256;
	
	return *((unsigned long *)rnd);
}

int disas(unsigned char *buff, int len)
{
	ud_t ud_obj;
	
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buff, len);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	
	if(!ud_disassemble(&ud_obj))
		return -1;
	
	if(strcmp("invalid", ud_insn_asm(&ud_obj)) == 0)
		return -1;
	
	return ud_insn_len(&ud_obj);
}

int print_codes(unsigned char *code, int size, int tanka)
{
	int i;
	for(i=0; i < size; i++){
		if(tanka){
			if(i==5 || i==12 || i==17 || i==24)
				printf("\n");
		}
		printf("%02x ", code[i]);
	}
	printf("\n");
	return 0;
}

int shift_codes(unsigned char *p, int *m, int size)
{
	int i, j;
	for(i=0; i < size; i++){
		if(p[i] == 0x90)
			continue;
		for(j=0; j < i; j++){
			if(p[j] != 0x90)
				continue;
			p[j] = p[i]; m[j] = 0;
			p[i] = 0x90; m[i] = 1;
			break;
		}
	}
	return 0;
}

unsigned char * set_memalign(int size)
{
	int r;
	unsigned char *mem = NULL;
	
	r = posix_memalign((void **)&mem, sysconf(_SC_PAGE_SIZE), size);
	if(r != 0)
		return NULL;
	mprotect(mem, size, PROT_READ | PROT_WRITE | PROT_EXEC);
	
	memset(mem, 0xcc, size);
	return mem;
}

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

int check_codes(unsigned char *d)
{
	int pos = check_prefix(d);
	
	if(d[pos+0] == 0x0f && d[pos+1] == 0x34) // sysenter
		return 1;
	
	if(d[pos+0] == 0xcd && d[pos+1] == 0x80) // int 80h
		return 1;
	
	if(d[pos+0] == 0xcc) // int3h
		return 1;
	
	return 0;
}

int sandbox(int pid, unsigned char *p, int size, int (*f)(int, struct user_regs_struct *))
{
	static int regs_flag = 0;
	static struct user_regs_struct back_regs;
	
	unsigned long rnd;
	int step_num, status, ret = 0;
	
	unsigned char zeros[256] = {0};
	unsigned char buff[16];
	
	int eip_range_size = size;
	unsigned long eip_range = (unsigned long)p;
	
	struct user_regs_struct regs;
	
	
	ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
	waitpid(pid, &status, 0);
	if(WIFEXITED(status))
		return -1;
	
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	if(regs_flag == 0){
		memcpy(&back_regs, &regs, sizeof(regs));
		regs_flag = 1;
	}else{
		memcpy(&regs, &back_regs, sizeof(regs));
	}
	
	regs.eax = get_randvalue((unsigned char *)&rnd, 4);
	regs.ecx = get_randvalue((unsigned char *)&rnd, 4);
	regs.edx = get_randvalue((unsigned char *)&rnd, 4);
	regs.ebx = get_randvalue((unsigned char *)&rnd, 4);
	regs.ebp = 0;
	regs.esi = get_randvalue((unsigned char *)&rnd, 4);
	regs.edi = get_randvalue((unsigned char *)&rnd, 4);

	regs.eflags = 0x200246;
	
	write_data(pid, regs.esp-256, zeros, 256);
	
	write_data(pid, regs.eip, p, size);
	ptrace(PTRACE_SETREGS, pid, 0, &regs);
	ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
	
	step_num = 0;
	while(step_num < 256){
		
		waitpid(pid, &status, 0);
		if(WIFEXITED(status))
			break;
		
		ptrace(PTRACE_GETREGS, pid, 0, &regs);
		read_data(pid, regs.eip, buff, 16);
		
		if(back_regs.esp < regs.esp)
			break;
		
		if(f(pid, &regs)){
			ret = 1;
			break;
		}
		
		if(check_codes(buff)) // check sysenter, int 80h, int3h
			break;
		
		// out of mem range
		if(regs.eip < eip_range || (eip_range + eip_range_size) < regs.eip)
			break;
		
		ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
		step_num += 1;
	}
	
	regs.eip = (unsigned long)p;
	ptrace(PTRACE_SETREGS, pid, 0, &regs);
	ptrace(PTRACE_SINGLESTEP, pid, 0, NULL);
	
	if(ret == 1)
		return 1;
	
	return 0;
}

int check_code(unsigned char *buff, int size)
{
	int pos = 0;
	int len = 0;
	
	while(pos < size){
		if((len = disas(buff + pos, 16)) < 1)
			return pos;
		pos += len;
	}
	
	return pos;
}

int creation(int pid, unsigned char *p, int *m, int size, int (*f)(int, struct user_regs_struct *))
{
	int cnt = 0;
	int i, pos, len, ret, ret2;
	
	unsigned char codes[256];
	unsigned char buff[256];
	
	if(256 < size)
		return -1;
	
	for(i=0; i < size; i++)
		codes[i] = p[i];
	
	while(1){
		cnt += 1;
		get_randvalue(buff, size);
		for(i=0; i < size; i++){
			if(m[i] == 0)
				buff[i] = codes[i];
		}
		if(check_code(buff, size) > size)
			continue;

		memset(p+0, 0xcc, 1024);
		memcpy(p+1, buff, size);
		
		ret = sandbox(pid, p, 1024, f);
		
		if(ret > 0){
			
			memset(p+0, 0xcc, 1024);
			memcpy(p+1, buff, size);
			
			ret = sandbox(pid, p, 1024, f);
			
			if(ret > 0){
				for(i=0; i < size; i++)
					p[i] = buff[i];
				break;
			}
		}
	}
	
	pos = 0;
	while(pos < size){
		
		unsigned char backup[16];
		
		len = disas(p + pos, 16);
		if(len < 1){
			while(pos < size)
				p[pos++] = 0x90;
			break;
		}
		
		if(m[pos] == 1){
			
			memcpy(backup, p + pos, len);
			memcpy(buff, p, size);
			
			ret = len;
			while(ret--)
				buff[pos+ret] = 0x90;
			
			memset(p+0, 0xcc, 1024);
			memcpy(p+1, buff, size);
			
			ret = sandbox(pid, p, 1024, f);
			
			memset(p+0, 0xcc, 1024);
			memcpy(p+1, buff, size);
			
			ret2= sandbox(pid, p, 1024, f);
			
			memcpy(p, buff, size);
			
			if(ret == 1 && ret2 == 1){
				memset(p + pos, 0x90, len);
				for(i=pos; i < (pos+len); i++)
					m[i] = 1;
			}else{
				memcpy(p + pos, backup, len);
				for(i=pos; i < (pos+len); i++)
					m[i] = 0;
			}
		}
		
		pos += len;
	}
	
	return cnt;
}

int get_codelen(unsigned char *p)
{
	int i = 0;
	while(p[i] != 0x90)
		i++;
	return i;
}

int write_tanka(char *name, unsigned char *data)
{
	FILE *fp;
	if((fp = fopen(name, "r+")) == NULL)
		return -1;
	fseek(fp, 0x3b4, SEEK_SET);
	fwrite(data, 1, 31, fp);
	fclose(fp);
	return 0;
}

int make_tanka(char *s)
{
	unsigned char *p;
	int pid, i, j, sub;
	int map[256];
	int num[] = {5, 12, 17, 24, 31, 0};
	
	int *mm;
	unsigned char *pp;
	
	int (*f[])(int, struct user_regs_struct *) = {
		check_eax_0,
		check_eax_1,
		check_ebx_0,
		check_ebx_1,
		check_edx_0,
		check_edx_1,
		check_ecx_0,
		NULL
	};
	
	if((p = set_memalign(1024)) == NULL){
		fprintf(stderr, "err: set_memalign\n");
		return -1;
	}
	
	if((pid = fork()) == -1){
		fprintf(stderr, "err: fork\n");
		return -1;
	}
	
	if(pid == 0){
		if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1){
			fprintf(stderr, "err: ptrace,TRACEME\n");
			return -1;
		}
		((void (*)())p)();
		return 0;
	}
	
	memset(p, 0x90, 32);
	for(i=0; i < 32; i++){
		if(i < 5)
			map[i] = 0;
		else
			map[i] = 1;
	}
	
	p[0] = 0x68;
	p[1] = s[0]; p[2] = s[1]; p[3] = s[2]; p[4] = s[3];

	pp = p;
	mm = map;
	
	print_codes(pp, num[0], 0);
	for(i=0, j=1; f[i] != NULL; i++){
		creation(pid, pp, mm, num[j], f[i]);
		memset(pp+num[j], 0x90, 32-num[j]);
		shift_codes(pp+num[j-1], mm+num[j-1], num[j]-num[j-1]);
		sub = (num[j]-num[j-1]) - get_codelen(pp+num[j-1]);
		//printf("%d: %d\n", sub, i);
		if(sub < 3){
			while(sub){
				mm[num[j]-sub] = 0;
				sub--;
			}
			print_codes(pp+num[j-1], num[j]-num[j-1], 0);
			j++;
		}
		if(num[j] == 0)
			break;
	}
	
	i = 0;
	while(num[j] != 0){
		if(i == 0){
			sub = (num[j]-num[j-1]) - get_codelen(pp+num[j-1]);
			if(sub >= 4){
				p[num[j]-sub+0] = 0xcd;
				p[num[j]-sub+1] = 0x80;
				p[num[j]-sub+2] = 0x58;
				p[num[j]-sub+3] = 0xc3;
				i = 1;
			}
		}
		print_codes(pp+num[j-1], num[j]-num[j-1], 0);
		j += 1;
	}
	
	write_tanka("tanka", pp);
	return 0;
}

int main(int argc, char *argv[], char *argp[])
{
	if(argc < 2 || strlen(argv[1]) != 4){
		fprintf(stderr, "usage\n");
		fprintf(stderr, "  $ a.out <4bytes string>\n");
		return 1;
	}
	
	g_check_string = *((unsigned long *)argv[1]);
	make_tanka(argv[1]);
	
	return 0;
}

