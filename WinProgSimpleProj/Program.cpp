#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <algorithm>

#define APPTITLE "Hello World"

#define COLOR_RED RGB(255, 0, 0)
#define COLOR_GREEN RGB(0,255,0)
#define COLOR_BLUE RGB(0,0,255)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_YELLOW RGB(255,255,0)

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 500
#define WINDOW_HEADER 40


BOOL InitInstance(HINSTANCE, int);
ATOM MyRegisterClass(HINSTANCE);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
void DrawStuff(HDC);

void line(int, int, int, int,  HDC, COLORREF);

void line(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color)
{
	y0 = WINDOW_HEIGHT - y0 - WINDOW_HEADER;
	y1 = WINDOW_HEIGHT - y1 - WINDOW_HEADER;
	/*x1 = x1 * 2;
	y1 = y1 * 2;*/
	/*for (int x = x0; x <= x1; x++)
	{
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0*(1. - t) + y1*t;
		SetPixel(hdc, x, y, color);
	}*/
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) // it is steep
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) // make it left-to-right
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;

	float derror2 = std::abs(dy) * 2;
	float error2 = 0.0;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			SetPixel(hdc, y, x, color);
		}
		else
		{
			SetPixel(hdc, x, y, color);
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

int GetWindowHeight(HDC hdc)
{
	BITMAP structBitmapHeader;
	memset(&structBitmapHeader, 0, sizeof(BITMAP));
	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);
	return structBitmapHeader.bmHeight;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	char *szHello = "Hello, World!";
	RECT rt;

	switch (message)
	{
		case WM_PAINT:
			// get the dimensions of the window
			GetClientRect(hWnd, &rt);

			// start drawing on device context
			hdc = BeginPaint(hWnd, &ps);

			// draw my stuff
			DrawStuff(hdc);

			// stop drawing
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;			
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// create windows class structure
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	// fill the struct with info
	wc.style						= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc				= (WNDPROC) WinProc;
	wc.cbClsExtra				= 0;
	wc.cbWndExtra				= 0;
	wc.hInstance				= hInstance;
	wc.hIcon						= NULL;
	wc.hCursor					= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground			= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName			= NULL;
	wc.lpszClassName			= APPTITLE;
	wc.hIconSm					= NULL;

	return RegisterClassEx(&wc);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	// create a new window
	hWnd = CreateWindow(
		APPTITLE,
		APPTITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void DrawStuff(HDC image)
{
	line(13, 20, 80, 40, image, COLOR_WHITE);
	line(20, 13, 40, 80, image, COLOR_RED);
	line(80, 40, 13, 20, image, COLOR_RED);
	line(150, 50, 180, 340, image, COLOR_BLUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	// main message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}