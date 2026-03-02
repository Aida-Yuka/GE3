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
	DirectXBase* GetDxBase()const { return dxBase_; }

	//rootSigunature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	//共通描画設定
	void commonDraw();

	DirectXBase* dxBase_;

private:
	//RootSignatureを設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	//ルートシグネチャの設定
	void RootSignatureSetting();
	//グラフィックスパイプラインを生成
	void GraphicsPipeline();
};