#include <Windows.h>
#include <stdlib.h>
#include <commctrl.h>
#include <string>
#include <vector>

using namespace std;

#define MAX_COL 10
#define MAX_ROW 10
#define PADDING 20

int cols = 5, rows = 5;

const wchar_t CLASS_NAME[] = L"MyWindowClass";

RECT MainRect;

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void CreateTable(HDC hdc, int tableWidth, int TextBlockHeight, int rowsCount, int colCount);

vector<vector<string>> stringMatrix = {
	{"ads", "dsfsd", "sdf", "ds", "fdsa"},
	{"ads", "dsfsd", "sdf", "ds", "wryi"},
	{"ads", "dkkkkkkkkkkkkkyghjiuygvhjgiyufghi7ygudasdasdasdsadd", "sdf", "ds", "fdsa"},
	{"ads", "dkkkkkkkkkkkkkyghjiuygvhjgiyufghi7ygudasdasdasdsadd", "sdf", "ds", "fdsa"},
	{"ads", "dsfsd", "sdf", "ds", "fdsa"}
};

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
		TEXTMETRIC textMetric;
		GetTextMetrics(hCmpDC, &textMetric);
		int BlockHeight = 2 * textMetric.tmHeight;
		LOGBRUSH br;
		br.lbStyle = BS_SOLID;
		br.lbColor = 0xEECCCC;
		HBRUSH brush, SpriteBrush;
		brush = CreateBrushIndirect(&br);
		FillRect(hCmpDC, &MainRect, brush);
		DeleteObject(brush);
		CreateTable(hCmpDC, MainRect.right - MainRect.left, BlockHeight, rows, cols);
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, MainRect.right - MainRect.left, MainRect.bottom - MainRect.top, hCmpDC, 0, 0, SRCCOPY);;
		DeleteDC(hCmpDC);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		InvalidateRect(hwnd, nullptr, TRUE);
		break;
	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

string GetLongestString(vector<string> row, size_t startIndex)
{
	if (startIndex >= row.size())
	{
		return "";
	}
	string longestString = row[startIndex];
	for (size_t j = startIndex; j < startIndex + cols; j++)
	{
		if (row[j].length() > longestString.length())
			longestString = row[j];
	}

	return longestString;
}

void CreateTable(HDC hdc, int tableWidth, int textBlockHeight, int rowsCount, int colCount)
{
	int TextBlockWidth = (MainRect.right - MainRect.left) / colCount;
	int totalHeight = textBlockHeight / 2;
	for (int i = 0; i < rowsCount; i++) 
	{
		string longestString = GetLongestString(stringMatrix[i], stringMatrix[i].size());
		int maxRowHeight = textBlockHeight;
		for (int j = 0; j < colCount; ++j) 
		{
			RECT Block;
			Block.left = j * TextBlockWidth;
			Block.right = Block.left + TextBlockWidth;
			Block.top = totalHeight;
			Block.bottom = Block.top + textBlockHeight;
			wstring line = wstring(stringMatrix[i][j].begin(), stringMatrix[i][j].end());
			int currentBlockHeight = DrawText(hdc, line.c_str(), -1, &Block, DT_WORDBREAK | DT_CENTER | DT_END_ELLIPSIS);
			maxRowHeight = currentBlockHeight > maxRowHeight ? currentBlockHeight : maxRowHeight;
		}
		MoveToEx(hdc, 0, totalHeight - textBlockHeight / 2, nullptr);
		LineTo(hdc, MainRect.right, totalHeight - textBlockHeight / 2);
		totalHeight += maxRowHeight + textBlockHeight / 2;
	}
}