// ===================================================================
// SimpleCubeRendererComponent.cpp
// シンプルなキューブ描画実装
// ===================================================================
#include "SimpleCubeRendererComponent.h"

using namespace DirectX::SimpleMath;

// ===================================================================
// 初期化
// ===================================================================
void SimpleCubeRendererComponent::Init()
{
    // キューブの頂点データ作成（24頂点、各面4頂点）
    m_Vertices.resize(24);

    // 面1（上面 +Y）
    m_Vertices[0].position = Vector3(-1, 1, 1);
    m_Vertices[1].position = Vector3(1, 1, 1);
    m_Vertices[2].position = Vector3(-1, 1, -1);
    m_Vertices[3].position = Vector3(1, 1, -1);
    for (int i = 0; i < 4; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(0, 1, 0);
        m_Vertices[i].uv = Vector2((i % 2), (i / 2));
    }

    // 面2（奥面 -Z）
    m_Vertices[4].position = Vector3(-1, 1, -1);
    m_Vertices[5].position = Vector3(1, 1, -1);
    m_Vertices[6].position = Vector3(-1, -1, -1);
    m_Vertices[7].position = Vector3(1, -1, -1);
    for (int i = 4; i < 8; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(0, 0, -1);
        m_Vertices[i].uv = Vector2((i % 2), ((i - 4) / 2));
    }

    // 面3（右面 +X）
    m_Vertices[8].position = m_Vertices[3].position;
    m_Vertices[9].position = m_Vertices[1].position;
    m_Vertices[10].position = m_Vertices[7].position;
    m_Vertices[11].position = Vector3(1, -1, 1);
    for (int i = 8; i < 12; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(1, 0, 0);
        m_Vertices[i].uv = Vector2((i % 2), ((i - 8) / 2));
    }

    // 面4（下面 -Y）
    m_Vertices[12].position = m_Vertices[7].position;
    m_Vertices[13].position = m_Vertices[11].position;
    m_Vertices[14].position = m_Vertices[6].position;
    m_Vertices[15].position = Vector3(-1, -1, 1);
    for (int i = 12; i < 16; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(0, -1, 0);
        m_Vertices[i].uv = Vector2((i % 2), ((i - 12) / 2));
    }

    // 面5（手前面 +Z）
    m_Vertices[16].position = m_Vertices[1].position;
    m_Vertices[17].position = m_Vertices[0].position;
    m_Vertices[18].position = m_Vertices[11].position;
    m_Vertices[19].position = m_Vertices[15].position;
    for (int i = 16; i < 20; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(0, 0, 1);
        m_Vertices[i].uv = Vector2((i % 2), ((i - 16) / 2));
    }

    // 面6（左面 -X）
    m_Vertices[20].position = m_Vertices[0].position;
    m_Vertices[21].position = m_Vertices[2].position;
    m_Vertices[22].position = m_Vertices[15].position;
    m_Vertices[23].position = m_Vertices[6].position;
    for (int i = 20; i < 24; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(-1, 0, 0);
        m_Vertices[i].uv = Vector2((i % 2), ((i - 20) / 2));
    }

    // インデックスデータ（36インデックス、12三角形）
    m_Indices = {
        0, 1, 2,    1, 3, 2,    // 上面
        4, 5, 6,    5, 7, 6,    // 奥面
        8, 9, 11,   11, 10, 8,  // 右面
        12, 13, 15, 15, 14, 12, // 下面
        16, 17, 19, 19, 18, 16, // 手前面
        20, 21, 23, 23, 22, 20, // 左面
    };

    // バッファ作成
    m_VertexBuffer.Create(m_Vertices);
    m_IndexBuffer.Create(m_Indices);

    // ResourceManager経由でシェーダー読み込み
    m_ShaderHandle = M_RESOURCE.LoadShader(
        "shader/litTextureVS.hlsl",
        "shader/litTexturePS.hlsl");

    // テクスチャ読み込み（指定されている場合）
    if (!m_TexturePath.empty())
    {
        SetTexture(m_TexturePath);
    }

    // マテリアル作成
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Ambient = Color(1, 1, 1, 1);
    mtrl.Specular = Color(0, 0, 0, 0);
    mtrl.Emission = Color(0, 0, 0, 0);
    mtrl.Shininess = 0.0f;
    mtrl.TextureEnable = !m_TexturePath.empty();

    m_MaterialHandle = M_RESOURCE.CreateMaterial(
        "SimpleCube_Material_" + m_TexturePath,
        mtrl);

    m_Initialized = true;
}

// ===================================================================
// 描画
// ===================================================================
void SimpleCubeRendererComponent::Draw(Camera* camera)
{
    if (!m_Initialized || !m_ShaderHandle)
    {
        return;
    }

    // カメラ設定（WORLD層のみ）
    if (camera && GetRenderLayer() == RenderLayer::WORLD)
    {
        camera->SetCamera(0); // 3Dモード
    }

    // ワールド行列設定
    Matrix worldMatrix = m_pOwner->GetTransform().GetWorldMatrix();
    Renderer::SetWorldMatrix(&worldMatrix);

    // 描画処理
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
    devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // シェーダー設定
    m_ShaderHandle->SetGPU();

    // バッファ設定
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();

    // テクスチャ設定
    if (m_TextureHandle)
    {
        m_TextureHandle->SetGPU();
    }

    // マテリアル設定
    if (m_MaterialHandle)
    {
        m_MaterialHandle->SetGPU();
    }

    // 描画
    devicecontext->DrawIndexed(
        static_cast<UINT>(m_Indices.size()),
        0,
        0);
}

// ===================================================================
// 終了処理
// ===================================================================
void SimpleCubeRendererComponent::Uninit()
{
    m_ShaderHandle = ResourceHandle<Shader>();
    m_TextureHandle = ResourceHandle<Texture>();
    m_MaterialHandle = ResourceHandle<Material>();

    m_Vertices.clear();
    m_Indices.clear();
}

// ===================================================================
// テクスチャ設定
// ===================================================================
void SimpleCubeRendererComponent::SetTexture(const std::string& texturePath)
{
    if (texturePath.empty())
    {
        m_TextureHandle = ResourceHandle<Texture>();
        m_TexturePath = "";
        return;
    }

    // ResourceManager経由でテクスチャ読み込み
    m_TextureHandle = M_RESOURCE.LoadTexture(texturePath);
    m_TexturePath = texturePath;

    // マテリアル更新（テクスチャ有効化）
    if (m_Initialized)
    {
        MATERIAL mtrl{};
        mtrl.Diffuse = Color(1, 1, 1, 1);
        mtrl.Ambient = Color(1, 1, 1, 1);
        mtrl.TextureEnable = TRUE;

        m_MaterialHandle = M_RESOURCE.CreateMaterial(
            "SimpleCube_Material_" + m_TexturePath,
            mtrl);
    }
}

// ===================================================================
// 色設定
// ===================================================================
void SimpleCubeRendererComponent::SetColor(const DirectX::SimpleMath::Color& color)
{
    m_Color = color;

    // 頂点カラーを更新
    for (auto& v : m_Vertices)
    {
        v.color = m_Color;
    }

    if (m_Initialized)
    {
        m_VertexBuffer.Modify(m_Vertices);
    }
}