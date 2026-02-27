#include "SceneManager.h"
#include "Game.h"
#include "SoundManager.h"

#include <memory>
#include <iostream>

void SceneManager::Init()
{
	// シーンを設定
	m_scene = std::make_unique<Scene>();
	m_currentScene = m_scene->GetStartScene();

	// ローディング画面を初期化
	m_loadingScreen = std::make_unique<LoadingScreen>();
	m_loadingScreen->Init();

	// ローディング状態を設定
	m_isLoading = true;
	m_isInitializing = true;

	// 初期シーンの初期化を非同期で開始
	m_initThread = std::make_unique<std::thread>(&SceneManager::AsyncInitScene, this, m_currentScene);
}

void SceneManager::UnInit()
{
	// 初期化スレッドの終了を待機
	WaitForInitThread();

	// ローディング画面の終了処理
	if (m_loadingScreen)
	{
		m_loadingScreen->Uninit();
		m_loadingScreen.reset();
	}

	// 最終シーンの後処理
	if (m_scene->GetScene(m_currentScene)->IsInitialized())
	{
		m_scene->GetScene(m_currentScene)->UnInit();
	}
}

void SceneManager::Update()
{
	// 初期化中の場合はローディング画面のみ更新
	if (m_isInitializing.load())
	{
		// ローディング画面のアニメーション更新
		if (m_loadingScreen)
		{
			m_loadingScreen->Update(Game::GetDeltaTime());
		}

		// 初期化完了を確認
		if (m_scene->GetScene(m_currentScene)->IsInitialized())
		{
			// 追加で待機する　※今は使わない
			if (!m_isExtraWaiting)
			{
				// スレッドの終了を待機
				WaitForInitThread();

				// 延長待機を開始
				m_isExtraWaiting = true;
				m_loadingExtraTimer = 0.0f;	// 延長時間

				std::cout << "[SceneManager] Scene initialization completed. Extra loading wait started." << std::endl;
			}
			else
			{
				// 延長タイマーをカウントダウン
				m_loadingExtraTimer -= Game::GetDeltaTime();
				if (m_loadingExtraTimer <= 0.0f)
				{
					// 延長終了、ローディング終了
					m_isExtraWaiting = false;
					m_isInitializing = false;
					m_isLoading = false;

					std::cout << "[SceneManager] Extra loading wait finished." << std::endl;
				}
			}
		}
		return;
	}

	// 通常更新
	m_scene->GetScene(m_currentScene)->Update();
}

void SceneManager::Draw()
{
	// ローディング中はローディング画面を描画
	if (m_isLoading.load())
	{
		if (m_loadingScreen)
		{
			m_loadingScreen->Draw();
		}
		return;
	}

	// 視点を設定から描画まで		
	m_scene->GetScene(m_currentScene)->Draw();
}

// 各シーンからシーン変更通知を受け取る
void SceneManager::ChangeScene()
{
	// 初期化中はシーン変更を無視
	if (m_isInitializing.load())
	{
		return;
	}

	SCENE next = (SCENE)m_scene->GetScene(m_currentScene)->GetNextScene();
	if (next == SCENE_NONE) return;

	// 初期化スレッドの終了を待機（念のため）
	WaitForInitThread();

	// 旧シーンの終了処理
	m_scene->GetScene(m_currentScene)->UnInit();

	// 新シーンに切り替え
	m_currentScene = next;

	// ローディング状態に設定
	m_isLoading = true;
	m_isInitializing = true;

	// シーン初期化を非同期で開始
	m_initThread = std::make_unique<std::thread>(&SceneManager::AsyncInitScene, this, m_currentScene);
}

// ===================================================================
// 非同期初期化処理
// ===================================================================
void SceneManager::AsyncInitScene(SCENE scene)
{
	std::cout << "[SceneManager] Async initialization started for scene: " << static_cast<int>(scene) << std::endl;

	try
	{
		// シーンの初期化を実行
		m_scene->GetScene(scene)->Init();
	}
	catch (const std::exception& e)
	{
		std::cerr << "[SceneManager] Exception during async init: " << e.what() << std::endl;
	}

	std::cout << "[SceneManager] Async initialization finished" << std::endl;
}

// ===================================================================
// 初期化スレッドの終了待機
// ===================================================================
void SceneManager::WaitForInitThread()
{
	if (m_initThread && m_initThread->joinable())
	{
		m_initThread->join();
		m_initThread.reset();
	}
}
