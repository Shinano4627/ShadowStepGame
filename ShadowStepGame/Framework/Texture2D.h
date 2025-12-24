#pragma once
#include "MeshComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include <vector>

//=======================================
// Texture2Dクラス
//=======================================
class Texture2D : public MeshComponent
{
private:
	// 描画リソース
	VertexBuffer<VERTEX_3D> m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
	
	std::vector<VERTEX_3D> m_Vertices;
	std::vector<unsigned int> m_Indices;

	bool m_Initialized = false;

	// 描画の為の情報（見た目に関わる部分）
	ResourceHandle<Texture> m_TextureHandle;
	ResourceHandle<Shader> m_ShaderHandle;
	ResourceHandle<Material> m_MaterialHandle;

	// テクスチャデータパス
	std::string m_TexturePath;

	// UV座標の情報
	float m_NumU = 1;
	float m_NumV = 1;
	float m_SplitX = 1;
	float m_SplitY = 1;

public:
	//=======================================
	// コンストラクタ
	//=======================================
	Texture2D(
		const std::string& texturePath,
		const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1))
		: m_Initialized(false)
		, m_TexturePath(texturePath)
	{
		m_Color = color;	// 色を設定
		SetRenderLayer(RenderLayer::UI);	// UI層に設定
	}

	//=======================================
	// ライフサイクル
	//=======================================
	void Init() override;
	void Update() override;
	void Draw(Camera* cam) override;
	void Uninit() override;

	//=======================================
	// テクスチャ設定
	//=======================================
	void SetTexture(const std::string& texturePath);
	const std::string& GetTexturePath() const { return m_TexturePath; }

	//=======================================
	// UV座標設定
	//=======================================
	void SetUV(float nu, float nv, float sx, float sy);

	//=======================================
	// 色設定
	//=======================================
	void SetColor(const DirectX::SimpleMath::Color& color);

};

