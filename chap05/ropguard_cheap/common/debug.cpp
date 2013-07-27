// debug.cpp
//

#include <Windows.h>

#include "debug.h"

int WriteLog(char *szData)
{
#ifdef DEBUG_PRINT
	char szModuleName[1024];
	GetModuleFileName(NULL, szModuleName, sizeof(szModuleName));
	char *filename = strrchr(szModuleName, '\\');
	if(filename == NULL)
		return -1;
	filename++;

	char szDebug[1024];
	wsprintf(szDebug, "[PID:%d][Module:%s] %s", 
		GetCurrentProcessId(), filename, szData);
	
	OutputDebugString(szDebug);
#endif
	return 0;
}
