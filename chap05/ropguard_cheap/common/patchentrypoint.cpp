// patchentrypoint.cpp
//

#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <winnt.h>

#include "createprocess.h"
#include "debug.h"

typedef NTSTATUS (NTAPI *pfnNtQueryInformationProcess)(
	IN  HANDLE ProcessHandle,
	IN  PROCESSINFOCLASS ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN  ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

pfnNtQueryInformationProcess myNtQueryInformationProcess;

// Load NTDLL Library and get entry address
// for NtQueryInformationProcess
int LoadNTDLLFunctions()
{
	HMODULE hNtDll = LoadLibrary("ntdll.dll");
	if(hNtDll == NULL){
		WriteLog("Error: load ntdll.dll in LoadNTDLLFunctions");
		return 0;
	}
	
	myNtQueryInformationProcess = (pfnNtQueryInformationProcess)
		GetProcAddress(hNtDll, "NtQueryInformationProcess");
	if(myNtQueryInformationProcess == NULL){
		WriteLog("Error: myNtQueryInformationProcess == NULL");
		return 0;
	}
	
	return 1;
}

// helper functions and macros for parsing PE headers
#define SIZE_OF_NT_SIGNATURE (sizeof(DWORD))

// about PE format
//   see: msdn.microsoft.com/en-us/windows/hardware/gg463119.aspx
LPVOID WINAPI GetModuleEntryPoint(LPVOID lpFile)
{
	PIMAGE_OPTIONAL_HEADER poh = (PIMAGE_OPTIONAL_HEADER)
		((LPVOID)((BYTE *)(lpFile) + 
		((PIMAGE_DOS_HEADER)(lpFile))->e_lfanew + 
		SIZE_OF_NT_SIGNATURE + sizeof(IMAGE_FILE_HEADER)));
	if(poh != NULL)
		return (LPVOID)poh->AddressOfEntryPoint;

	WriteLog("Error: can't find entrypoint in GetModuleEntryPoint");
	return NULL;
}

// returns the entry point of the main module of the process proc
DWORD GetEntryPoint(HANDLE proc)
{
	DWORD imagebase;
	DWORD enrypoint;

	// load NtQueryInformationProcess
	if(!LoadNTDLLFunctions()){
		WriteLog("Error: LoadNTDLLFunctions in GetEntryPoint");
		return 0;
	}

	// get peb address
	NTSTATUS ntret;
	PROCESS_BASIC_INFORMATION pbi;
	ntret = (*myNtQueryInformationProcess)
		(proc, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
	if(ntret != 0){
		WriteLog("Error: myNtQueryInformationProcess in GetEntryPoint");
		return 0;
	}

	// get base address of module
	BOOL bret = ReadProcessMemory(proc, (LPCVOID)
		((DWORD)(pbi.PebBaseAddress) + 8), &imagebase, sizeof(imagebase), NULL);
	if(!bret){
		WriteLog("Error: ReadProcessMemory for getting base address in GetEntryPoint");
		return 0;
	}

	// read PE header
	unsigned char *pe = new unsigned char[4096]; // whole memory page should be quite enough
	if(!ReadProcessMemory(proc, (LPCVOID)(imagebase), pe, 4096, NULL)){
		WriteLog("Error: ReadProcessMemory for read PE header in GetEntryPoint");
		delete [] pe;
		return 0;
	}

	enrypoint = imagebase + (DWORD)(GetModuleEntryPoint((LPVOID)pe));
	delete [] pe;

	return enrypoint;
}

// patches the entry point of the main thread to go into infinite loop
// dll is injected when this loop is reached,
// after which the old entry point data is restored
int PatchEntryPoint(HANDLE proc, HANDLE thread, char *dllName)
{
	DWORD entryPoint;
	DWORD oldProtect1, oldProtect2;
	unsigned char oldHeader[2];
	unsigned char newHeader[2];
	CONTEXT context;

	if((entryPoint = GetEntryPoint(proc)) == 0){
		WriteLog("Error: getting entry point");
		return 0;
	}

	// make entry point writeable
	VirtualProtectEx(proc, (LPVOID)entryPoint, 2, PAGE_EXECUTE_READWRITE, &oldProtect1);

	// store 2 bytes from entry point
	if(!ReadProcessMemory(proc, (LPCVOID)(entryPoint), oldHeader, 2, NULL)){
		WriteLog("Error: reading data from entry point");
		return 0;
	}

	newHeader[0] = 0xEB; // JMP-2 the meaning of while(1){}
	newHeader[1] = 0xFE;

	// patch entry point to go into infinite loop
	if(!WriteProcessMemory(proc, (LPVOID)(entryPoint), newHeader, 2, NULL)){
		WriteLog("Error: writing to entry point");
		return 0;
	}

	// execute thread
	ResumeThread(thread);

	// wait until entry point is reached
	do{
		Sleep(100);
		context.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(thread, &context);
	}while(context.Eip != entryPoint);

	InjectDLL(proc, dllName);

	// stop thread
	SuspendThread(thread);
	
	// return original code to entry point
	if(!WriteProcessMemory(proc, (LPVOID)(entryPoint), oldHeader, 2, NULL)){
		WriteLog("Error: rewriting to entry point");
		return 0;
	}

	// restore protection
	VirtualProtectEx(proc, (LPVOID)entryPoint, 2, oldProtect1, &oldProtect2);

	return 1;
}

