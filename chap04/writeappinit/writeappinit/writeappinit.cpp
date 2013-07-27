// writeappinit.cpp
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2){
		fprintf(stderr, "%s <DLL Name>\n", argv[0]);
		return 1;
	}
	
	HKEY hKey;
	LSTATUS lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
		NULL, KEY_ALL_ACCESS, &hKey);
	if(lResult != ERROR_SUCCESS){
		printf("Error: RegOpenKeyEx failed.\n");
		return -1;
	}

	DWORD dwSize, dwType;
	TCHAR szDllName[256];

	RegQueryValueEx(hKey, "AppInit_DLLs", NULL, &dwType, NULL, &dwSize);
    RegQueryValueEx(hKey, "AppInit_DLLs", NULL, &dwType, (LPBYTE)szDllName, &dwSize);
	printf("AppInit_DLLs: %s -> ", szDllName);
	lstrcpy(szDllName, argv[1]);
	
	lResult = RegSetValueEx(hKey, "AppInit_DLLs", 
		0, REG_SZ, (PBYTE)szDllName, lstrlen(szDllName) + 1);
	if(lResult != ERROR_SUCCESS){
		printf("Error: RegSetValueEx failed.\n");
	}

	RegQueryValueEx(hKey, "AppInit_DLLs", NULL, &dwType, NULL, &dwSize);
    RegQueryValueEx(hKey, "AppInit_DLLs", NULL, &dwType, (LPBYTE)szDllName, &dwSize);
	printf("%s\n", szDllName);

	RegCloseKey(hKey);
	return 0;
}

