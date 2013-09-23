// ----------------------------------------------------------------------
// on Ubuntu
//   $ gcc assembl.c disasm.c asmserv.c main.c -o progtest -lm
// ----------------------------------------------------------------------

// Free Disassembler and Assembler -- Demo program
//
// Copyright (C) 2001 Oleh Yuschuk
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#define MAINPROG                       // Place all unique variables here

#include <stdio.h>
//#include <string.h>
#include <ctype.h>
//#include <dir.h>
#include <math.h>
#include <float.h>
#pragma hdrstop

#include "disasm.h"

void main(void) {                      // Old form. So what?
  int i,j,n;
  ulong l;
  char *pasm;
  t_disasm da;
  t_asmmodel am;
  char s[TEXTLEN],errtext[TEXTLEN];

  // Demonstration of Disassembler.
  printf("Disassembler:\n");
  
  // Quickly determine size of command.
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00\x11\x22\x33\x44\x55\x66",
    10,0x400000,&da,DISASM_SIZE);
  printf("Size of command = %i bytes\n",l);

  // ADD [475AE0],1 MASM mode, lowercase, don't show default segment
  ideal=0; lowercase=1; putdefseg=0;
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00",
    10,0x400000,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   (MASM)\n",l,da.dump,da.result);

  // ADD [475AE0],1 IDEAL mode, uppercase, show default segment
  ideal=1; lowercase=0; putdefseg=1;
  l=Disasm("\x81\x05\xE0\x5A\x47\x00\x01\x00\x00\x00",
    10,0x400000,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   (IDEAL)\n",l,da.dump,da.result);

  // CALL 45187C
  l=Disasm("\xE8\x1F\x14\x00\x00",
    5,0x450458,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   jmpconst=%08X\n",l,da.dump,da.result,da.jmpconst);

  // JNZ 450517
  l=Disasm("\x75\x72",
    2,0x4504A3,&da,DISASM_CODE);
  printf("%3i  %-24s  %-24s   jmpconst=%08X\n",l,da.dump,da.result,da.jmpconst);

  // Demonstration of Assembler.
  printf("\nAssembler:\n");

  // Assemble one of the commands above. First try form with 32-bit immediate.
  pasm="ADD [DWORD 475AE0],1";
  printf("%s:\n",pasm);
  j=Assemble(pasm,0x400000,&am,0,0,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.code[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);

  // Then variant with 8-bit immediate constant.
  j=Assemble(pasm,0x400000,&am,0,2,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.code[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);

  // Error, unable to determine size of operands.
  pasm="MOV [475AE0],1";
  printf("%s:\n",pasm);
  j=Assemble(pasm,0x400000,&am,0,4,errtext);
  n=sprintf(s,"%3i  ",j);
  for (i=0; i<j; i++) n+=sprintf(s+n,"%02X ",am.code[i]);
  if (j<=0) sprintf(s+n,"  error=\"%s\"",errtext);
  printf("%s\n",s);
};
