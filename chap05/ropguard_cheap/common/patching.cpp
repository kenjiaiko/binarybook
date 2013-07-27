
#include <stdio.h>
#include <windows.h>

#include "ropsettings.h"
#include "ropcheck.h"
#include "createprocess.h"

#include "debug.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

// contains informations on the x86 instructions
struct x86opcode {
	unsigned char opcode;
	char type;        // instruction type, 1:register or memoty operands (mod/reg/r/m field), 2:prefix
	char jump;        // determins if it is a call or jump instruction, 
	                  // 0:no, 1:call(or other functions to be ignored), 2:conditional jump, 3:unconditional jump
	char argSize;     // size of the function's arguments
	char stackChange; // stack increment/decrement done by the instruction
};

// info on all x86 instructions
x86opcode opcodes[] = {
	{0x00,	1,	0,	0,	0},	//add
	{0x01,	1,	0,	0,	0},	//add
	{0x02,	1,	0,	0,	0},	//add
	{0x03,	1,	0,	0,	0},	//add
	{0x04,	0,	0,	1,	0},	//add
	{0x05,	0,	0,	4,	0},	//add
	{0x06,	0,	0,	0,	-1},//push
	{0x07,	0,	0,	0,	+1},//pop
	{0x08,	1,	0,	0,	0},	//or
	{0x09,	1,	0,	0,	0},	//or
	{0x0A,	1,	0,	0,	0},	//or
	{0x0B,	1,	0,	0,	0},	//or
	{0x0C,	0,	0,	1,	0},	//or
	{0x0D,	0,	0,	4,	0},	//or
	{0x0E,	0,	0,	0,	-1},//push
	{0x0F,	0,	0,	0,	0},	//two-byte instruction prefix

	{0x10,	1,	0,	0,	0},	//adc
	{0x11,	1,	0,	0,	0},	//adc
	{0x12,	1,	0,	0,	0},	//adc
	{0x13,	1,	0,	0,	0},	//adc
	{0x14,	0,	0,	1,	0},	//adc
	{0x15,	0,	0,	4,	0},	//adc
	{0x16,	0,	0,	0,	-1},//push
	{0x17,	0,	0,	0,	+1},//pop
	{0x18,	1,	0,	0,	0},	//sbb
	{0x19,	1,	0,	0,	0},	//sbb
	{0x1A,	1,	0,	0,	0},	//sbb
	{0x1B,	1,	0,	0,	0},	//sbb
	{0x1C,	0,	0,	1,	0},	//sbb
	{0x1D,	0,	0,	4,	0},	//sbb
	{0x1E,	0,	0,	0,	-1},//push
	{0x1F,	0,	0,	0,	+1},//pop

	{0x20,	1,	0,	0,	0},	//and
	{0x21,	1,	0,	0,	0},	//and
	{0x22,	1,	0,	0,	0},	//and
	{0x23,	1,	0,	0,	0},	//and
	{0x24,	0,	0,	1,	0},	//and
	{0x25,	0,	0,	4,	0},	//and
	{0x26,	2,	0,	0,	0},	//ES segment override prefix
	{0x27,	0,	0,	0,	0},	//Decimal Adjust AL after Addition
	{0x28,	1,	0,	0,	0},	//sub
	{0x29,	1,	0,	0,	0},	//sub
	{0x2A,	1,	0,	0,	0},	//sub
	{0x2B,	1,	0,	0,	0},	//sub
	{0x2C,	0,	0,	1,	0},	//sub
	{0x2D,	0,	0,	4,	0},	//sub
	{0x2E,	2,	0,	0,	0},	//CS segment override prefix
	{0x2F,	0,	0,	0,	0},	//Decimal Adjust AL after Subtraction

	{0x30,	1,	0,	0,	0},	//xor
	{0x31,	1,	0,	0,	0},	//xor
	{0x32,	1,	0,	0,	0},	//xor
	{0x33,	1,	0,	0,	0},	//xor
	{0x34,	0,	0,	1,	0},	//xor
	{0x35,	0,	0,	4,	0},	//xor
	{0x36,	2,	0,	0,	0},	//SS segment override prefix
	{0x37,	0,	0,	0,	0},	//aaa
	{0x38,	1,	0,	0,	0},	//cmp
	{0x39,	1,	0,	0,	0},	//cmp
	{0x3A,	1,	0,	0,	0},	//cmp
	{0x3B,	1,	0,	0,	0},	//cmp
	{0x3C,	0,	0,	1,	0},	//cmp
	{0x3D,	0,	0,	4,	0},	//cmp
	{0x3E,	2,	0,	0,	0},	//DS segment override prefix
	{0x3F,	0,	0,	0,	0},	//aas

	{0x40,	0,	0,	0,	0},	//inc
	{0x41,	0,	0,	0,	0},	//inc
	{0x42,	0,	0,	0,	0},	//inc
	{0x43,	0,	0,	0,	0},	//inc
	{0x44,	0,	0,	0,	0},	//inc
	{0x45,	0,	0,	0,	0},	//inc
	{0x46,	0,	0,	0,	0},	//inc
	{0x47,	0,	0,	0,	0},	//inc
	{0x48,	0,	0,	0,	0},	//dec
	{0x49,	0,	0,	0,	0},	//dec
	{0x4A,	0,	0,	0,	0},	//dec
	{0x4B,	0,	0,	0,	0},	//dec
	{0x4C,	0,	0,	0,	0},	//dec
	{0x4D,	0,	0,	0,	0},	//dec
	{0x4E,	0,	0,	0,	0},	//dec
	{0x4F,	0,	0,	0,	0},	//dec

	{0x50,	0,	0,	0,	-1},//push
	{0x51,	0,	0,	0,	-1},//push
	{0x52,	0,	0,	0,	-1},//push
	{0x53,	0,	0,	0,	-1},//push
	{0x54,	0,	0,	0,	-1},//push
	{0x55,	0,	0,	0,	-1},//push
	{0x56,	0,	0,	0,	-1},//push
	{0x57,	0,	0,	0,	-1},//push
	{0x58,	0,	0,	0,	+1},//pop
	{0x59,	0,	0,	0,	+1},//pop
	{0x5A,	0,	0,	0,	+1},//pop
	{0x5B,	0,	0,	0,	+1},//pop
	{0x5C,	0,	0,	0,	+1},//pop
	{0x5D,	0,	0,	0,	+1},//pop
	{0x5E,	0,	0,	0,	+1},//pop
	{0x5F,	0,	0,	0,	+1},//pop

	{0x60,	0,	0,	0,	-8},//pusha
	{0x61,	0,	0,	0,	+8},//popa
	{0x62,	1,	0,	0,	0},	//BOUND
	{0x63,	1,	0,	0,	0},	//ARPL
	{0x64,	2,	0,	0,	0},	//prefix
	{0x65,	2,	0,	0,	0},	//prefix
	{0x66,	2,	0,	0,	0},	//prefix
	{0x67,	2,	0,	0,	0},	//prefix
	{0x68,	0,	0,	4,	-1},//push
	{0x69,	1,	0,	4,	0},	//imul
	{0x6A,	0,	0,	1,	-1},//push
	{0x6B,	1,	0,	1,	0},	//imul
	{0x6C,	0,	0,	0,	0},	//ins
	{0x6D,	0,	0,	0,	0},	//ins
	{0x6E,	0,	0,	0,	0},	//outs
	{0x6F,	0,	0,	0,	0},	//outs

	{0x70,	0,	2,	1,	0},	//jump
	{0x71,	0,	2,	1,	0},	//jump
	{0x72,	0,	2,	1,	0},	//jump
	{0x73,	0,	2,	1,	0},	//jump
	{0x74,	0,	2,	1,	0},	//jump
	{0x75,	0,	2,	1,	0},	//jump
	{0x76,	0,	2,	1,	0},	//jump
	{0x77,	0,	2,	1,	0},	//jump
	{0x78,	0,	2,	1,	0},	//jump
	{0x79,	0,	2,	1,	0},	//jump
	{0x7A,	0,	2,	1,	0},	//jump
	{0x7B,	0,	2,	1,	0},	//jump
	{0x7C,	0,	2,	1,	0},	//jump
	{0x7D,	0,	2,	1,	0},	//jump
	{0x7E,	0,	2,	1,	0},	//jump
	{0x7F,	0,	2,	1,	0},	//jump

	{0x80,	1,	0,	1,	0},
	{0x81,	1,	0,	4,	0},
	{0x82,	1,	0,	1,	0},
	{0x83,	1,	0,	1,	0},
	{0x84,	1,	0,	0,	0},
	{0x85,	1,	0,	0,	0},
	{0x86,	1,	0,	0,	0},
	{0x87,	1,	0,	0,	0},
	{0x88,	1,	0,	0,	0}, //mov
	{0x89,	1,	0,	0,	0},
	{0x8A,	1,	0,	0,	0},
	{0x8B,	1,	0,	0,	0},
	{0x8C,	1,	0,	0,	0},
	{0x8D,	1,	0,	0,	0},
	{0x8E,	1,	0,	0,	0},
	{0x8F,	1,	0,	0,	+1},//pop

	{0x90,	0,	0,	0,	0}, //nop
	{0x91,	0,	0,	0,	0},
	{0x92,	0,	0,	0,	0},
	{0x93,	0,	0,	0,	0},
	{0x94,	0,	0,	0,	0},
	{0x95,	0,	0,	0,	0},
	{0x96,	0,	0,	0,	0},
	{0x97,	0,	0,	0,	0},
	{0x98,	0,	0,	0,	0}, //CBW
	{0x99,	0,	0,	0,	0},
	{0x9A,	0,	1,	6,	0}, //callf
	{0x9B,	0,	0,	0,	0},
	{0x9C,	0,	0,	0,	-1},
	{0x9D,	0,	0,	0,	+1},
	{0x9E,	0,	0,	0,	0},
	{0x9F,	0,	0,	0,	0},

	{0xA0,	0,	0,	4,	0}, //mov
	{0xA1,	0,	0,	4,	0}, //mov
	{0xA2,	0,	0,	4,	0}, //mov
	{0xA3,	0,	0,	4,	0}, //mov
	{0xA4,	0,	0,	0,	0}, 
	{0xA5,	0,	0,	0,	0}, 
	{0xA6,	0,	0,	0,	0}, 
	{0xA7,	0,	0,	0,	0}, 
	{0xA8,	0,	0,	1,	0}, //test
	{0xA9,	0,	0,	4,	0}, 
	{0xAA,	0,	0,	0,	0}, 
	{0xAB,	0,	0,	0,	0}, 
	{0xAC,	0,	0,	0,	0}, 
	{0xAD,	0,	0,	0,	0}, 
	{0xAE,	0,	0,	0,	0}, 
	{0xAF,	0,	0,	0,	0}, 

	{0xB0,	0,	0,	1,	0}, //mov
	{0xB1,	0,	0,	1,	0}, 
	{0xB2,	0,	0,	1,	0}, 
	{0xB3,	0,	0,	1,	0}, 
	{0xB4,	0,	0,	1,	0}, 
	{0xB5,	0,	0,	1,	0}, 
	{0xB6,	0,	0,	1,	0}, 
	{0xB7,	0,	0,	1,	0}, 
	{0xB8,	0,	0,	4,	0}, 
	{0xB9,	0,	0,	4,	0}, 
	{0xBA,	0,	0,	4,	0}, 
	{0xBB,	0,	0,	4,	0}, 
	{0xBC,	0,	0,	4,	0}, 
	{0xBD,	0,	0,	4,	0}, 
	{0xBE,	0,	0,	4,	0}, 
	{0xBF,	0,	0,	4,	0}, 

	{0xC0,	1,	0,	1,	0}, 
	{0xC1,	1,	0,	1,	0}, 
	{0xC2,	0,	0,	2,	0}, //retn
	{0xC3,	0,	0,	0,	0}, //retn
	{0xC4,	1,	0,	0,	0}, 
	{0xC5,	1,	0,	0,	0}, 
	{0xC6,	1,	0,	1,	0}, 
	{0xC7,	1,	0,	4,	0}, 
	{0xC8,	0,	1,	3,	0}, //enter
	{0xC9,	0,	1,	0,	0}, //leave
	{0xCA,	0,	0,	2,	0}, //retn
	{0xCB,	0,	0,	0,	0}, //retn
	{0xCC,	0,	1,	0,	0}, 
	{0xCD,	0,	1,	1,	0}, 
	{0xCE,	0,	1,	0,	0}, 
	{0xCF,	0,	1,	0,	0}, 

	{0xD0,	1,	0,	0,	0}, 
	{0xD1,	1,	0,	0,	0}, 
	{0xD2,	1,	0,	0,	0}, 
	{0xD3,	1,	0,	0,	0}, 
	{0xD4,	0,	0,	1,	0}, 
	{0xD5,	0,	0,	1,	0}, 
	{0xD6,	0,	0,	0,	0}, 
	{0xD7,	0,	0,	0,	0}, 
	{0xD8,	1,	0,	0,	0}, 
	{0xD9,	1,	0,	0,	0}, 
	{0xDA,	1,	0,	0,	0}, 
	{0xDB,	1,	0,	0,	0}, 
	{0xDC,	1,	0,	0,	0}, 
	{0xDD,	1,	0,	0,	0}, 
	{0xDE,	1,	0,	0,	0}, 
	{0xDF,	1,	0,	0,	0},

	{0xE0,	0,	2,	1,	0}, 
	{0xE1,	0,	2,	1,	0}, 
	{0xE2,	0,	2,	1,	0}, 
	{0xE3,	0,	2,	1,	0}, 
	{0xE4,	0,	0,	1,	0}, 
	{0xE5,	0,	0,	1,	0}, 
	{0xE6,	0,	0,	1,	0}, 
	{0xE7,	0,	0,	1,	0}, 
	{0xE8,	0,	1,	4,	0}, 
	{0xE9,	0,	3,	4,	0}, 
	{0xEA,	0,	3,	6,	0}, 
	{0xEB,	0,	3,	1,	0}, 
	{0xEC,	0,	0,	0,	0}, 
	{0xED,	0,	0,	0,	0}, 
	{0xEE,	0,	0,	0,	0}, 
	{0xEF,	0,	0,	0,	0},

	{0xF0,	2,	0,	0,	0}, //lock
	{0xF1,	0,	1,	0,	0},
	{0xF2,	2,	0,	0,	0},
	{0xF3,	2,	0,	0,	0},
	{0xF4,	0,	0,	0,	0},
	{0xF5,	0,	0,	0,	0},
	{0xF6,	1,	0,	0,	0},
	{0xF7,	1,	0,	0,	0},
	{0xF8,	0,	0,	0,	0},
	{0xF9,	0,	0,	0,	0},
	{0xFA,	0,	0,	0,	0},
	{0xFB,	0,	0,	0,	0},
	{0xFC,	0,	0,	0,	0},
	{0xFD,	0,	0,	0,	0},
	{0xFE,	1,	0,	0,	0},
	{0xFF,	1,	0,	0,	0},

	//2-byte opcodes
	{0x00,	1,	0,	0,	0},	
	{0x01,	1,	1,	0,	0},	
	{0x02,	1,	0,	0,	0},	
	{0x03,	1,	0,	0,	0},	
	{0x04,	0,	0,	0,	0},	
	{0x05,	0,	0,	0,	0},	
	{0x06,	0,	0,	0,	0},
	{0x07,	0,	0,	0,	0},	
	{0x08,	0,	0,	0,	0},	
	{0x09,	0,	0,	0,	0},	
	{0x0A,	0,	0,	0,	0},	
	{0x0B,	0,	0,	0,	0},	
	{0x0C,	0,	0,	0,	0},	
	{0x0D,	1,	0,	0,	0},	
	{0x0E,	1,	0,	0,	0},	
	{0x0F,	1,	0,	0,	0},	

	{0x10,	1,	0,	0,	0},	
	{0x11,	1,	0,	0,	0},	
	{0x12,	1,	0,	0,	0},	
	{0x13,	1,	0,	0,	0},	
	{0x14,	1,	0,	0,	0},	
	{0x15,	1,	0,	0,	0},	
	{0x16,	1,	0,	0,	0},
	{0x17,	1,	0,	0,	0},	
	{0x18,	1,	0,	0,	0},	
	{0x19,	1,	0,	0,	0},	
	{0x1A,	1,	0,	0,	0},	
	{0x1B,	1,	0,	0,	0},	
	{0x1C,	1,	0,	0,	0},	
	{0x1D,	1,	0,	0,	0},	
	{0x1E,	1,	0,	0,	0},	
	{0x1F,	1,	0,	0,	0},	

	{0x20,	1,	0,	0,	0},	
	{0x21,	1,	0,	0,	0},	
	{0x22,	1,	0,	0,	0},	
	{0x23,	1,	0,	0,	0},	
	{0x24,	1,	0,	0,	0},	
	{0x25,	1,	0,	0,	0},	
	{0x26,	1,	0,	0,	0},
	{0x27,	1,	0,	0,	0},	
	{0x28,	1,	0,	0,	0},	
	{0x29,	1,	0,	0,	0},	
	{0x2A,	1,	0,	0,	0},	
	{0x2B,	1,	0,	0,	0},	
	{0x2C,	1,	0,	0,	0},	
	{0x2D,	1,	0,	0,	0},	
	{0x2E,	1,	0,	0,	0},	
	{0x2F,	1,	0,	0,	0},	

	{0x30,	0,	0,	0,	0},	
	{0x31,	0,	0,	0,	0},	
	{0x32,	0,	0,	0,	0},	
	{0x33,	0,	0,	0,	0},	
	{0x34,	0,	1,	0,	0},	
	{0x35,	0,	1,	0,	0},	
	{0x36,	0,	0,	0,	0},
	{0x37,	0,	0,	0,	0},	
	{0x38,	0,	1,	0,	0},
	{0x39,	0,	1,	0,	0},
	{0x3A,	0,	1,	0,	0},
	{0x3B,	0,	1,	0,	0},
	{0x3C,	0,	1,	0,	0},
	{0x3D,	0,	1,	0,	0},
	{0x3E,	0,	1,	0,	0},
	{0x3F,	0,	1,	0,	0},

	{0x40,	1,	0,	0,	0},	
	{0x41,	1,	0,	0,	0},	
	{0x42,	1,	0,	0,	0},	
	{0x43,	1,	0,	0,	0},	
	{0x44,	1,	0,	0,	0},	
	{0x45,	1,	0,	0,	0},	
	{0x46,	1,	0,	0,	0},
	{0x47,	1,	0,	0,	0},	
	{0x48,	1,	0,	0,	0},	
	{0x49,	1,	0,	0,	0},	
	{0x4A,	1,	0,	0,	0},	
	{0x4B,	1,	0,	0,	0},	
	{0x4C,	1,	0,	0,	0},	
	{0x4D,	1,	0,	0,	0},	
	{0x4E,	1,	0,	0,	0},	
	{0x4F,	1,	0,	0,	0},	

	{0x50,	1,	0,	0,	0},	
	{0x51,	1,	0,	0,	0},	
	{0x52,	1,	0,	0,	0},	
	{0x53,	1,	0,	0,	0},	
	{0x54,	1,	0,	0,	0},	
	{0x55,	1,	0,	0,	0},	
	{0x56,	1,	0,	0,	0},
	{0x57,	1,	0,	0,	0},	
	{0x58,	1,	0,	0,	0},	
	{0x59,	1,	0,	0,	0},	
	{0x5A,	1,	0,	0,	0},	
	{0x5B,	1,	0,	0,	0},	
	{0x5C,	1,	0,	0,	0},	
	{0x5D,	1,	0,	0,	0},	
	{0x5E,	1,	0,	0,	0},	
	{0x5F,	1,	0,	0,	0},	

	{0x60,	1,	0,	0,	0},	
	{0x61,	1,	0,	0,	0},	
	{0x62,	1,	0,	0,	0},	
	{0x63,	1,	0,	0,	0},	
	{0x64,	1,	0,	0,	0},	
	{0x65,	1,	0,	0,	0},	
	{0x66,	1,	0,	0,	0},
	{0x67,	1,	0,	0,	0},	
	{0x68,	1,	0,	0,	0},	
	{0x69,	1,	0,	0,	0},	
	{0x6A,	1,	0,	0,	0},	
	{0x6B,	1,	0,	0,	0},	
	{0x6C,	1,	0,	0,	0},	
	{0x6D,	1,	0,	0,	0},	
	{0x6E,	1,	0,	0,	0},	
	{0x6F,	1,	0,	0,	0},	

	{0x70,	1,	0,	1,	0},	
	{0x71,	1,	0,	1,	0},	
	{0x72,	1,	0,	1,	0},	
	{0x73,	1,	0,	1,	0},	
	{0x74,	1,	0,	0,	0},	
	{0x75,	1,	0,	0,	0},	
	{0x76,	1,	0,	0,	0},
	{0x77,	0,	0,	0,	0},	
	{0x78,	1,	0,	0,	0},	
	{0x79,	1,	0,	0,	0},	
	{0x7A,	1,	0,	0,	0},	
	{0x7B,	1,	0,	0,	0},	
	{0x7C,	1,	0,	0,	0},	
	{0x7D,	1,	0,	0,	0},	
	{0x7E,	1,	0,	0,	0},	
	{0x7F,	1,	0,	0,	0},	

	{0x80,	0,	2,	4,	0},	
	{0x81,	0,	2,	4,	0},	
	{0x82,	0,	2,	4,	0},	
	{0x83,	0,	2,	4,	0},	
	{0x84,	0,	2,	4,	0},	
	{0x85,	0,	2,	4,	0},	
	{0x86,	0,	2,	4,	0},
	{0x87,	0,	2,	4,	0},	
	{0x88,	0,	2,	4,	0},	
	{0x89,	0,	2,	4,	0},	
	{0x8A,	0,	2,	4,	0},	
	{0x8B,	0,	2,	4,	0},	
	{0x8C,	0,	2,	4,	0},	
	{0x8D,	0,	2,	4,	0},	
	{0x8E,	0,	2,	4,	0},	
	{0x8F,	0,	2,	4,	0},	

	{0x90,	1,	0,	0,	0},	
	{0x91,	1,	0,	0,	0},	
	{0x92,	1,	0,	0,	0},	
	{0x93,	1,	0,	0,	0},	
	{0x94,	1,	0,	0,	0},	
	{0x95,	1,	0,	0,	0},	
	{0x96,	1,	0,	0,	0},
	{0x97,	1,	0,	0,	0},	
	{0x98,	1,	0,	0,	0},	
	{0x99,	1,	0,	0,	0},	
	{0x9A,	1,	0,	0,	0},	
	{0x9B,	1,	0,	0,	0},	
	{0x9C,	1,	0,	0,	0},	
	{0x9D,	1,	0,	0,	0},	
	{0x9E,	1,	0,	0,	0},	
	{0x9F,	1,	0,	0,	0},	

	{0xA0,	0,	0,	0,	-1},	
	{0xA1,	0,	0,	0,	+1},	
	{0xA2,	0,	0,	0,	0},	
	{0xA3,	1,	0,	0,	0},	
	{0xA4,	1,	0,	1,	0},	
	{0xA5,	1,	0,	0,	0},	
	{0xA6,	0,	1,	0,	0},	
	{0xA7,	0,	1,	0,	0},	
	{0xA8,	0,	0,	0,	-1},	
	{0xA9,	0,	0,	0,	+1},	
	{0xAA,	0,	0,	0,	0},	
	{0xAB,	1,	0,	0,	0},	
	{0xAC,	1,	0,	1,	0},	
	{0xAD,	1,	0,	0,	0},	
	{0xAE,	1,	0,	0,	0},	
	{0xAF,	1,	0,	0,	0},	

	{0xB0,	1,	0,	0,	0},	
	{0xB1,	1,	0,	0,	0},	
	{0xB2,	1,	0,	0,	0},	
	{0xB3,	1,	0,	0,	0},	
	{0xB4,	1,	0,	0,	0},	
	{0xB5,	1,	0,	0,	0},	
	{0xB6,	1,	0,	0,	0},
	{0xB7,	1,	0,	0,	0},	
	{0xB8,	1,	0,	0,	0},	
	{0xB9,	1,	0,	0,	0},	
	{0xBA,	1,	0,	1,	0},	
	{0xBB,	1,	0,	0,	0},	
	{0xBC,	1,	0,	0,	0},	
	{0xBD,	1,	0,	0,	0},	
	{0xBE,	1,	0,	0,	0},	
	{0xBF,	1,	0,	0,	0},	

	{0xC0,	1,	0,	0,	0},	
	{0xC1,	1,	0,	0,	0},	
	{0xC2,	1,	0,	1,	0},	
	{0xC3,	1,	0,	1,	0},	
	{0xC4,	1,	0,	1,	0},	
	{0xC5,	1,	0,	1,	0},	
	{0xC6,	1,	0,	1,	0},
	{0xC7,	1,	0,	0,	0},	
	{0xC8,	0,	0,	0,	0},	
	{0xC9,	0,	0,	0,	0},	
	{0xCA,	0,	0,	0,	0},	
	{0xCB,	0,	0,	0,	0},	
	{0xCC,	0,	0,	0,	0},	
	{0xCD,	0,	0,	0,	0},	
	{0xCE,	0,	0,	0,	0},	
	{0xCF,	0,	0,	0,	0},	

	{0xD0,	1,	0,	0,	0},	
	{0xD1,	1,	0,	0,	0},	
	{0xD2,	1,	0,	0,	0},	
	{0xD3,	1,	0,	0,	0},	
	{0xD4,	1,	0,	0,	0},	
	{0xD5,	1,	0,	0,	0},	
	{0xD6,	1,	0,	0,	0},
	{0xD7,	1,	0,	0,	0},	
	{0xD8,	1,	0,	0,	0},	
	{0xD9,	1,	0,	0,	0},	
	{0xDA,	1,	0,	0,	0},	
	{0xDB,	1,	0,	0,	0},	
	{0xDC,	1,	0,	0,	0},	
	{0xDD,	1,	0,	0,	0},	
	{0xDE,	1,	0,	0,	0},	
	{0xDF,	1,	0,	0,	0},	

	{0xE0,	1,	0,	0,	0},	
	{0xE1,	1,	0,	0,	0},	
	{0xE2,	1,	0,	0,	0},	
	{0xE3,	1,	0,	0,	0},	
	{0xE4,	1,	0,	0,	0},	
	{0xE5,	1,	0,	0,	0},	
	{0xE6,	1,	0,	0,	0},
	{0xE7,	1,	0,	0,	0},	
	{0xE8,	1,	0,	0,	0},	
	{0xE9,	1,	0,	0,	0},	
	{0xEA,	1,	0,	0,	0},	
	{0xEB,	1,	0,	0,	0},	
	{0xEC,	1,	0,	0,	0},	
	{0xED,	1,	0,	0,	0},	
	{0xEE,	1,	0,	0,	0},	
	{0xEF,	1,	0,	0,	0},	

	{0xF0,	1,	0,	0,	0},	
	{0xF1,	1,	0,	0,	0},	
	{0xF2,	1,	0,	0,	0},	
	{0xF3,	1,	0,	0,	0},	
	{0xF4,	1,	0,	0,	0},	
	{0xF5,	1,	0,	0,	0},	
	{0xF6,	1,	0,	0,	0},
	{0xF7,	1,	0,	0,	0},	
	{0xF8,	1,	0,	0,	0},	
	{0xF9,	1,	0,	0,	0},	
	{0xFA,	1,	0,	0,	0},	
	{0xFB,	1,	0,	0,	0},	
	{0xFC,	1,	0,	0,	0},	
	{0xFD,	1,	0,	0,	0},	
	{0xFE,	1,	0,	0,	0},	
	{0xFF,	0,	1,	0,	0}	
};

// decodes the instruction pointed to by eip and increases the eip accordingly
// returns 0 on failure
// instructions should be NULL (used for debugging only)
int FollowInstruction(unsigned char *instructions, unsigned long *eip)
{
	unsigned char modregrm, sib;
	unsigned short opcode = 0;
	bool operandOverridePrefix = false;
	bool addressOverridePrefix = false;
	long displacement;
	long argsize = 0;

	// handle prefixes
	while(1){
		if(opcodes[instructions[*eip]].type == 2){
			if(instructions[*eip] == 0x66)
				operandOverridePrefix = true;
			if(instructions[*eip] == 0x67)
				addressOverridePrefix = true;
			(*eip)++;
			continue;
		}
		break;
	}

	// load the opcode
	opcode = instructions[*eip];
	(*eip)++;
	if(opcode == 0x0F){
		opcode = 0x0100 + instructions[*eip];
		(*eip)++;
	}

	if(addressOverridePrefix && ((opcode == 0x00) || (opcode == 0x20)))
		return 0; // can't handle such instructions currently

	// handle register-based instructions (mod/reg/rm byte, sib byte, displacement etc)
	if(opcodes[opcode].type == 1){
		modregrm = instructions[*eip];
		(*eip)++;

		// hack, fix for test instruction
		if((opcode == 0xF6) && ((modregrm & 0x30) == 0)){
			 argsize ++;
		}else if((opcode == 0xF7) && ((modregrm & 0x30) == 0)){
			if(operandOverridePrefix)
				argsize += 2;
			else
				argsize += 4;
		}

		if((modregrm & 0xC0) == 0x40)
			displacement = 1;
		else if((modregrm & 0xC0) == 0x80){
			if(addressOverridePrefix)
				displacement = 2;
			else
				displacement = 4;
		}else if((modregrm & 0xC7) == 0x05){
			// displacement-only addressing mode
			if(addressOverridePrefix)
				displacement = 2;
			else
				displacement = 4;
		} else
			displacement = 0;	

		if(((modregrm & 0x07) == 0x04) && ((modregrm & 0xC0) != 0xC0)){
			sib = instructions[*eip];
			(*eip)++;
			if(((modregrm & 0xC0) == 0x00) && ((sib & 0x07) == 0x05))
				displacement = 4;
		}

		(*eip) += displacement;	
	}

	// increase eip by operand size
	if((opcode == 0xA0) || (opcode == 0xA1) || (opcode == 0xA2) || (opcode == 0xA3)){
		// hack, fix for certain types of mov instruction
		if(addressOverridePrefix)
			argsize += 2;
		else
			argsize += 4;
	}
	else if(operandOverridePrefix && (opcodes[opcode].argSize == 4))
		argsize += 2;
	else if(operandOverridePrefix && (opcodes[opcode].argSize == 6))
		argsize += 4;
	else
		argsize += opcodes[opcode].argSize;

	(*eip) += argsize;

	return 1;
}

// patch a critical function, replacing its prologue with the jump to patch prologue that calls ROPCheck
// moduleName:      name of the module that contains the critical function
// functionName:    name of the critical function
// originalAddress: here the original address of critical function will be stored
// patchcode:       a pointer to executable memory that will contain the patched function prologue
int PatchFunction(char *moduleName, 
				  char *functionName, 
				  unsigned long *originalAddress, 
				  unsigned char *patchcode)
{
	unsigned long functionAddress, patchHeaderEnd;
	unsigned int i;
	DWORD oldProtect, newProtect;

	// get the address of function to be patched
	functionAddress = (unsigned long)
		GetProcAddress(GetModuleHandle(moduleName), functionName);
	if(functionAddress == NULL){
		stringstream errorreport;
		errorreport << "Error: Could not get address of " << moduleName << ":" << functionName;
		WriteLog((char *)(errorreport.str().c_str()));
		return 0;
	}

	unsigned int numFunctions = GetNumGuardedFunctions();
	ROPGuardedFunction *guardedFunctions = GetGuardedFunctions();

	// don't patch same function twice
	for(i=0; i < numFunctions; i++){
		if(guardedFunctions[i].originalAddress == (functionAddress ^ ADDR_SCRAMBLE_KEY))
			return 0;
	}

	*originalAddress = functionAddress;
	patchHeaderEnd = functionAddress;

	while((patchHeaderEnd - functionAddress) < 5){
		if(!FollowInstruction(NULL, &patchHeaderEnd)){
			stringstream errorreport;
			errorreport << "Error: Could not determine function header of " << moduleName << ":" << functionName;
			WriteLog((char *)(errorreport.str().c_str()));
			return 0;			
		}
	}

	unsigned char *patchcode2 = (unsigned char *)(functionAddress);

	patchcode[0] = 0x81; // SUB ESP, PRESERVE_STACK
	patchcode[1] = 0xEC;
	*((unsigned long *)(&(patchcode[2]))) = GetROPSettings()->preserveStack;
	patchcode[6] = 0x60; // PUSHAD
	patchcode[7] = 0x54; // PUSH ESP
	patchcode[8] = 0x68; // PUSH functionAddress
	// scramble functionAddress so that it wouldn't confuse ropcheck later
	*((unsigned long *)(&(patchcode[9]))) = functionAddress ^ ADDR_SCRAMBLE_KEY;
	patchcode[13] = 0xE8; // CALL ROPCheck
	*((unsigned long *)(&(patchcode[14]))) = (unsigned long)(&ROPCheck) - (unsigned long)(&(patchcode[18]));
	patchcode[18] = 0x81; // ADD ESP, PRESERVE_STACK + space taken by PUSHAD
	patchcode[19] = 0xC4;
	*((unsigned long *)(&(patchcode[20]))) = (GetROPSettings()->preserveStack + 8 * 4);

	if(strcmp(functionName, "CreateProcessInternalW") != 0){
		// instructions from the header of function being patched
		for(i=0; i < (patchHeaderEnd - functionAddress); i++){
			patchcode[24+i] = patchcode2[i];
		}
		patchcode[24+i] = 0xE9; // jmp patchHeaderEnd
		*((unsigned long *)(&(patchcode[25+i]))) = patchHeaderEnd - (unsigned long)(&(patchcode[29+i]));
	}else{
		patchcode[24] = 0xE9; // jmp CreateProcessInternalGuarded
		*((unsigned long *)(&(patchcode[25]))) = (unsigned long)
			(&CreateProcessInternalGuarded) - (unsigned long)(&(patchcode[29]));
		// instructions from the header of function being patched
		for(i=0; i < (patchHeaderEnd - functionAddress); i++){
			patchcode[50+i] = patchcode2[i];
		}
		patchcode[50+i] = 0xE9; // jmp patchHeaderEnd
		*((unsigned long *)(&(patchcode[51+i]))) = patchHeaderEnd - (unsigned long)(&(patchcode[55+i]));

		SetCreateProcessInternalOriginalPtr((unsigned long)(&(patchcode[50])));
	}

	// change access rights so we can patch the dll
	VirtualProtect((LPVOID)functionAddress, 
		patchHeaderEnd - functionAddress, PAGE_EXECUTE_READWRITE, &oldProtect);

	patchcode2[0] = 0xE9; // jmp patchcode
	*((unsigned long *)(&(patchcode2[1]))) = (unsigned long)(patchcode) - (unsigned long)(&(patchcode2[5]));
	for(i=5; i < (patchHeaderEnd - functionAddress); i++){
		patchcode2[i] = 0x90;
	}

	//return old access rights
	VirtualProtect((LPVOID)functionAddress,patchHeaderEnd-functionAddress,oldProtect,&newProtect);

	return 1;
}

extern bool protectionEnabled;

// patches all critical functions as defined in the configuration
int PatchFunctions()
{
	// disable protection while we patch functions
	protectionEnabled = false;

	int i;
	int ret;
	int numFunctions = GetNumGuardedFunctions();
	ROPGuardedFunction *guardedFunctions = GetGuardedFunctions();
	DWORD oldProtect;

	unsigned char *patchcode;
	int patchcodesize = numFunctions * 100;
	int patchsizeused = 0;

	// allocate memory for the extra code that will be used in patching the functions
	patchcode = (unsigned char *)VirtualAlloc(
		NULL, patchcodesize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(!patchcode)
		return 0;

	// patch every function
	for(i=0; i < numFunctions; i++){

		// patch function
		ret = PatchFunction(guardedFunctions[i].moduleName, 
			guardedFunctions[i].functionName,
			&(guardedFunctions[i].originalAddress),
			&(patchcode[patchsizeused]));

		if(ret){
			guardedFunctions[i].originalAddress = guardedFunctions[i].originalAddress ^ ADDR_SCRAMBLE_KEY;
			guardedFunctions[i].patchedAddress = (unsigned long)(&(patchcode[patchsizeused]));
			patchsizeused += 100;
		}else{
			guardedFunctions[i].originalAddress = 0;
		}
	}

	// protect the patch code from writing
	VirtualProtect(patchcode, patchcodesize, PAGE_EXECUTE_READ, &oldProtect);

	// enable protection
	protectionEnabled = true;

	return 1;
}
