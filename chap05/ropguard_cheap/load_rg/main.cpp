// main.cpp
//

#include <Windows.h>
#include <stdio.h>

#include "../common/createprocess.h"
#include "../common/ropsettings.h"
#include "../common/debug.h"

#pragma warning(disable : 4996)

// prints the usage information
void PrintUsage(void)
{
	fprintf(stderr, "Usage: load_rg.exe <PID>\n");
	fprintf(stderr, "       load_rg.exe \"<command>\"\n");
}

// returns true if string is numeric
bool IsNumeric(char *str)
{
	int n = strlen(str);
	for(int i=0; i < n; i++){
		if((str[i] < '0') || ('9' < str[i]))
			return false;
	}
	return true;
}

// arg is PID or target program path
int load_ropguard(char *arg)
{
	// get the full path of ropguard.dll
	char dllpath[1024];
	if(GetModuleFileName(NULL, dllpath, sizeof(dllpath) - 1) == 0){
		fprintf(stderr, "Error: could not obtain current executable path\n");
		return -1;
	}
	
	char *filename = strrchr(dllpath, '\\');
	if(filename == NULL){
		fprintf(stderr, "Error: could not obtain current executable path\n");
		return -1;
	}
	
	filename++;
	lstrcpy(filename, "ropguard.dll");
	
	HMODULE h = LoadLibraryEx("ropguard.dll", NULL, 0);
	ReadROPSettings();

	// if the first argument is a number it's considered to be a PID
	if(IsNumeric(arg)){
		// protect existing process
		GuardExistingProcess(atol(arg), dllpath);
	}else{
		// create new protected process
		CreateNewGuardedProcess(arg, dllpath, true);
	}

	FreeLibrary(h);
	return 0;
}

// entry point
int main(int argc, char *argv[])
{
	if(argc < 2){
		PrintUsage();
		return 1;
	}
	load_ropguard(argv[1]);
	return 0;
}
