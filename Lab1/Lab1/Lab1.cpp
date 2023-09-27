#ifndef UNICODE
#define UNICODE
#endif 

#include "Lab1.h"

HINSTANCE hInst;
WCHAR szTitle[] = L"Сапёр";
WCHAR szWindowClass[] = L"MinesweeperClass";

const int cellSize = 40;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;

	PlaceMines();

	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szWindowClass;

	if (!RegisterClassExW(&wcex))
	{
		return 1;
	}

	HWND hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		415,
		440,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd)
	{
		return 2;
	}

	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		for (int y = 0; y < boardHeight; y++)
		{
			for (int x = 0; x < boardWidth; x++)
			{
				HWND hButton = CreateWindow(
					L"BUTTON",
					L"",
					WS_CHILD | WS_VISIBLE |  BS_PUSHBUTTON,
					x * cellSize, y * cellSize, cellSize, cellSize,
					hWnd,
					(HMENU)(y * boardWidth + x + 1001),
					hInst,
					nullptr
				);

				InvalidateRect(hButton, NULL, TRUE);
			}
		}
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		if (wmId >= 1001 && wmId <= 1001 + boardHeight * boardWidth)
		{
			int x = (wmId - 1001) % boardWidth;
			int y = (wmId - 1001) / boardWidth;


			if (GetKeyState(VK_SHIFT) < 0)
			{
				MarkCell(x, y);
			}
			else
			{
				OpenCell(x, y);
				if (CheckWinOrLose() == 0)
				{
					const wchar_t* message = L"Поздравляем, вы выиграли!";
					MessageBox(NULL, message, L"Выигрыш!", MB_ICONINFORMATION | MB_OK);
				}
				else if (CheckWinOrLose() == 2)
				{
					const wchar_t* message = L"Вы проиграли.";
					MessageBox(NULL, message, L"Проигрыш", MB_ICONINFORMATION | MB_OK);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}
		else
		{
			switch (wmId)
			{
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		
		for (int y = 0; y < boardHeight; y++)
		{
			for (int x = 0; x < boardWidth; x++)
			{
				int buttonId = 1001 + y * boardWidth + x;
				HWND hButton = GetDlgItem(hWnd, buttonId);

				if (IsMarked(x, y))
				{
					SetWindowText(hButton, L"P");
				}
				else if (IsOpen(x, y))
				{
					if (HasMine(x, y))
					{
						SetWindowText(hButton, L"X");
					}
					else if (GetAdjacentMines(x, y) > 0)
					{
						std::wstring text = std::to_wstring(GetAdjacentMines(x, y));
						SetWindowText(hButton, text.c_str());
					}
					else
					{
						SetWindowText(hButton, L"0");
					}
				}
				else
				{
					SetWindowText(hButton, L"");
				}
			}
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
