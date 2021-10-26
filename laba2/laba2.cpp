#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

#define MAX_COL 10
#define MAX_ROW 10

const wchar_t CLASS_NAME[] = L"MyWindowClass";

RECT MainRect;

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR args, int nCmdShow)
{
	//MessageBox(NULL, L"HELLO", "meafecd", MB_OK);
	HWND hwnd;
	WNDCLASS wc = { };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindProc;
	wc.hInstance = hInst;
	wc.lpszClassName = CLASS_NAME;


	if (!RegisterClass(&wc))
		return -1;
	hwnd = CreateWindowEx(0, CLASS_NAME, L"TextTable", WS_OVERLAPPEDWINDOW, 100, 100, 500, 500, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	MSG msg = { };
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc, hCmpDC;
	HBITMAP bitmap;
	switch (msg)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &MainRect);
		hCmpDC = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, MainRect.right - MainRect.left, MainRect.bottom - MainRect.top);
		SelectObject(hCmpDC, bitmap);
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0xEECCCC;
		HBRUSH brush, SpriteBrush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &MainRect, brush);
		DeleteObject(brush);
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, MainRect.right - MainRect.left, MainRect.bottom - MainRect.top, hCmpDC, 0, 0, SRCCOPY);;
		DeleteDC(hCmpDC);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}

	case WM_SIZE:
		GetClientRect(hwnd, &MainRect);
		break;
	case WM_ERASEBKGND:
		return 1;
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}