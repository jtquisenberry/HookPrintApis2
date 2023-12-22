// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void PrintError(LPTSTR lpszFunction);
HANDLE GetTargetExe(__in LPCWSTR lpcwszDll, __in LPCWSTR targetPath);
HANDLE GetTargetExe(__in int pid);
bool is_numeric(char* str);
BOOL WINAPI InjectDll(__in HANDLE hProcess);

int main(int argc, char* argv[])
{
    int pid = 0;
    char* first_arg = argv[1];

    HANDLE hProcess;

    if (is_numeric(first_arg)) {
        pid = atoi(first_arg);
        HANDLE hProcess = GetTargetExe(pid);
    }
    else {
        wchar_t selfdir[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, selfdir, MAX_PATH);
        PathRemoveFileSpec(selfdir);

        std::wstring dllPath = std::wstring(selfdir) + TEXT("\\hooks.dll");
        std::wstring targetPath = std::wstring(selfdir) + TEXT("\\target.exe");

        if (InjectDll(dllPath.c_str(), targetPath.c_str())) {
            printf("Dll was successfully injected.\n");
        }
        else {
            printf("Terminating the Injector app...");
        }

        HANDLE hProcess = GetTargetExe();
    }


    _getch();

    return 0;
}

bool is_numeric(char* str) {
    printf(str);
    printf("\n");

    int total = strlen(str);
    int numeric_count = 0;

    for (int i = 0; i < strlen(str); i++) {
        if (isdigit(str[i]))
            numeric_count++;
    }
    
    if (total - numeric_count == 0)
        return true;
    
    return false;
}

HANDLE GetTargetExe(__in int pid) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    return hProcess;
}

HANDLE GetTargetExe(__in LPCWSTR lpcwszDll, __in LPCWSTR targetPath)
{
    STARTUPINFO             startupInfo;
    PROCESS_INFORMATION     processInformation;

    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(targetPath, NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInformation))
    {
        PrintError(TEXT("CreateProcess"));
        return FALSE;
    }

    HANDLE hProcess = processInformation.hProcess;
    return hProcess;
}



BOOL WINAPI InjectDll(__in HANDLE hProcess)
{
    
    SIZE_T nLength;
    LPVOID lpLoadLibraryW = NULL;
    LPVOID lpRemoteString;


    lpLoadLibraryW = GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryW");

    if (!lpLoadLibraryW)
    {
        PrintError(TEXT("GetProcAddress"));
        // close process handle
        CloseHandle( hProcess);
        return FALSE;
    }

    nLength = wcslen(lpcwszDll) * sizeof(WCHAR);

    // allocate mem for dll name
    lpRemoteString = VirtualAllocEx(hProcess, NULL, nLength + 1, MEM_COMMIT, PAGE_READWRITE);
    if (!lpRemoteString)
    {
        PrintError(TEXT("VirtualAllocEx"));

        // close process handle
        CloseHandle(hProcess);

        return FALSE;
    }

    // write dll name
    if (!WriteProcessMemory(hProcess, lpRemoteString, lpcwszDll, nLength, NULL)) {

        PrintError(TEXT("WriteProcessMemory"));
        // free allocated memory
        VirtualFreeEx(hProcess, lpRemoteString, 0, MEM_RELEASE);

        // close process handle
        CloseHandle(hProcess);

        return FALSE;
    }

    // call loadlibraryw
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)lpLoadLibraryW, lpRemoteString, NULL, NULL);

    if (!hThread) {
        PrintError(TEXT("CreateRemoteThread"));
    }
    else {
        WaitForSingleObject(hThread, 4000);

        //resume suspended process
        ResumeThread(processInformation.hThread);
    }

    //  free allocated memory
    VirtualFreeEx(hProcess, lpRemoteString, 0, MEM_RELEASE);

    // close process handle
    CloseHandle(hProcess);

    return TRUE;
}

void PrintError(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);

    wprintf(L"%s", lpDisplayBuf);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}