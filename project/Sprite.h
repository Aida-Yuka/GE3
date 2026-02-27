#pragma once
#include "MyMath.h"

class SpriteBase;

//頂点データ
struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

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

	//バッファリソース
	//void VertexResource(VertexBuffer);
	//void IndexResource(IndexBuffer);
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファリソース
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};

