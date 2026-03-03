#include "Input.h"
#include "MyMath.h"
#include "WindowsAPI.h"
#include "DirectXBase.h"
#include "Logger.h"
#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "Sprite.h"
#include "SpriteBase.h"

#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <iostream>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

//MaterialData構造体と読み込み関数
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//＝＝＝1.中で必要となる変数の宣言＝＝＝
	MaterialData materialData;//構築するMaterialData
	std::string line;
	//＝＝＝2.ファイルを開く＝＝＝
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//＝＝＝3.実際にファイルを読み、ModelDataを構築していく＝＝＝
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	//＝＝＝4.materialDataを返す＝＝＝
	return materialData;
}

//ModelData構造体と読み込み関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//＝＝＝1.中で必要となる変数の宣言＝＝＝
	ModelData modelData;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	//＝＝＝2.ファイルを開く＝＝＝
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//＝＝＝3.実際にファイルを読み、ModelDataを構築していく＝＝＝
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);	 
		}
		else if (identifier == "f")
		{
			VertexData triangle[3];
			//面は三角形限定
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (uint32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');//
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				position.x *= -1.0f;
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				texcoord.y = 1.0f - texcoord.y;
				Vector3 normal = normals[elementIndices[2] - 1];
				//normal.x *= -1.0f;
				VertexData vertex = { position,texcoord };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord/*, normal*/ };
			}

			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib")
		{
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	//＝＝＝4.ModelDataを返す＝＝＝
	return modelData;
}

///＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝///

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer(); // デバッグ層を有効化
		debugController->SetEnableGPUBasedValidation(TRUE); // GPU側の不整合もチェック
	}
#endif

	//COMの初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//ポインタ
	Input* input = nullptr;
	WindowsAPI* windowsAPI = nullptr;
	DirectXBase* dxBase = nullptr;
	MyMath* myMath = nullptr;
	D3DResourceLeakChecker leakChecker;

	MSG msg{};

	//文字列を格納する
	std::string str0("STRING!!!");

	//整列を文字列にする
	std::string str1{ std::to_string(10) };

	//文字の出力
	Logger::Log(StringUtility::ConvertString(std::format(L"WSTRING{}\n", L"abc")));

	//MyMathの初期化
	myMath = new MyMath();

	//WindowsAPIの初期化
	windowsAPI = new WindowsAPI();
	windowsAPI->Initialize();

	//入力の初期化
	input = new Input();
	input->Initialize(windowsAPI);

	//DirectXの初期化
	dxBase = new DirectXBase();
	dxBase->Initialize(windowsAPI);

#pragma region 基盤システムの初期化

	SpriteBase* spriteBase = nullptr;
	//スプライト共通部の初期化
	spriteBase = new SpriteBase;
	spriteBase->Initialize(dxBase);

#pragma endregion 基盤システムの初期化

#pragma region 最初のシーンの初期化

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteBase);

#pragma endregion 最初のシーンの初期化

	//HRESULTはWindows系のエラーコードであり、関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	//hr = CreateDXGIFactory(IID_PPV_ARGS(&dxBase->dxgiFactory));
	//assert(SUCCEEDED(hr));

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Tableで使用する数
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//DescriptorSizeを取得しておく
	const uint32_t desriptorSizeSRV = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t desriptorSizeRTV = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t desriptorSizeDSV = dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//dxcCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	spriteBase->Initialize(dxBase);
	sprite->Initialize(spriteBase);

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	//通常
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	//加算合成
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/

	//減算合成
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/

	//乗算合成
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;*/

	//スクリーン合成
	/*blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;*/

	//汎用
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//モデル読み込み
	//ModelData modelData = LoadObjFile("resources", "plane.obj");
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = dxBase->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	sprite->GetVertexBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());

	//Transform関数を作る
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//WVP用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxBase->CreateBufferResource(sizeof(Matrix4x4));
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = myMath->MakeIdentity4x4();

	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };
	Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(WindowsAPI::kClientWidth) / float(WindowsAPI::kClientHeight), 0.1f, 100.0f);

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxBase->LoadTexture("resources/uvChecker.png");
	//DirectX::ScratchImage mipImages = dxBase->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxBase->CreateTextureResource(metadata);
	dxBase->UploadTextureData(textureResource, mipImages);

	// SRVの作成
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	//先頭はImGuiが使っているのでその次を使う
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxBase->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxBase->GetSRVGPUDescriptorHandle(1);
	dxBase->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	////ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, dxBase->GetDevice(), dxBace->commandList);
	
	////intermediateResouseをReleaseする
	//intermediateResource->Release();

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;//細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//プロセッサの近くに配置

	//heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	///////////

	//ウィンドウの×ボタンが押されるまでループ
	//while (msg.message != WM_QUIT)
	while (true)
	{
		//Windowsのメッセージ処理
		if (windowsAPI->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		///=== ゲームの処理 ===///

		//フレームの開始
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//キー入力の更新
		input->Update();

		//数字の0キーが押されていたら
		//if (input->PushKey(DIK_0))
		if (input->TriggerKey(DIK_A))
		{
			OutputDebugStringA("Hit0\n");
		}

		Material* material=sprite->GetMaterialData();

		//色の変更機能
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("material", &material->color.x, ImGuiColorEditFlags_AlphaPreview);
		ImGui::DragFloat("rotate.y", &transform.rotate.y, 0.1f);
		ImGui::DragFloat3("transform", &transform.translate.x, 0.1f);
		ImGui::DragFloat2("Sprite transform", &transform.translate.x, 1.0f);
		ImGui::End();

		//開発用のUIの処理
		ImGui::ShowDemoWindow();

		//ImGuiの内部コマンドを生成する
		ImGui::Render();


		sprite->Update();

		//描画前処理
		dxBase->PreDraw();

		//Spriteの描画準備
		spriteBase->commonDraw();
		//wvp用のCBufferの場所を設定
		//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

		//dxBace->commandList->DrawInstanced(6, 1, 0, 0);//描画
		//ModelDataの描画
		//dxBase->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		/*Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		*wvpData = worldMatrix;*/

		////transform.rotate.y += 0.03f;
		//Matrix4x4 worldMatrix = myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		//Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		//Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
		//Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(WindowsAPI::kClientWidth) / float(WindowsAPI::kClientHeight), 0.1f, 100.0f);
		//Matrix4x4 worldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
		//*wvpData = worldViewProjectionMatrix;


		//描画処理
		sprite->Draw();

		//実際のcommandListの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxBase->GetCommandList());

		//描画後処理
		dxBase->PostDraw();
	}

	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//COMの終了処理
	CoUninitialize();
	CoUninitialize();

	//解放処理
	//CloseHandle(fenceEvent);
	//fence->Release();
	//rtvDescriptorHeap->Release();
	//swapChainResources[0]->Release();
	//swapChainResources[1]->Release();
	//swapChain->Release();
	//dxBase->GetcommandList()->Release();
	//commandAllocator->Release();
	//commandQueue->Release();
	//useAdapter->Release();
	//dxgiFactory->Release();

	//vertexResource->Release();
	//graphicsPipelineState->Release();
	/*signatureBlob->Release();
	if (errorBlob)
	{
		errorBlob->Release();
	}*/
	//rootSignature->Release();
	//pixelShaderBlob->Release();
	//vertexShaderBlob->Release();

	//sprite->materialBuffer->Release();
	//wvpResource->Release();

	//srvDescriptorHeap->Release();
	//textureResource->Release();
	//depthStencilResource->Release();
	//dsvDescriptorHeap->Release();

	/*vertexResourceSprite->Release();
	transformationMatrixResourceSprite->Release();
	if (indexResourceSprite)
	{
		indexResourceSprite->Release();
		indexResourceSprite = nullptr;
	}*/

	delete sprite;
	delete spriteBase;
	
	//入力解放
	delete input;

	//DirectX解放
	delete dxBase;
	
	/*#ifdef _DEBUG
			debugController->Release();
	#endif*/

	//WindowsAPIの終了処理
	windowsAPI->Finalize();

	//WindousAPIの解放
	delete windowsAPI;

	return 0;
}
