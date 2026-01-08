#include	<vector>
#include	<iostream>
#include	<unordered_map>
#include	<cassert>
#include	"Texture.h"
#include	"AssimpPerse.h"

#pragma comment(lib, "assimp-vc143-mtd.lib")

namespace AssimpPerse
{
	using namespace DirectX::SimpleMath;

	// マテリアル情報をassimpを使用して取得する
	void FBXLoader::GetMaterialData(const aiScene* pScene, std::string texturedirectory)
	{
		// マテリアル数分テクスチャ格納エリアを用意する
		m_Textures.resize(pScene->mNumMaterials);

		// マテリアル数文ループ
		for (unsigned int m = 0; m < pScene->mNumMaterials; m++)
		{
			aiMaterial* material = pScene->mMaterials[m];

			// マテリアル名取得
			std::string mtrlname = std::string(material->GetName().C_Str());
			std::cout << mtrlname << std::endl;

			// マテリアル情報
			aiColor4D ambient;
			aiColor4D diffuse;
			aiColor4D specular;
			aiColor4D emission;
			float shininess;

			// アンビエント
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient)) {
			}
			else {
				ambient = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
			}

			// ディフューズ
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
			}
			else {
				diffuse = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
			}

			// スペキュラ
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular)) {
			}
			else {
				specular = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
			}

			// エミッション
			if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emission)) {
			}
			else {
				emission = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
			}

			// シャイネス
			if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess)) {
			}
			else {
				shininess = 0.0f;
			}

			// このマテリアルに紐づいているディフューズテクスチャ数分ループ
			std::vector<std::string> texpaths{};

			for (unsigned int t = 0; t < material->GetTextureCount(aiTextureType_DIFFUSE); t++)
			{
				aiString path{};

				// t番目のテクスチャパス取得
				if (AI_SUCCESS == material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, t), path))
				{
					// テクスチャパス取得
					std::string texpath = std::string(path.C_Str());
					std::cout << texpath << std::endl;

					// テクスチャパスに「:」が含まれていれば絶対パスなのでパスを加工する
					if (texpath.find(':') != std::string::npos) {
						// スラッシュまたはバックスラッシュが最後に現れる位置を探す
						size_t pos = texpath.find_last_of("/\\");
						if (pos != std::string::npos)
						{
							// 最後のスラッシュまたはバックスラッシュの次から文字列を返す
							texpath = texpath.substr(pos + 1);
						}
					}
					texpaths.push_back(texpath);

					// 内蔵テクスチャかどうかを判断する
					if (auto tex = pScene->GetEmbeddedTexture(path.C_Str())) {

						std::unique_ptr<Texture> texture = std::make_unique<Texture>();

						// 内蔵テクスチャの場合
						bool sts = texture->LoadFromFemory(
							(unsigned char*)tex->pcData,			// 先頭アドレス
							tex->mWidth);			// テクスチャサイズ（メモリにある場合幅がサイズ）	
						if (sts) {
							m_Textures[m] = std::move(texture);
						}
						std::cout << "Embedded" << std::endl;

					}
					else {
						// 外部テクスチャファイルの場合
						std::unique_ptr<Texture> texture;
						texture = std::make_unique<Texture>();

						std::string texname = texturedirectory + "/" + texpath;

						bool sts = texture->Load(texname);
						if (sts) {
							m_Textures[m] = std::move(texture);
						}

						std::cout << "other Embedded" << std::endl;
					}
				}
				// ディフューズテクスチャがなかった場合
				else
				{
					// 外部テクスチャファイルの場合
					std::unique_ptr<Texture> texture;
					texture = std::make_unique<Texture>();
					m_Textures[m] = std::move(texture);
				}
			}

			// マテリアル情報を保存
			AssimpPerse::MATERIAL mtrl{};
			mtrl.mtrlname = mtrlname;
			mtrl.Ambient = ambient;
			mtrl.Diffuse = diffuse;
			mtrl.Specular = specular;
			mtrl.Emission = emission;
			mtrl.Shininess = shininess;
			if (texpaths.size() == 0)
			{
				mtrl.texturename = "";
			}
			else
			{
				mtrl.texturename = texpaths[0];
			}
			m_Materials.push_back(mtrl);
		}
	}

	const aiScene* FBXLoader::LoadModelData(std::string filename, std::string texturedirectory, bool flip, bool simpleMode)
	{
		//データを一度クリア
		m_Vertices.clear();		// 頂点データ
		m_Indices.clear();		// インデックスデータ
		m_Subsets.clear();		// サブセット情報
		m_Materials.clear();	// マテリアル
		m_Textures.clear(); 	// ディフューズテクスチャ群
		m_Bones.clear();		// ボーンデータ

		if (m_Importer != nullptr) {
			delete m_Importer;
		}
		m_Importer = new Assimp::Importer();

		int flag = 0;
		if (simpleMode)
		{
			flag |= aiProcess_Triangulate;					// 非三角ポリゴンを三角に割る
			flag |= aiProcess_JoinIdenticalVertices;		// 同一位置頂点を一つに統合する
			flag |= aiProcess_FlipUVs;						//　UV値をY軸を基準に反転させる
			flag |= aiProcess_PreTransformVertices;			// ノードを一つに統合 !!アニメーション情報が消えることに注意!!
			if (flip) flag |= aiProcess_MakeLeftHanded;		// 左手系座標に変換
		}
		else
		{
			//		flag |= aiProcessPreset_TargetRealtime_MaxQuality;	// リアルタイム レンダリング用にデータを最適化するデフォルトの後処理構成。
			flag |= aiProcess_Triangulate;
			flag |= aiProcess_PopulateArmatureData;				// 標準的なボーン,アーマチュアの設定
			if (flip) flag |= aiProcess_ConvertToLeftHanded;	// 左手系変更オプションがまとまったもの
		}

		// シーン情報を構築
		const aiScene* g_pScene = m_Importer->ReadFile(filename.c_str(), flag);

		if (g_pScene == nullptr)
		{
			std::cout << "load error" << filename.c_str() << m_Importer->GetErrorString() << std::endl;
			assert(g_pScene != nullptr);
			return nullptr;
		}

		// ボーン情報配列準備
		CreateBone(g_pScene->mRootNode);

		// ボーンの配列インデックスを格納する
		unsigned int num = 0;
		for (auto& data : m_Bones) {
			data.second.idx = num;
			num++;
		}

		// マテリアル情報取得
		GetMaterialData(g_pScene, texturedirectory);

		// メッシュ数文ループ（マテリアル毎にメッシュを分割するように指定している）
		m_Vertices.resize(g_pScene->mNumMeshes);

		for (unsigned int m = 0; m < g_pScene->mNumMeshes; m++)
		{
			aiMesh* mesh = g_pScene->mMeshes[m];

			// メッシュ名取得
			std::string meshname = std::string(mesh->mName.C_Str());

			//　頂点数分ループ
			for (unsigned int vidx = 0; vidx < mesh->mNumVertices; vidx++)
			{
				// 頂点データ
				VERTEX	v{};
				v.meshname = meshname;		// メッシュ名セット

				// 座標		
				v.pos = mesh->mVertices[vidx];

				// この頂点が使用しているマテリアルのインデックス番号（メッシュ内の）
				// を使用してマテリアル名をセット
				v.materialindex = mesh->mMaterialIndex;

				v.mtrlname = m_Materials[mesh->mMaterialIndex].mtrlname;

				// 法線あり？
				if (mesh->HasNormals()) {
					v.normal = mesh->mNormals[vidx];
				}
				else
				{
					v.normal = aiVector3D(0.0f, 0.0f, 0.0f);
				}

				// 頂点カラー？（０番目）
				if (mesh->HasVertexColors(0)) {
					v.color = mesh->mColors[0][vidx];
				}
				else
				{
					v.color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
				}

				// テクスチャあり？（０番目）
				if (mesh->HasTextureCoords(0)) {
					v.texcoord = mesh->mTextureCoords[0][vidx];
				}
				else
				{
					v.texcoord = aiVector3D(0.0f, 0.0f, 0.0f);
				}

				v.boneIndex = { -1, -1, -1, -1 };
				v.boneWeight = { 0.0f, 0.0f, 0.0f, 0.0f };
				v.boneCount = 0;					

				// 頂点データを追加
				m_Vertices[m].push_back(v);
			}

			// ボーン情報取得
			std::vector<BONE> boneList = GetBoneInfo(mesh);

			// 頂点情報にボーン情報紐づけ
			for (auto& bone : boneList)
			{
				for (auto& w : bone.weights)
				{
					// デバッグ出力
					if (w.vertexindex >= m_Vertices[m].size()) {
						//std::cout << "範囲外" << std::endl;
						continue;
					}

					int& idx = m_Vertices[m][w.vertexindex].boneCount;

					if (idx < 0 || idx >= 4) {
						//std::cout << "不正なboneCount" << std::endl;
						continue;
					}

					m_Vertices[m][w.vertexindex].boneIndex[idx] = m_Bones[w.bonename].idx;		// indexをセット
					m_Vertices[m][w.vertexindex].boneWeight[idx] = w.weight;					// weight値をセット
					idx++;
					assert(idx <= 4);
				}
			}
		}

		// メッシュ数文ループ
		// インデックスデータ作成
		m_Indices.resize(g_pScene->mNumMeshes);
		for (unsigned int m = 0; m < g_pScene->mNumMeshes; m++)
		{
			aiMesh* mesh = g_pScene->mMeshes[m];

			// メッシュ名取得
			std::string meshname = std::string(mesh->mName.C_Str());

			// インデックス数分ループ
			for (unsigned int fidx = 0; fidx < mesh->mNumFaces; fidx++)
			{
				aiFace face = mesh->mFaces[fidx];

				assert(face.mNumIndices == 3);	// 三角形のみ対応

				// インデックスデータを追加
				for (unsigned int i = 0; i < face.mNumIndices; i++)
				{
					m_Indices[m].push_back(face.mIndices[i]);
				}
			}
		}

		// サブセット情報を生成
		m_Subsets.resize(g_pScene->mNumMeshes);
		for (unsigned int m = 0; m < m_Subsets.size(); m++)
		{
			m_Subsets[m].IndexNum = (unsigned int)m_Indices[m].size();
			m_Subsets[m].VertexNum = (unsigned int)m_Vertices[m].size();
			m_Subsets[m].VertexBase = 0;
			m_Subsets[m].IndexBase = 0;
			m_Subsets[m].meshname = m_Vertices[m][0].meshname;
			m_Subsets[m].mtrlname = m_Vertices[m][0].mtrlname;
			m_Subsets[m].materialindex = m_Vertices[m][0].materialindex;
		}

		// サブセット情報を相対的なものにする	
		for (int m = 0; m < m_Subsets.size(); m++)
		{
			// 頂点バッファのベースを計算
			m_Subsets[m].VertexBase = 0;
			for (int i = m - 1; i >= 0; i--) {
				m_Subsets[m].VertexBase += m_Subsets[i].VertexNum;
			}

			// インデックスバッファのベースを計算
			m_Subsets[m].IndexBase = 0;
			for (int i = m - 1; i >= 0; i--) {
				m_Subsets[m].IndexBase += m_Subsets[i].IndexNum;
			}
		}

		return g_pScene;
	}

	const aiScene* FBXLoader::LoadAnimation(const char* FileName, bool flip)
	{
		int flag = 0;
		if (flip) flag |= aiProcess_ConvertToLeftHanded;	// 左手系変更オプションがまとまったもの
		const aiScene* ret = aiImportFile(FileName, flag);
		assert(ret);

		return ret;
	}

	// 再帰的ボーン生成
	void FBXLoader::CreateBone(const aiNode* node)
	{
		BONE bone;

		m_Bones[node->mName.C_Str()] = bone;

		for (unsigned int n = 0; n < node->mNumChildren; n++)
		{
			CreateBone(node->mChildren[n]);
		}
	}

	// サブセットに紐づいているボーン情報を取得する
	std::vector<BONE> FBXLoader::GetBoneInfo(const aiMesh* mesh) {

		std::vector<BONE> bones;		// このサブセットメッシュで使用されているボーンコンテナ

		// ボーン数分ループ
		for (unsigned int bidx = 0; bidx < mesh->mNumBones; bidx++) {

			BONE bone{};

			// ボーン名取得
			bone.Bonename = std::string(mesh->mBones[bidx]->mName.C_Str());
			// メッシュノード名
			if (mesh->mBones[bidx]->mNode != nullptr)
			{
				bone.Meshname = std::string(mesh->mBones[bidx]->mNode->mName.C_Str());
			}
			else {
				std::cout << "ノード情報がNULL" << std::endl;
			}

			// アーマチュアノード名
			if (mesh->mBones[bidx]->mArmature != nullptr)
			{
				bone.Armaturename = std::string(mesh->mBones[bidx]->mArmature->mName.C_Str());
			}
			else {
				std::cout << "アーマチュア情報がNULL" << std::endl;
			}
			// ボーンオフセット行列取得
			bone.OffsetMatrix = aiMtxToDxMtx(mesh->mBones[bidx]->mOffsetMatrix);

			// ウェイト情報抽出
			bone.weights.clear();
			for (unsigned int widx = 0; widx < mesh->mBones[bidx]->mNumWeights; widx++) {

				WEIGHT w;
				w.meshname = bone.Meshname;										// メッシュ名
				w.bonename = bone.Bonename;										// ボーン名

				w.weight = mesh->mBones[bidx]->mWeights[widx].mWeight;			// 重み
				w.vertexindex = mesh->mBones[bidx]->mWeights[widx].mVertexId;	// 頂点インデックス
				bone.weights.emplace_back(w);
			}

			// コンテナに登録
			bones.emplace_back(bone);

			// ボーン辞書にも反映させる
			m_Bones[mesh->mBones[bidx]->mName.C_Str()].OffsetMatrix = aiMtxToDxMtx(mesh->mBones[bidx]->mOffsetMatrix);   // 20231231 DX化

		}

		return bones;
	}

	DirectX::SimpleMath::Matrix FBXLoader::aiMtxToDxMtx(const aiMatrix4x4& aimatrix) {

		DirectX::SimpleMath::Matrix dxmtx = {
		   aimatrix.a1,aimatrix.b1,aimatrix.c1,aimatrix.d1,
		   aimatrix.a2,aimatrix.b2,aimatrix.c2,aimatrix.d2,
		   aimatrix.a3,aimatrix.b3,aimatrix.c3,aimatrix.d3,
		   aimatrix.a4,aimatrix.b4,aimatrix.c4,aimatrix.d4
		};

		return dxmtx;
	}
}
