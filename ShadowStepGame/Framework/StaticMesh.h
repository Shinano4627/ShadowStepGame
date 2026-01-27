// ===================================================================
// StaticMesh.h
// Assimpを使った静的メッシュクラス（ResourceManager対応版）
// ===================================================================
#pragma once

#include <simplemath.h>
#include <string>
#include <vector>
#include <memory>
#include "Texture.h"
#include "Mesh.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "AssimpPerse.h"

class StaticMesh : public Mesh
{
private:
	AssimpPerse::FBXLoader m_FBXLoader;	// Assimpを利用したデータ読み込み
	const aiScene* m_pScene = nullptr;		// ロード済みモデル情報
    std::vector<MATERIAL> m_Materials;                      // マテリアル情報
    std::vector<std::string> m_TextureNames;                // テクスチャ名
    std::vector<SUBSET> m_Subsets;                          // サブセット情報
    std::vector<ResourceHandle<Texture>> m_TextureHandles;  // テクスチャハンドル（ResourceManager経由）
	std::unordered_map<std::string, AssimpPerse::BONE> m_Bones;	// ボーンデータ
	std::unordered_map<std::string, const aiScene*> m_Animations;		// アニメーションデータ
	std::vector<DirectX::SimpleMath::Matrix> m_Bonecombmtxcontainer;				// ボーンコンビネーション行列の配列
private:
	void UpdateBoneMatrix(const aiNode* node, const DirectX::SimpleMath::Matrix& matrix);
public:
    // ===================================================================
    // メッシュ読み込み
    // ===================================================================
    void Load(const std::string& filename, const std::string& textureDirectory = "");
	// ===================================================================
    // アニメーション
    // ===================================================================
	const aiScene* LoadAnimation(const aiScene* data, const char* FileName, const char* Name, bool flip);
	void UpdateAnimation(const char* AnimationName, int Frame);
	// ===================================================================
    // データ取得
    // ===================================================================
    const std::vector<MATERIAL>& GetMaterials() const {
        return m_Materials;
    }

    const std::vector<SUBSET>& GetSubsets() const {
        return m_Subsets;
    }

    const std::vector<std::string>& GetTextureNames() const {
        return m_TextureNames;
    }

    // テクスチャハンドル取得（ResourceManager対応版）
    const std::vector<ResourceHandle<Texture>>& GetTextureHandles() const {
        return m_TextureHandles;
    }

    // 互換性のためのテクスチャ取得（非推奨）
    std::vector<std::unique_ptr<Texture>> GetTextures();

	const std::vector<DirectX::SimpleMath::Matrix>& GetBoneMatrices() const {
		return m_Bonecombmtxcontainer;
	}
};
