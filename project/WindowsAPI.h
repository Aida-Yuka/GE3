#pragma once

#include <cstdint>
#include <Windows.h>

//WindowsAPI
class WindowsAPI
{
public://静的メンバ変数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	//getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return wndclass.hInstance; }

public://メンバ変数
	//初期化
	void Initialize();
	//終了
	void Finalize();
	//メッセージの処理
	bool ProcessMessage();

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	//ウィンドウクラスの設定
	WNDCLASS wndclass{};
};