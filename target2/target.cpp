// target.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <wchar.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

/* Note: could also use malloc() and free() */
void printAdapterInfo();
void printToPrinter();
void printToPrinter2();

int __cdecl main()
{
    printToPrinter();
    
    /*
    while (true) {
        printAdapterInfo();
        _getch();
    }
    */


   
    return 0;
}


void printToPrinter2()
{
    HANDLE pHandle;

    DOC_INFO_1 pDocInfo;
    memset(&pDocInfo, 0, sizeof(pDocInfo));
    pDocInfo.pDatatype = L"RAW";
    pDocInfo.pDocName = L"MyPrintControl";
    pDocInfo.pOutputFile = NULL;

    PRINTER_DEFAULTS lpdef; // this is for the parameter in the OpenPrinter API.
    ZeroMemory(&lpdef, sizeof(lpdef));
    lpdef.DesiredAccess = PRINTER_ACCESS_USE;
    lpdef.pDatatype = L"RAW";


    DWORD  pcWritten = 0;
    //"\x1B(s3B18.90" // To print 18.90 in Bold.
//	char *cBuffer = "\x1b*p300x600YHelloWorld"; // Ec is used for the escape.
    char cBuffer[10] = "ToPrint";


    int x = -1;

    x = OpenPrinter(L"hp LaserJet 1320 PCL 6", &pHandle, NULL);
    int JobID = 0;

    JobID = StartDocPrinter(pHandle, 1, (LPBYTE)&pDocInfo);


    x = StartPagePrinter(pHandle);


    x = WritePrinter(pHandle, "\x1B(s3B18.90", 20, &pcWritten);


    //	x = WritePrinter(pHandle,cBuffer,sizeof(cBuffer),&pcWritten);
    //		MessageBox("In write printer loop","OK");


    //	if( x = WritePrinter(pHandle,"\f",(DWORD)1,&pcWritten))
    //		MessageBox("In Eject","BYE");

    EndPagePrinter(pHandle);
    EndDocPrinter(pHandle);
    ClosePrinter(pHandle);

}


void printToPrinter()
{
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/openprinter
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/documentproperties
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/getdefaultprinter

    // A pointer to a variable that receives a handle (not thread safe) to the open printer or print server object.
    HANDLE phPrinter;

    // A pointer to a DEVMODE structure that receives the printer configuration data specified by the user.
    DEVMODE* pDevModeOutput;

    // A pointer to a null-terminated string that specifies the name of the device for which the 
    // printer-configuration property sheet is displayed.
    wchar_t pDeviceName[MAX_PATH];

    // On input, specifies the size, in characters, of the pszBuffer buffer.On output, receives the size, in 
    // characters, of the printer name string, including the terminating null character.
    DWORD pcchBuffer(ARRAYSIZE(pDeviceName));
    
    GetDefaultPrinter(pDeviceName, &pcchBuffer);
    // Microsoft Print to PDF (redirected 1)
    // >>> pcchBuffer
    // 38
    
    OpenPrinter(pDeviceName, &phPrinter, NULL);
    // wprintf(pDeviceName)
    // Microsoft Print to PDF (redirected 1)
    // >>> phPrinter
    // 0x0000027ea837b5a0

    int size = DocumentProperties(NULL, phPrinter, pDeviceName, NULL, NULL, 0);
    // >>> size
    // 5420

    pDevModeOutput = (DEVMODE*)malloc(size);
    DocumentProperties(NULL, phPrinter, pDeviceName, pDevModeOutput, NULL, DM_OUT_BUFFER);
    // wprintf(pDevModeOutput->dmDeviceName);
    // Microsoft Print to PDF (redire

    HDC printerDC = CreateDC(L"WINSPOOL", pDeviceName, NULL, pDevModeOutput);
    // >>> printerDC
    // 0x00000000572140d1 {unused=??? }

    DOCINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    StartDoc(printerDC, &info);


    StartPage(printerDC);
    Rectangle(printerDC, 100, 100, 200, 200);
    EndPage(printerDC);
    EndDoc(printerDC);
    DeleteDC(printerDC);
    ClosePrinter(phPrinter);
}





// This is MSDN example: https://docs.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo?redirectedfrom=MSDN
void printAdapterInfo() {
    /* Declare and initialize variables */

    // It is possible for an adapter to have multiple
    // IPv4 addresses, gateways, and secondary WINS servers
    // assigned to the adapter. 
    //
    // Note that this sample code only prints out the 
    // first entry for the IP address/mask, and gateway, and
    // the primary and secondary WINS server for each adapter. 

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;
    
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return;
    }
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);
            printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            printf("\tAdapter Addr: \t");
            for (i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }
            printf("\tIndex: \t%d\n", pAdapter->Index);
            
            pAdapter = pAdapter->Next;
            printf("\n");
        }
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);

}

void ShowError(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();
}