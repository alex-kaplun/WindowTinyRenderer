#include <Windows.h>
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <time.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include "model.h"
#include <geometry.h>

#define APPTITLE "Hello World"

#define COLOR_RED RGB(255, 0, 0)
#define COLOR_GREEN RGB(0,255,0)
#define COLOR_BLUE RGB(0,0,255)
#define COLOR_WHITE RGB(255, 255, 255)
#define COLOR_YELLOW RGB(255,255,0)

#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define WINDOW_HEADER 40

Model *model = NULL;
const int width = 800;
const int height = 800;

const Vec3f light_dir(0, 0, -1);

BOOL InitInstance(HINSTANCE, int);
ATOM MyRegisterClass(HINSTANCE);
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
void DrawStuff(HDC);

void line(int, int, int, int,  HDC, COLORREF);
void line(Vec2i, Vec2i, HDC, COLORREF);
void triangle(Vec2i, Vec2i, Vec2i, HDC, COLORREF);


void line(int x0, int y0, int x1, int y1, HDC hdc, COLORREF color)
{
	y0 = WINDOW_HEIGHT - y0 - WINDOW_HEADER;
	y1 = WINDOW_HEIGHT - y1 - WINDOW_HEADER;
	
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
void line(Vec2i p0, Vec2i p1, HDC hdc, COLORREF color)
{
	line(p0.x, p0.y, p1.x, p1.y, hdc, color);
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, HDC hdc, COLORREF color)
{
	if (t0.y == t1.y && t0.y == t2.y)
		return;
	
	if (t0.y > t1.y)
		std::swap(t0, t1);
	if (t0.y > t2.y)
		std::swap(t0, t2);
	if (t1.y > t2.y)
		std::swap(t1, t2);

	/*line(t0, t1, hdc, COLOR_YELLOW);
	line(t1, t2, hdc, COLOR_YELLOW);
	line(t2, t0, hdc, COLOR_YELLOW);*/
	
	int total_height = t2.y - t0.y;
	for (int i=0; i <= total_height; i++)
	{
		bool is_upper_half = i > t1.y - t0.y;
		int segment_height = is_upper_half ? t2.y - t1.y + 1 : t1.y - t0.y + 1;
		float alpha =	(float)(i) / total_height;
		float beta =	(float)(i + (is_upper_half ? t0.y - t1.y : 0)) / segment_height;
 		Vec2i A =						t0 + (t2 - t0)*alpha;
		Vec2i B = is_upper_half ?	t1 + (t2 - t1)*beta : t0 + (t1 - t0)*beta;
		line(A.x, i + t0.y, B.x, i + t0.y, hdc, color);
	}

	/*line(t0, t1, hdc, COLOR_GREEN);
	line(t1, t2, hdc, COLOR_GREEN);
	line(t2, t0, hdc, COLOR_RED);*/
}

	
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	char *szHello = "Hello, World!";
	RECT rt;

	//std::wostringstream ss;

	switch (message)
	{
		case WM_PAINT:
			// get the dimensions of the window
			GetClientRect(hWnd, &rt);

			// start drawing on device context
			hdc = BeginPaint(hWnd, &ps);

			/*std::clock_t start;
			double duration;
			start = std::clock();*/
			
			// draw my stuff
			DrawStuff(hdc);

			/*duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
			
			ss << duration;

			MessageBoxW(0, ss.str().c_str(), L"Sup!", 0);*/

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
	model = new Model("obj/african_head/african_head.obj");
	
	for (int i = 0; i < model->nfaces(); i++)
	{
		/*std::vector<int> face = model->face(i);
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j + 1) % 3]);
			int x0 = (v0.x + 1.)*WINDOW_WIDTH / 2.2;
			int y0 = (v0.y + 1.)*WINDOW_HEIGHT / 2.2;
			int x1 = (v1.x + 1.)*WINDOW_WIDTH / 2.2;
			int y1 = (v1.y + 1.)*WINDOW_HEIGHT / 2.2;
			line(x0, y0, x1, y1, image, COLOR_WHITE);
		}*/
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			screen_coords[j] = Vec2i((v.x + 1.)*width / 2.2, (v.y + 1.)*height / 2.2);
			world_coords[j] = v;
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n*light_dir;
		if (intensity > 0)
		{
			triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, RGB(intensity * 255, intensity * 255, intensity * 255));
		}
	}	

	/*Vec2i t0[3] = { Vec2i(10,70),		Vec2i(50,160),		Vec2i(70,80) };
	Vec2i t1[3] = { Vec2i(180,50),	Vec2i(150,1),		Vec2i(70,180) };
	Vec2i t2[3] = { Vec2i(180,150),	Vec2i(120,160),	Vec2i(130,180) };

	triangle(t0[0], t0[1], t0[2], image, COLOR_RED);
	triangle(t1[0], t1[1], t1[2], image, COLOR_WHITE);
	triangle(t2[0], t2[1], t2[2], image, COLOR_BLUE);*/
	
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