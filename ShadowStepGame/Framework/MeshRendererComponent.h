// ===================================================================
// MeshRendererComponent.h
// GameObjectに描画機能を追加するコンポーネント（ResourceManager対応版）
// ===================================================================
#pragma once
#include "GameObject.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Renderer.h"
#include "ResourceManager.h"
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
    // リソースハンドル（ResourceManager経由で管理）
    ResourceHandle<StaticMesh> m_MeshHandle;
    ResourceHandle<Shader> m_ShaderHandle;

    // マテリアル（サブセットごとに）
    std::vector<std::unique_ptr<Material>> m_Materials;

    // 描画ヘルパー（頂点・インデックスバッファの管理）
    MeshRenderer m_Renderer;

    // モデルデータパス
    std::string m_ModelPath;

    // テクスチャディレクトリパス
    std::string m_TexturePath;

    // アニメーション
    std::string m_curAnimation;
    int m_Frame = 0;
    
    // 初期化済みフラグ
    bool m_Initialized = false;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    MeshRendererComponent(
        const std::string& modelPath,
        const std::string& texturePath = "")
        : m_Initialized(false)
        , m_ModelPath(modelPath)
        , m_TexturePath(texturePath)
    {
        SetRenderLayer(RenderLayer::WORLD);
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        // メッシュがセットされていればレンダラーを初期化
        if (m_MeshHandle)
        {
            auto* mesh = m_MeshHandle.Get();
            if (mesh)
            {
                // MeshRendererを初期化（頂点・インデックスバッファを作成）
                m_Renderer.Init(*mesh);

                // マテリアルを作成
                CreateMaterials();

                // 初期化済みフラグを立てる
                m_Initialized = true;
            }
        }
    }

    // ===================================================================
    // メッシュ設定（shared_ptr版）
    // ===================================================================
    void SetMesh(std::shared_ptr<StaticMesh> mesh)
    {
        if (!mesh)
        {
            std::cerr << "[MeshRendererComponent] Null mesh provided" << std::endl;
            return;
        }

        // ResourceHandleを作成（既存のshared_ptrをラップ）
        m_MeshHandle = ResourceHandle<StaticMesh>(mesh, m_ModelPath);

        // MeshRendererを初期化
        m_Renderer.Init(*mesh);

        // マテリアルを作成
        CreateMaterials();

        // 初期化済みフラグを立てる
        m_Initialized = true;
    }

    // ===================================================================
    // メッシュ設定（ResourceHandle版）
    // ===================================================================
    void SetMesh(const ResourceHandle<StaticMesh>& meshHandle)
    {
        m_MeshHandle = meshHandle;

        if (m_MeshHandle)
        {
            auto* mesh = m_MeshHandle.Get();
            if (mesh)
            {
                // MeshRendererを初期化
                m_Renderer.Init(*mesh);

                // マテリアルを作成
                CreateMaterials();

                // 初期化済みフラグを立てる
                m_Initialized = true;
            }
        }
    }

    // ===================================================================
    // シェーダー設定（shared_ptr版）
    // ===================================================================
    
    // 使用するシェーダーを設定
    void SetShader(std::shared_ptr<Shader> shader)
    {
        if (!shader)
        {
            std::cerr << "[MeshRendererComponent] Null shader provided" << std::endl;
            return;
        }

        // ResourceHandleを作成
        m_ShaderHandle = ResourceHandle<Shader>(shader, "CustomShader");
    }

    // ===================================================================
    // シェーダー設定（ResourceHandle版）
    // ===================================================================
    void SetShader(const ResourceHandle<Shader>& shaderHandle)
    {
        m_ShaderHandle = shaderHandle;
    }

    // ===================================================================
    // シェーダー取得
    // ===================================================================
    std::shared_ptr<Shader> GetShader() const
    {
        return m_ShaderHandle.GetShared();
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
    const std::string& GetModelPath() const { return m_ModelPath; }
    const std::string& GetTexturePath() const { return m_TexturePath; }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        auto* mesh = m_MeshHandle.Get();
        if (!mesh) return;
        
        m_Frame++;
        mesh->UpdateAnimation(m_curAnimation.c_str(), m_Frame);
    }

    // ===================================================================
    // 描画処理
    // ===================================================================
    void Draw(Camera* camera) override
    {
        // 初期化チェック
        if (!m_Initialized || !m_MeshHandle || !m_ShaderHandle)
        {
            return;
        }

        auto* mesh = m_MeshHandle.Get();
        auto* shader = m_ShaderHandle.Get();

        if (!mesh || !shader)
        {
            return;
        }

        // カメラ設定（WORLD層のみ）
        if (camera && GetRenderLayer() == RenderLayer::WORLD)
        {
            camera->SetCamera(0); // 3Dモード
        }

        // Transformからワールド行列を取得
        Matrix worldMatrix = m_pOwner->GetTransform().GetWorldMatrix();
        // GPUに設定
        Renderer::SetWorldMatrix(&worldMatrix);

        // シェーダー設定
        shader->SetGPU();

        // ボーン行列をGPUに設定
        Renderer::ResetBoneMatrix();
        const auto& boneMatrices = mesh->GetBoneMatrices();
        if (boneMatrices.size() > 0)
        {
            Renderer::SetBoneMatrix(boneMatrices);
        }  

        // 描画前処理
        m_Renderer.BeforeDraw();

        // サブセットごとに描画
        const auto& subsets = mesh->GetSubsets();
        const auto& textureHandles = mesh->GetTextureHandles();

        for (size_t i = 0; i < subsets.size(); i++)
        {
            const auto& subset = subsets[i];

            // マテリアルをGPUに設定
            if (i < m_Materials.size())
            {
                m_Materials[i]->SetGPU();
            }

            // テクスチャをGPUに設定
            if (subset.MaterialIdx < textureHandles.size() && textureHandles[subset.MaterialIdx])
            {
                auto* texture = textureHandles[subset.MaterialIdx].Get();
                if (texture)
                {
                    texture->SetGPU();
                }
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

        // リソースハンドルをリセット
        m_MeshHandle = ResourceHandle<StaticMesh>();
        m_ShaderHandle = ResourceHandle<Shader>();

        m_Initialized = false;
    }

    // ===================================================================
    // アニメーション関係
    // ===================================================================
    void ChangeAnimation(std::string newAnimaiton)
    {
        m_Frame = 0;
        m_curAnimation = newAnimaiton;
    }

private:
    // ===================================================================
    // マテリアル作成
    // ===================================================================
    void CreateMaterials()
    {
        auto* mesh = m_MeshHandle.Get();
        if (!mesh) return;

        // 既存のマテリアルをクリア
        m_Materials.clear();

        // メッシュからマテリアル情報を取得
        const auto& meshMaterials = mesh->GetMaterials();

        // マテリアルごとにMaterialオブジェクトを作成
        for (size_t i = 0; i < meshMaterials.size(); i++)
        {
            const auto& mtrl = meshMaterials[i];

            // ユニークな名前を生成
            std::string materialName = m_ModelPath + "_Material_" + std::to_string(i);

            // ResourceManager経由でマテリアルを作成（キャッシュ活用）
            auto materialHandle = M_RESOURCE.CreateMaterial(materialName, mtrl);

            if (materialHandle)
            {
                // shared_ptrとして取得してunique_ptrに変換
                auto material = std::make_unique<Material>(mtrl);
                m_Materials.push_back(std::move(material));
            }
        }
    }
};