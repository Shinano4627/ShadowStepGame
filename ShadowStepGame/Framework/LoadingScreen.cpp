// ===================================================================
// LoadingScreen.cpp ローディング画面クラスの実装
// ===================================================================

#include "LoadingScreen.h"
#include "XmlRW.h"

using namespace DirectX::SimpleMath;

// ===================================================================
// コンストラクタ
// ===================================================================
LoadingScreen::LoadingScreen()
{
}

// ===================================================================
// デストラクタ
// ===================================================================
LoadingScreen::~LoadingScreen()
{
	Uninit();
}

// ===================================================================
// 初期化
// ===================================================================
void LoadingScreen::Init()
{
	if (m_Initialized) return;

	// ===================================================================
	// SceneCommon.xmlからローディング画面のデータを取得
	// ===================================================================
	XmlRW xml;
	std::vector<ObjectData> objects;
	int ret = xml.GetObjectData("SceneCommon", objects);

	std::string bgTexturePath = "";  
	std::string animTexturePath = "";         

	if (ret == 0)
	{
		for (const auto& obj : objects)
		{
			// 背景データ
			if (obj.objectName == "LoadingBackGround")
			{
				if (!obj.texture.empty())
				{
					bgTexturePath = obj.texture;
				}
			}
			// アニメーションデータ
			else if (obj.objectName == "LoadingAnimation")
			{
				if (!obj.texture.empty())
				{
					animTexturePath = obj.texture;
				}
				// 位置を設定
				m_AnimPosX = obj.pos[0];
				m_AnimPosY = obj.pos[1];
				// サイズを設定
				m_AnimWidth = obj.scl[0];
				m_AnimHeight = obj.scl[1];
			}
		}
	}

	// シェーダー作成
	m_Shader = std::make_shared<Shader>();
	m_Shader->Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	// マテリアル作成
	m_Material = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.TextureEnable = true;
	m_Material->Create(mtrl);

	// テクスチャ読み込み
	m_BackgroundTexture = M_RESOURCE.LoadTexture(bgTexturePath.c_str());
	m_AnimTexture = M_RESOURCE.LoadTexture(animTexturePath.c_str());

	// 背景の頂点設定
	SetupBackgroundVertices();

	// アニメーションの頂点設定
	SetupAnimationVertices();

	m_Initialized = true;
}

// ===================================================================
// 背景の頂点設定（画面全体）
// ===================================================================
void LoadingScreen::SetupBackgroundVertices()
{
	float halfWidth = static_cast<float>(Application::GetWidth()) / 2.0f;
	float halfHeight = static_cast<float>(Application::GetHeight()) / 2.0f;

	m_BGVertices.resize(4);

	m_BGVertices[0].position = Vector3(-halfWidth, halfHeight, 0);
	m_BGVertices[1].position = Vector3(halfWidth, halfHeight, 0);
	m_BGVertices[2].position = Vector3(-halfWidth, -halfHeight, 0);
	m_BGVertices[3].position = Vector3(halfWidth, -halfHeight, 0);

	for (int i = 0; i < 4; i++)
	{
		m_BGVertices[i].color = Color(1, 1, 1, 1);
	}

	m_BGVertices[0].uv = Vector2(0, 0);
	m_BGVertices[1].uv = Vector2(1, 0);
	m_BGVertices[2].uv = Vector2(0, 1);
	m_BGVertices[3].uv = Vector2(1, 1);

	m_BGVertexBuffer.Create(m_BGVertices);

	m_BGIndices.resize(4);
	m_BGIndices[0] = 0;
	m_BGIndices[1] = 1;
	m_BGIndices[2] = 2;
	m_BGIndices[3] = 3;

	m_BGIndexBuffer.Create(m_BGIndices);
}

// ===================================================================
// アニメーションの頂点設定
// ===================================================================
void LoadingScreen::SetupAnimationVertices()
{
	float halfWidth = m_AnimWidth / 2.0f;
	float halfHeight = m_AnimHeight / 2.0f;

	m_AnimVertices.resize(4);

	// 右下に配置
	m_AnimVertices[0].position = Vector3(m_AnimPosX - halfWidth, m_AnimPosY + halfHeight, 0);
	m_AnimVertices[1].position = Vector3(m_AnimPosX + halfWidth, m_AnimPosY + halfHeight, 0);
	m_AnimVertices[2].position = Vector3(m_AnimPosX - halfWidth, m_AnimPosY - halfHeight, 0);
	m_AnimVertices[3].position = Vector3(m_AnimPosX + halfWidth, m_AnimPosY - halfHeight, 0);

	for (int i = 0; i < 4; i++)
	{
		m_AnimVertices[i].color = Color(1, 1, 1, 1);
	}

	// 初期UV（最初のフレーム）
	m_AnimVertices[0].uv = Vector2(0.0f, 0.0f);
	m_AnimVertices[1].uv = Vector2(0.25f, 0.0f);
	m_AnimVertices[2].uv = Vector2(0.0f, 1.0f);
	m_AnimVertices[3].uv = Vector2(0.25f, 1.0f);

	m_AnimVertexBuffer.Create(m_AnimVertices);

	m_AnimIndices.resize(4);
	m_AnimIndices[0] = 0;
	m_AnimIndices[1] = 1;
	m_AnimIndices[2] = 2;
	m_AnimIndices[3] = 3;

	m_AnimIndexBuffer.Create(m_AnimIndices);
}

// ===================================================================
// 更新
// ===================================================================
void LoadingScreen::Update(float deltaTime)
{
	if (!m_Initialized) return;

	// フレーム時間を更新
	m_FrameTime += deltaTime;

	// フレーム切り替え
	if (m_FrameTime >= m_FrameDuration)
	{
		m_FrameTime -= m_FrameDuration;
		m_CurrentFrame = (m_CurrentFrame + 1) % m_TotalFrames;
		UpdateAnimationUV();
	}
}

// ===================================================================
// アニメーションUV更新
// ===================================================================
void LoadingScreen::UpdateAnimationUV()
{
	// 4×1スプライトシートのUV計算
	float frameWidth = 1.0f / static_cast<float>(m_TotalFrames);
	float u0 = static_cast<float>(m_CurrentFrame) * frameWidth;
	float u1 = u0 + frameWidth;

	m_AnimVertices[0].uv = Vector2(u0, 0.0f);
	m_AnimVertices[1].uv = Vector2(u1, 0.0f);
	m_AnimVertices[2].uv = Vector2(u0, 1.0f);
	m_AnimVertices[3].uv = Vector2(u1, 1.0f);

	// 頂点バッファ更新
	m_AnimVertexBuffer.Create(m_AnimVertices);
}

// ===================================================================
// 描画
// ===================================================================
void LoadingScreen::Draw()
{
	if (!m_Initialized) return;

	ID3D11DeviceContext* deviceContext = Renderer::GetDeviceContext();

	// ビュー・プロジェクション行列設定（UI用正射影）
	Matrix viewMatrix = Matrix::Identity;
	Matrix projMatrix = DirectX::XMMatrixOrthographicLH(
		static_cast<float>(Application::GetWidth()),
		static_cast<float>(Application::GetHeight()),
		0.0f, 1000.0f
	);
	Renderer::SetViewMatrix(&viewMatrix);
	Renderer::SetProjectionMatrix(&projMatrix);

	// ワールド行列（単位行列）
	Matrix worldMatrix = Matrix::Identity;
	Renderer::SetWorldMatrix(&worldMatrix);

	// トポロジー設定
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// シェーダー・マテリアル設定
	m_Shader->SetGPU();
	m_Material->SetGPU();

	// UV行列（通常の場合）
	Renderer::SetUV(0, 0, 1, 1);

	// ===================================================================
	// 背景描画
	// ===================================================================
	m_BGVertexBuffer.SetGPU();
	m_BGIndexBuffer.SetGPU();
	m_BackgroundTexture->SetGPU();

	deviceContext->DrawIndexed((UINT)m_BGIndices.size(), 0, 0);

	m_BackgroundTexture->UnbindGPU();

	// ===================================================================
	// アニメーション描画
	// ===================================================================
	/*m_AnimVertexBuffer.SetGPU();
	m_AnimIndexBuffer.SetGPU();
	m_AnimTexture->SetGPU();

	deviceContext->DrawIndexed((UINT)m_AnimIndices.size(), 0, 0);

	m_AnimTexture->UnbindGPU();*/
}

// ===================================================================
// 終了処理
// ===================================================================
void LoadingScreen::Uninit()
{
	m_Initialized = false;
}
