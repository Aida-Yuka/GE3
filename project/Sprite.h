#pragma once
#include "MyMath.h"
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

	SpriteBase* spriteBase = nullptr;
	MyMath* myMath = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource{};
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファリソース
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};

