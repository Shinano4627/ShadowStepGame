// ===================================================================
// MeshRendererComponent.h
// GameObjectに描画機能を追加するコンポーネント
// ===================================================================
#pragma once
#include "GameObject.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Renderer.h"
#include <memory>
#include <vector>

// ===================================================================
// MeshRendererComponent
// GameObjectに３Dメッシュの描画機能を追加
// ===================================================================
class MeshRendererComponent : public MeshComponent
{
private:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    
    // リソース(ResourceManagerから取得したshared_ptr)
    // 複数Objectで同じリソースを共有
    std::shared_ptr<StaticMesh> m_Mesh; // 描画するメッシュデータ
    std::shared_ptr<Shader> m_Shader;   // 使用するシェーダ

    // マテリアル(サブセットごとに)
    std::vector<std::unique_ptr<Material>> m_Materials;

    // 描画ヘルパー(頂点・インデックスバッファの管理)
    MeshRenderer m_Renderer;

    // モデルデータパス
    std::string m_modelPath;

    // テクスチャデータパス
    std::string m_texturePath;

    // 初期化済みフラグ
    bool m_Initialized = false;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    MeshRendererComponent(std::string _modelPath, std::string _texturePath)
        : m_Initialized(false),
        m_modelPath(_modelPath),
        m_texturePath(_texturePath)
        {}

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        // メッシュがセットされてればレンダラーを初期化
        if (m_Mesh)
        {
            // MeshRendererを初期化（頂点・インデックスバッファを作成）
            m_Renderer.Init(*m_Mesh);

            // マテリアルを作成
            CreateMaterials();

            // 初期化済みフラグを立てる
            m_Initialized = true;
        }
    }

    // ===================================================================
    // メッシュ設定
    // ===================================================================
    void SetMesh(std::shared_ptr<StaticMesh> mesh)
    {
        m_Mesh = mesh;

        if (m_Mesh)
        {
            // MeshRendererを初期化
            m_Renderer.Init(*m_Mesh);

            // マテリアルを作成
            CreateMaterials();
            
            // 初期化済みフラグを立てる
            m_Initialized = true;
        }
    }

    // ===================================================================
    // シェーダー設定
    // ===================================================================
    
    // 使用するシェーダーを設定
    void SetShader(std::shared_ptr<Shader> shader)
    {
        m_Shader = shader;
    }

    // 使用されているシェーダを取得
    std::shared_ptr<Shader> GetShader() const
    {
        return m_Shader;
    }

    // ===================================================================
    // マテリアル取得
    // ===================================================================
    
    // 指定したインデックスのマテリアルを取得
    Material* GetMaterial(size_t index)
    {
        if (index < m_Materials.size())
        {
            return m_Materials[index].get();
        }
        return nullptr;
    }
    
    // すべてのマテリアルを取得
    const std::vector<std::unique_ptr<Material>>& GetMaterials() const
    {
        return m_Materials;
    }

    // ===================================================================
    // データ取得
    // ===================================================================
    std::string GetModelPath() { return m_modelPath; }     // モデルデータの格納パス
    std::string GetTexturePath() { return m_texturePath; }    // テクスチャデータパス

    // ===================================================================
    // 描画処理
    // ===================================================================
    void Draw(Camera* camera) override
    {
        // 初期化チェック
        if (!m_Initialized || !m_Mesh || !m_Shader)
        {
            return;
        }

        // Transformからワールド行列を取得
        Matrix worldMatrix = m_pOwner->GetTransform().GetWorldMatrix();
        // GPUに設定
        Renderer::SetWorldMatrix(&worldMatrix);

        // シェーダー設定
        m_Shader->SetGPU();

        // 描画前処理
        m_Renderer.BeforeDraw();

        // サブセットごとに描画
        const auto& subsets = m_Mesh->GetSubsets();     // サブセット情報を取得
        const auto& textures = m_Mesh->GetTextures();   // テクスチャリストを取得

        for (size_t i = 0; i < subsets.size(); i++)
        {
            const auto& subset = subsets[i];

            // マテリアルをGPUに設定
            if (i < m_Materials.size())
            {
                m_Materials[i]->SetGPU();
            }

            // テクスチャをGPUに設定
            if (subset.MaterialIdx < textures.size() && textures[subset.MaterialIdx])
            {
                textures[subset.MaterialIdx]->SetGPU();
            }

            // サブセット描画
            m_Renderer.DrawSubset(
                subset.IndexNum,    // 描画するインデックス数
                subset.IndexBase,   // インデックスバッファの開始位置
                subset.VertexBase   // 頂点バッファの開始位置
            );
        }
    }

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override
    {
        // マテリアルをクリア
        m_Materials.clear();

        // メッシュとシェーダーをリセット
        m_Mesh.reset();
        m_Shader.reset();

        m_Initialized = false;
    }

private:
    // ===================================================================
    // マテリアル作成
    // ===================================================================
    void CreateMaterials()
    {
        if (!m_Mesh) return;

        // 既存のマテリアルをクリア
        m_Materials.clear();

        // メッシュからマテリアル情報を取得
        const auto& meshMaterials = m_Mesh->GetMaterials();

        // マテリアルごとにMaterialオブジェクトを作成
        for (const auto& mtrl : meshMaterials)
        {
            auto material = std::make_unique<Material>(mtrl);
            m_Materials.push_back(std::move(material));
        }
    }
};