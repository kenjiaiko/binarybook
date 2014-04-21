#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <udis86.h>

int get_randvalue(unsigned char *rnd, int size)
{
	static int first = 1;
	
	if(first){
		srand(time(NULL));
		first = 0;
	}
	
	while(size--)
		rnd[size] = rand() % 256;
	
	return 0;
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

int check_tanka(unsigned char *buff)
{
	int pos  = 0;
	int flag = 0;
	
	while(pos < 31){
		
		int len = disas(buff + pos, 16);
		if(len < 1)
			return -1;
		
		pos += len;
		if(pos==5 || pos==12 || pos==17 || pos==24 || pos==31)
			flag += 1;
	}
	
	if(flag != 5)
		return -1;
	
	return 0;
}

int main(int argc, char *argv[])
{
	int i, num, cnt;
	unsigned char buff[31];
	
	if(argc < 2){
		fprintf(stderr, "$ a.out <num>\n");
		return 1;
	}
	
	num = atoi(argv[1]);
	
	cnt = 0;
	for(i=0; i < num; i++){
		get_randvalue(buff, 31);
		if(check_tanka(buff) == 0)
			cnt += 1;
	}
	
	printf("Result: %d / %d\n", cnt, num);
	return 0;
}

