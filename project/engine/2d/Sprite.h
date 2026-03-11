#pragma once
#include "MyMath.h"
#include "DirectXBase.h"
#include "TextureManager.h"
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
	void Initialize(SpriteBase* spriteBase, std::string textureFilePath);

	//更新
	void Update();

	//描画
	void Draw();

	//トランスポイント
	Transform transform{};

	//getter
	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexBuffer() const { return vertexBuffer.Get(); }
	VertexData* GetVertexData() const { return vertexData; }
	Material* GetMaterialData() const { return materialData; }

	const Vector2& GetPosition() const { return position; }
	float GetRotation() const { return rotation; }
	const Vector4& GetColor() const { return materialData->color; }
	const Vector2 GetSize() const { return size; }

	const Vector2& GetAnchorPoint() const { return anchorPoint; }
	bool GetIsFilpX() const { return isFlipX_; }
	bool GetIsFilpY() const { return isFlipY_; }
	const Vector2& GetTextureLeftTop() const { return textureLeftTop; }
	const Vector2& GetTextureSize() const { return textureSize; }

	//setter
	void SetPosition(const Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const Vector4 & color) { materialData->color = color; }
	void SetSize(const Vector2& size) { this->size = size; }

	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }
	void SetIsFilpX(const bool isFlipX) { this->isFlipX_ = isFlipX; }
	void SetIsFilpY(const bool isFlipY) { this->isFlipY_ = isFlipY; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

private:
	SpriteBase* spriteBase = nullptr;
	MyMath* myMath = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer{};
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixBuffer{};
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	//バッファリソースの使い道を補足するバッファリソース
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};

	//テクスチャ番号
	uint32_t textureIndex = 0;

	//座標
	Vector2 position = { 600.0f,300.0f };
	//回転
	float rotation = 0.0f;
	//サイズ
	Vector2 size = { 640.0f,360.0f };

	//アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };
	//Vector2 anchorPoint = { 0.0f,1.0f };
	//Vector2 anchorPoint = { 0.5f,0.5f };
	//Vector2 anchorPoint = { -0.5f,-0.5f };

	//左右フリップ
	bool isFlipX_ = false;

	//上下フリップ
	bool isFlipY_ = false;

	//テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };

	//テクスチャ切り出しサイズ
	Vector2 textureSize = { 100.0f,100.0f };

	//テクスチャサイズをイメージに合わせる
	void AdjustTextureSize();
};