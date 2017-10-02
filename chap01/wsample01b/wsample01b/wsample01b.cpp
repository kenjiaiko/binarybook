
#include <Windows.h>
#include <tchar.h>
#include <shlobj.h>

int cpy(void)
{
	// 获取自身文件路径
	TCHAR szThis[2048];
	GetModuleFileName(NULL, szThis, sizeof(szThis));
	// 获取启动文件夹路径
	TCHAR szStartup[2048];
	SHGetFolderPath(NULL, CSIDL_STARTUP, 
		NULL, SHGFP_TYPE_CURRENT, szStartup);
	lstrcat(szStartup, _T("\\wsample01b.exe"));
	// 将自身复制到启动文件夹
	CopyFile(szThis, szStartup, FALSE);
	return 0;
}

int APIENTRY _tWinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPTSTR    lpCmdLine, 
	int       nCmdShow)
{
	cpy();
	MessageBox(GetActiveWindow(), 
		_T("Copied!"), _T("MESSAGE"), MB_OK);
	return 0;
}