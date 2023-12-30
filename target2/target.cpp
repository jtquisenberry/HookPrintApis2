// target.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <wchar.h>
#include <vector>
#include <string>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

/* Note: could also use malloc() and free() */
void printAdapterInfo();
void printToPrinter(string dcSource, string logFile, string bitmapFile, string printerName);
int CaptureAnImagePrinter(HDC hDC, HBITMAP hbmScreen, HDC hdcWindow);
void DrawGraphics(HDC hDC);
void reportJob(HANDLE phPrinter, int jobId);
void drawPrinter(HANDLE phPrinter, HDC hDC);
void drawDesktop(HDC windowDC);
void workflowPrinterDcName(string printerName);
int CaptureAnImageDc(HDC hdcMemDC);
void workflowPrinterAndBmp();

int __cdecl main(int argc, char* argv[])
{
    std::vector<std::string> arguments;

    // --logfile D : \Projects\printer_log.txt --bitmap D : \Projects\printer_bmp.bmp --source DefaultPrinterr

    string logFile = "";
    string bitmapFile = "";
    string dcSource = "Default";    
    string printerName = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  -h, --help  Show this help message\n";
        }
        else if (arg == "-v" || arg == "--version") {
            std::cout << "MyProgram version 1.0\n";
        }
        else if (arg == "--logfile") {
            logFile = argv[i + 1];
        }
        else if (arg == "--bitmap") {
            bitmapFile = argv[i + 1];
        }
        else if (arg == "--printerName") {
            printerName = argv[i + 1];
        }
        else if (arg == "--source") {
            string temp = argv[i + 1];
            if (temp == "Default") {
                dcSource = temp;
            }
            else if (temp == "User") {
                dcSource = temp;
            }
            else if (temp == "Desktop") {
                dcSource = temp;
            }
            else if (temp == "Name") {
                dcSource = temp;
            }
            else if (temp == "Both") {
                dcSource = temp;
            }
            
        }
        else {
            arguments.push_back(arg);
        }
    }

    // Process custom arguments
    for (const std::string& arg : arguments) {
        // Add your custom argument processing logic here
        std::cout << "Custom argument: " << arg << "\n";
    }

    printToPrinter(dcSource, logFile, bitmapFile, printerName);
    
    /*
    while (true) {
        printAdapterInfo();
        _getch();
    }
    */
   
    return 0;
}


void workflowPrinterDcUser() {
    HDC printerDC = 0x0;
    
    PRINTDLG pd;
    HWND hwnd = GetDesktopWindow();

    // Initialize PRINTDLG
    ZeroMemory(&pd, sizeof(pd));
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner = hwnd;
    pd.hDevMode = NULL;     // Don't forget to free or store hDevMode.
    pd.hDevNames = NULL;     // Don't forget to free or store hDevNames.
    pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
    pd.nCopies = 1;
    pd.nFromPage = 0xFFFF;
    pd.nToPage = 0xFFFF;
    pd.nMinPage = 1;
    pd.nMaxPage = 0xFFFF;

    if (!(PrintDlg(&pd) == TRUE))
    {
        // ShowError(L"PrintDlg");
    }
    else
    {
        printerDC = pd.hDC;
    }

    PDEVMODE returnedDevmode = NULL;
    // In this example, the DEVMODE structure returned by 
    //    the printer dialog box is copied to a local memory
    //    block and a pointer to the printer name that is 
    //    stored in the copied DEVMODE structure is saved.
    //
    //  Lock the handle to get a pointer to the DEVMODE structure.
    returnedDevmode = (PDEVMODE)GlobalLock(pd.hDevMode);
    WCHAR* dmDeviceName = returnedDevmode->dmDeviceName;

    HANDLE pHandle;
    int status = -999;
    status = OpenPrinter(dmDeviceName, &pHandle, NULL);

    wprintf(L"\n");
    wprintf(L"hwndOwner: 0x%#016p\n", pd.hwndOwner);
    wprintf(L"nFromPage: %d\n", pd.nFromPage);
    wprintf(L"nToPage: %d\n", pd.nToPage);
    wprintf(L"nMinPage: %d\n", pd.nMinPage);
    wprintf(L"nMaxPage: %d\n", pd.nMaxPage);
    wprintf(L"nCopies: %d\n", pd.nCopies);
    wprintf(L"lpPrintTemplateName: %s\n", pd.lpPrintTemplateName);

    drawPrinter(pHandle, printerDC);
    ClosePrinter(pHandle);
    DeleteDC(printerDC);

    return;
}



void workflowPrinterDcDefault() {
    HANDLE phPrinter;
    
    HDC printerDC;
    
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
    wprintf(L"PRINTER\n");
    wprintf(L"Device Name: %s\n", pDeviceName);
    wprintf(L"Handle: 0x%#016p\n", phPrinter);
    wprintf(L"\n");
    // PRINTER
    // Device Name : Bullzip PDF Printer
    // Handle : 0x000002307C40ADE0

    int size = DocumentProperties(NULL, phPrinter, pDeviceName, NULL, NULL, 0);
    pDevModeOutput = (DEVMODE*)malloc(size);
    DocumentProperties(NULL, phPrinter, pDeviceName, pDevModeOutput, NULL, DM_OUT_BUFFER);
    // wprintf(pDevModeOutput->dmDeviceName);
    // Microsoft Print to PDF (redire
    wprintf(L"DocumentProperties\n");
    wprintf(L"dmDeviceName: %s\n", pDevModeOutput->dmDeviceName);
    wprintf(L"dmFormName: %s\n", pDevModeOutput->dmFormName);
    wprintf(L"dmPrintQuality: %d\n", pDevModeOutput->dmPrintQuality);
    wprintf(L"dmScale: %d\n", pDevModeOutput->dmScale);
    wprintf(L"dmCopies: %d\n", pDevModeOutput->dmCopies);
    wprintf(L"\n");
    // DocumentProperties
    // dmDeviceName: Bullzip PDF Printer
    // dmFormName : Letter
    // dmPrintQuality : 300
    // dmScale : 100
    // dmCopies : 1

    printerDC = CreateDC(L"WINSPOOL", pDeviceName, NULL, pDevModeOutput);
    wprintf(L"Device Context\n");
    wprintf(L"Handle: 0x%#016p\n", printerDC);
    wprintf(L"\n");
    // Device Context
    // Handle: 0x000000007C215306

    drawPrinter(phPrinter, printerDC);
    DeleteDC(printerDC);

    return;
}



void workflowPrinterDcName(string printerName) {
    HANDLE phPrinter;

    HDC printerDC;

    // A pointer to a DEVMODE structure that receives the printer configuration data specified by the user.
    DEVMODE* pDevModeOutput;

    // Convert string printerName to LPWSTR pDeviceName
    // Initializing an object of wstring
    wstring temp = wstring(printerName.begin(), printerName.end());
    // Applying c_str() method on temp
    LPWSTR pDeviceName = (LPWSTR)temp.c_str();

    OpenPrinter(pDeviceName, &phPrinter, NULL);
    wprintf(L"PRINTER\n");
    wprintf(L"Device Name: %s\n", pDeviceName);
    wprintf(L"Handle: 0x%#016p\n", phPrinter);
    wprintf(L"\n");
    // PRINTER
    // Device Name : Bullzip PDF Printer
    // Handle : 0x000002307C40ADE0

    int size = DocumentProperties(NULL, phPrinter, pDeviceName, NULL, NULL, 0);
    pDevModeOutput = (DEVMODE*)malloc(size);
    DocumentProperties(NULL, phPrinter, pDeviceName, pDevModeOutput, NULL, DM_OUT_BUFFER);
    // wprintf(pDevModeOutput->dmDeviceName);
    // Microsoft Print to PDF (redire
    wprintf(L"DocumentProperties\n");
    wprintf(L"dmDeviceName: %s\n", pDevModeOutput->dmDeviceName);
    wprintf(L"dmFormName: %s\n", pDevModeOutput->dmFormName);
    wprintf(L"dmPrintQuality: %d\n", pDevModeOutput->dmPrintQuality);
    wprintf(L"dmScale: %d\n", pDevModeOutput->dmScale);
    wprintf(L"dmCopies: %d\n", pDevModeOutput->dmCopies);
    wprintf(L"\n");
    // DocumentProperties
    // dmDeviceName: Bullzip PDF Printer
    // dmFormName : Letter
    // dmPrintQuality : 300
    // dmScale : 100
    // dmCopies : 1

    printerDC = CreateDC(L"WINSPOOL", pDeviceName, NULL, pDevModeOutput);
    wprintf(L"Device Context\n");
    wprintf(L"Handle: 0x%#016p\n", printerDC);
    wprintf(L"\n");
    // Device Context
    // Handle: 0x000000007C215306

    drawPrinter(phPrinter, printerDC);
    DeleteDC(printerDC);

    return;
}





void workflowDesktopDc() {
    // hWnd: A handle to the window whose DC is to be retrieved.If this value is NULL, GetDC retrieves the 
    // DC for the entire screen.
    HDC hDC = GetDC(NULL);
    drawDesktop(hDC);
    DeleteDC(hDC);
}

void drawPrinter(HANDLE phPrinter, HDC printerDC) {
    int status = -999; 
    int jobId = -999;
    
    DOCINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    jobId = StartDoc(printerDC, &info);
    reportJob(phPrinter, jobId);
    StartPage(printerDC);
    DrawGraphics(printerDC);

    HDC memDC2 = CreateCompatibleDC(printerDC);


    BITMAP structBitmapHeader;
    memset(&structBitmapHeader, 0, sizeof(BITMAP));
    BITMAP structBitmapHeader2;
    memset(&structBitmapHeader2, 0, sizeof(BITMAP));
    HGDIOBJ hBitmap = GetCurrentObject(printerDC, OBJ_BITMAP);
    GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);
    HGDIOBJ hBitmap2 = GetCurrentObject(memDC2, OBJ_BITMAP);
    GetObject(hBitmap2, sizeof(BITMAP), &structBitmapHeader2);
    int bmWidth = structBitmapHeader.bmWidth;
    int bmHeight = structBitmapHeader.bmHeight;
    int bmWidth2 = structBitmapHeader2.bmWidth;
    int bmHeight2 = structBitmapHeader2.bmHeight;





    HDC memDC = CreateCompatibleDC(printerDC);
    HBITMAP memBM = CreateCompatibleBitmap(printerDC, 400, 400);
    SelectObject(memDC, memBM);
    CaptureAnImagePrinter(memDC, memBM, printerDC);



    status = EndPage(printerDC);
    status = EndDoc(printerDC);
    status = DeleteDC(printerDC);

    return;
}

void drawDesktop(HDC windowDC) {
    DrawGraphics(windowDC);
    CaptureAnImageDc(windowDC);
}

void reportJob(HANDLE phPrinter, int jobId) {
    int status = -999;
    //int JobId = -999;

    int Level = 1;  // To retrieve a JOB_INFO_1 structure.
    JOB_INFO_1* pJobInfo = 0;
    DWORD cbBuf = 0;
    DWORD pcbNeeded;
    DWORD bytesNeeded = 0;
    status = GetJob(phPrinter, jobId, Level, (LPBYTE)pJobInfo, cbBuf, &pcbNeeded);
    pJobInfo = (JOB_INFO_1*)malloc(pcbNeeded);
    status = GetJob(phPrinter, jobId, Level, (LPBYTE)pJobInfo, pcbNeeded, &pcbNeeded);
    wprintf(L"JOB_INFO_1\n");
    wprintf(L"JobId: %d\n", pJobInfo->JobId);
    wprintf(L"pPrinterName: %s\n", pJobInfo->pPrinterName);
    wprintf(L"pMachineName: %s\n", pJobInfo->pMachineName);
    wprintf(L"Status: %d\n", pJobInfo->Status);
    wprintf(L"cbSize: %d\n", pJobInfo->Priority);
    wprintf(L"Position: %d\n", pJobInfo->Position);
    wprintf(L"TotalPages: %s\n", pJobInfo->TotalPages);
    wprintf(L"\n");
    // JOB_INFO_1
    // JobId: 45
    // pPrinterName : Bullzip PDF Printer
    // pMachineName : \\JQHOME08
    // Status : 8
    // cbSize : 1
    // Position : 18
    // TotalPages : (null)
    // Submitted : ?
    
    return;
}

void workflowPrinterAndBmp() {
    MessageBox(NULL, L"workflowPrinterAndBmp", L"workflowPrinterAndBmp", 1);
    return;
}


void printToPrinter(string dcSource, string logFile, string bitmapFile, string printerName)
{
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/openprinter
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/documentproperties
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/getdefaultprinter
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getdesktopwindow
    // https://learn.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes?redirectedfrom=MSDN

    // HDC hDC = 0x0;


    if (dcSource == "User") {
        workflowPrinterDcUser();
    }
    else if (dcSource == "Default") {
        workflowPrinterDcDefault();
    }
    else if (dcSource == "Name") {
        workflowPrinterDcName(printerName);
    }
    else if (dcSource == "Desktop") {
        workflowDesktopDc();
    }
    else if (dcSource == "Both") {
        workflowPrinterAndBmp();
    }
    else {
        workflowPrinterDcDefault();
    }



    return;
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

    //LPVOID lpMsgBuf;
    //LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();
}

// int CaptureAnImage(HWND hWnd)


void DrawGraphics(HDC hDC)
{
    int status = -999;

    HPEN pen = CreatePen(PS_SOLID, 10, RGB(0, 192, 0));
    HBRUSH brush = CreateSolidBrush(RGB(192, 0, 0));
    HBRUSH original = (HBRUSH)SelectObject(hDC, brush);

    TCHAR text[] = L"Defenestration can be hazardous";
    TextOut(hDC, 150, 150, text, ARRAYSIZE(text));

    status = Rectangle(hDC, 100, 100, 400, 200);
    status = Rectangle(hDC, 500, 500, 900, 900);

    return;
}

int CaptureAnImagePrinter(HDC hdcMemDC, HBITMAP hbmScreen, HDC hdcWindow)
{
    //HDC hdcScreen;
    //HDC hdcWindow;
    //HDC hdcMemDC = NULL;
    //HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;

    int result = -999;

    // Bit block transfer into our compatible memory DC.
    BitBlt(hdcMemDC,
        0, 0,
        400, 400,
        hdcWindow,
        0, 0,
        SRCCOPY);

    // Get the BITMAP from the HBITMAP.
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    hFile = CreateFile(L"d:\\captureqwsxA.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Close the handle for the file that was created.
    CloseHandle(hFile);

    // Clean up.
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    

    return 0;
}

int CaptureAnImageDc(HDC hdcWindow)
{
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char* lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;

    int result = -999;


    BITMAP structBitmapHeader;
    memset(&structBitmapHeader, 0, sizeof(BITMAP));
    HGDIOBJ hBitmap = GetCurrentObject(hdcWindow, OBJ_BITMAP);
    GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);
    int bmWidth = structBitmapHeader.bmWidth;
    int bmHeight = structBitmapHeader.bmHeight;

    // Create a compatible DC, which is used in a BitBlt from the window DC.
    hdcMemDC = CreateCompatibleDC(hdcWindow);

    BITMAP structBitmapHeader2;
    memset(&structBitmapHeader2, 0, sizeof(BITMAP));
    HGDIOBJ hBitmap2 = GetCurrentObject(hdcMemDC, OBJ_BITMAP);
    GetObject(hBitmap2, sizeof(BITMAP), &structBitmapHeader2);
    int bmWidth2 = structBitmapHeader2.bmWidth;
    int bmHeight2 = structBitmapHeader2.bmHeight;



    // Bit block transfer into our compatible memory DC.
    BitBlt(hdcMemDC,
        0, 0,
        bmWidth, bmHeight,
        hdcWindow,
        0, 0,
        SRCCOPY);

    // Create a compatible bitmap from the Window DC.
    hbmScreen = CreateCompatibleBitmap(hdcWindow, bmWidth, bmHeight);

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC, hbmScreen);

    // Bit block transfer into our compatible memory DC.
    BitBlt(hdcMemDC,
        0, 0,
        bmWidth, bmHeight,
        hdcWindow,
        0, 0,
        SRCCOPY);

    // Get the BITMAP from the HBITMAP.
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char*)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer 
    // that's pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
    hFile = CreateFile(L"d:\\captureqwsxA.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size.
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    // Size of the file.
    bmfHeader.bfSize = dwSizeofDIB;

    // bfType must always be BM for Bitmaps.
    bmfHeader.bfType = 0x4D42; // BM.

    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Close the handle for the file that was created.
    CloseHandle(hFile);

    // Clean up.
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);


    return 0;
}