// ===================================================================
// SimpleCubeRendererComponent.h
// 外部リソース不要のシンプルなキューブ描画コンポーネント
// ===================================================================
#pragma once
#include "MeshComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <vector>

class SimpleCubeRendererComponent : public MeshComponent
{
private:
    // 描画リソース
    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;
    std::shared_ptr<Shader> m_Shader;

    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    bool m_Initialized = false;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SimpleCubeRendererComponent(const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1))
    {
        // 色を設定
        m_Color = color;
        // レイヤーをWORLDに設定
        SetRenderLayer(RenderLayer::WORLD);
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        using namespace DirectX::SimpleMath;

        // キューブの頂点データ作成
        m_Vertices.resize(24);

        // 1面（上）
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

        // 2面（奥）
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

        // 3面（右）
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

        // 4面（下）
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

        // 5面（手前）
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

        // 6面（左）
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

        // インデックスデータ
        m_Indices = {
            0, 1, 2,    1, 3, 2,
            4, 5, 6,    5, 7, 6,
            8, 9, 11,   11, 10, 8,
            12, 13, 15, 15, 14, 12,
            16, 17, 19, 19, 18, 16,
            20, 21, 23, 23, 22, 20,
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
};