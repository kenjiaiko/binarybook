#include <stdio.h>
#include <udis86.h>

int disas(unsigned char *buff, int len)
{
	ud_t ud_obj;
	
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buff, len);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	
	while(ud_disassemble(&ud_obj)){
		printf("%02d: %s\n", 
			ud_insn_len(&ud_obj), 
			ud_insn_asm(&ud_obj));
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	unsigned char code[] = {
		0x90,       // nop
		0x8b, 0xd4, // mov edx,esp
		0xcc,       // int3h
	};
	disas(code, 4);
	return 0;
}

