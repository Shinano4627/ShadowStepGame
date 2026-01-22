// ===================================================================
// Texture2D.cpp
// 2Dテクスチャレンダリングコンポーネント実装
// ===================================================================
#include "Texture2D.h"

using namespace std;
using namespace DirectX::SimpleMath;

// ===================================================================
// 初期化処理
// ===================================================================
void Texture2D::Init()
{
    // 頂点データ作成
    m_Vertices.resize(4);

    m_Vertices[0].position = Vector3(-0.5f, 0.5f, 0);
    m_Vertices[1].position = Vector3(0.5f, 0.5f, 0);
    m_Vertices[2].position = Vector3(-0.5f, -0.5f, 0);
    m_Vertices[3].position = Vector3(0.5f, -0.5f, 0);

    // 頂点カラー設定
    for (int i = 0; i < 4; i++)
    {
        m_Vertices[i].color = m_Color;
        m_Vertices[i].normal = Vector3(0, 0, -1);
    }

    // UV座標設定
    m_Vertices[0].uv = Vector2(0, 0);
    m_Vertices[1].uv = Vector2(1, 0);
    m_Vertices[2].uv = Vector2(0, 1);
    m_Vertices[3].uv = Vector2(1, 1);

    // 頂点バッファ生成
    m_VertexBuffer.Create(m_Vertices);

    // インデックスデータ作成
    m_Indices = { 0, 1, 2, 3 };
    m_IndexBuffer.Create(m_Indices);

    // ResourceManager経由でシェーダー読み込み
    m_ShaderHandle = M_RESOURCE.LoadShader(
        "shader/unlitTextureVS.hlsl",
        "shader/unlitTexturePS.hlsl");

    // テクスチャ読み込み（パスが指定されている場合）
    if (!m_TexturePath.empty())
    {
        SetTexture(m_TexturePath);
    }

    // マテリアル作成
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.TextureEnable = !m_TexturePath.empty();

    m_MaterialHandle = M_RESOURCE.CreateMaterial(
        "Texture2D_Material_" + m_TexturePath,
        mtrl);

    m_Initialized = true;
}

// ===================================================================
// 更新処理
// ===================================================================
void Texture2D::Update()
{
    // 特に処理なし
}

// ===================================================================
// 描画処理
// ===================================================================
void Texture2D::Draw(Camera* cam)
{
    if (!m_Initialized || !m_ShaderHandle)
    {
        return;
    }

    // カメラ設定（2D用）
    if (cam)
    {
        cam->SetCamera(1); // 2Dモード
    }

    // ワールド行列設定
    Matrix worldMatrix = m_pOwner->GetTransform().GetWorldMatrix();
    Renderer::SetWorldMatrix(&worldMatrix);

    // 描画処理
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
    devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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

    // UV設定
    float u = m_NumU - 1.0f;
    float v = m_NumV - 1.0f;
    float uw = 1.0f / m_SplitX;
    float vh = 1.0f / m_SplitY;
    Renderer::SetUV(u, v, uw, vh);

    // 描画
    devicecontext->DrawIndexed(
        static_cast<UINT>(m_Indices.size()),
        0,
        0);
}

// ===================================================================
// 終了処理
// ===================================================================
void Texture2D::Uninit()
{
    // リソースハンドルは自動的に解放される
    m_TextureHandle = ResourceHandle<Texture>();
    m_ShaderHandle = ResourceHandle<Shader>();
    m_MaterialHandle = ResourceHandle<Material>();

    m_Vertices.clear();
    m_Indices.clear();
}

// ===================================================================
// テクスチャ設定
// ===================================================================
void Texture2D::SetTexture(const std::string& texturePath)
{
    if (texturePath.empty())
    {
        std::cout << "[Texture2D] Empty texture path provided" << std::endl;
        m_TextureHandle = ResourceHandle<Texture>();
        return;
    }

    // ResourceManager経由でテクスチャ読み込み
    m_TextureHandle = M_RESOURCE.LoadTexture(texturePath);
    m_TexturePath = texturePath;

    // マテリアル更新（テクスチャ有効化）
    if (m_MaterialHandle)
    {
        MATERIAL mtrl{};
        mtrl.Diffuse = Color(1, 1, 1, 1);
        mtrl.TextureEnable = TRUE;

        m_MaterialHandle = M_RESOURCE.CreateMaterial(
            "Texture2D_Material_" + m_TexturePath,
            mtrl);
    }
}

// ===================================================================
// UV座標設定
// ===================================================================
void Texture2D::SetUV(float nu, float nv, float sx, float sy)
{
    m_NumU = nu;
    m_NumV = nv;
    m_SplitX = sx;
    m_SplitY = sy;
}

void Texture2D::UpdateUV(bool updateU, bool updateV)
{
    if (updateU)
    {
        m_NumU += 1.0f;

        if (m_NumU > m_SplitX)
        {
            m_NumU -= m_SplitX;
        }
    }

    if (updateV)
    {
        m_NumV += 1.0f;
        if (m_NumV > m_SplitY)
        {
            m_NumV -= m_SplitY;
        }
    }
}

// ===================================================================
// 色設定
// ===================================================================
void Texture2D::SetColor(const DirectX::SimpleMath::Color& color)
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