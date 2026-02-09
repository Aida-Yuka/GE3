#pragma once

#include "WindowsAPI.h"
#include "Logger.h"
#include "StringUtility.h"

#include <d3d12.h>//
#include <dxgi1_6.h>//
#include <wrl.h>//
#include <array>

#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include <dxcapi.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DirectXBase
{
public://メンバ変数
	//初期化
	void Initialize(WindowsAPI* windowsAPI);

	/// <summary>
	/// デスクリプタヒープを生成する
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	//RTV
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvDescriptorSize;
	UINT rtvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	//SRV
	UINT srvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
	//DSV
	UINT dsvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = nullptr;

	//スワップチェーンリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	//スワップチェーン
	Microsoft::WRL::ComPtr < IDXGISwapChain1> swapChain = nullptr;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr < ID3D12CommandAllocator> commandAllocator = nullptr;
	//commandAllocator = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue = nullptr;

	//===『レンダーターゲットビューの初期化』での生成物===
	//スワップチェーンから引っ張ってきたリソース(バックバッファ)
	ID3D12Resource* swapChainResources2[2] = { nullptr };
	//RTVハンドル
	static const uint32_t kRTVNumber = 2;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[kRTVNumber];

	//深度バッファの成果物
	ID3D12Resource* depthStencilResource = nullptr;

	//フェンスの成果物
	Microsoft::WRL::ComPtr < ID3D12Fence> fence = nullptr;

	//ビューポート矩形の設定
	D3D12_VIEWPORT viewport{};

	//シザリング矩形の設定
	D3D12_RECT scissorRect{};

	//DXCコンパイラの生成
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	//描画前処理

	//描画後処理

public://外部公開
	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

private://プライベート関数
	//デバイスの初期化
	void DeviceInitialize();

	//コマンド関連の初期化
	void CommandInitialize();

	//スワップチェーンの生成
	void SwapChainGenerate();

	//深度バッファの生成
	void DepthBufferGenerate();

	//各種デスクリプタヒープの生成
	void DescriptorHeapGenerate();

	//レンダーターゲットビューの初期化
	void RenderTargetViewInitialize();

	//深度ステンシルビューの初期化
	void DepthStencilViewInitialize();

	//フェンスの生成
	void FenceGenerate();

	//ビューポート矩形の初期化
	void ViewportInitialize();

	//シザリング矩形の初期化
	void ScissorRectInitialize();

	//DXCコンパイラの生成
	void DxcCompilerGenerate();

	//ImGuiの初期化
	void ImGuiInitialize();

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	
	//WindowsAPI
	WindowsAPI* windowsAPI = nullptr;

	///<summary>
	///指定番号のCPUデスクリプタハンドルを取得する
	///</summary>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	///<summary>
	///指定番号のGPUデスクリプタハンドルを取得する
	///</summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
};