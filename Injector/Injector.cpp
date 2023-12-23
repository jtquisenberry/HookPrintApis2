// Injector.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <Windows.h>
#include <winternl.h>
#include <tchar.h>
#pragma comment(lib, "ntdll")

#include "stdafx.h"

void PrintError(LPTSTR lpszFunction);
PROCESS_INFORMATION StartTargetExe(__in LPCWSTR targetPath);
HANDLE GetTargetExe(__in int pid);
bool is_numeric(char* str);
BOOL WINAPI InjectDll(__in HANDLE hProcess, __in LPCWSTR lpcwszDll);
BOOL WINAPI InjectDll(__in PROCESS_INFORMATION pi, __in LPCWSTR lpcwszDll);
DWORD GetMainThreadId(DWORD pId); 
BOOL ListProcessThreads(DWORD dwOwnerPID);
// BOOL ListProcessThreads(DWORD dwOwnerPID);

int main(int argc, char* argv[])
{
    int pid = 0;
    char* first_arg = argv[1];

    HANDLE hProcess;
    PROCESS_INFORMATION pi;
    

    wchar_t selfdir[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, selfdir, MAX_PATH);
    PathRemoveFileSpec(selfdir);
    std::wstring dllPath = std::wstring(selfdir) + TEXT("\\hooks.dll");

    if (is_numeric(first_arg)) {
        pid = atoi(first_arg);
        hProcess = GetTargetExe(pid);

        if (InjectDll(hProcess, dllPath.c_str())) {
            printf("Dll was successfully injected.\n");
        }
        else {
            printf("Terminating the Injector app...");
        }
        
    }
    else {
        //wchar_t selfdir[MAX_PATH] = { 0 };
        //GetModuleFileName(NULL, selfdir, MAX_PATH);
        //PathRemoveFileSpec(selfdir);

        //std::wstring dllPath = std::wstring(selfdir) + TEXT("\\hooks.dll");
        std::wstring targetPath = std::wstring(selfdir) + TEXT("\\target.exe");

        pi = StartTargetExe(targetPath.c_str());
        
        //InjectDll(hProcess);
        if (InjectDll(pi, dllPath.c_str())) {
            printf("Dll was successfully injected.\n");
        }
        else {
            printf("Terminating the Injector app...");
        }
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

PROCESS_INFORMATION StartTargetExe(__in LPCWSTR targetPath)
{
    STARTUPINFO             startupInfo;
    PROCESS_INFORMATION     processInformation;

    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(STARTUPINFO);

    if (!CreateProcess(targetPath, NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInformation))
    {
        PrintError(TEXT("CreateProcess"));
        return processInformation;
    }

    HANDLE hProcess = processInformation.hProcess;

    /*
    APP_MEMORY_INFORMATION aaa;
    GetProcessInformation(hProcess, PROCESS_INFORMATION_CLASS::ProcessAppMemoryInfo, &aaa, sizeof(MEMORY_PRIORITY_INFORMATION));
    */

    //GetMainThreadId(processInformation.dwProcessId);
    ListProcessThreads(processInformation.dwProcessId);
    return processInformation;
}



BOOL WINAPI InjectDll(__in HANDLE hProcess, __in LPCWSTR lpcwszDll)
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

        /*
        ResumeThread(processInformation.hThread);
        */
    }

    //  free allocated memory
    VirtualFreeEx(hProcess, lpRemoteString, 0, MEM_RELEASE);

    // close process handle
    CloseHandle(hProcess);

    return TRUE;
}



BOOL WINAPI InjectDll(__in PROCESS_INFORMATION processInformation, __in LPCWSTR lpcwszDll)
{

    SIZE_T nLength;
    LPVOID lpLoadLibraryW = NULL;
    LPVOID lpRemoteString;

    HANDLE hProcess = processInformation.hProcess;

    lpLoadLibraryW = GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryW");

    if (!lpLoadLibraryW)
    {
        PrintError(TEXT("GetProcAddress"));
        // close process handle
        CloseHandle(hProcess);
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









BOOL ListProcessThreads(DWORD dwOwnerPID)
{
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
    THREADENTRY32 te32;

    // Take a snapshot of all running threads  
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
        return(FALSE);

    // Fill in the size of the structure before using it. 
    te32.dwSize = sizeof(THREADENTRY32);

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if (!Thread32First(hThreadSnap, &te32))
    {
        PrintError(TEXT("Thread32First")); // show cause of failure
        CloseHandle(hThreadSnap);          // clean the snapshot object
        return(FALSE);
    }

    

    
    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    do
    {
        if (te32.th32OwnerProcessID == dwOwnerPID)
        {
            _tprintf(TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
            _tprintf(TEXT("\n     Base priority  = %d"), te32.tpBasePri);
            _tprintf(TEXT("\n     Delta priority = %d"), te32.tpDeltaPri);
            _tprintf(TEXT("\n"));
        }
    } while (Thread32Next(hThreadSnap, &te32));
    

    CloseHandle(hThreadSnap);
    return(TRUE);
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