// createprocess.h
//

// a function that will replace CreateProcessInternalW
// needs to have the same prototype
DWORD WINAPI CreateProcessInternalGuarded(
	__in         DWORD unknown1,  // always (?) NULL
	__in_opt     LPCTSTR lpApplicationName,
	__inout_opt  LPTSTR lpCommandLine,
	__in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
	__in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	__in         BOOL bInheritHandles,
	__in         DWORD dwCreationFlags,
	__in_opt     LPVOID lpEnvironment,
	__in_opt     LPCTSTR lpCurrentDirectory,
	__in         LPSTARTUPINFO lpStartupInfo,
	__out        LPPROCESS_INFORMATION lpProcessInformation,
	__in         DWORD unknown2   // always (?) NULL
);

// stores the original address of CreateProcessInternalW
void SetCreateProcessInternalOriginalPtr(unsigned long address);

// DLL injection using CreateRemoteThread method
// injects a DLL with path dllName into a process with handle proc
int InjectDLL(HANDLE proc, char *dllName);

// creates a new process with command given in commandLine and injects dll whose path is dllName into it
int CreateNewGuardedProcess(char *commandLine, char *dllName, bool patchEntryPoint);

// injects dll whose path is given in dllName into proces with PID pid
int GuardExistingProcess(int pid, char *dllName);