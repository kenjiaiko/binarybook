//
// gcc disas.c -o disas.cgi -ludis86
//
#include <stdio.h>
#include <stdlib.h>

#include "udis86.h"

int main(void)
{
    ud_t ud_obj;
    char x[4];
    unsigned char buff[256];
    int i, j;

    printf("Content-Type: text/html\r\n");
    printf("\r\n");

    char *qs = getenv("QUERY_STRING");
    if(qs == NULL)
        return 1;

    for(i=0, j=0; qs[i] == '%'; i+=3, j++){
        if(j >= sizeof(buff))
            break;
        x[0] = *(qs+i+1);
        x[1] = *(qs+i+2);
        x[2] = '\0';
        buff[j] = (unsigned char)strtoul(x, NULL, 16);
    }

    ud_init(&ud_obj);
    ud_set_input_buffer(&ud_obj, buff, j);
    ud_set_mode(&ud_obj, 32);
    ud_set_syntax(&ud_obj, UD_SYN_INTEL);

    while(ud_disassemble(&ud_obj)){
        //printf("%d:%s", ud_insn_len(&ud_obj), ud_insn_asm(&ud_obj));
        printf("%10s: %s\n", ud_insn_hex(&ud_obj), ud_insn_asm(&ud_obj));
    }

    return 0;
}

