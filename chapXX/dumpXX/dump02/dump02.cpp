// dump02.cpp
// 

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>

#pragma comment (lib, "dbghelp.lib")

BOOL CALLBACK DumpCallback(
	PVOID pParam, 
	const PMINIDUMP_CALLBACK_INPUT pInput, 
	PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	static int memflag = 0;

	if(pInput == NULL || pOutput == NULL)
		return FALSE;

	BOOL bRet = FALSE; 

	switch(pInput->CallbackType)
	{
	case IncludeModuleCallback:
		_tprintf(_T("IncludeModuleCallback (BaseOfImage): %08x\n"), 
			pInput->IncludeModule.BaseOfImage);
		bRet = TRUE;
		break;
	case IncludeThreadCallback:
		_tprintf(_T("IncludeThreadCallback (ThreadId): %08x\n"), 
			pInput->IncludeThread.ThreadId);
		bRet = TRUE;
		break;
	case ModuleCallback:
		_tprintf(_T("ModuleCallback (FullPath): %s\n"), 
			pInput->Module.FullPath);
		bRet = TRUE;
		break;
	case ThreadCallback:
		_tprintf(_T("ThreadCallback (ThreadHandle): %08x\n"), 
			pInput->Thread.ThreadHandle);
		bRet = TRUE;
		break;
	case ThreadExCallback:
		_tprintf(_T("ThreadExCallback (BackingStoreBase): %08x\n"), 
			pInput->ThreadEx.BackingStoreBase);
		bRet = TRUE;
		break;
	case MemoryCallback:
		memflag = 1;
		_tprintf(_T("MemoryCallback\n"));
		bRet = TRUE;
		break;
	case CancelCallback:
		_tprintf(_T("CancelCallback\n"));
		if(memflag == 1){
			pOutput->Cancel      = FALSE;
			pOutput->CheckCancel = TRUE; 
		}else{
			pOutput->Cancel      = FALSE;
			pOutput->CheckCancel = FALSE; 
		}
		bRet = TRUE;
		break;
	}

	return bRet;
}

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

	mci.CallbackRoutine    = (MINIDUMP_CALLBACK_ROUTINE)DumpCallback;;
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
