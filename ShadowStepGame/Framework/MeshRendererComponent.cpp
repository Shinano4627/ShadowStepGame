// ===================================================================
// MeshRendererComponent.cpp
// GameObjectに描画機能を追加するコンポーネント
// ===================================================================
#include "MeshRendererComponent.h"
#include "ResourceManager.h"
#include "SimpleCubeRendererComponent.h"
#include "XmlRW.h"

// ===================================================================
// 初期化
// ===================================================================
void MeshRendererComponent::Init()
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
// 描画処理
// ===================================================================
void MeshRendererComponent::Draw(Camera* camera)
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

	// UV行列を設定
	Renderer::SetUV(0, 0, 1, 1); // GPUにセット

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
		if (subset.MaterialIdx < m_Materials.size())
		{
			m_Materials[subset.MaterialIdx]->SetGPU();
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

void MeshRendererComponent::LoadModel(const std::vector<AnimationData>& animations)
{
	bool modelLoaded = false;

	// ===================================================================
	// OBJモデルの読み込みを試みる
	// ===================================================================
	try
	{
		// MeshRendererComponent作成
		auto* meshRenderer = m_pOwner->GetMeshComponent<MeshRendererComponent>();

		// ResourceManager経由でメッシュ読み込み
		auto meshHandle = M_RESOURCE.LoadMesh(m_ModelPath, m_TexturePath);

		// アニメーションの読み込み
		for (auto anim : animations)
		{
			M_RESOURCE.LoadAnimation(meshHandle.GetShared(), anim.path.c_str(), anim.name.c_str(), true);
		}

		if (meshHandle)
		{
			// shared_ptrとして取得してセット
			meshRenderer->SetMesh(meshHandle.GetShared());

			// シェーダー設定
			ResourceHandle<Shader> shaderHandle;
			if (animations.size() != 0)
			{
				shaderHandle = M_RESOURCE.LoadShader("shader/litTextureAnimationVS.hlsl", "shader/litTexturePS.hlsl");
			}
			else
			{
				shaderHandle = M_RESOURCE.LoadShader("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");
			}

			if (shaderHandle)
			{
				meshRenderer->SetShader(shaderHandle.GetShared());
			}

			modelLoaded = true;
		}
		else
		{
#ifdef _DEBUG	
			//assert(mesh);	// デバッグ時のみメッシュ読み込み失敗にエラーを出す
#endif	// _DEBUG
			std::cerr << "[SceneResult] Model file not found or failed to load" << std::endl;
			//m_pOwner->RemoveMeshComponent<MeshRendererComponent>();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception loading model: " << e.what() << std::endl;
		// MeshRendererComponentを削除（もし追加されていたら）
		//if (modelObject->GetMeshComponent<MeshRendererComponent>())
		//{
		//	modelObject->RemoveMeshComponent<MeshRendererComponent>();
		//}
	}

	// ===================================================================
	// フォールバック：OBJ読み込み失敗時はSimpleCubeを使用
	// ===================================================================
	if (!modelLoaded)
	{
		std::cout << "[SceneResult] Using fallback: SimpleCubeRenderer" << std::endl;

		//auto* cubeRenderer = modelObject->AddMeshComponent<SimpleCubeRendererComponent>(Color(1, 1, 0, 1)); // 黄色
		//cubeRenderer->SetRenderLayer(RenderLayer::WORLD);
	}
}

