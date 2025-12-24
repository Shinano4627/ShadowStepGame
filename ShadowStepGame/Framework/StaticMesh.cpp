// ===================================================================
// StaticMesh.cpp
// Assimpを使った静的メッシュクラスの実装（ResourceManager対応版）
// ===================================================================

#include "StaticMesh.h"
#include "AssimpPerse.h"
#include <iostream>

// ===================================================================
// メッシュ読み込み
// ===================================================================
void StaticMesh::Load(const std::string& filename, const std::string& textureDirectory)
{
    std::vector<AssimpPerse::SUBSET> subsets{};
    std::vector<std::vector<AssimpPerse::VERTEX>> vertices{};
    std::vector<std::vector<unsigned int>> indices{};
    std::vector<AssimpPerse::MATERIAL> materials{};

    // assimpを使用してモデルデータを取得
    AssimpPerse::GetModelData(filename, textureDirectory);

    subsets = AssimpPerse::GetSubsets();      // サブセット情報取得
    vertices = AssimpPerse::GetVertices();    // 頂点データ（メッシュ単位）
    indices = AssimpPerse::GetIndices();      // インデックスデータ（メッシュ単位）
    materials = AssimpPerse::GetMaterials();  // マテリアル情報取得

    // ===================================================================
    // テクスチャをResourceManager経由で読み込み
    // ===================================================================
    auto assimpTextures = AssimpPerse::GetTextures();
    m_TextureHandles.clear();
    m_TextureHandles.reserve(assimpTextures.size());

    for (size_t i = 0; i < assimpTextures.size(); i++)
    {
        if (assimpTextures[i])
        {
            // テクスチャが既に読み込まれている場合
            // （埋め込みテクスチャなど）
            std::string texturePath = filename + "_EmbeddedTexture_" + std::to_string(i);

            // ResourceHandleでラップ
            ResourceHandle<Texture> handle(
                std::move(assimpTextures[i]),
                texturePath
            );

            m_TextureHandles.push_back(handle);
        }
        else
        {
            // 空のハンドル
            m_TextureHandles.push_back(ResourceHandle<Texture>());
        }
    }

    // ===================================================================
    // 頂点データ作成
    // ===================================================================
    for (const auto& mv : vertices)
    {
        for (const auto& v : mv)
        {
            VERTEX_3D vertex{};
            vertex.position = DirectX::SimpleMath::Vector3(v.pos.x, v.pos.y, v.pos.z);
            vertex.normal = DirectX::SimpleMath::Vector3(v.normal.x, v.normal.y, v.normal.z);
            vertex.uv = DirectX::SimpleMath::Vector2(v.texcoord.x, v.texcoord.y);
            vertex.color = DirectX::SimpleMath::Color(v.color.r, v.color.g, v.color.b, v.color.a);

            m_vertices.emplace_back(vertex);
        }
    }

    // ===================================================================
    // インデックスデータ作成
    // ===================================================================
    for (const auto& mi : indices)
    {
        for (const auto& index : mi)
        {
            m_indices.emplace_back(index);
        }
    }

    // ===================================================================
    // サブセットデータ作成
    // ===================================================================
    for (const auto& sub : subsets)
    {
        SUBSET subset{};
        subset.VertexBase = sub.VertexBase;       // 頂点の開始位置
        subset.VertexNum = sub.VertexNum;         // サブセット内の頂点数
        subset.IndexBase = sub.IndexBase;         // インデックスの開始位置
        subset.IndexNum = sub.IndexNum;           // サブセット内のインデックス数
        subset.MtrlName = sub.mtrlname;           // マテリアル名
        subset.MaterialIdx = sub.materialindex;   // マテリアル配列のインデックス
        m_Subsets.emplace_back(subset);
    }

    // ===================================================================
    // マテリアルデータ作成
    // ===================================================================
    for (const auto& m : materials)
    {
        MATERIAL material{};
        material.Ambient = DirectX::SimpleMath::Color(
            m.Ambient.r, m.Ambient.g, m.Ambient.b, m.Ambient.a);
        material.Diffuse = DirectX::SimpleMath::Color(
            m.Diffuse.r, m.Diffuse.g, m.Diffuse.b, m.Diffuse.a);
        material.Specular = DirectX::SimpleMath::Color(
            m.Specular.r, m.Specular.g, m.Specular.b, m.Specular.a);
        material.Emission = DirectX::SimpleMath::Color(
            m.Emission.r, m.Emission.g, m.Emission.b, m.Emission.a);
        material.Shininess = m.Shininess;

        if (m.texturename.empty())
        {
            material.TextureEnable = FALSE;
            m_TextureNames.emplace_back("");
        }
        else
        {
            material.TextureEnable = TRUE;
            m_TextureNames.emplace_back(m.texturename);
        }

        m_Materials.emplace_back(material);
    }

    std::cout << "[StaticMesh] Loaded: " << filename << std::endl;
    std::cout << "  Vertices: " << m_vertices.size() << std::endl;
    std::cout << "  Indices: " << m_indices.size() << std::endl;
    std::cout << "  Subsets: " << m_Subsets.size() << std::endl;
    std::cout << "  Materials: " << m_Materials.size() << std::endl;
    std::cout << "  Textures: " << m_TextureHandles.size() << std::endl;
}

// ===================================================================
// 互換性のためのテクスチャ取得（非推奨）
// ===================================================================
std::vector<std::unique_ptr<Texture>> StaticMesh::GetTextures()
{
    std::vector<std::unique_ptr<Texture>> result;
    result.reserve(m_TextureHandles.size());

    for (auto& handle : m_TextureHandles)
    {
        if (handle)
        {
            // 新しいTextureインスタンスを作成してコピー
            // 注意: これは効率的ではないため、GetTextureHandles()の使用を推奨
            result.push_back(std::make_unique<Texture>());
            // TODO: Textureクラスにコピーコンストラクタが必要
        }
        else
        {
            result.push_back(nullptr);
        }
    }

    return result;
}