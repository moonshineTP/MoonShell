#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int seconds = 0;
HWND hLabel;

void UpdateLabel(HWND hwnd) {
    wchar_t buf[32];
    swprintf(buf, 32, L"%d", seconds);
    SetWindowTextW(hLabel, buf);
}

DWORD WINAPI TimerThread(LPVOID param) {
    HWND hwnd = (HWND)param;
    while (seconds > 0) {
        Sleep(1000);
        seconds--;
        PostMessage(hwnd, WM_APP + 1, 0, 0);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    // Parse argument
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc < 2) {
        MessageBoxW(NULL, L"Usage: countdown.exe <seconds>", L"Countdown", MB_OK | MB_ICONINFORMATION);
        return 0;
    }
    seconds = _wtoi(argv[1]);
    if (seconds <= 0) {
        MessageBoxW(NULL, L"Please enter a positive number of seconds.", L"Countdown", MB_OK | MB_ICONERROR);
        return 0;
    }

    // Register window class
    const wchar_t CLASS_NAME[] = L"CountdownWindow";
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClassW(&wc);

    // Create window
    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Countdown Timer",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL
    );
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);

    // Create label
    hLabel = CreateWindowW(
        L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        40, 30, 140, 40,
        hwnd, NULL, hInstance, NULL
    );
    HFONT hFont = CreateFontW(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
    SendMessageW(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    UpdateLabel(hwnd);

    // Start timer thread
    CreateThread(NULL, 0, TimerThread, hwnd, 0, NULL);

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    DeleteObject(hFont);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_APP + 1:
        UpdateLabel(hwnd);
        if (seconds == 0) {
            MessageBoxW(hwnd, L"Hết giờ!", L"Countdown", MB_OK | MB_ICONINFORMATION);
            PostQuitMessage(0);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}