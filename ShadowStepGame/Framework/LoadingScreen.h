// ===================================================================
// LoadingScreen.h ローディング画面クラス
// ===================================================================
#pragma once

#include "SystemCommon.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Application.h"
#include "ResourceManager.h"
#include <memory>

class LoadingScreen
{
private:
	// ===================================================================
	// 背景用
	// ===================================================================
	ResourceHandle<Texture> m_BackgroundTexture;
	std::shared_ptr<Shader> m_Shader;
	std::unique_ptr<Material> m_Material;
	VertexBuffer<VERTEX_3D> m_BGVertexBuffer;
	IndexBuffer m_BGIndexBuffer;
	std::vector<VERTEX_3D> m_BGVertices;
	std::vector<unsigned int> m_BGIndices;

	// ===================================================================
	// アニメーション用
	// ===================================================================
	ResourceHandle<Texture> m_AnimTexture;
	VertexBuffer<VERTEX_3D> m_AnimVertexBuffer;
	IndexBuffer m_AnimIndexBuffer;
	std::vector<VERTEX_3D> m_AnimVertices;
	std::vector<unsigned int> m_AnimIndices;

	// アニメーション制御
	int m_CurrentFrame = 0;			// 現在のフレーム
	int m_TotalFrames = 4;			// 総フレーム数（4×1スプライトシート）
	float m_FrameTime = 0.0f;		// フレーム経過時間
	float m_FrameDuration = 0.2f;	// 1フレームあたりの秒数

	// アニメーション位置・サイズ
	float m_AnimPosX = 300.0f;		// アニメーション表示位置X
	float m_AnimPosY = -200.0f;		// アニメーション表示位置Y
	float m_AnimWidth = 650.0f;		// アニメーション幅
	float m_AnimHeight = 500.0f;	// アニメーション高さ

	bool m_Initialized = false;

public:
	LoadingScreen();
	~LoadingScreen();

	void Init();
	void Update(float deltaTime);
	void Draw();
	void Uninit();

	bool IsInitialized() const { return m_Initialized; }

private:
	// 背景の頂点設定
	void SetupBackgroundVertices();
	// アニメーションの頂点設定
	void SetupAnimationVertices();
	// UV座標の更新
	void UpdateAnimationUV();
};
