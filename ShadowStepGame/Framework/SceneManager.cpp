#include "SceneManager.h"

#include <memory>

void SceneManager::Init()
{
	// シーンを設定
	m_scene = std::make_unique<Scene>();
	m_currentScene = m_scene->GetStartScene();

	// 初期シーンの初期化処理
	m_scene->GetScene(m_currentScene)->Init();
}

void SceneManager::UnInit()
{
	// 最終シーンの後処理
	if (m_scene->GetScene(m_currentScene)->IsInitialized())
	{
		m_scene->GetScene(m_currentScene)->UnInit();
	}
}

void SceneManager::Update()
{
	// 初期化処理がまだの場合実行
	if (!m_scene->GetScene(m_currentScene)->IsInitialized())
	{
		m_scene->GetScene(m_currentScene)->Init();
	}

	// 毎ターン実行される処理を記載
	m_scene->GetScene(m_currentScene)->Update();
}

void SceneManager::Draw()
{
	// 頂点情報設定から描画まで		
	m_scene->GetScene(m_currentScene)->Draw();
}

// 各シーンからシーン変更通知を受け取った
void SceneManager::ChangeScene()
{
	SCENE next = (SCENE)m_scene->GetScene(m_currentScene)->GetNextScene();
	if (next == SCENE_NONE) return;

	// 現シーンの終了処理
	m_scene->GetScene(m_currentScene)->UnInit();

	// 次シーンに切り替え
	m_currentScene = next;

	// シーン開始
	m_scene->GetScene(m_currentScene)->Init();

}

