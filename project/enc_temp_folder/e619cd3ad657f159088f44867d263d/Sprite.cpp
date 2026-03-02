#include "Sprite.h"
#include "SpriteBase.h"

void Sprite::Initialize(SpriteBase* spriteBase)
{
	//引数で受け取ってメンバ変数に記録する
	this->spriteBase = spriteBase;

	//★===VertexResourceを作る===
	vertexResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(vertexData) * 4);

	//★===IndexResourceを作る===
	indexResource = spriteBase->GetDxBase()->CreateBufferResource(sizeof(uint32_t) * 6);

	//★===VertexBufferViewを作成する(値を設定するだけ)===
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//★===IndexBufferViewを作成する(値を設定するだけ)===
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//★===VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる===
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//1枚目の三角形
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData[3].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 0.0f,0.0f };
	vertexData[4].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 1.0f,0.0f };
	vertexData[5].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	//★===IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる===
	//インデックスリソースにデータを書き込む
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
}