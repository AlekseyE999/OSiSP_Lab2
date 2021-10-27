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
void CreateTable(HDC hdc, int TextBlockHeight, int rowsCount, int colCount);
int GetRowHeight(HDC hdc, vector<string> row, int colWidth);
string GetLongestString(vector<string> row, size_t count);

vector<vector<string>> stringMatrix = {
	{"The lives of guards", "The presidencies", "on Death Row are affected", "to be reunited with", " by one of their charges"},
	{"a black man accused", "dsfsd", "the Vietnam War, the Watergate scandal", "of child murder ", "wryi"},
	{"and other historical events ", "dkkkkkkkkkkkk", " yet who has", "of an Alabama man with an IQ of 75", "fdsa"},
	{"ABS", "dkkkkkkkkkkkkkyghjiuygvhjgiyufghi7ygudasdasdasdsadd", "sdf", "a mysterious gift", "unfold from the perspective"},
	{"whose only desire is", "Kennedy and Johnson", "ZeWardo", "his childhood sweetheart.", "and rape"}
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
		CreateTable(hCmpDC, BlockHeight, rows, cols);
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

string GetLongestString(vector<string> row, size_t count)
{
	string longestString = row[0];
	for (size_t j = 0; j < count; j++)
	{
		if (row[j].length() > longestString.length()) longestString = row[j];
	}
	return longestString;
}

int GetRowHeight(HDC hdc, vector<string> row, int colWidth)
{
	string longestString = GetLongestString(row, row.size());
	RECT nonDrawableBlock;
	nonDrawableBlock.left = 0;
	nonDrawableBlock.top = 0;
	nonDrawableBlock.bottom = 1;
	nonDrawableBlock.right = colWidth;
	wstring line = wstring(longestString.begin(), longestString.end());
	int Blockheight = DrawText(hdc, line.c_str(), line.length(), &nonDrawableBlock, DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER);
	return Blockheight;
}

void CreateTable(HDC hdc, int textBlockHeight, int rowsCount, int colCount)
{
	int TextBlockWidth = (MainRect.right - MainRect.left) / colCount;
	int totalHeight = textBlockHeight/2;
	for (int i = 0; i < rowsCount; i++) 
	{
		int maxRowHeight = textBlockHeight;
		int BlockHeight = GetRowHeight(hdc, stringMatrix[i], TextBlockWidth);
		for (int j = 0; j < colCount; ++j) 
		{
			RECT Block;
			Block.left = j * TextBlockWidth;
			Block.right = Block.left + TextBlockWidth;
			Block.top = totalHeight;
			Block.bottom = Block.top + BlockHeight;
			wstring line = wstring(stringMatrix[i][j].begin(), stringMatrix[i][j].end());
			int currentBlockHeight = DrawText(hdc, line.c_str(), line.length(), &Block, DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER | DT_END_ELLIPSIS);
			if (currentBlockHeight > maxRowHeight) maxRowHeight = currentBlockHeight;
		}
		MoveToEx(hdc, 0, totalHeight - textBlockHeight/2, nullptr);
		LineTo(hdc, MainRect.right, totalHeight - textBlockHeight/2);
		totalHeight += maxRowHeight + textBlockHeight/2;
	}
	MoveToEx(hdc, 0, totalHeight - textBlockHeight/2, nullptr);
	LineTo(hdc, MainRect.right, totalHeight - textBlockHeight/2);
}