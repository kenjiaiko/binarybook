// wdbg01a.cpp : 定义命令行应用程序入口点
//

#include "stdafx.h"

#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    
    if(argc < 2){
        fprintf(stderr, "C:\\>%s <sample.exe>\n", argv[0]);
        return 1;
    }

    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(STARTUPINFO);

    BOOL r = CreateProcess(
        NULL, argv[1], NULL, NULL, FALSE, 
        NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED | DEBUG_PROCESS,
        NULL, NULL, &si, &pi);
    if(!r)
        return -1;

    ResumeThread(pi.hThread);

    while(1) {
        DEBUG_EVENT de;
        if(!WaitForDebugEvent(&de, INFINITE))
            break;
        
        DWORD dwContinueStatus = DBG_CONTINUE;
        
        switch(de.dwDebugEventCode)
        {
        case CREATE_PROCESS_DEBUG_EVENT:
            printf("CREATE_PROCESS_DEBUG_EVENT\n");
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            printf("CREATE_THREAD_DEBUG_EVENT\n");
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            printf("EXIT_THREAD_DEBUG_EVENT\n");
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            printf("EXIT_PROCESS_DEBUG_EVENT\n");
            break;
        case EXCEPTION_DEBUG_EVENT:
            if(de.u.Exception.ExceptionRecord.ExceptionCode != 
				EXCEPTION_BREAKPOINT)
			{
                dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
			}
            printf("EXCEPTION_DEBUG_EVENT\n");
            break;
        case OUTPUT_DEBUG_STRING_EVENT:
            printf("OUTPUT_DEBUG_STRING_EVENT\n");
            break;
        case RIP_EVENT:
            printf("RIP_EVENT\n");
            break;
        case LOAD_DLL_DEBUG_EVENT:
            printf("LOAD_DLL_DEBUG_EVENT\n");
            break;
        case UNLOAD_DLL_DEBUG_EVENT:
            printf("UNLOAD_DLL_DEBUG_EVENT\n");
            break;
        }
        if(de.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
            break;
        ContinueDebugEvent(
            de.dwProcessId, de.dwThreadId, dwContinueStatus);
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}

