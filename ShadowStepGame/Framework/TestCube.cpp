#include "TestCube.h"
#include "IOManager.h"
#include "Game.h"

using namespace DirectX;

//=======================================
//初期化処理
//=======================================
void TestCube::Init()
{
	// 頂点データ
	m_Vertices.resize(24);

	// 1面
	m_Vertices[0].position = SimpleMath::Vector3(-10,  10,  10);
	m_Vertices[1].position = SimpleMath::Vector3( 10,  10,  10);
	m_Vertices[2].position = SimpleMath::Vector3(-10,  10, -10);
	m_Vertices[3].position = SimpleMath::Vector3( 10,  10, -10);

	m_Vertices[0].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[1].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[2].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[3].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[0].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[1].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[2].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[3].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[0].normal = SimpleMath::Vector3(0, 1, 0);
	m_Vertices[1].normal = SimpleMath::Vector3(0, 1, 0);
	m_Vertices[2].normal = SimpleMath::Vector3(0, 1, 0);
	m_Vertices[3].normal = SimpleMath::Vector3(0, 1, 0);

	// ２面
	m_Vertices[4].position = SimpleMath::Vector3(-10,  10, -10);
	m_Vertices[5].position = SimpleMath::Vector3( 10,  10, -10);
	m_Vertices[6].position = SimpleMath::Vector3(-10, -10, -10);
	m_Vertices[7].position = SimpleMath::Vector3( 10, -10, -10);

	m_Vertices[4].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[5].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[6].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[7].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[4].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[5].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[6].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[7].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[4].normal = SimpleMath::Vector3(0, 0, -1);
	m_Vertices[5].normal = SimpleMath::Vector3(0, 0, -1);
	m_Vertices[6].normal = SimpleMath::Vector3(0, 0, -1);
	m_Vertices[7].normal = SimpleMath::Vector3(0, 0, -1);

	// ３面
	m_Vertices[ 8].position = m_Vertices[3].position;
	m_Vertices[ 9].position = m_Vertices[1].position;
	m_Vertices[10].position = m_Vertices[7].position;
	m_Vertices[11].position = SimpleMath::Vector3(10, -10,  10);

	m_Vertices[ 8].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[ 9].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[10].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[11].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[ 8].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[ 9].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[10].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[11].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[ 8].normal = SimpleMath::Vector3(1, 0, 0);
	m_Vertices[ 9].normal = SimpleMath::Vector3(1, 0, 0);
	m_Vertices[10].normal = SimpleMath::Vector3(1, 0, 0);
	m_Vertices[11].normal = SimpleMath::Vector3(1, 0, 0);

	// ４面
	m_Vertices[12].position = m_Vertices[7].position;
	m_Vertices[13].position = m_Vertices[11].position;
	m_Vertices[14].position = m_Vertices[6].position;
	m_Vertices[15].position = SimpleMath::Vector3(-10, -10,  10);

	m_Vertices[12].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[13].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[14].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[15].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[12].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[13].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[14].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[15].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[12].normal = SimpleMath::Vector3(0, -1, 0);
	m_Vertices[13].normal = SimpleMath::Vector3(0, -1, 0);
	m_Vertices[14].normal = SimpleMath::Vector3(0, -1, 0);
	m_Vertices[15].normal = SimpleMath::Vector3(0, -1, 0);

	// ５面
	m_Vertices[16].position = m_Vertices[1].position;
	m_Vertices[17].position = m_Vertices[0].position;
	m_Vertices[18].position = m_Vertices[11].position;
	m_Vertices[19].position = m_Vertices[15].position;

	m_Vertices[16].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[17].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[18].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[19].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[16].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[17].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[18].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[19].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[16].normal = SimpleMath::Vector3(0, 0, 1);
	m_Vertices[17].normal = SimpleMath::Vector3(0, 0, 1);
	m_Vertices[18].normal = SimpleMath::Vector3(0, 0, 1);
	m_Vertices[19].normal = SimpleMath::Vector3(0, 0, 1);

	// ６面
	m_Vertices[20].position = m_Vertices[0].position;
	m_Vertices[21].position = m_Vertices[2].position;
	m_Vertices[22].position = m_Vertices[15].position;
	m_Vertices[23].position = m_Vertices[6].position;

	m_Vertices[20].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[21].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[22].color = SimpleMath::Color(1, 1, 1, 1);
	m_Vertices[23].color = SimpleMath::Color(1, 1, 1, 1);

	m_Vertices[20].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[21].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[22].uv = SimpleMath::Vector2(0, 0);
	m_Vertices[23].uv = SimpleMath::Vector2(0, 0);

	m_Vertices[20].normal = SimpleMath::Vector3(-1, 0, 0);
	m_Vertices[21].normal = SimpleMath::Vector3(-1, 0, 0);
	m_Vertices[22].normal = SimpleMath::Vector3(-1, 0, 0);
	m_Vertices[23].normal = SimpleMath::Vector3(-1, 0, 0);

	// 頂点バッファ生成
	m_VertexBuffer.Create(m_Vertices);

	// インデックデータ
	m_Indices.resize(36);

	m_Indices =
	{
		0, 1, 2,
		1, 3, 2,
		4, 5, 6,
		5, 7, 6,
		8, 9, 11,
		11,10,8,
		12,13,15,
		15,14,12,
		16,17,19,
		19,18,16,
		20,21,23,
		23,22,20,
	};


	// インデックスバッファ生成
	m_IndexBuffer.Create(m_Indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// 初期化完了フラグ
	m_isInitialized = true;
}

//=======================================
//更新処理
//=======================================
void TestCube::Update()
{
	static POINT  lp_pre;
	
	IO_MANAGER.Update();

	// ===================================================================
	// デルタタイムを取得（追加）
	// ===================================================================
	float dt = Game::GetDeltaTime();

	// マウス左ボタンで回転操作
	if (IO_MANAGER.GetKeyDownKeyBord(VK_LBUTTON))
	{
		GetCursorPos(&lp_pre);
	}
	if (IO_MANAGER.GetKeyPressKeyBord(VK_LBUTTON))
	{
		POINT lp;
		std::cout << __FUNCTION__ << ":Left Click" << std::endl;
		GetCursorPos(&lp);
		float rotateY = (float)(lp_pre.x - lp.x) / 100.f;
		float rotateX = (float)(lp_pre.y - lp.y) / 100.f;
		m_Rotation.y += rotateY;
		m_Rotation.x += rotateX;
		lp_pre = lp;
	}
	else
	{
		// 秒間0.5ラジアン回転（約28.6度/秒）
		m_Rotation.x += 0.5f * dt;
		m_Rotation.y += 0.5f * dt;

		// Z軸はもう少し速く（秒間2.5ラジアン = 約143度/秒）
		m_Rotation.z += 2.5f * dt;
	}
}

//=======================================
//描画処理
//=======================================
void TestCube::Draw()
{
	
}

void TestCube::Draw(Camera* cam)
{
	//カメラを選択する
	cam->SetCamera();

	// SRT情報作成
	SimpleMath::Matrix r = SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	SimpleMath::Matrix t = SimpleMath::Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	SimpleMath::Matrix s = SimpleMath::Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	SimpleMath::Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	devicecontext->DrawIndexed(
		m_Indices.size(),	// 描画するインデックス数
		0,					// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void TestCube::UnInit()
{
	m_isInitialized = false;
}
