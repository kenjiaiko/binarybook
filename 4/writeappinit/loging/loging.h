#ifdef LOGING_EXPORTS
#define LOGING_API extern "C" __declspec(dllexport)
#else
#define LOGING_API extern "C" __declspec(dllimport)
#endif

LOGING_API int CallSetWindowsHookEx(VOID);
LOGING_API int CallUnhookWindowsHookEx(VOID);
