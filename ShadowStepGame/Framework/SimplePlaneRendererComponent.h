// ===================================================================
// SimplePlaneRendererComponent.h
// シンプルな平面描画
// ===================================================================
#pragma once
#include "MeshComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include <vector>

class SimplePlaneRendererComponent : public MeshComponent
{
private:
    // 描画リソース
    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    // リソースハンドル
    ResourceHandle<Shader> m_ShaderHandle;
    ResourceHandle<Texture> m_TextureHandle;
    ResourceHandle<Material> m_MaterialHandle;

    // テクスチャパス
    std::string m_TexturePath;

    bool m_Initialized = false;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SimplePlaneRendererComponent(
        const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1),
        const std::string& texturePath = "")
        : m_TexturePath(texturePath)
    {
        m_Color = color;
        SetRenderLayer(RenderLayer::WORLD);
    }

    // ===================================================================
    // ライフサイクル
    // ===================================================================
    void Init() override;
    void Draw(Camera* camera) override;
    void Uninit() override;

    // ===================================================================
    // テクスチャ設定
    // ===================================================================
    void SetTexture(const std::string& texturePath);
    const std::string& GetTexturePath() const { return m_TexturePath; }

    // ===================================================================
    // 色設定
    // ===================================================================
    void SetColor(const DirectX::SimpleMath::Color& color);

};