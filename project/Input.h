#pragma once

#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>

#include "Windows.h"
#include <windows.h>
#include <wrl.h>
#include "windowsAPI.h"

class Input
{
public:
	//namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	///<param name="keyNumber">キー番号</param>
	///<returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	///<param name="KeyNumber">キー番号</param>
	///<returns>トリガーか</returns>
		bool TriggerKey(BYTE keyNumber);

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WindowsAPI* windowsAPI);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private://メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	//全キーの状態
	BYTE key[256] = {};

	//前回の全キーの状態
	BYTE keyPre[256] = {};

	//windowsAPI
	WindowsAPI* windowsAPI_ = nullptr;
};