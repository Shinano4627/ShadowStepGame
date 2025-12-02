#pragma once

#include "Scene.h"
#include "singleton.h"

class SceneManager
{
#pragma region 変数
private:
	SCENE m_currentScene;
	std::unique_ptr<Scene> m_scene;

public:
	// シーン共通で使用
	//int score;	
#pragma endregion

#pragma region 関数
private:

public:
	void Init();
	void UnInit();
	void Update();
	void Draw();
	void ChangeScene();

	std::string GetSceneName(SCENE _scene) { return m_scene->GetSceneName(_scene); }
	std::vector<std::string>& GetSceneNameAll() { return m_scene->GetSceneNameAll(); }
#pragma endregion
};

#define SCENE_MANAGER Singleton<SceneManager>::GetInstance()
