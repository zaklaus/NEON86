// player.exe: A sample program loading the game data using Neon86 game engine
//

#include "stdafx.h"

#include <windowsx.h>

#include "NeonEngine.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPWSTR className, LPWSTR titleName, RECT resolution);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	HWND hWnd;
	RECT rect;
	rect.left = 300;
	rect.top = 100;
	rect.right = 1024;
	rect.bottom = 768;

	if (!lpCmdLine)
		lpCmdLine = "data";

	hWnd = BuildWindow(hInstance, nCmdShow, L"NeonClass", L"NEON 86 | PLAYER", rect);
	if (!ENGINE->Init(hWnd, rect))
	{
		MessageBox(NULL, L"Failed to start engine!", L"Engine load failure", MB_OK);
		ENGINE->Release();
		return 0;
	}

	if (!FILESYSTEM->LoadGame(lpCmdLine))
	{
		MessageBox(NULL, L"Failed to load game!", L"Game load failure", MB_OK);
		ENGINE->Release();
		return 0;
	}

	VM->Play();

	MSG msg;

    while(ENGINE->IsRunning())
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

		ENGINE->Think();
    }

	VM->Stop();
	ENGINE->Release();

	return 0;
}

HWND BuildWindow(HINSTANCE instance, BOOL cmdShow, LPWSTR className, LPWSTR titleName, RECT rect)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = className;

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		className,
		titleName,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		rect.left, rect.top,
		rect.right, rect.bottom,
		NULL,
		NULL,
		instance,
		NULL);

	ShowWindow(hWnd, cmdShow);

	return hWnd;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		rect.right -= rect.left;
		rect.bottom -= rect.top;
			
		ENGINE->Resize(rect);
	} break;

    case WM_DESTROY:
    {
        ENGINE->Shutdown();
        return 0;
    } break;

	case WM_KEYDOWN:
	{
		if (INPUT->GetKey(wParam))
			break;

		INPUT->SetKey(wParam, TRUE);
		INPUT->SetKeyDown(wParam, TRUE);
	} break;

	case WM_KEYUP:
	{
        INPUT->SetKey(wParam, FALSE);
        INPUT->SetKeyUp(wParam, TRUE);
	} break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}



