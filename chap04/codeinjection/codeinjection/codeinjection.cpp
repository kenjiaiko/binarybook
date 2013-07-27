// codeinjection.cpp
//

#include "stdafx.h"

#include <windows.h>


typedef HWND (WINAPI *GETFORGROUNDWINDOW)(void);
typedef int  (WINAPI *MSGBOX)(HWND, PCTSTR, PCTSTR, UINT);


typedef struct _injectdata {
	TCHAR szTitle[32];
	TCHAR szMessage[32];
	HANDLE hProcess;
	PDWORD pdwCodeRemote;
	PDWORD pdwDataRemote;
	MSGBOX fnMessageBox;
	GETFORGROUNDWINDOW fnGetForegroundWindow;
} INJECTDATA, *PINJECTDATA;


static DWORD WINAPI func(PINJECTDATA myAPI) 
{
	myAPI->fnMessageBox((HWND)myAPI->fnGetForegroundWindow(),
		myAPI->szMessage, myAPI->szTitle, MB_OK);
	
	/*
	if(myAPI->pCodeRemote != NULL)
		VirtualFreeEx(myAPI->hProcess, 
		myAPI->pCodeRemote, 0, MEM_RELEASE);
	if(myAPI->pDataRemote != NULL)
		VirtualFreeEx(myAPI->hProcess, 
		myAPI->pDataRemote, 0, MEM_RELEASE);
	*/
	
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE h = LoadLibrary("user32.dll");
	if(h == NULL){
		printf("ERR: LoadLibrary\n");
		return -1;
	}

	INJECTDATA id;

	id.fnGetForegroundWindow = (GETFORGROUNDWINDOW)
		GetProcAddress(
		GetModuleHandle("user32"), "GetForegroundWindow");

	id.fnMessageBox = (MSGBOX)
		GetProcAddress(
		GetModuleHandle("user32"), "MessageBoxA");
	
	lstrcpy(id.szTitle, "Message");
	lstrcpy(id.szMessage, "Hello World!");

	HWND hTarget = FindWindow("IEFrame", NULL);
	if(hTarget == NULL){
		printf("ERR: FindWindow\n");
		goto _END1;
	}

	DWORD dwPID; // PID of iexplore.exe
	GetWindowThreadProcessId(hTarget, (DWORD *)&dwPID);
	id.hProcess = OpenProcess(PROCESS_CREATE_THREAD | 
		PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | 
		PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwPID);
	if(id.hProcess == NULL){
		printf("ERR: OpenProcess\n");
		goto _END1;
	}

	DWORD dwLen;
	if((id.pdwCodeRemote = (PDWORD)VirtualAllocEx(id.hProcess, 
		0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL)
	{
		printf("ERR: VirtualAllocEx(pdwCodeRemote)\n");
		goto _END2;
	}
	if((id.pdwDataRemote = (PDWORD)VirtualAllocEx(id.hProcess,
		0, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL)
	{
		printf("ERR: VirtualAllocEx(pdwDataRemote)\n");
		goto _END3;
	}

	WriteProcessMemory(id.hProcess, 
		id.pdwCodeRemote, &func, 4096, &dwLen);
	WriteProcessMemory(id.hProcess,
		id.pdwDataRemote, &id, sizeof(INJECTDATA), &dwLen);
	
	HANDLE hThread = CreateRemoteThread(id.hProcess, NULL, 0,
		(LPTHREAD_START_ROUTINE)id.pdwCodeRemote, id.pdwDataRemote, 
		0, &dwLen);
	if(hThread == NULL){
		printf("ERR: CreateRemoteThread\n");
		goto _END4;
	}
	
	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, (PDWORD)&dwPID);
	CloseHandle(hThread);

_END4:
	VirtualFreeEx(id.hProcess, id.pdwDataRemote, 0, MEM_RELEASE);
_END3:
	VirtualFreeEx(id.hProcess, id.pdwCodeRemote, 0, MEM_RELEASE);
_END2:
	CloseHandle(id.hProcess);
_END1:
	FreeLibrary(h);
	return 0;
}

