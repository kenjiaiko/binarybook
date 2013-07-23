#ifdef DETOURSHOOK_EXPORTS
#define DETOURSHOOK_API __declspec(dllexport)
#else
#define DETOURSHOOK_API __declspec(dllimport)
#endif

DETOURSHOOK_API int WINAPI HookedMessageBoxA(HWND hWnd, 
	LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
