#include <windows.h>
#include <D2D1.h>// header for Direct2D
#pragma comment(lib,"d2d1.lib")  // for link option

#define SAFE_RELEASE(P) if(P){P->Release() ; P = NULL ;}

ID2D1Factory* pD2DFactory = NULL; // Direct2D factory
ID2D1HwndRenderTarget* pRenderTarget = NULL; // Render target
ID2D1SolidColorBrush* pBlackBrush = NULL; // A black brush, reflect the line color

RECT rc; // Render area
HWND g_Hwnd; // Window handle

VOID CreateD2DResource(HWND hWnd)
{
	if (!pRenderTarget)
	{
		HRESULT hr;

		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create D2D factory failed!", L"Error", 0);
			return;
		}

		// Obtain the size of the drawing area
		GetClientRect(hWnd, &rc);

		// Create a Direct2D render target
		hr = pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				hWnd,
				D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)
				),
			&pRenderTarget
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create render target failed!", L"Error", 0);
			return;
		}

		// Create a brush
		hr = pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::Black),
			&pBlackBrush
			);
		if (FAILED(hr))
		{
			MessageBox(hWnd, L"Create brush failed!", L"Error", 0);
			return;
		}
	}
}

VOID DrawRectangle()
{
	CreateD2DResource(g_Hwnd);

	pRenderTarget->BeginDraw();

	// Clear background color white
	pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

	// Draw Rectangle
	pRenderTarget->DrawRectangle(
		D2D1::RectF(100.f, 100.f, 500.f, 500.f),
		pBlackBrush
		);

	HRESULT hr = pRenderTarget->EndDraw();

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Draw failed!", L"Error", 0);
		return;
	}
}

VOID Cleanup()
{
	SAFE_RELEASE(pRenderTarget);
	SAFE_RELEASE(pBlackBrush);
	SAFE_RELEASE(pD2DFactory);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		DrawRectangle();
		ValidateRect(g_Hwnd, NULL);
		return 0;

	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_ESCAPE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
	}
	break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{

	WNDCLASSEX winClass;

	winClass.lpszClassName = L"Direct2D";
	winClass.cbSize = sizeof(WNDCLASSEX);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WndProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = NULL;
	winClass.hIconSm = NULL;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = NULL;
	winClass.lpszMenuName = NULL;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;

	if (!RegisterClassEx(&winClass))
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"), L"error", MB_ICONERROR);
		return 0;
	}

	g_Hwnd = CreateWindowEx(NULL,
		L"Direct2D", // window class name
		L"Draw Rectangle", // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		600, // initial x size
		600, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters

	ShowWindow(g_Hwnd, iCmdShow);
	UpdateWindow(g_Hwnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}