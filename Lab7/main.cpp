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
#define STEP 1
#define BTN_ID 10000
#define RBTN_ID 20000
#define N 1600
#define M 3
#define BTN_WIDTH 120
#define BTN_HEIGHT 30
#define VISIBLE_BTN_COUNT 100

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
    static HWND btns[VISIBLE_BTN_COUNT];
	static HWND rbtns[VISIBLE_BTN_COUNT];
    static int startIndex = 0;
    static int selectedIndex = -1;

    switch (message) {
    case WM_CREATE: {
        for (int i = 0; i < VISIBLE_BTN_COUNT; i++){
            CreateWindow("BUTTON", "",
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                0, i * BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(BTN_ID + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

            CreateWindow("BUTTON", "",
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                BTN_WIDTH + 20, i* BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(RBTN_ID + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        }

		selectedIndex = M - 1;
        std::string btn_name;
        std::string rbtn_name;

        for (int i = 0; i < VISIBLE_BTN_COUNT; i++) {
            btn_name = "Кнопка " + std::to_string(i + startIndex + 1);
            rbtn_name = "Радио " + std::to_string(i + startIndex + 1);
            SetWindowText(btns[i] = GetDlgItem(hWnd, BTN_ID + i), btn_name.c_str());
            SetWindowText(rbtns[i] = GetDlgItem(hWnd, RBTN_ID + i), rbtn_name.c_str());

			SetWindowLongPtr(btns[i], GWLP_ID, BTN_ID + i + startIndex);
			SetWindowLongPtr(rbtns[i], GWLP_ID, RBTN_ID + i + startIndex);

            SendMessage(btns[i], BM_SETSTATE, (selectedIndex == i + startIndex), 0);
			SendMessage(rbtns[i], BM_SETCHECK, (selectedIndex == i + startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);
		}

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
        if (id >= BTN_ID && id < BTN_ID + N) {
			selectedIndex = id - BTN_ID;
        }
        if (id >= RBTN_ID && id < RBTN_ID + N){
			selectedIndex = id - RBTN_ID;
        }

        for (int i = 0; i < VISIBLE_BTN_COUNT; i++) {
			SendMessage(btns[i], BM_SETSTATE, (selectedIndex == i + startIndex), 0);
			SendMessage(rbtns[i], BM_SETCHECK, (selectedIndex == i + startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        return 0;
    }
    case WM_SIZE: {
        RECT cleintRect;
		GetClientRect(hWnd, &cleintRect);

		int visibleBtn = (cleintRect.bottom - cleintRect.top) / BTN_HEIGHT;
		if (visibleBtn < 1) { visibleBtn = 1; }
		if (visibleBtn > VISIBLE_BTN_COUNT) { visibleBtn = VISIBLE_BTN_COUNT; }
		if (visibleBtn > N) { visibleBtn = N; }

        if (startIndex > N - visibleBtn) { startIndex = N - visibleBtn; }
		if (startIndex < 0) { startIndex = 0; }

        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        vScroll.nMin = 0;
        vScroll.nMax = N - 1;
        vScroll.nPage = visibleBtn;
		vScroll.nPos = startIndex;
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        return 0;
    }
    case WM_VSCROLL: {
        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &vScroll);
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

        if (vScroll.nPos < vScroll.nMin) { vScroll.nPos = vScroll.nMin; }
		if (vScroll.nPos > vScroll.nMax) { vScroll.nPos = vScroll.nMax; }

        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        startIndex = vScroll.nPos;
		int visibleBtn = vScroll.nPage;
        std::string btn_name;
        std::string rbtn_name;
        for (int i = 0; i < VISIBLE_BTN_COUNT; i++) {
            if (i >= visibleBtn || i + startIndex >= N)
            {
                ShowWindow(btns[i], SW_HIDE);
                ShowWindow(rbtns[i], SW_HIDE);
                continue;
            }
            ShowWindow(btns[i], SW_SHOW);
            ShowWindow(rbtns[i], SW_SHOW);

			btn_name = "Кнопка " + std::to_string(i + startIndex + 1);
            rbtn_name = "Радио " + std::to_string(i + startIndex + 1);
            SetWindowText(btns[i], btn_name.c_str());
            SetWindowText(rbtns[i], rbtn_name.c_str());

			SetWindowLongPtr(btns[i], GWLP_ID, BTN_ID + i + startIndex);
			SetWindowLongPtr(rbtns[i], GWLP_ID, RBTN_ID + i + startIndex);

			SendMessage(btns[i], BM_SETSTATE, (selectedIndex == i + startIndex), 0);
			SendMessage(rbtns[i], BM_SETCHECK, (selectedIndex == i + startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);
        }

        return 0;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================
