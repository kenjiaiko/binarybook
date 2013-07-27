// setwindowshook.cpp
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2){
		fprintf(stderr, "%s <DLL Name>\n", argv[0]);
		return 1;
	}

	HMODULE h = LoadLibrary(argv[1]);
	if(h == NULL)
		return -1;

	int (__stdcall *fcall) (VOID);
	fcall = (int (WINAPI *)(VOID))
		GetProcAddress(h, "CallSetWindowsHookEx");
	if(fcall == NULL){
		fprintf(stderr, "ERROR: GetProcAddress\n");
		goto _Exit;
	}

	int (__stdcall *ffree) (VOID);
	ffree = (int (WINAPI *)(VOID))
		GetProcAddress(h, "CallUnhookWindowsHookEx");
	if(ffree == NULL){
		fprintf(stderr, "ERROR: GetProcAddress\n");
		goto _Exit;
	}

	if(fcall()){
		fprintf(stderr, "ERROR: CallSetWindowsHookEx\n");
		goto _Exit;
	}
	printf("Call SetWindowsHookEx\n");

	getchar();

	if(ffree()){
		fprintf(stderr, "ERROR: CallUnhookWindowsHookEx\n");
		goto _Exit;
	}
	printf("Call UnhookWindowsHookEx\n");

_Exit:
	FreeLibrary(h);
	return 0;
}

