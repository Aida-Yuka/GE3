#pragma once
#include <d3dcompiler.h>
#include "DirectXBase.h"
#include "WindowsAPI.h"

class SpriteBase
{
public://メンバ変数
	//初期化
	void Initialize(DirectXBase* dxBase);

	//getter
	//D3D12DescriptorHeap* GetDxBase() const { return dxBase_; }

	//rootSigunature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	//共通描画設定
	void commonDraw();

	//getter
	DirectXBase* GetDirectXBase()const { return dxBase_; }

private:
	//ルートシグネチャの設定
	void RootSignatureSetting();
	//グラフィックスパイプラインを生成
	void GraphicsPipeline();

	DirectXBase* dxBase_ = nullptr;
};