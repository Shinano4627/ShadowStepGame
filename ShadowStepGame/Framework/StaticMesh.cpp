// ===================================================================
// StaticMesh.cpp
// Assimpを使った静的メッシュクラスの実装（ResourceManager対応版）
// ===================================================================

#include "StaticMesh.h"
#include "AssimpPerse.h"
#include <iostream>

using namespace DirectX::SimpleMath;

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
	m_pScene = m_FBXLoader.LoadModelData(filename, textureDirectory, true);

	subsets = m_FBXLoader.GetSubsets();		// サブセット取得
	vertices = m_FBXLoader.GetVertices();		// 頂点データ（メッシュ単位）
	indices = m_FBXLoader.GetIndices();		// インデックスデータ（メッシュ単位）
	materials = m_FBXLoader.GetMaterials();	// マテリアル情報取得
    // ===================================================================
    // テクスチャをResourceManager経由で読み込み
    // ===================================================================
    auto assimpTextures = m_FBXLoader.GetTextures();
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
	m_Bones = m_FBXLoader.GetBones();			// ボーン情報取得	

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

			// ボーン情報を転送
			for (int i = 0; i < 4; i++)
			{
				vertex.boneIndex[i] = v.boneIndex[i];
				vertex.boneWeight[i] = v.boneWeight[i];
			}

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

const aiScene* StaticMesh::LoadAnimation(const aiScene* data, const char* FileName, const char* Name, bool flip)
{
	// すでに読み込み済みのデータがある場合
	if (data != nullptr)
	{
		m_Animations[Name] = data;
	}
	else
	{
		m_Animations[Name] = m_FBXLoader.LoadAnimation(FileName, flip);
	}	

	return m_Animations[Name];
}

// アニメーション更新
void StaticMesh::UpdateAnimation(const char* AnimationName, int Frame)
{
	if (m_Animations.count(AnimationName) == 0) return;
	if (!m_Animations[AnimationName]->HasAnimations()) return;

	Matrix rootMatrix;
	// SRTから行列を生成
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Quaternion rotation;
	rotation.x = 0.0f;
	rotation.y = 0.0f;
	rotation.z = 0.0f;
	rotation.w = 1.0f;

	Matrix scalemtx = Matrix::CreateScale(scale.x, scale.y, scale.z);
	Matrix rotmtx = Matrix::CreateFromQuaternion(rotation);
	Matrix transmtx = Matrix::CreateTranslation(position.x, position.y, position.z);

	rootMatrix = scalemtx * rotmtx * transmtx;

	// アニメーションデータからボーンマトリクス算出
	aiAnimation* animation = m_Animations[AnimationName]->mAnimations[0];

	for (unsigned int c = 0; c < animation->mNumChannels; c++)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[c];

		AssimpPerse::BONE* bone = &m_Bones[nodeAnim->mNodeName.C_Str()];
		int f;

		f = Frame % nodeAnim->mNumRotationKeys;				// 簡易実装
		aiQuaternion rot = nodeAnim->mRotationKeys[f].mValue;

		f = Frame % nodeAnim->mNumPositionKeys;				// 簡易実装
		aiVector3D pos = nodeAnim->mPositionKeys[f].mValue;

		// SRTから行列を生成
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
		Vector3 position = { pos.x, pos.y, pos.z };
		Quaternion rotation;
		rotation.x = rot.x;
		rotation.y = rot.y;
		rotation.z = rot.z;
		rotation.w = rot.w;

		Matrix scalemtx = Matrix::CreateScale(scale.x, scale.y, scale.z);
		Matrix rotmtx = Matrix::CreateFromQuaternion(rotation);
		Matrix transmtx = Matrix::CreateTranslation(position.x, position.y, position.z);

		bone->AnimationMatrix = scalemtx * rotmtx * transmtx;
	}

	UpdateBoneMatrix(m_pScene->mRootNode, rootMatrix);

	// 定数バッファに書き込むための情報を生成
	m_Bonecombmtxcontainer.clear();
	m_Bonecombmtxcontainer.resize(m_Bones.size());
	for (auto data : m_Bones) {
		// GPUに送る前に転置
		//m_Bonecombmtxcontainer[data.second.idx] = data.second.Matrix.Transpose();
		m_Bonecombmtxcontainer[data.second.idx] = data.second.Matrix;
	}
}

void StaticMesh::UpdateBoneMatrix(const aiNode* node, const Matrix& matrix)
{
	if (node->mName.length <= 0)
	{
		//std::cout << "ノード(ボーン)名が取得できていないorない" << std::endl;
		return;
	}
	// 引数で渡されたノード名をキーとしてボーン情報を取得する
	AssimpPerse::BONE* bone = &m_Bones[node->mName.C_Str()];

	// マトリクスの乗算順番に注意
	// ボーンコンビネーション行列生成				
	Matrix bonecombinationmtx = bone->OffsetMatrix * bone->AnimationMatrix * matrix;	// ボーンオフセット行列×ボーンアニメメーション行列×逆ボーンオフセット行列

	bone->Matrix = bonecombinationmtx;			// ボーンコンビネーション行列をボーン情報に反映させる

	// 自分の姿勢を表す行列を作成
	Matrix mybonemtx = bone->AnimationMatrix * matrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		UpdateBoneMatrix(node->mChildren[n], mybonemtx);
	}
}

