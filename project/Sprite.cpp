#include "Sprite.h"
#include "SpriteBase.h"

void Sprite::Initialize(SpriteBase* spriteBase)
{
	//引数で受け取ってメンバ変数に記録する
	this->spriteBase = spriteBase;

	///=====頂点リソースの作成=====///
	//★===VertexResourceを作る===
	vertexBuffer = this->spriteBase->GetDirectXBase()->CreateBufferResource(sizeof(VertexData) * 4);

	//★===IndexResourceを作る===
	indexBuffer = this->spriteBase->GetDirectXBase()->CreateBufferResource(sizeof(uint32_t) * 6);

	//★===VertexBufferViewを作成する(値を設定するだけ)===
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//★===IndexBufferViewを作成する(値を設定するだけ)===
	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//★===VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる===
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));


	//★===IndexResourceにデータを書き込むためのアドレスを取得してindexDataに割り当てる===
	//インデックスリソースにデータを書き込む
	indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	///=====マテリアルの作成=====///
	//マテリアルリソースを作る
	materialBuffer = this->spriteBase->GetDirectXBase()->CreateBufferResource(sizeof(Vector4));
	//materialBufferに書き込むためのアドレスを取得して、materialDataにデータを書き込む
	materialBuffer->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//マテリアルデータの初期値を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = myMath->MakeIdentity4x4();

	///=====座標変換行列=====///
	//座標変換行列リソースを作る
	transformationMatrixBuffer = this->spriteBase->GetDirectXBase()->CreateBufferResource(sizeof(TransformationMatrix));
	//materialResourceに書き込むためのアドレスを取得して、transformationMatrixDataにデータを書き込む
	transformationMatrixBuffer->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	//単位行列を書き込んでおく
	transformationMatrixData->WVP = myMath->MakeIdentity4x4();
	transformationMatrixData->World = myMath->MakeIdentity4x4();


	// 頂点データ回りを一度更新
	Update();

	// 画像の読み込み
	DirectX::ScratchImage mipImages = spriteBase->GetDirectXBase()->LoadTexture("resources/uvChecker.png");
	//DirectX::ScratchImage mipImages = dxBase->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = spriteBase->GetDirectXBase()->CreateTextureResource(metadata);
	spriteBase->GetDirectXBase()->UploadTextureData(textureResource, mipImages);

	// 読み込んだ画像からSRVの作成
	// SRVの作成
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU = spriteBase->GetDirectXBase()->GetSRVCPUDescriptorHandle(1);
	textureSrvHandleGPU = spriteBase->GetDirectXBase()->GetSRVGPUDescriptorHandle(1);
	spriteBase->GetDirectXBase()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

}

void Sprite::Update()
{
	//頂点リソースにデータを書き込む
	vertexData[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData[0].texcoord = { 0.0f,1.0f };
	vertexData[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData[1].texcoord = { 0.0f,0.0f };
	vertexData[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData[2].texcoord = { 1.0f,1.0f };
	vertexData[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexData[3].texcoord = { 1.0f,0.0f };

	//インデックスリソースにデータを書き込む
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;

	//Transform関数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//TransformからWorldMatrixを作る
	Matrix4x4 worldMatrix = myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//ViewMatrixを作って単位行列を代入
	Matrix4x4 viewMatrix = myMath->MakeIdentity4x4();
	//ProfectionMatrixを作って平行投影行列を書き込む
	Matrix4x4 projectionMatrix = myMath->MakeOrthographicMatrix(0.0f, 0.0f, float(WindowsAPI::kClientWidth), float(WindowsAPI::kClientHeight), 0.0f, 100.0f);
	
	transformationMatrixData->WVP = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->World = worldMatrix;
}

void Sprite::Draw()
{
	//VertexBufferViewを設定
	spriteBase->GetDirectXBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//IndexBufferViewを設定
	spriteBase->GetDirectXBase()->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	//TransformMatrixCBufferの場所を設定
	spriteBase->GetDirectXBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer->GetGPUVirtualAddress());
	//マテリアルCBufferの場所を設定
	spriteBase->GetDirectXBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialBuffer->GetGPUVirtualAddress());

	spriteBase->GetDirectXBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	//描画
	//spriteBase->GetDxBase()->commandList->DrawInstanced(6, 1, 0, 0);
	spriteBase->GetDirectXBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}