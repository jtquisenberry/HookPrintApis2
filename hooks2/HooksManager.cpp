#include <fstream>
#include "HooksManager.h"
#include <string>
#include <synchapi.h>

using namespace std;

bool HooksManager::isInitialized = false;

BOOL(__cdecl *HooksManager::HookFunction)(ULONG_PTR OriginalFunction, ULONG_PTR NewFunction);
VOID(__cdecl *HooksManager::UnhookFunction)(ULONG_PTR Function);
ULONG_PTR(__cdecl *HooksManager::GetOriginalFunction)(ULONG_PTR Hook);
DWORD FakeGetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
BOOL FakeTextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c);
BOOL FakeExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx);

//FILE* log2;


HooksManager::HooksManager()
{
    fputs("Top of HooksManager()\n", log_file);
    
    if (!isInitialized)
    {
        fputs("Top of HooksManager(), !isInitialized\n", log_file);
        
        if (HookFunction == NULL || UnhookFunction == NULL || GetOriginalFunction == NULL)
        {
            fputs("Top of HooksManager(), if(HookFunction)\n", log_file);
            
            HMODULE hHookEngineDll = LoadLibrary(L"D:\\Development\\git\\HookPrintApis2\\x64\\Debug\\NtHookEngine.dll");
            fputs("eeeee\n", log_file);
            fprintf(log_file, "puts\n");
            fprintf(log_file, "hHookEngineDll %p\n", hHookEngineDll);
            int error = GetLastError();
            fprintf(log_file, "Error: %d\n", error);

            fputs("HooksManager(), if(HookFunction) 2\n", log_file);

            HookFunction = (BOOL(__cdecl *)(ULONG_PTR, ULONG_PTR)) GetProcAddress(hHookEngineDll, "HookFunction");

            fputs("HooksManager(), if(HookFunction) 3\n", log_file);

            UnhookFunction = (VOID(__cdecl *)(ULONG_PTR)) GetProcAddress(hHookEngineDll, "UnhookFunction");

            fputs("HooksManager(), if(HookFunction) 4\n", log_file);

            GetOriginalFunction = (ULONG_PTR(__cdecl *)(ULONG_PTR)) GetProcAddress(hHookEngineDll, "GetOriginalFunction");

            fputs("HooksManager(), if(HookFunction) 5\n", log_file);
        }

        fputs("HooksManager(), A1\n", log_file);

        isInitialized = true;

        fputs("HooksManager(), A2\n", log_file);

        hookFunctions();

        fputs("HooksManager(), A3\n", log_file);
    }

    fputs("Bottom of HooksManager()\n", log_file);
}

HooksManager::~HooksManager()
{
    removeHooks();
}

void HooksManager::hookFunctions() {
    
    fputs("Top of hookFunctions()\n", log_file);

    fprintf(log_file, "%p %p %p\n", HookFunction, UnhookFunction, GetOriginalFunction);


    
    if (HookFunction == NULL || UnhookFunction == NULL || GetOriginalFunction == NULL)
        return;

    fputs("hookFunctions\n", log_file);

    // hLibrary = LoadLibrary(L"Iphlpapi.dll");
    hLibrary = LoadLibrary(L"gdi32full.dll");
    if (hLibrary == NULL) {
        fputs("hookFunctions if hLibrary\n", log_file);
        return;
    }

    // BOOL(__cdecl *HooksManager::HookFunction)(ULONG_PTR OriginalFunction, ULONG_PTR NewFunction);
    // HookFunction((ULONG_PTR)GetProcAddress(hLibrary, "GetAdaptersInfo"), (ULONG_PTR)FakeGetAdaptersInfo);
    // HookFunction((ULONG_PTR)GetProcAddress(hLibrary, "TextOutA"), (ULONG_PTR)FakeTextOutA);
    HookFunction((ULONG_PTR)GetProcAddress(hLibrary, "ExtTextOutW"), (ULONG_PTR)FakeExtTextOutW);
    fputs("Bottom of hookFunctions\n", log_file);
}
void HooksManager::removeHooks()
{
    if (HookFunction != NULL && UnhookFunction != NULL && GetOriginalFunction != NULL && hLibrary != NULL)
    {
        UnhookFunction((ULONG_PTR)GetProcAddress(hLibrary, "GetAdaptersInfo"));
    }
}



BOOL FakeExtTextOutW(HDC hdc, int x, int y, UINT options, const RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
{
    FILE* log2 = fopen("d:\\projects\\thefile2.txt", "a+");
    fputs("\n", log2);
    fputs("FakeExtTextOutW\n", log2);
    fprintf(log2, "%s\n", lpString);
    fprintf(log2, "%d\n", x);
    fprintf(log2, "%d\n", y);
    fprintf(log2, "%d\n", lprect->left);
    fprintf(log2, "%d\n", lprect->right);
    fprintf(log2, "%d\n", lprect->top);
    fprintf(log2, "%d\n", lprect->bottom);
    fprintf(log2, "%p\n", hdc);
    fputs("\n", log2);
    fclose(log2);
    return 0;


}





BOOL FakeTextOutA(HDC hdc, int x, int y, LPCSTR lpString, int c)
{
    FILE* log2 = fopen("d:\\projects\\thefile2.txt", "a+");
    fputs("\n", log2);
    fputs("TextOutA\n", log2);
    fputs("\n", log2);
    return 777;
}


DWORD FakeGetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
    FILE* log2 = fopen("d:\\projects\\thefile2.txt", "a+");
    fputs("cccccccccc\n", log2);
    
    Sleep(2000);

    cout << "\n";
    cout << "Start";
    cout << "\nCharacters";
    cout << "\n";
            
    for (int i = 0; i < 131; i++)
    {
        cout << pAdapterInfo->Description[i];
        cout << " ";
    }

    cout << "\n";
    cout << "\nString";
    cout << "\n";

    printf("\tDescription: \t%s\n", pAdapterInfo->Description);
    
    cout << "\n";
    cout << "\nInts";
    cout << "\n";

    for (int i = 0; i < 131; i++)
    {
        cout << (int)pAdapterInfo->Description[i];
        cout << " ";
    }
    cout << "\n";
    cout << "\n";
    cout << "\n";

    fclose(log2);







    DWORD(*OriginalGetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);

    OriginalGetAdaptersInfo = (DWORD(*)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)) HooksManager::GetOriginalFunction((ULONG_PTR)FakeGetAdaptersInfo);

    DWORD result = OriginalGetAdaptersInfo(pAdapterInfo, pOutBufLen);
    std::string fakeAdapterName = "{11111111-2222-3333-4444-555555555555}";
    std::string fakeAdapterDescription = "Fake Adapter 0001";

    if (pAdapterInfo != NULL)
    {
        strcpy_s(pAdapterInfo->AdapterName, sizeof(pAdapterInfo->AdapterName), fakeAdapterName.c_str());
        strcpy_s(pAdapterInfo->Description, sizeof(pAdapterInfo->Description), fakeAdapterDescription.c_str());

        for (int i = 0; i < sizeof(pAdapterInfo->Address); i++)
        {
            pAdapterInfo->Address[i] = (BYTE)i;
        }
    }

    return result;
}