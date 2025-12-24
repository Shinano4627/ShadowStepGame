// ===================================================================
// SimplePlaneRendererComponent.cpp
// シンプルな平面描画実装
// ===================================================================
#include "SimplePlaneRendererComponent.h"

using namespace DirectX::SimpleMath;

// ===================================================================
// 初期化
// ===================================================================
void SimplePlaneRendererComponent::Init()
{
    // 平面の頂点データ作成（Y軸が上向き）
    m_Vertices.resize(4);

    // 4頂点（平面、Y=0）
    m_Vertices[0].position = Vector3(-1, 0, 1);   // 左手前
    m_Vertices[1].position = Vector3(1, 0, 1);    // 右手前
    m_Vertices[2].position = Vector3(-1, 0, -1);  // 左奥
    m_Vertices[3].position = Vector3(1, 0, -1);   // 右奥

    // 法線は上向き
    for (int i = 0; i < 4; i++)
    {
        m_Vertices[i].normal = Vector3(0, 1, 0);
        m_Vertices[i].color = m_Color;
    }

    // UV座標（テクスチャリピート対応）
    m_Vertices[0].uv = Vector2(0, 0);
    m_Vertices[1].uv = Vector2(1, 0);
    m_Vertices[2].uv = Vector2(0, 1);
    m_Vertices[3].uv = Vector2(1, 1);

    // インデックスデータ（2つの三角形）
    m_Indices = { 0, 1, 2, 1, 3, 2 };

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
        "SimplePlane_Material_" + m_TexturePath,
        mtrl);

    m_Initialized = true;
}

// ===================================================================
// 描画
// ===================================================================
void SimplePlaneRendererComponent::Draw(Camera* camera)
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
void SimplePlaneRendererComponent::Uninit()
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
void SimplePlaneRendererComponent::SetTexture(const std::string& texturePath)
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
            "SimplePlane_Material_" + m_TexturePath,
            mtrl);
    }
}

// ===================================================================
// 色設定
// ===================================================================
void SimplePlaneRendererComponent::SetColor(const DirectX::SimpleMath::Color& color)
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