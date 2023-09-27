#include "Lab2.h"

HINSTANCE hInst;
HWND hwndStartButton;
HWND hwndStopButton;
HWND hwndShapeComboBox;
HWND hwndSpeedEdit;   
HWND hwndTrajectoryComboBox;
HWND hwndMainWnd;

HHOOK g_keyboardHook = NULL;

bool isAnimating = false; 
double currentX = 100; 
double currentY = 100;
int objectWidth = 50;

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            if (kbStruct->vkCode == VK_SPACE) {
                if (isAnimating)
                {
                    isAnimating = false;
                    KillTimer(hwndMainWnd, 1);
                    EnableWindow(hwndStartButton, TRUE);
                    EnableWindow(hwndStopButton, FALSE);
                }
                else
                {
                    isAnimating = true;
                    SetTimer(hwndMainWnd, 1, 50, NULL);
                    EnableWindow(hwndStartButton, FALSE);
                    EnableWindow(hwndStopButton, TRUE);
                }
            }
        }
    }

    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        hwndMainWnd = hwnd;

        hwndStartButton = CreateWindow(
            L"BUTTON", L"Старт", WS_CHILD | WS_VISIBLE,
            10, 10, 80, 30, hwnd, (HMENU)1, hInst, NULL
        );

        hwndStopButton = CreateWindow(
            L"BUTTON", L"Стоп", WS_CHILD | WS_VISIBLE | WS_DISABLED,
            100, 10, 80, 30, hwnd, (HMENU)2, hInst, NULL
        );

        hwndShapeComboBox = CreateWindow(
            L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            10, 50, 170, 100, hwnd, (HMENU)3, hInst, NULL
        );

        SendMessage(hwndShapeComboBox, CB_ADDSTRING, 0, (LPARAM)L"Круг");
        SendMessage(hwndShapeComboBox, CB_ADDSTRING, 0, (LPARAM)L"Квадрат");
        SendMessage(hwndShapeComboBox, CB_SETCURSEL, 0, 0);

        hwndSpeedEdit = CreateWindow(
            L"EDIT", L"200.0", WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 90, 80, 20, hwnd, (HMENU)4, hInst, NULL
        );

        hwndTrajectoryComboBox = CreateWindow(
            L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            200, 10, 170, 100, hwnd, (HMENU)6, hInst, NULL
        );

        SendMessage(hwndTrajectoryComboBox, CB_ADDSTRING, 0, (LPARAM)L"Прямая");
        SendMessage(hwndTrajectoryComboBox, CB_ADDSTRING, 0, (LPARAM)L"Кривая");
        SendMessage(hwndTrajectoryComboBox, CB_SETCURSEL, 0, 0);

        g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(NULL), 0);
    
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rect, hBackgroundBrush);
        DeleteObject(hBackgroundBrush);

        int selectedIndex = SendMessage(hwndShapeComboBox, CB_GETCURSEL, 0, 0);

        COLORREF color = RGB(255, 0, 0);

        if (selectedIndex == 0)
        {
            HBRUSH hBrush = CreateSolidBrush(color);
            SelectObject(hdc, hBrush);
            Ellipse(hdc, (int)currentX, (int)currentY, (int)(currentX + objectWidth), (int)(currentY + objectWidth));
            DeleteObject(hBrush);
        }
        else
        {
            HBRUSH hBrush = CreateSolidBrush(color);
            SelectObject(hdc, hBrush);
            Rectangle(hdc, (int)currentX, (int)currentY, (int)(currentX + objectWidth), (int)(currentY + objectWidth));
            DeleteObject(hBrush);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1:
            if (!isAnimating) {
                isAnimating = true;
                SetTimer(hwnd, 1, 50, NULL);
                EnableWindow(hwndStartButton, FALSE);
                EnableWindow(hwndStopButton, TRUE);
            }
            break;
        case 2:
            isAnimating = false;
            KillTimer(hwnd, 1);
            EnableWindow(hwndStartButton, TRUE);
            EnableWindow(hwndStopButton, FALSE);
            break;
        case 3:
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);

        if (g_keyboardHook != NULL) {
            UnhookWindowsHookEx(g_keyboardHook);
            g_keyboardHook = NULL;
        }
         
        break;
    case WM_TIMER:
        if (isAnimating) {
            int selectedTrajectory = SendMessage(hwndTrajectoryComboBox, CB_GETCURSEL, 0, 0);

            wchar_t speedBuffer[32];
            GetWindowText(hwndSpeedEdit, speedBuffer, sizeof(speedBuffer));
            double speed = _wtof(speedBuffer);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            int clientWidth = clientRect.right - clientRect.left;
            int clientHeight = clientRect.bottom - clientRect.top;

            switch (selectedTrajectory) {
            case 0:
                currentX += speed * 0.01;

                if (currentX >= clientWidth) {
                    currentX = -objectWidth;
                }
                break;
            case 1:
                currentX += speed * 0.01;
                currentY += (clientHeight / 100) * sin(currentX * 0.05);

                if (currentY < 0) 
                {
                    currentY = clientHeight;
                }
                if (currentY > clientHeight)
                {
                    currentY = 0;
                }

                if (currentX >= clientWidth)
                {
                    currentX = -objectWidth;
                }
                break;
            }

            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"MyClass", NULL };
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindow(L"MyClass", L"Графическое приложение", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
