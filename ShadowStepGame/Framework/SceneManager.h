#pragma once

#include "../Scene.h"
#include "singleton.h"
#include "LoadingScreen.h"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

class SceneManager
{
#pragma region 変数
private:
	SCENE m_currentScene;
	std::unique_ptr<Scene> m_scene;

	// ローディング画面
	std::unique_ptr<LoadingScreen> m_loadingScreen;
	std::atomic<bool> m_isLoading = false;

	// マルチスレッド用
	std::unique_ptr<std::thread> m_initThread;		// 初期化スレッド
	std::atomic<bool> m_isInitializing = false;		// 初期化中フラグ
	std::mutex m_sceneMutex;						// シーンアクセス用ミューテックス

public:
	// シーン共通で使用
	//int score;	
#pragma endregion

#pragma region 関数
private:
	// 非同期初期化処理
	void AsyncInitScene(SCENE scene);
	// 初期化スレッドの終了待機
	void WaitForInitThread();

public:
	void Init();
	void UnInit();
	void Update();
	void Draw();
	void ChangeScene();

	// ローディング中かどうか
	bool IsLoading() const { return m_isLoading.load(); }

	std::string GetSceneName(SCENE _scene) { return m_scene->GetSceneName(_scene); }
	std::vector<std::string>& GetSceneNameAll() { return m_scene->GetSceneNameAll(); }
#pragma endregion
};

#define SCENE_MANAGER Singleton<SceneManager>::GetInstance()
