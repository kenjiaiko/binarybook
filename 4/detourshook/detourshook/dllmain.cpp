// dllmain.cpp
//

#include "stdafx.h"
#include "detours.h"
#include "detourshook.h"


static int (WINAPI * TrueMessageBoxA)(HWND hWnd, LPCTSTR lpText, 
	LPCTSTR lpCaption, UINT uType) = MessageBoxA;

DETOURSHOOK_API int WINAPI HookedMessageBoxA(HWND hWnd, 
	LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	int nRet = TrueMessageBoxA(hWnd, lpText, "Hooked Message", uType);
	return nRet;
}


int DllProcessAttach(VOID)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TrueMessageBoxA, HookedMessageBoxA);	
	if(DetourTransactionCommit() == NO_ERROR)
		return -1;
	return 0;
}


int DllProcessDetach(VOID)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)TrueMessageBoxA, HookedMessageBoxA);
	DetourTransactionCommit();
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllProcessAttach();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DllProcessDetach();
		break;
	}
	return TRUE;
}
