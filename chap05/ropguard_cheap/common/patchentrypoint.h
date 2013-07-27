// patchentrypoint.h
//

// patches the entry point of the main thread to go into infinite loop
// dll is injected when this loop is reached,
// after which the old entry point data is restored
int PatchEntryPoint(HANDLE proc, HANDLE thread, char *dllName);