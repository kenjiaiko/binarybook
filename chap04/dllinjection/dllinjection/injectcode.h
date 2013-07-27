// injectcode.h
//

int InjectDLLtoProcessFromName(TCHAR *szTarget, TCHAR *szDllPath);
int InjectDLLtoProcessFromPid(DWORD dwPid, TCHAR *szDllPath);
int InjectDLLtoNewProcess(TCHAR *szCommandLine, TCHAR *szDllPath);
