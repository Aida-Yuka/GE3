#pragma once

#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include "DirectXBase.h"

#include "externals/DirectXTex/DirectXTex.h"

class DirectXBase;

class TextureManager
{
public:
	//シングルインインスタンスの取得
	static TextureManager* GetInstance();

	//初期化
	void Initialize(DirectXBase* dxBase);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャファイルのパス</param>
	void LoadTexture(const std::string& filePath);

	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	//終了
	void Finalize();

	//メタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	//シングルトン
	static uint32_t kSRVIndexTop;
	static TextureManager* instance;

	//テクスチャデータ1枚分のデータ
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata{};
		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
	}textureData;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager*) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	//テクスチャデータ
	std::vector<TextureData> textureDatas;

	DirectXBase* dxBase;
};