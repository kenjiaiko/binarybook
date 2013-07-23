// injectcode.cpp
//

#include "stdafx.h"
#include <tlhelp32.h>
#include "injectcode.h"


DWORD GetProcessIdFromName(TCHAR *szTargetProcessName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(hSnap == INVALID_HANDLE_VALUE)
		return 0;
	
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	
	DWORD dwProcessId = 0;
	BOOL bResult = Process32First(hSnap, &pe);

	while(bResult){
		if(!lstrcmp(pe.szExeFile, szTargetProcessName)){
			dwProcessId = pe.th32ProcessID;
			break;
		}
		bResult = Process32Next(hSnap, &pe);
	}
	CloseHandle(hSnap);
	
	return dwProcessId;
}


int InjectDLL(HANDLE hProcess, TCHAR *szDllPath)
{
	int szDllPathLen = lstrlen(szDllPath) + 1;

	PWSTR RemoteProcessMemory = (PWSTR)VirtualAllocEx(hProcess, 
		NULL, szDllPathLen, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	if(RemoteProcessMemory == NULL)
		return -1;
	
	BOOL bRet = WriteProcessMemory(hProcess, 
		RemoteProcessMemory, (PVOID)szDllPath, szDllPathLen, NULL);
	if(bRet == FALSE)
		return -1;
	
	PTHREAD_START_ROUTINE pfnThreadRtn;
	pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(
		GetModuleHandle("kernel32"), "LoadLibraryA");
	if(pfnThreadRtn == NULL)
		return -1;
	
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
		pfnThreadRtn, RemoteProcessMemory, 0, NULL);
	if(hThread == NULL)
		return -1;

	WaitForSingleObject(hThread, INFINITE);
	
	VirtualFreeEx(hProcess, 
		RemoteProcessMemory, szDllPathLen, MEM_RELEASE);

	CloseHandle(hThread);
	return 0;
}


int InjectDLLtoExistedProcess(DWORD dwPid, TCHAR *szDllPath)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | 
		PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION , FALSE, dwPid);
	if(hProcess == NULL)
		return -1;
	/*
	BOOL bJudgeWow64;
	IsWow64Process(hProcess, &bJudgeWow64);
	if(bJudgeWow64 == FALSE){
		CloseHandle(hProcess);
		return -1;
	}
	*/
	if(InjectDLL(hProcess, szDllPath))
		return -1;

	CloseHandle(hProcess);
	return 0;
}


int InjectDLLtoProcessFromName(TCHAR *szTarget, TCHAR *szDllPath)
{
	DWORD dwPid = GetProcessIdFromName(szTarget);
	if(dwPid == 0)
		return -1;
	if(InjectDLLtoExistedProcess(dwPid, szDllPath))
		return -1;
	return 0;
}


int InjectDLLtoProcessFromPid(DWORD dwPid, TCHAR *szDllPath)
{
	if(InjectDLLtoExistedProcess(dwPid, szDllPath))
		return -1;
	return 0;
}


int InjectDLLtoNewProcess(TCHAR *szCommandLine, TCHAR *szDllPath)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	BOOL bResult = CreateProcess(NULL, szCommandLine, NULL, NULL,
		FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
	if(bResult == FALSE)
		return -1;

	int nRet = -1;
	/*
	BOOL bJudgeWow64;
	IsWow64Process(pi.hProcess, &bJudgeWow64);
	if(bJudgeWow64 == FALSE)
		goto _Exit;
	*/
	if(InjectDLL(pi.hProcess, szDllPath))
		goto _Exit;

	nRet = 0;

_Exit:
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return nRet;
}
