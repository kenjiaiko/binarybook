// dump01.cpp
// 

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>

#pragma comment (lib, "dbghelp.lib")

void CreateDump(EXCEPTION_POINTERS *pep, int level)
{
	TCHAR szFilePath[1024];
	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));

	_tcscat_s(szFilePath, _T(".dmp"));

	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		_tprintf(_T("CreateFile failed. Error: %u \n"), GetLastError());
		return;
	}
	
	MINIDUMP_EXCEPTION_INFORMATION mdei;

	mdei.ThreadId          = GetCurrentThreadId();
	mdei.ExceptionPointers = pep;
	mdei.ClientPointers    = FALSE;

	MINIDUMP_CALLBACK_INFORMATION mci;

	mci.CallbackRoutine    = NULL;
	mci.CallbackParam      = 0;

	MINIDUMP_TYPE mdt;

	switch(level)
	{
	case 0:
		mdt = (MINIDUMP_TYPE)(MiniDumpNormal);
		break;
	case 1:
		mdt = (MINIDUMP_TYPE)(
			MiniDumpWithIndirectlyReferencedMemory |
			MiniDumpScanMemory);
		break;
	case 2:
		mdt = (MINIDUMP_TYPE)(
			MiniDumpWithPrivateReadWriteMemory | 
			MiniDumpWithDataSegs | 
			MiniDumpWithHandleData |
			MiniDumpWithFullMemoryInfo | 
			MiniDumpWithThreadInfo | 
			MiniDumpWithUnloadedModules);
		break;
	default:
		mdt = (MINIDUMP_TYPE)(
			MiniDumpWithFullMemory | 
			MiniDumpWithFullMemoryInfo |
			MiniDumpWithHandleData | 
			MiniDumpWithThreadInfo | 
			MiniDumpWithUnloadedModules);
		break;
	}
	
	BOOL rv = MiniDumpWriteDump(
		GetCurrentProcess(), GetCurrentProcessId(), 
		hFile, mdt, (pep != NULL) ? &mdei : NULL, NULL, &mci);
	if(rv == FALSE){
		_tprintf(_T("MiniDumpWriteDump failed. Error: %u \n"), 
			GetLastError());
	}
	
	CloseHandle(hFile);
	return;
}

int main(int argc, char* argv[]) 
{
	int dumplevel = 0; // dumplevel: 0-3

	if(argc >= 2)
		dumplevel = atoi(argv[1]);

	__try
	{
		*(DWORD *)0 = 0x12345678;

	}__except(
		CreateDump(GetExceptionInformation(), dumplevel), 
		EXCEPTION_EXECUTE_HANDLER)
	{
		_tprintf( _T("Dumped!!\n"));
	}

	return 0; 
}
