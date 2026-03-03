// ===================================================================
// SceneTitle.cpp
// タイトルシーン実装
// ===================================================================
#include "SceneTitle.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "SoundManager.h"

// コンポーネント

#include <iostream>

void SceneTitle::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneTitle] Init START" << std::endl;

    m_TotalFrames = m_SpriteCols * m_SpriteRows;


    if (m_GameObjectList == nullptr)
    {
        // リストクラスのインスタンス作成
        m_GameObjectList = std::make_unique<GameObjectList>();
    }
    // 既存オブジェクトを削除
    m_GameObjectList->DeleteObjectList();

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    m_GameObjectList->MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_TITLE).c_str());

    // 追加コンポーネント
    {
        std::cout << "[SceneTitle] TitleCube created" << std::endl;
    }

    // シェーダー作成
    m_Shader = std::make_shared<Shader>();
    m_Shader->Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

    // マテリアル作成
    m_Material = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.TextureEnable = true;
    m_Material->Create(mtrl);

    // テクスチャ読み込み
    m_TitleTexture = M_RESOURCE.LoadTexture("asset/texture/title/start_text.png");

    // 頂点生成
    SetupTitleVertices();


    // カメラ初期化
    m_Camera.Init();

    // BGMの開始
    SOUND_MANAGER.PlayBGM(SOUND_LABEL::SOUND_LABEL_BGM_TITLE);

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneTitle] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== TITLE SCENE ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ENTER - Start Game" << std::endl;
    std::cout << "  SPACE - Go to TestCube" << std::endl;
    std::cout << "" << std::endl;
}

void SceneTitle::UnInit()
{
    std::cout << "[SceneTitle] UnInit" << std::endl;
    m_GameObjectList->DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    // BGMの停止
    SOUND_MANAGER.Stop(SOUND_LABEL::SOUND_LABEL_BGM_TITLE);

    m_isInitialized = false;
}

void SceneTitle::Update()
{
    if (m_TotalFrames <= 0)
        return;

    // カメラ更新
    m_Camera.Update();

    // Enterキーでゲーム開始
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneTitle] ENTER pressed - Starting Game" << std::endl;
        m_nextScene = SCENE_PROTO;
        return;
    }

    // GameObjectリストを更新
    m_GameObjectList->UpdateObjectList();

    m_FrameTime += 1.0f / 60.0f;

    if (m_FrameTime >= m_FrameDuration)
    {
        m_FrameTime -= m_FrameDuration;
        m_CurrentFrame = (m_CurrentFrame + 1) % m_TotalFrames;

        UpdateTitleUV();
    }

}

void SceneTitle::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    m_GameObjectList->DrawLayer(&m_Camera, RenderLayer::UI);

    DrawTitleText();

}

void SceneTitle::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    m_GameObjectList->DrawLayer(camera, RenderLayer::WORLD);
}

void SceneTitle::SetupTitleVertices()
{
    float hw = m_Width * 0.5f;
    float hh = m_Height * 0.5f;

    m_Vertices.resize(4);

    m_Vertices[0].position = { m_PosX - hw, m_PosY + hh, 0 };
    m_Vertices[1].position = { m_PosX + hw, m_PosY + hh, 0 };
    m_Vertices[2].position = { m_PosX - hw, m_PosY - hh, 0 };
    m_Vertices[3].position = { m_PosX + hw, m_PosY - hh, 0 };

    for (int i = 0; i < 4; i++)
        m_Vertices[i].color = Color(1, 1, 1, 1);

    float uSize = 1.0f / m_SpriteCols;
    float vSize = 1.0f / m_SpriteRows;

    m_Vertices[0].uv = { 0, 0 };
    m_Vertices[1].uv = { uSize, 0 };
    m_Vertices[2].uv = { 0, vSize };
    m_Vertices[3].uv = { uSize, vSize };

    m_VertexBuffer.Create(m_Vertices);

    m_Indices = { 0,1,2,3 };
    m_IndexBuffer.Create(m_Indices);
}

void SceneTitle::UpdateTitleUV()
{
    int column = m_CurrentFrame % m_SpriteCols;
    int row = m_CurrentFrame / m_SpriteCols;

    float uSize = 1.0f / m_SpriteCols;
    float vSize = 1.0f / m_SpriteRows;

    float u0 = column * uSize;
    float v0 = row * vSize;

    float u1 = u0 + uSize;
    float v1 = v0 + vSize;


    m_Vertices[0].uv = { u0, v0 };
    m_Vertices[1].uv = { u1, v0 };
    m_Vertices[2].uv = { u0, v1 };
    m_Vertices[3].uv = { u1, v1 };


    m_VertexBuffer.Create(m_Vertices);
}

void SceneTitle::DrawTitleText()
{
    ID3D11DeviceContext* dc = Renderer::GetDeviceContext();

    Matrix view = Matrix::Identity;
    Matrix proj = DirectX::XMMatrixOrthographicLH(
        (float)Application::GetWidth(),
        (float)Application::GetHeight(),
        0.0f, 1000.0f
    );


    Renderer::SetViewMatrix(&view);
    Renderer::SetProjectionMatrix(&proj);

    Matrix world = Matrix::Identity;
    Renderer::SetWorldMatrix(&world);

    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    m_Shader->SetGPU();
    m_Material->SetGPU();

    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    m_TitleTexture->SetGPU();

    dc->DrawIndexed((UINT)m_Indices.size(), 0, 0);

    m_TitleTexture->UnbindGPU();
}
