// loging.cpp
//

#include "stdafx.h"
#include "loging.h"


HHOOK g_hhook = NULL;

static LRESULT WINAPI GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	return(CallNextHookEx(NULL, code, wParam, lParam));
}


LOGING_API int CallSetWindowsHookEx(VOID) 
{
	if(g_hhook != NULL)
		return -1;

	MEMORY_BASIC_INFORMATION mbi;
	if(VirtualQuery(CallSetWindowsHookEx, &mbi, sizeof(mbi)) == 0)
		return -1;
	HMODULE hModule = (HMODULE) mbi.AllocationBase;

	g_hhook = SetWindowsHookEx(
		WH_GETMESSAGE, GetMsgProc, hModule, 0);
	if(g_hhook == NULL)
		return -1;

	return 0;
}


LOGING_API int CallUnhookWindowsHookEx(VOID) 
{
	if(g_hhook == NULL)
		return -1;

	UnhookWindowsHookEx(g_hhook);
	g_hhook = NULL;
	return 0;
}