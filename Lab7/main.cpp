// Test.cpp
//
#define STRICT
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#include <string>
#include "resource.h"

#define SCLASS "a;eohgqeruiopugoqeig"
#define WINDOW_WIDTH 0.2
#define WINDOW_HEIGHT 0.5
#define STEP 30
#define BTN_ID 10000
#define RBTN_ID 20000
#define N 160
#define M 3
#define BTN_WIDTH 120
#define BTN_HEIGHT 30

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI  WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE, // hPrevInstance,
    _In_ LPSTR, // lpCmdLine,
    _In_ int nCmdShow
)
{
    LPCTSTR szClass = SCLASS;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (::RegisterClass(&wc) == 0)
    {
        return -1;
    }

    double screenWidth = GetSystemMetrics(SM_CXSCREEN);
    double screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create Main window
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    HWND overlappedWindow = ::CreateWindow(szClass, "MainWindow", WS_OVERLAPPEDWINDOW | WS_VSCROLL,
        (int)((screenWidth - (screenWidth * WINDOW_WIDTH)) / 2),
        (int)((screenHeight - (screenHeight * WINDOW_HEIGHT)) / 2),
        (int)(screenWidth * WINDOW_WIDTH), (int)(screenHeight * WINDOW_HEIGHT),
        NULL, hMenu, hInstance, NULL);
    if (overlappedWindow == NULL) {
        return -1;
    }

    ::ShowWindow(overlappedWindow, nCmdShow);

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::DispatchMessage(&msg);
    }

    return 0;
}
//=========================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE: {
        std::string btn_name;
        std::string rbtn_name;
        for (int i = 0; i < N; i++)
        {
            btn_name = "Кнопка " + std::to_string(i + 1);
            CreateWindow("BUTTON", btn_name.c_str(),
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                0, 0 + i * BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(BTN_ID + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

            rbtn_name = "Радио " + std::to_string(i + 1);
            CreateWindow("BUTTON", rbtn_name.c_str(),
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                BTN_WIDTH + 20, 0 + i* BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(RBTN_ID + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        }

        SendDlgItemMessage(hWnd, BTN_ID + M-1, BM_CLICK, TRUE, 0);
        SendDlgItemMessage(hWnd, RBTN_ID + M - 1, BM_SETCHECK, BST_CHECKED, 0);

        return 0;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_FILE_EXIT: {
            DestroyWindow(hWnd);
            return 0;
        }
        }
        UINT id = LOWORD(wParam);
        if (id >= BTN_ID && id < BTN_ID_MAX) {
            for (int i = 0; i < N; i++)
            {
                SendDlgItemMessage(hWnd, RBTN_ID + i, BM_SETCHECK, BST_UNCHECKED, 0);
                SendDlgItemMessage(hWnd, BTN_ID + i, BM_SETSTATE, FALSE, 0);
            }
            SendDlgItemMessage(hWnd, RBTN_ID + id - BTN_ID, BM_SETCHECK, BST_CHECKED, 0);
            SendDlgItemMessage(hWnd, id, BM_SETSTATE, TRUE, 0);
            return 0;
        }
        if (id >= RBTN_ID && id < RBTN_ID_MAX){
            for (int i = 0; i < N; i++)
            {
                SendDlgItemMessage(hWnd, BTN_ID + i, BM_SETSTATE, FALSE, 0);
            }
            SendDlgItemMessage(hWnd, BTN_ID + id - RBTN_ID, BM_SETSTATE, TRUE, 0);
            return 0;
        }
        return 0;
    }
    case WM_SIZE: {
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE;
        vScroll.nMin = 0;
        vScroll.nMax = (N * BTN_HEIGHT) - (clientRect.bottom - clientRect.top);
        vScroll.nPage = clientRect.bottom - clientRect.top;
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_VSCROLL: {
        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &vScroll);
        int yPos = vScroll.nPos;
        int shift = 0;

        switch (LOWORD(wParam)) {
        case SB_LINEDOWN:
        case SB_PAGEDOWN: {
            shift = vScroll.nMax - vScroll.nPos;
            if (shift > STEP) {
                shift = STEP;
            }
            vScroll.nPos += shift;
            break;
        }
        case SB_LINEUP:
        case SB_PAGEUP: {
            shift = vScroll.nPos;
            if (shift > STEP) {
                shift = STEP;
            }
            shift = -shift;
            vScroll.nPos += shift;
            break;
        }
        case SB_THUMBTRACK: {
            shift = vScroll.nTrackPos - vScroll.nPos;
            vScroll.nPos = vScroll.nTrackPos;
            break;
        }
        }

        vScroll.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        ScrollWindow(hWnd, 0, yPos - vScroll.nPos, NULL, NULL);
        UpdateWindow(hWnd);

        return 0;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================
