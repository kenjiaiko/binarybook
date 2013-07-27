// dllmain.cpp
//

#include <Windows.h>
#include <stdio.h>

#include "../common/ropsettings.h"
#include "../common/ropcheck.h"
#include "../common/patching.h"
#include "../common/debug.h"

BOOL APIENTRY DllMain(HMODULE hModule, 
					  DWORD ul_reason_for_call, 
					  LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

class ROPGuard
{
public:
	// this code gets called when ROPGuard's dll is injected into a process
	ROPGuard(){

		WriteLog("Log: start ropguard");

		char fullpath[2048];
		if(!GetModuleFileName(NULL, fullpath, sizeof(fullpath) - 256)){
			WriteLog("Error: could not obtain main module path");
			return;
		}
		char *filename = strrchr(fullpath, '\\');
		if(filename == NULL){
			WriteLog("Error: could not obtain main module path");
			return;
		}
		filename++;
		// nothing to do in load_rg.exe
		if(lstrcmp(filename, "load_rg.exe") == 0){
			WriteLog("Log: not patch load_rg.exe");
			//return;
		}

		// read settings
		if(!ReadROPSettings()){
			WriteLog("Error: ReadROPSettings failed.");
			return;
		}
		
		WriteLog("Log: ReadROPSettings ok");

		// patch all critical functions
		PatchFunctions();
		
		WriteLog("Log: PatchFunctions ok");
		WriteLog("Successfully loaded ROPGuard dll into target process");
	}
};

// ROPGuard object
ROPGuard h;
