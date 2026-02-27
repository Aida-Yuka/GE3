#pragma once
#include <d3dcompiler.h>
#include "DirectXBase.h"
#include "WindowsAPI.h"

class SpriteBase
{
public://メンバ変数
	//初期化
	void Initialize(DirectXBase* dxBase);

	//RootSignatureを設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	
	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	//getter
	DirectXBase* GetDxBase()const { return dxBase_; }

	//rootSigunature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	//共通描画設定
	void commonDraw();

private:
	//ルートシグネチャの設定
	void RootSignatureSetting();
	//グラフィックスパイプラインを生成
	void GraphicsPipeline();

	DirectXBase* dxBase_;
};