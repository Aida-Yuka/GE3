#include "Input.h"
#include "MyMath.h"
#include "WindowsAPI.h"
#include "DirectXBase.h"
#include "Logger.h"
#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "Sprite.h"
#include "SpriteBase.h"
#include "TextureManager.h"

#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <iostream>

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

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxBase);

	//Textureを読んで転送する
	TextureManager::GetInstance()->LoadTexture("resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resources/monsterBall.png");

#pragma region 基盤システムの初期化

	SpriteBase* spriteBase = nullptr;
	//スプライト共通部の初期化
	spriteBase = new SpriteBase;
	spriteBase->Initialize(dxBase);

#pragma endregion 基盤システムの初期化

#pragma region 最初のシーンの初期化

	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteBase, "resources/uvChecker.png");
	sprite->Initialize(spriteBase, "resources/monsterBall.png");
	//複数枚描画
	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i)
	{
		sprites.push_back(new Sprite());

		if (i % 2 == 0)
		{
			sprites[i]->Initialize(spriteBase, "resources/uvChecker.png");
		}
		else
		{
			sprites[i]->Initialize(spriteBase, "resources/monsterBall.png");
		}

		sprites[i]->SetPosition({ 100.0f * i, 100.0f });
		sprites[i]->SetSize({ 64.0f,64.0f });
		sprites[i]->SetAnchorPoint(Vector2(0.5f, 0.5f));
		sprites[i]->SetTextureSize(Vector2(64.0f + 64.0f*i, 64.0f + 64.0f * i));
		sprites[i]->SetIsFilpY(true);
		sprites[i]->SetIsFilpX(true);
	}

#pragma endregion 最初のシーンの初期化

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

		Material* material = sprite->GetMaterialData();

		//色の変更機能
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("material", &material->color.x, ImGuiColorEditFlags_AlphaPreview);
		ImGui::DragFloat("rotate.y", &sprite->transform.translate.y, 0.1f);
		ImGui::DragFloat3("transform", &sprite->transform.translate.x, 0.1f);
		ImGui::DragFloat2("Sprite transform", &sprite->transform.translate.x, 1.0f);
		ImGui::End();

		//開発用のUIの処理
		ImGui::ShowDemoWindow();

		//ImGuiの内部コマンドを生成する
		ImGui::Render();

		//描画処理
		sprite->Update();
		for (Sprite* s : sprites)
		{
			s->Update();
		}

		//描画前処理
		dxBase->PreDraw();

		//Spriteの描画準備
		spriteBase->commonDraw();
		
		////座標を変化させる
		//Vector2 position = sprite->GetPosition();
		//position += Vector2{ 0.1f,0.1f };
		//sprite->SetPosition(position);

		////角度を変化させる
		//float rotation = sprite->GetRotation();
		//rotation += 0.01f;
		//sprite->SetRotation(rotation);

		////色を変化させる
		//Vector4 color = sprite->GetColor();
		//color.x += 0.01f;
		//if (color.x > 1.0f)
		//{
		//	color.x -= 1.0f;
		//}
		//sprite->SetColor(color);

		////サイズを変化させる
		//Vector2 size = sprite->GetSize();
		//size.x += 0.1f;
		//size.y += 0.1f;
		//sprite->SetSize(size);

		//描画処理
		sprite->Draw();
		for (Sprite* sprite:sprites)
		{
			for (uint32_t i = 0; i < 5; ++i)
			{
				sprites[i]->Draw();
			}
		}

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
	delete sprite;
	for (Sprite*sprite:sprites)
	{
		delete sprite;
	}
	delete spriteBase;
	
	//入力解放
	delete input;

	//テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Finalize();

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
