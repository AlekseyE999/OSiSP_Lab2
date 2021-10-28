#include <Windows.h>
#include <stdlib.h>
#include <commctrl.h>
#include <string>
#include <vector>

using namespace std;

#define CHANGE_COL 7
#define CHANGE_ROW 8
#define MAX_COL 5
#define MAX_ROW 5
#define PADDINGBLOCK 50

int cols = 0, rows = 0;

const wchar_t CLASS_NAME[] = L"MyWindowClass";

RECT MainRect;

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
void CreateTable(HDC hdc, int TextBlockHeight, int rowsCount, int colCount);
int GetRowHeight(HDC hdc, vector<string> row, int startIndex, int colWidth);
string GetLongestString(vector<string> row, size_t count);

vector<string> stringMatrix = {
	"The lives of guards", "The presidencies", "on Death Row are affected", "to be reunited with", " by one of their charges",
	"a black man accused", "dsfsd", "the Vietnam War, the Watergate scandal", "of child murder ", "wryi",
	"and other historical events ", "dkkkkkkkkkkkk", " yet who has", "of an Alabama man with an IQ of 75", "fdsa",
	"ABS", "dkkkkkkkkkkkkkyghjiuygvhjgiyufghi7ygudasdasdasdsadd", "sdf", "a mysterious gift", "unfold from the perspective",
	"whose only desire is", "Kennedy and Johnson", "ZeWardo", "his childhood sweetheart.", "and rape"
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
	HWND hWndEditRow = CreateWindowEx(0, L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 70, 30, 120, 20, hwnd, (HMENU)CHANGE_ROW, NULL, NULL);
	HWND hWndEditColumn = CreateWindowEx(0, L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 230, 30, 120, 20, hwnd, (HMENU)CHANGE_COL, NULL, NULL);
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
		TextOut(hCmpDC, 90, 10, L"Row", lstrlen(L"Row"));
		TextOut(hCmpDC, 250, 10, L"Column", lstrlen(L"Column"));
		CreateTable(hCmpDC, BlockHeight, rows, cols);
		SetStretchBltMode(hdc, COLORONCOLOR);
		BitBlt(hdc, 0, 0, MainRect.right - MainRect.left, MainRect.bottom - MainRect.top, hCmpDC, 0, 0, SRCCOPY);;
		DeleteDC(hCmpDC);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wp))
		{
		case CHANGE_COL:
			switch (HIWORD(wp))
			{
			case EN_CHANGE:
			{
				wchar_t wc[255];
				LPWSTR buff;
				GetWindowText((HWND)lp, wc, 255);
				std::wstring ws(wc);
				std::string EnterNumber = std::string(ws.begin(), ws.end());
				if (EnterNumber != "") cols = std::stoi(EnterNumber);
				else cols = 0;
				if (cols <= MAX_COL) InvalidateRect(hwnd, &MainRect, NULL);
				else cols = 0;
			}
			}
			break;

		case CHANGE_ROW:
			switch (HIWORD(wp))
			{
			case EN_CHANGE:
			{
				wchar_t wc[255];
				LPWSTR buff;
				GetWindowText((HWND)lp, wc, 255);
				std::wstring ws(wc);
				std::string EnterNumber = std::string(ws.begin(), ws.end());
				if (EnterNumber != "") rows = std::stoi(EnterNumber);
				else rows = 0;
				if (rows <= MAX_ROW) InvalidateRect(hwnd, &MainRect, NULL);
				else rows = 0;
			}
			}
			break;
		}
		break;
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
	string longestString = row[startIndex];
	for (size_t j = startIndex; j < startIndex + cols; j++)
	{
		if (j >= row.size())
		{
			return longestString;
		}

		if (row[j].length() > longestString.length()) longestString = row[j];
	}
	return longestString;
}

int GetRowHeight(HDC hdc, vector<string> row, int startIndex, int colWidth)
{
	string longestString = GetLongestString(row, startIndex);
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
	if (cols < 1 || rows < 1)
	{
		return;
	}
	int TextBlockWidth = (MainRect.right - MainRect.left) / colCount;
	int totalHeight = textBlockHeight / 2 + PADDINGBLOCK * 2;
	for (int i = 0; i < rowsCount; i++)
	{
		int maxRowHeight = textBlockHeight;
		int BlockHeight = GetRowHeight(hdc, stringMatrix, i * cols, TextBlockWidth);
		for (int j = 0; j < colCount; j++)
		{
			int Index = i * cols + j;
			RECT Block;
			Block.left = j * TextBlockWidth;
			Block.top = totalHeight;
			Block.right = Block.left + TextBlockWidth;
			Block.bottom = Block.top + BlockHeight;
			wstring line = wstring(stringMatrix[Index].begin(), stringMatrix[Index].end());
			int currentBlockHeight = DrawText(hdc, line.c_str(), line.length(), &Block, DT_WORDBREAK | DT_EDITCONTROL | DT_CENTER | DT_END_ELLIPSIS);
			if (currentBlockHeight > maxRowHeight) maxRowHeight = currentBlockHeight;
			for (int k = 0; k < colCount + 1; k++)
			{
				MoveToEx(hdc, TextBlockWidth * k, totalHeight - textBlockHeight / 2, nullptr);
				LineTo(hdc, TextBlockWidth * k, totalHeight + maxRowHeight);
			}
			MoveToEx(hdc, TextBlockWidth * i, totalHeight - textBlockHeight / 2, nullptr);
			LineTo(hdc, TextBlockWidth * i, totalHeight + maxRowHeight);
		}
		MoveToEx(hdc, 0, totalHeight - textBlockHeight / 2, nullptr);
		LineTo(hdc, MainRect.right, totalHeight - textBlockHeight / 2);
		totalHeight += maxRowHeight + textBlockHeight / 2;
	}
	MoveToEx(hdc, 0, totalHeight - textBlockHeight / 2, nullptr);
	LineTo(hdc, MainRect.right, totalHeight - textBlockHeight / 2);
}