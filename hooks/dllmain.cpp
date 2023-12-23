// dllmain.cpp : Defines the entry point for the DLL application.
#include <fstream>

#include "stdafx.h"

#include "HooksManager.h"

FILE* log_file;

using namespace std;
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    log_file = fopen("d:\\projects\\thefile.txt", "w");
    fputs("aaaaaaaaaaaaaaaaaaaaaaaaaa", log_file);
    static HooksManager* manager = new HooksManager();

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //MessageBox(NULL, L"DLL_PROCESS_ATTACH", L"DLL_PROCESS_ATTACH", MB_ICONINFORMATION);
        break;
    case DLL_THREAD_ATTACH:
        //MessageBox(NULL, L"DLL_THREAD_ATTACH", L"DLL_THREAD_ATTACH", MB_ICONINFORMATION);
        break;
    case DLL_THREAD_DETACH:
        //MessageBox(NULL, L"DLL_THREAD_DETACH", L"DLL_THREAD_DETACH", MB_ICONINFORMATION);
        break;
    case DLL_PROCESS_DETACH:
        //MessageBox(NULL, L"DLL_PROCESS_DETACH", L"DLL_PROCESS_DETACH", MB_ICONINFORMATION);
        break;
    }
    
    fclose(log_file);
    return TRUE;
}
