// dllmain.cp
//

#include "stdafx.h"


int WriteLog(TCHAR *szData)
{
	TCHAR szTempPath[1024];
	GetTempPath(sizeof(szTempPath), szTempPath);
	lstrcat(szTempPath, "loging.log");
	
	TCHAR szModuleName[1024];
	GetModuleFileName(GetModuleHandle(NULL), 
		szModuleName, sizeof(szModuleName));

	TCHAR szHead[1024];
	wsprintf(szHead, "[PID:%d][Module:%s] ", 
		GetCurrentProcessId(), szModuleName);

	HANDLE hFile = CreateFile(
		szTempPath, GENERIC_WRITE, 0, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	SetFilePointer(hFile, 0, NULL, FILE_END);

	DWORD dwWriteSize;
	WriteFile(hFile, szHead, lstrlen(szHead), &dwWriteSize, NULL);
	WriteFile(hFile, szData, lstrlen(szData), &dwWriteSize, NULL);

	CloseHandle(hFile);
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
		WriteLog("DLL_PROCESS_ATTACH\n");
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		WriteLog("DLL_PROCESS_DETACH\n");
		break;
	}
	return TRUE;
}

