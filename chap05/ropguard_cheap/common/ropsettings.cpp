
#include <windows.h>

#include "ropsettings.h"
#include "debug.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#pragma warning(disable : 4996)

// contains ROPGuard's configuration
ROPSettings *ropSettings;

// returns the ropSettings object
ROPSettings *GetROPSettings()
{
	return ropSettings;
}

int GetNumGuardedFunctions()
{
	// return sizeof(guardedFunctions) / sizeof(ROPGuardedFunction);
	return ropSettings->GetNumGuardedFunctions();
}

ROPGuardedFunction *GetGuardedFunctions()
{
	// return guardedFunctions;
	return ropSettings->GetGuardedFunctions();
}

// see ropsettings.h
void ROPSettings::AddFunction(
	const char *moduleName, const char *functionName, int stackIncrement)
{
	for(int i=0; i < numGuardedFunctions; i++){
		if(strcmp(guardedFunctions[i].moduleName, moduleName) != 0)
			continue;
		if(strcmp(guardedFunctions[i].functionName, functionName) != 0)
			continue;
		guardedFunctions[i].stackIncrement = stackIncrement;
		return;
	}

	guardedFunctions = (ROPGuardedFunction *)realloc(
		guardedFunctions, (numGuardedFunctions + 1) * sizeof(ROPGuardedFunction));

	strncpy(guardedFunctions[numGuardedFunctions].moduleName, 
		moduleName, RG_MODULE_NAME_LEN - 1);
	strncpy(guardedFunctions[numGuardedFunctions].functionName, 
		functionName, RG_FUNCTION_NAME_LEN - 1);

	guardedFunctions[numGuardedFunctions].originalAddress = 0;
	guardedFunctions[numGuardedFunctions].patchedAddress  = 0;
	guardedFunctions[numGuardedFunctions].stackIncrement  = stackIncrement;
	
	numGuardedFunctions++;
	return;
}

int ReadROPSettings()
{
	ropSettings = new ROPSettings();
	ropSettings->AddFunction("kernel32.dll", "WinExec", 2);
	return 1;
}