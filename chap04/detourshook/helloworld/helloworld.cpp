// helloworld.cpp
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE h = LoadLibrary("detourshook.dll");
	MessageBoxA(GetForegroundWindow(), 
		"Hello World! using MessageBoxA", "Message", MB_OK);
	FreeLibrary(h);
	return 0;
}
