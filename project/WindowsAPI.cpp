#include "WindowsAPI.h"

#include "externals/imgui/imgui.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//ウィンドウプロシージャ
LRESULT CALLBACK WindowsAPI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg)
	{
		//ウィンドウが破棄されたら
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowsAPI::Initialize()
{
//ウィンドウプロ―ジャー
	wndclass.lpfnWndProc = WindowProc;

	//ウィンドウクラス
	wndclass.lpszClassName = L"CG2WindowClass";

	//インスタンスハンドル
	wndclass.hInstance = GetModuleHandle(nullptr);

	//カーソル
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	RegisterClass(&wndclass);

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成
	hwnd = CreateWindow
	(
		wndclass.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wndclass.hInstance,
		nullptr
	);

	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);
}

void WindowsAPI::Finalize()
{
	CloseWindow(hwnd);
	CoUninitialize();
}

bool WindowsAPI::ProcessMessage()
{
	MSG msg{};

	//Windowにメッセージが来ていたら最優先で処理する
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}
