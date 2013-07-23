// createprocess.cpp
//

#include <stdio.h>
#include <windows.h>

#include "createprocess.h"
#include "patchentrypoint.h"
#include "ropsettings.h"
#include "debug.h"

// function pointer to unpatched CreateProcessInternalW 
DWORD (WINAPI *CreateProcessInternalOriginal)(
	__in         DWORD unknown1,  // always (?) NULL
	__in_opt     LPCTSTR lpApplicationName,
	__inout_opt  LPTSTR lpCommandLine,
	__in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in         BOOL bInheritHandles,
	__in         DWORD dwCreationFlags,
	__in_opt     LPVOID lpEnvironment,
	__in_opt     LPCTSTR lpCurrentDirectory,
	__in         LPSTARTUPINFO lpStartupInfo,
	__out        LPPROCESS_INFORMATION lpProcessInformation,
	__in         DWORD unknown2   // always (?) NULL
);

// stores the original address of CreateProcessInternalW
void SetCreateProcessInternalOriginalPtr(unsigned long address)
{
	CreateProcessInternalOriginal = (DWORD (WINAPI *)
		(DWORD, LPCTSTR, LPTSTR, LPSECURITY_ATTRIBUTES, 
		LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCTSTR, 
		LPSTARTUPINFO, LPPROCESS_INFORMATION, DWORD))address;
}

// DLL injection using CreateRemoteThread method
// injects a DLL with path dllName into a process with handle proc
int InjectDLL(HANDLE proc, char *dllName)
{
	LPVOID RemoteString, LoadLibAddy;
	
	if(proc == NULL){
		WriteLog("Error: proc == NULL in InjectDLL");
		return 0;
	}

	LoadLibAddy = (LPVOID)GetProcAddress(
		GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if(LoadLibAddy == NULL){
		WriteLog("Error: GetProcAddress failed in InjectDLL");
		return 0;
	}
	
	RemoteString = (LPVOID)VirtualAllocEx(proc, 
		NULL, strlen(dllName) + 1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(proc, (LPVOID)RemoteString, dllName ,strlen(dllName) + 1, NULL);

	HANDLE hThread = CreateRemoteThread(proc, 
		NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);
	if(hThread == INVALID_HANDLE_VALUE){
		WriteLog("Error: CreateRemoteThread failed in InjectDLL");
		return 0;
	}
	
	WaitForSingleObject(hThread, INFINITE);
	
	// cleanup
	VirtualFreeEx(proc, RemoteString, strlen(dllName) + 1, MEM_RELEASE);
	CloseHandle(hThread);
	
	return 1;
}

// injects dll whose path is given in dllName into proces with PID pid
int GuardExistingProcess(int pid, char *dllName)
{
	HANDLE proc = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE | 
		PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, pid);
	if(proc == NULL){
		char buf[1024];
		wsprintf(buf, "Error: opening process %d, error code %d\n", 
			pid, GetLastError());
		WriteLog(buf);
		return 0;
	}

	BOOL parent64, child64;
	IsWow64Process(GetCurrentProcess(), &parent64);
	IsWow64Process(proc, &child64);
	if(parent64 != child64){
		WriteLog("Error: Current version of ROPGuard cannot protect 64-bit processes");
	}else{
		InjectDLL(proc, dllName);
	}

	CloseHandle(proc);

	return 1;
}

// creates a new process with command given in commandLine and injects dll whose path is dllName into it
int CreateNewGuardedProcess(char *commandLine, char *dllName, bool patchEntryPoint)
{
	STARTUPINFO startupinfo;
	PROCESS_INFORMATION processinfo;

	ZeroMemory(&startupinfo, sizeof(startupinfo));
	startupinfo.cb = sizeof(startupinfo);
	ZeroMemory(&processinfo, sizeof(processinfo));

	BOOL result = CreateProcess(
		NULL, commandLine, NULL, NULL, FALSE, CREATE_SUSPENDED, 
		NULL, NULL, &startupinfo, &processinfo);
	if(!result){
		char buf[1024];
		wsprintf(buf, "Error: creating process (%d)", GetLastError());
		WriteLog(buf);
		return 0;
	}

	BOOL parent64, child64;
	IsWow64Process(GetCurrentProcess(), &parent64);
	IsWow64Process(processinfo.hProcess, &child64);
	if(parent64 != child64){
		WriteLog("Error: Current version of ROPGuard cannot protect 64-bit processes");
	}else{
		if(patchEntryPoint){
			PatchEntryPoint(processinfo.hProcess, processinfo.hThread, dllName);
		}else{
			InjectDLL(processinfo.hProcess, dllName);
		}
	}

	// resume normal execution
	ResumeThread(processinfo.hThread);

	// cleanup
	CloseHandle(processinfo.hThread);
	CloseHandle(processinfo.hProcess);

	return 1;
}

// a function that will replace CreateProcessInternalW
// gets called whenever a process creates a child process
DWORD WINAPI CreateProcessInternalGuarded(
	__in         DWORD unknown1,  // always (?) NULL
	__in_opt     LPCTSTR lpApplicationName,
	__inout_opt  LPTSTR lpCommandLine,
	__in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in         BOOL bInheritHandles,
	__in         DWORD dwCreationFlags,
	__in_opt     LPVOID lpEnvironment,
	__in_opt     LPCTSTR lpCurrentDirectory,
	__in         LPSTARTUPINFO lpStartupInfo,
	__out        LPPROCESS_INFORMATION lpProcessInformation,
	__in         DWORD unknown2   // always (?) NULL
)
{
	DWORD ret;
	DWORD newCreationFlags;

	// start new process in suspended state to inject dll into it
	newCreationFlags = dwCreationFlags | CREATE_SUSPENDED;

	ret = (*CreateProcessInternalOriginal)(
		unknown1,
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		newCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation,
		unknown2);
	if(!ret)
		return ret;

	BOOL parent64, child64;
	IsWow64Process(GetCurrentProcess(), &parent64);
	IsWow64Process(lpProcessInformation->hProcess, &child64);
	if(parent64 != child64){
		WriteLog("Error: Current version of ROPGuard cannot protect 64-bit processes");
		if((dwCreationFlags & CREATE_SUSPENDED) == 0)
			ResumeThread(lpProcessInformation->hThread);
		return ret;
	}

	// get the path of the ropguard dll
	char dllpath[2048];
	HMODULE dllhandle = GetModuleHandle("ropguard.dll");
	if((!dllhandle) || (!GetModuleFileName(dllhandle, dllpath, sizeof(dllpath) - 256))){
		WriteLog("Error: could not obtain ropguard.dll path");
		if((dwCreationFlags & CREATE_SUSPENDED) == 0)
			ResumeThread(lpProcessInformation->hThread);
		return ret;
	}

	// inject ropguard dll into the newly created process
	char buf[1024];
	if((dwCreationFlags & CREATE_SUSPENDED) == 0){
		wsprintf(buf, "Log: create process PID: %d", lpProcessInformation->dwProcessId);
		WriteLog(buf);
		PatchEntryPoint(lpProcessInformation->hProcess, lpProcessInformation->hThread, dllpath);
	}else{
		wsprintf(buf, "Log: create process PID: %d", lpProcessInformation->dwProcessId);
		WriteLog(buf);
		InjectDLL(lpProcessInformation->hProcess, dllpath);
	}

	// resume process if necessary
	if((dwCreationFlags & CREATE_SUSPENDED) == 0)
		ResumeThread(lpProcessInformation->hThread);

	return ret;
}
