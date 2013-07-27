// wdbg02a.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <Windows.h>
#include "udis86.h"

#pragma comment(lib, "libudis86.lib")


int disas(unsigned char *buff, char *out, int size)
{
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buff, 32);

	ud_set_mode(&ud_obj, 32);

	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	if(ud_disassemble(&ud_obj)){
		sprintf_s(out, size, "%14s  %s", 
			ud_insn_hex(&ud_obj), ud_insn_asm(&ud_obj));
	}else{
		return -1;
	}

	return (int)ud_insn_len(&ud_obj);
}


int exception_debug_event(DEBUG_EVENT *pde)
{
	DWORD dwReadBytes;

	HANDLE ph = OpenProcess(
		PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_VM_OPERATION, 
		FALSE, pde->dwProcessId);
	if(!ph)
		return -1;

	HANDLE th = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, 
		FALSE, pde->dwThreadId);
	if(!th)
		return -1;

	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(th, &ctx);
	
	char asm_string[256];
	unsigned char asm_code[32];

	ReadProcessMemory(ph, (VOID *)ctx.Eip, asm_code, 32, &dwReadBytes);
	if(disas(asm_code, asm_string, sizeof(asm_string)) == -1)
		asm_string[0] = '\0';

	printf("Exception: %08x (PID:%d, TID:%d)\n", 
		pde->u.Exception.ExceptionRecord.ExceptionAddress,
		pde->dwProcessId, pde->dwThreadId);
	printf("  %08x: %s\n", ctx.Eip, asm_string);
	printf("    Reg: EAX=%08x ECX=%08x EDX=%08x EBX=%08x\n", 
		ctx.Eax, ctx.Ecx, ctx.Edx, ctx.Ebx);
	printf("         ESI=%08x EDI=%08x ESP=%08x EBP=%08x\n", 
		ctx.Esi, ctx.Edi, ctx.Esp, ctx.Ebp);

	SetThreadContext(th, &ctx);
	CloseHandle(th);
	CloseHandle(ph);
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	if(argc < 2){
		fprintf(stderr, "C:\\>%s <sample.exe>\n", argv[0]);
		return 1;
	}

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(STARTUPINFO);

	BOOL r = CreateProcess(
		NULL, argv[1], NULL, NULL, FALSE, 
		NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED | DEBUG_PROCESS,
		NULL, NULL, &si, &pi);
	if(!r)
		return -1;

	ResumeThread(pi.hThread);

	int process_counter = 0;

	do{
		DEBUG_EVENT de;
		if(!WaitForDebugEvent(&de, INFINITE))
			break;
		
		DWORD dwContinueStatus = DBG_CONTINUE;
		
		switch(de.dwDebugEventCode)
		{
		case CREATE_PROCESS_DEBUG_EVENT:
			process_counter++;
			break;
		case EXIT_PROCESS_DEBUG_EVENT:
			process_counter--;
			break;
		case EXCEPTION_DEBUG_EVENT:
			if(de.u.Exception.ExceptionRecord.ExceptionCode != 
				EXCEPTION_BREAKPOINT)
			{
				dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
			}
			exception_debug_event(&de);
			break;
		}

		ContinueDebugEvent(
			de.dwProcessId, de.dwThreadId, dwContinueStatus);

	}while(process_counter > 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 0;
}
