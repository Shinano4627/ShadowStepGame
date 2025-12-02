// ===================================================================
// SimplePlaneRendererComponent.h
// シンプルな床（平面）描画コンポーネント
// ===================================================================
#pragma once
#include "MeshComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <vector>

class SimplePlaneRendererComponent : public MeshComponent
{
private:
    // 描画リソース
    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;
    std::shared_ptr<Shader> m_Shader;

    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    bool m_Initialized = false;
    float m_Size;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SimplePlaneRendererComponent(
        float size = 10.0f, const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1))
        : m_Size(size)
    {
        // 色を設定
        m_Color = color;
        // レイヤーをWORLDに設定
        SetRenderLayer(RenderLayer::WORLD);
    }

    SimplePlaneRendererComponent(const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1))
    {
        // 色を設定
        m_Color = color;

        m_Size = 0;
        // レイヤーをWORLDに設定
        SetRenderLayer(RenderLayer::WORLD);
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        using namespace DirectX::SimpleMath;

        // 平面の頂点データ作成（Y軸が上）
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

        // UV座標
        m_Vertices[0].uv = Vector2(0, 0);
        m_Vertices[1].uv = Vector2(1, 0);
        m_Vertices[2].uv = Vector2(0, 1);
        m_Vertices[3].uv = Vector2(1, 1);

        // インデックスデータ（2つの三角形）
        m_Indices = {
            0, 1, 2,
            1, 3, 2
        };

        // バッファ作成
        m_VertexBuffer.Create(m_Vertices);
        m_IndexBuffer.Create(m_Indices);

        // シェーダー作成
        m_Shader = std::make_shared<Shader>();
        m_Shader->Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

        m_Initialized = true;
    }

    // ===================================================================
    // 描画
    // ===================================================================
    void Draw(Camera* camera) override
    {
        if (!m_Initialized || !m_Shader)
        {
            return;
        }

        // カメラ設定（WORLD層のみ）
        if (camera && GetRenderLayer() == RenderLayer::WORLD)
        {
            camera->SetCamera();
        }

        // Transform からワールド行列を取得
        Matrix worldMatrix = m_pOwner->GetTransform().GetWorldMatrix();
        Renderer::SetWorldMatrix(&worldMatrix);

        // 描画
        ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
        devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_Shader->SetGPU();
        m_VertexBuffer.SetGPU();
        m_IndexBuffer.SetGPU();

        devicecontext->DrawIndexed(
            static_cast<UINT>(m_Indices.size()),
            0,
            0);
    }

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override
    {
        m_Shader.reset();
        m_Vertices.clear();
        m_Indices.clear();
    }

    // ===================================================================
    // 色設定
    // ===================================================================
    void SetColor(const DirectX::SimpleMath::Color& color)
    {
        m_Color = color;
        for (auto& v : m_Vertices)
        {
            v.color = m_Color;
        }
        if (m_Initialized)
        {
            m_VertexBuffer.Modify(m_Vertices);
        }
    }

    DirectX::SimpleMath::Color GetColor() { return m_Color; };
};