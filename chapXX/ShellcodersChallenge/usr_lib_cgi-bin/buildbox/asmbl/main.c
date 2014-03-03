
//
// # gcc assembl.c disasm.c asmserv.c main.c -o asmbl.cgi -lm
// 

#define STRICT
#define MAINPROG


#define MAX_BYTE_LENGTH    64


#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <float.h>
#pragma hdrstop

#include "disasm.h"

int html(char *s)
{
	static int flag = 0;

	if(flag == 0){
		flag = 1;
		printf("Content-Type:text/html\r\n");
		printf("\r\n");
	}

	printf("%s", s);
	return 0;
}

unsigned char decode_byte(char *s)
{
	char x[4];
	x[0] = *(s+1);
	x[1] = *(s+2);
	x[2] = '\0';
	return (unsigned char)strtoul(x, NULL, 16);
}

int url_decode(char *s, char *d)
{
	char *h = d;
	for( ; *s != '\0'; s++, d++){
		switch(*s)
		{
		case '%':
			*d = (char)(decode_byte(s) & 0x7F);
			s += 2;
			break;
		case '+':
			*d = ' ';
			break;
		default:
			*d = *s;
		}
	}
	*d = '\0';
	return (int)(d - h);
}

int check_len(char *s)
{
	int i;
	char c;

	for(i=0; s[i] != '\0'; i++){
		c = s[i];
		if(c == '%')
			continue;
		if('A' <= c && c <= 'Z')
			continue;
		if('a' <= c && c <= 'z')
			continue;
		if('0' <= c && c <= '9')
			continue;
		break;
	}

	s[i] = '\0';
	return i;
}

int str2bin(char *s, unsigned char *p, int p_size)
{
	int i, j;
	for(i=0, j=0; s[i] == '%'; i+=3, j++){
		if(j >= p_size)
			break;
		p[j] = decode_byte(s + i);
	}
	return j;
}

int main(void)
{
	int i, len = 0;
	char assembl[256] = {0};
	unsigned char bin[256] = {0};
	char errtext[TEXTLEN];

	char *q = getenv("QUERY_STRING");
	if(q == NULL)
		return html("error: getenv");

	len = strlen(q);
	if(len <= 0)
		return html("error: strlen");

	if(*q == '%'){
		// disas
		t_disasm da;
		len = check_len(q);
		if(len == 0 || (len % 3) != 0)
			return html("error: check_len");
		len = str2bin(q, bin, sizeof(bin));
		if(len > MAX_BYTE_LENGTH)
			return html("error: max byte length");
		Disasm(bin, len, 0, &da, DISASM_CODE);
		html(da.result);
	}else{
		// asmbl
		t_asmmodel am;
		len = url_decode(q, assembl);
		if(len > MAX_BYTE_LENGTH)
			return html("error: max byte length");
		len = Assemble(assembl, 0, &am, 0, 0, errtext);
		if(len <= 0)
			return html(errtext);
		html("");
		for(i=0; i < len; i++){
			printf("%02x ", am.code[i] & 0xFF);
		}
	}
	return 0;
}

