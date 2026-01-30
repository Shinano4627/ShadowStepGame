//=======================================
// Game.cpp
// ゲームループ実装
//=======================================
#include "Game.h"
#include "Renderer.h"
#include "SceneManager.h"
#include "DataManager.h"
#include "ResourceManager.h"
#include "SoundManager.h"
#include "IOManager.h"
#include "PhysicsManager.h"
#include "CursorManager.h"

//=======================================
// グローバル変数
//=======================================
static float g_DeltaTime = 0.0f;
static float g_TotalTime = 0.0f;
static float g_CurrentFPS = 0.0f;

//=======================================
// コンストラクタ・デストラクタ
//=======================================

// コンストラクタ
Game::Game()
{

}

// デストラクタ
Game::~Game()
{

}

//=======================================
// ライフサイクル
//=======================================

// 初期化
void Game::Init()
{
	// 描画終了処理
	Renderer::Init();

	// マネージャークラス初期化処理
	SOUND_MANAGER.Init();
	//PHYSICS_MANAGER.Init();
	SCENE_MANAGER.Init();
	DATA_MANAGER.Init();
	M_RESOURCE.Init();
	IO_MANAGER.Init();
	CURSOR_MANAGER.Init();

	// タイマー初期化
	m_Timer.Reset();
}

// 更新
void Game::Update()
{
	// タイマー更新
	m_Timer.Update();
	
	// グローバル変数に保存
	g_DeltaTime = m_Timer.GetDeltaTime();
	g_TotalTime = m_Timer.GetTotalTime();
	g_CurrentFPS = m_Timer.GetFPS();

	// シーン変更
	SCENE_MANAGER.ChangeScene();

	// 入力更新
	IO_MANAGER.Update();

	// 物理演算・衝突判定
	//PHYSICS_MANAGER.Update();

	// マネージャークラス更新処理
	SCENE_MANAGER.Update();
}

// 描画
void Game::Draw()
{
	// 描画前処理
	Renderer::DrawStart();

	// シーン描画
	SCENE_MANAGER.Draw();

	// 描画後処理
	Renderer::DrawEnd();
}

// 終了処理
void Game::Uninit()
{
	// マネージャークラス終了処理
	IO_MANAGER.UnInit();
	M_RESOURCE.UnInit();
	DATA_MANAGER.UnInit();
	SCENE_MANAGER.UnInit();
	//PHYSICS_MANAGER.UnInit();
	CURSOR_MANAGER.UnInit();
	SOUND_MANAGER.UnInit();

	// 描画終了処理
	Renderer::Uninit();

	// シングルトンクラスの終了処理　（インスタンス削除）
	SingletonFinalizer::finalize();
}

// ===================================================================
// 静的関数の実装
// ===================================================================

// デルタタイム取得
float Game::GetDeltaTime()
{
	return g_DeltaTime;
}

// 総経過時間取得
float Game::GetTotalTime()
{
	return g_TotalTime;
}

// FPS取得
float Game::GetFPS()
{
	return g_CurrentFPS;
}