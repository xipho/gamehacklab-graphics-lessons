// GraphicsLessons.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "GraphicsLessons.h"

#pragma comment(lib, "msimg32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND mHwnd; // Main Window Handle
RECT clientRect;
HBITMAP pattern;
HBITMAP kitty;
BLENDFUNCTION blf = { AC_SRC_OVER, 0, 255, 0 };
bool forward = true;
int alpha = 0;
HDC hdc;
bool isDrawing = true;
HDC backDC;
HBITMAP backBMP;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawFrame();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_GRAPHICSLESSONS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHICSLESSONS));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHICSLESSONS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	pattern = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	kitty = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		100, 100, 1280, 720, nullptr, nullptr, hInstance, nullptr);



	if (!hWnd)
	{
		return FALSE;
	}

	mHwnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&DrawFrame, NULL, NULL, NULL);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, backDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		isDrawing = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawFrame() {

	GetClientRect(mHwnd, &clientRect);
	hdc = GetDC(mHwnd);
	backDC = CreateCompatibleDC(hdc);
	backBMP = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
	HGDIOBJ oldBMP = SelectObject(backDC, backBMP);
	ReleaseDC(mHwnd, hdc);

	while (isDrawing) {

		//FillRect(hdc, &clientRect, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
		HPEN newPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
		HBRUSH newBrush = CreatePatternBrush(pattern); //CreateSolidBrush(RGB(255, 255, 0));

		HGDIOBJ oldBrush = SelectObject(backDC, newBrush);
		HGDIOBJ oldPen = SelectObject(backDC, newPen);

		Rectangle(backDC, clientRect.left + 10, clientRect.top + 10, clientRect.right - 10, clientRect.bottom - 10);

		BITMAP kittyObject;
		GetObject(kitty, sizeof(BITMAP), &kittyObject);

		HDC memDC = CreateCompatibleDC(backDC);
		SelectObject(memDC, kitty);

		//BitBlt(hdc, 30, 50, kittyObject.bmWidth, kittyObject.bmHeight, memDC, 0, 0, SRCCOPY);
		//StretchBlt(hdc, 30, 50, kittyObject.bmWidth - 100, kittyObject.bmHeight + 25, memDC, 0, 0, 
		//				kittyObject.bmWidth, kittyObject.bmHeight, SRCCOPY);

		if (alpha >= 255) forward = false;
		if (alpha <= 0) forward = true;

		if (forward) alpha += 1;
		else alpha -= 1;

		blf.SourceConstantAlpha = alpha;
		AlphaBlend(backDC, 20, 50, kittyObject.bmWidth, kittyObject.bmHeight, memDC, 0, 0,
			kittyObject.bmWidth, kittyObject.bmHeight, blf);

		DeleteDC(memDC);

		SelectObject(backDC, oldPen);
		SelectObject(backDC, oldBrush);

		DeleteObject(newPen);
		DeleteObject(newBrush);

		RedrawWindow(mHwnd, NULL, NULL, RDW_INVALIDATE);

		Sleep(1000 / 60);
	}

	SelectObject(backDC, oldBMP);
	DeleteObject(backBMP);
	DeleteDC(backDC);
}