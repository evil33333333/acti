#define _WIN32_WINNT 0x600
#include <iostream>
#include <array>
#include <vector>
#include <windows.h>
#include <shellapi.h>
#include <slpublic.h>
#include <rpc.h>
#include "Decoder.h"
#pragma comment(lib, "Slwga.lib")
#pragma comment(lib, "Rpcrt4.lib")
#define MAX_LOADSTRING 100
#define IDS_APP_TITLE			103
#define IDR_MAINFRAME			128
#define IDD_IACTIVATE_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_IACTIVATE			107
#define IDI_SMALL				108
#define IDC_IACTIVATE			109
#define IDC_MYICON				2
#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif

#define ACTIVATE 1337

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
HBITMAP MainImage;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void AddWindowComponents(HWND hwnd);
void ActivateWindows();
bool isActivated();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_IACTIVATE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IACTIVATE));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IACTIVATE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IACTIVATE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 500, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case ACTIVATE: {
                MessageBeep(MB_OK);
                ActivateWindows();
                break;
            }
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CREATE: {
        AddWindowComponents(hWnd);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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

void AddWindowComponents(HWND hwnd) {
    HWND buttonObject = CreateWindowExW(0L, L"Button", L"Activate Windows", WS_VISIBLE | WS_CHILD, 190, 200, 100, 30, hwnd, (HMENU)ACTIVATE, NULL, NULL);
    SendMessage(buttonObject, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
}

void ActivateWindows() {
    std::vector<unsigned char> digital_product_id = Decoder::GetDigitalProductId();
    std::string product_key = Decoder::DecodeSystemProductKey(digital_product_id);
    std::string slmgr_cmd = "/c slmgr /ipk " + product_key;
    ShellExecuteA(NULL, "open", "cmd", slmgr_cmd.c_str(), NULL, SW_HIDE);
    ShellExecuteA(NULL, "open", "cmd", "/c slmgr /skms s8.uk.to", NULL, SW_HIDE);
    ShellExecuteA(NULL, "open", "cmd", "/c slmgr /ato", NULL, SW_HIDE);
    if (isActivated()) {
        MessageBeep(MB_OK);
        MessageBoxA(NULL, "Payload completed. Windows has been activated.", "Activated", 0);
    }
    else {
        MessageBeep(MB_ERR_INVALID_CHARS);
        MessageBoxA(NULL, "Payload completed. Windows has not been activated.", "Activated", 0);
    }
}


bool isActivated()
{
    unsigned char uuid_bytes[] = { 0x35, 0x35, 0x63, 0x39, 0x32, 0x37, 0x33, 0x34, 0x2d, 0x64, 0x36,
                                0x38, 0x32, 0x2d, 0x34, 0x64, 0x37, 0x31, 0x2d, 0x39, 0x38, 0x33,
                                0x65, 0x2d, 0x64, 0x36, 0x65, 0x63, 0x33, 0x66, 0x31, 0x36, 0x30,
                                0x35, 0x39, 0x66, '\0'};
    GUID uuid;
    SL_GENUINE_STATE state;
    if (UuidFromString((RPC_WSTR)uuid_bytes, &uuid)) {
        SLIsGenuineLocal(&uuid, &state, nullptr);
        return state == SL_GEN_STATE_IS_GENUINE;
    }
    return false;
}
