// dllmain.cpp
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(GetForegroundWindow(), 
			"DLL_PROCESS_ATTACH", "Message", MB_OK);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		MessageBoxA(GetForegroundWindow(), 
			"DLL_PROCESS_DETACH", "Message", MB_OK);
		break;
	}
	return TRUE;
}

