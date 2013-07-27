
#include <Windows.h>
#include <tchar.h>

int APIENTRY _tWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPTSTR    lpCmdLine, 
	int       nCmdShow)
{
	if(lstrcmp(lpCmdLine, _T("2012")) == 0){
		MessageBox(GetActiveWindow(), 
			_T("Hello! 2012"), _T("MESSAGE"), MB_OK);
	}else{
		MessageBox(GetActiveWindow(), 
			_T("Hello! Windows"), _T("MESSAGE"), MB_OK);
	}	
	return 0;
}
