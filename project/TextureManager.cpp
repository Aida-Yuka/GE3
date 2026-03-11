#include "TextureManager.h"
#include "DirectXBase.h"

#include <d3d12.h>

#include "externals/DirectXTex/d3dx12.h"

TextureManager* TextureManager::instance = nullptr;

//ImGuiで0番を使用するため、1番から利用
uint32_t TextureManager::kSRVIndexTop = 1;

//テクスチャファイル読み込み関数
void TextureManager::LoadTexture(const std::string& filePath)
{
	//読み込み枚数上限チェック
	assert(textureDatas.size() + kSRVIndexTop < DirectXBase::kMaxSRVCount);
	//読み込み済みテクスチャを検索
	auto it = std::find_if(textureDatas.begin(), textureDatas.end(), [&](TextureData& textureData) {return textureData.filePath == filePath; });
	if (it != textureDatas.end())
	{
		//読み込み済みなら早期return
		return;
	}

	//①Textureデータを読む
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));
	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));
	//ミップマップ付きのデータを返す
	//return mipImages;
	//テクスチャデータを追加
	textureDatas.resize(textureDatas.size() + 1);
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas.back();

	textureData.filePath = filePath;
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxBase->CreateTextureResource(textureData.metadata);

	
	//テクスチャデータ転送
	//③TextureResourceにデータを転送する
	//[[nodiscard]]
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(dxBase->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(textureData.resource.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxBase->CreateBufferResource(intermediateSize);
	UpdateSubresources(dxBase->GetCommandList(), textureData.resource.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Texture転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureData.resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	dxBase->GetCommandList()->ResourceBarrier(1, &barrier);

	// ExcueCommand
	dxBase->ExcuteCommand();
	// WaitForSignal
	dxBase->WaitForSignal();
	// CommansReset
	dxBase->CommandReset();

	

	//SRVの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//SRVの設定を行う
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	////先頭はImGuiが使っているのでその次を使う
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxBase->GetSRVCPUDescriptorHandle(1);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxBase->GetSRVGPUDescriptorHandle(1);
	//テクスチャデータの要素数番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;
	textureData.srvHandleCPU = dxBase->GetSRVCPUDescriptorHandle/*CPUハンドルを取得*/(srvIndex);
	textureData.srvHandleGPU = dxBase->GetSRVGPUDescriptorHandle/*GPUハンドルを取得*/(srvIndex);

	dxBase->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

	// テクスチャを追加
	textureDatas.push_back(textureData);
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	auto it = std::find_if(textureDatas.begin(), textureDatas.end(), [&](TextureData& textureData) {return textureData.filePath == filePath; }/*テクスチャデータ検索*/);
	if (it != textureDatas.end())
	{
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{
	//範囲外指定違反チェック
	assert(textureIndex < textureDatas.size());

	TextureData& textureData = textureDatas[textureIndex];/*テクスチャデータの参照*/
	return textureData.srvHandleGPU;
}

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Initialize(DirectXBase* dxBase)
{
	this->dxBase = dxBase;
	//SRVの数と同数
	textureDatas.reserve(DirectXBase::kMaxSRVCount);
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

//メタデータ取得
const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{
	// 範囲外指定違反チェック
	assert(textureIndex < textureDatas.size());

	TextureData& textureData = textureDatas[textureIndex];
	return textureData.metadata;
}