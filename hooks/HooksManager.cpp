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
            fputs("Error above\n", log_file);

            HookFunction = (BOOL(__cdecl *)(ULONG_PTR, ULONG_PTR)) GetProcAddress(hHookEngineDll, "HookFunction");
            UnhookFunction = (VOID(__cdecl *)(ULONG_PTR)) GetProcAddress(hHookEngineDll, "UnhookFunction");
            GetOriginalFunction = (ULONG_PTR(__cdecl *)(ULONG_PTR)) GetProcAddress(hHookEngineDll, "GetOriginalFunction");
        }

        isInitialized = true;

        hookFunctions();
    }
}

HooksManager::~HooksManager()
{
    removeHooks();
}

void HooksManager::hookFunctions() {
    if (HookFunction == NULL || UnhookFunction == NULL || GetOriginalFunction == NULL)
        return;

    hLibrary = LoadLibrary(L"Iphlpapi.dll");
    if (hLibrary == NULL) {
        return;
    }

    HookFunction((ULONG_PTR)GetProcAddress(hLibrary, "GetAdaptersInfo"), (ULONG_PTR)FakeGetAdaptersInfo);
}
void HooksManager::removeHooks()
{
    if (HookFunction != NULL && UnhookFunction != NULL && GetOriginalFunction != NULL && hLibrary != NULL)
    {
        UnhookFunction((ULONG_PTR)GetProcAddress(hLibrary, "GetAdaptersInfo"));
    }
}

DWORD FakeGetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
    FILE* log2 = fopen("d:\\projects\\thefile2.txt", "w");
    
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