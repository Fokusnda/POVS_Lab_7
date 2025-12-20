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

#define N 20000
#define M 3
#define BTN_WIDTH 100
#define BTN_HEIGHT 30

struct AppState {
    HWND* hBtns;
    HWND* hRbtns;
    int startIndex;
    int selectedIndex;
    int rBtnCount;
};

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
        AppState* state = new AppState;
		state->startIndex = 0;
		state->selectedIndex = M - 1;

        RECT rc;
		GetClientRect(hWnd, &rc);
        state->rBtnCount = GetSystemMetrics(SM_CYSCREEN) / BTN_HEIGHT;

        state->hBtns = new HWND[N];
        state->hRbtns = new HWND[N];

        std::string btnText;
        std::string rbtnText;

        for (int i = 0; i < state->rBtnCount; i++) {
			btnText = "Кнопка " + std::to_string(i + 1);
            rbtnText = "Радио " + std::to_string(i + 1);

            state->hBtns[i] = CreateWindow("BUTTON", btnText.c_str(),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                0, i * BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(ID_BTN + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

            state->hRbtns[i] = CreateWindow("BUTTON", rbtnText.c_str(),
                WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                10 + BTN_WIDTH, i * BTN_HEIGHT, BTN_WIDTH, BTN_HEIGHT,
                hWnd, (HMENU)(INT_PTR)(ID_BTN + i),
                (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        }

        SendMessage(state->hBtns[state->selectedIndex], BM_SETSTATE, TRUE, 0);
		SendMessage(state->hRbtns[state->selectedIndex], BM_SETCHECK, BST_CHECKED, 0);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)state);

        return 0;
    }
    case WM_SIZE: {
		AppState* state = (AppState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        int visibleBtn = clientRect.bottom / BTN_HEIGHT;
		if (visibleBtn < 1) { visibleBtn = 1; }
		else if (visibleBtn > N) { visibleBtn = N; }

        if (state->startIndex > N - visibleBtn) { state->startIndex = N - visibleBtn; }

        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        vScroll.nMin = 0;
        vScroll.nMax = N - 1;
        vScroll.nPage = visibleBtn;
        vScroll.nPos = state->startIndex;
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        std::string btnText;
        std::string rbtnText;
        for (int i = 0; i < state->rBtnCount; i++) {
            if (i >= visibleBtn || i + state->startIndex >= N)
            {
                ShowWindow(state->hBtns[i], SW_HIDE);
                ShowWindow(state->hRbtns[i], SW_HIDE);
                continue;
            }
            ShowWindow(state->hBtns[i], SW_SHOW);
            ShowWindow(state->hRbtns[i], SW_SHOW);

            btnText = "Кнопка " + std::to_string(i + state->startIndex + 1);
            rbtnText = "Радио " + std::to_string(i + state->startIndex + 1);
            SetWindowText(state->hBtns[i], btnText.c_str());
            SetWindowText(state->hRbtns[i], rbtnText.c_str());

            SetWindowLongPtr(state->hBtns[i], GWLP_ID, ID_BTN + i + state->startIndex);
            SetWindowLongPtr(state->hRbtns[i], GWLP_ID, ID_RBTN + i + state->startIndex);

            SendMessage(state->hBtns[i], BM_SETSTATE, (state->selectedIndex == i + state->startIndex), 0);
            SendMessage(state->hRbtns[i], BM_SETCHECK, (state->selectedIndex == i + state->startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        return 0;
    }
    case WM_VSCROLL: {
        AppState* state = (AppState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        SCROLLINFO vScroll = { sizeof(vScroll) };
        vScroll.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &vScroll);

        int newPos = state->startIndex;

        switch (LOWORD(wParam)) {
        case SB_LINEUP:     newPos = state->startIndex - 1; break;
        case SB_LINEDOWN:   newPos = state->startIndex + 1; break;
        case SB_PAGEUP:     newPos = state->startIndex - state->rBtnCount; break;
        case SB_PAGEDOWN:   newPos = state->startIndex + state->rBtnCount; break;
        case SB_THUMBTRACK: newPos = vScroll.nTrackPos; break;
        }

        if (newPos < 0) { newPos = 0; }
        if (newPos > (int)(N - vScroll.nPage)) { newPos = N - vScroll.nPage; }

        if (newPos == state->startIndex) { break; }

        state->startIndex = newPos;

        vScroll.fMask = SIF_POS;
        vScroll.nPos = state->startIndex;
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        int visibleBtn = vScroll.nPage;
        std::string btnText;
        std::string rbtnText;
        for (int i = 0; i < state->rBtnCount; i++) {
            if (i >= visibleBtn || i + state->startIndex >= N) {
                ShowWindow(state->hBtns[i], SW_HIDE);
                ShowWindow(state->hRbtns[i], SW_HIDE);
                continue;
            }

            btnText = "Кнопка " + std::to_string(i + state->startIndex + 1);
            rbtnText = "Радио " + std::to_string(i + state->startIndex + 1);
            SetWindowText(state->hBtns[i], btnText.c_str());
            SetWindowText(state->hRbtns[i], rbtnText.c_str());

            SetWindowLongPtr(state->hBtns[i], GWLP_ID, ID_BTN + i + state->startIndex);
            SetWindowLongPtr(state->hRbtns[i], GWLP_ID, ID_RBTN + i + state->startIndex);

            SendMessage(state->hBtns[i], BM_SETSTATE, (state->selectedIndex == i + state->startIndex), 0);
            SendMessage(state->hRbtns[i], BM_SETCHECK, (state->selectedIndex == i + state->startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);

            ShowWindow(state->hBtns[i], SW_SHOW);
            ShowWindow(state->hRbtns[i], SW_SHOW);
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
        AppState* state = (AppState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        UINT id = LOWORD(wParam);
        if (id >= ID_BTN && id < ID_BTN + N) {
            state->selectedIndex = id - ID_BTN;
        }
        if (id >= ID_RBTN && id < ID_RBTN + N) {
            state->selectedIndex = id - ID_RBTN;
        }

        for (int i = 0; i < state->rBtnCount; i++) {
            SendMessage(state->hBtns[i], BM_SETSTATE, (state->selectedIndex == i + state->startIndex), 0);
            SendMessage(state->hRbtns[i], BM_SETCHECK, (state->selectedIndex == i + state->startIndex) ? BST_CHECKED : BST_UNCHECKED, 0);
        }
        return 0;
    }
    case WM_DESTROY:
        AppState* state = (AppState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        delete[] state->hBtns;
        delete[] state->hRbtns;
        delete state;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);

        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================
