#pragma once
#include "MyMath.h"
#include "DirectXBase.h"
#include <d3d12.h>
#include <wrl.h>

class SpriteBase;

//マテリアルデータ
struct Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

class Sprite
{
public://メンバ変数
	//初期化
	void Initialize(SpriteBase* spriteBase);

	//更新
	void Update();

	//描画
	void Draw();

	//getter
	ID3D12Resource* GetVertexBuffer() const { return vertexBuffer.Get(); }
	Material* GetMaterialData() const { return materialData; }

private:
	DirectXBase* dxBase = nullptr; 
	SpriteBase* spriteBase = nullptr;
	MyMath* myMath = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> ConstantBuffer{};
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	//バッファリソースの使い道を補足するバッファリソース
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
};

