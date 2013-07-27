// dllinjection.cpp
//

#include "stdafx.h"
#include "injectcode.h"


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 4){
		fprintf(stderr, 
			"%s <mode> <PID|ProcName|Command> <DllPath>\n", argv[0]);
		return 1;
	}

	if(lstrcmp(argv[1], "Name") == 0){
		InjectDLLtoProcessFromName(argv[2], argv[3]);
	}
	if(lstrcmp(argv[1], "PID") == 0){
		InjectDLLtoProcessFromPid((DWORD)atol(argv[2]), argv[3]);
	}
	if(lstrcmp(argv[1], "New") == 0){
		InjectDLLtoNewProcess(argv[2], argv[3]);
	}

	return 0;
}
