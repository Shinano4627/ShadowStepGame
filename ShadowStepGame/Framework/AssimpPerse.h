#pragma once
#include	<assimp/Importer.hpp>
#include	<assimp/scene.h>
#include	<assimp/postprocess.h>
#include	<assimp/cimport.h>
#include	<array>
#include	<SimpleMath.h>
#include	"Texture.h"

namespace AssimpPerse
{
	struct VERTEX {
		std::string meshname;		// メッシュ名
		aiVector3D	pos;			// 位置
		aiVector3D	normal;			// 法線
		aiColor4D	color;			// 頂点カラー	
		aiVector3D	texcoord;		// テクスチャ座標	
		int			materialindex;	// マテリアルインデックス
		std::string mtrlname;		// マテリアル名
		std::array<int, 4> boneIndex;
		std::array<float, 4> boneWeight;
		int		boneCount = 0;
	};

	struct SUBSET {
		std::string meshname;		// メッシュ名
		int materialindex;			// マテリアルインデックス
		unsigned int VertexBase;	// 頂点バッファのベース
		unsigned int VertexNum;		// 頂点数
		unsigned int IndexBase;		// インデックスバッファのベース
		unsigned int IndexNum;		// インデックス数
		std::string	 mtrlname;		// マテリアル名
	};

	struct MATERIAL {
		std::string mtrlname;		// マテリアル名
		aiColor4D	Ambient;		// アンビエント
		aiColor4D	Diffuse;		// ディフューズ
		aiColor4D	Specular;		// スペキュラ
		aiColor4D	Emission;		// エミッション
		float		Shininess;		// シャイネス
		std::string texturename;	// テクスチャ名
	};

	// ウェイト情報
	struct WEIGHT {
		std::string bonename;						// ボーン名
		std::string meshname;						// メッシュ名
		float weight;								// ウェイト値
		int	vertexindex;							// 頂点インデックス
	};

	//ボーン構造体
	struct BONE
	{
		std::string Bonename;						// ボーン名
		std::string Meshname;						// メッシュ名
		std::string Armaturename;					// アーマチュア名
		DirectX::SimpleMath::Matrix Matrix;
		DirectX::SimpleMath::Matrix AnimationMatrix;
		DirectX::SimpleMath::Matrix OffsetMatrix;
		int			idx;
		std::vector<WEIGHT> weights;				// このボーンが影響を与える頂点インデックス・ウェイト値
	};

	struct CBBoneCombMatrix {
		DirectX::XMFLOAT4X4 BoneCombMtx[400];						// ボーンコンビネーション行列
	};

	class FBXLoader
	{
	private:
		std::vector<std::vector<VERTEX>> m_Vertices{};		// 頂点データ（メッシュ単位）
		std::vector<std::vector<unsigned int>> m_Indices{};	// インデックスデータ（メッシュ単位）
		std::vector<SUBSET> m_Subsets{};					// サブセット情報
		std::vector<MATERIAL> m_Materials{};				// マテリアル
		std::vector<std::unique_ptr<Texture>> m_Textures;	// ディフューズテクスチャ群
		std::unordered_map<std::string, BONE> m_Bones;			// ボーンデータ（名前で参照）
		ID3D11Buffer* m_BoneCombMtxCBuffer = nullptr;						// 定数バッファ
		Assimp::Importer* m_Importer = nullptr;

	private:
		void GetMaterialData(const aiScene* pScene, std::string texturedirectory);
		void CreateBone(const aiNode* node);
		std::vector<BONE> GetBoneInfo(const aiMesh* mesh);
		DirectX::SimpleMath::Matrix aiMtxToDxMtx(const aiMatrix4x4& aimatrix);
	public:
		const aiScene* LoadModelData(std::string filename, std::string texturedirectory, bool flip = false, bool simple = false);	// モデルの読み込み
		const aiScene* LoadAnimation(const char* FileName, bool flip);

		std::vector<std::vector<VERTEX>> GetVertices() { return m_Vertices;};	 // 頂点データ（メッシュ単位）
		std::vector<std::vector<unsigned int>> GetIndices() { return m_Indices; };	// インデックスデータ（メッシュ単位）
		std::vector<MATERIAL> GetMaterials() { return m_Materials; };	// マテリアル
		std::vector<std::unique_ptr<Texture>> GetTextures() { return std::move(m_Textures); };	// ディフューズTxtureコンテナを返す
		std::unordered_map<std::string, BONE> GetBones() { return std::move(m_Bones); };
		std::vector<SUBSET> GetSubsets() { return m_Subsets; };
	};
}
