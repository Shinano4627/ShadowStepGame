// ***Manual changes prohibited***
#pragma once
#include "SceneBase.h"

// Scene
#include "SceneDebug.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneResult.h"
// Add Header

enum SCENE
{
	SCENE_DEBUG = -1,
	SCENE_TITLE,
	SCENE_GAME,
	SCENE_RESULT,
	SCENE_NUM,

	SCENE_NONE = 999,
};

class Scene
{
private:
	std::unordered_map<SCENE, std::unique_ptr<SceneBase>> m_sceneTable;
	std::vector<std::string> m_sceneNameList;
	SCENE m_startScene = SCENE_DEBUG;	// 最初に実行されるシーン

public:
	Scene()
	{
		m_sceneTable[SCENE_DEBUG] = std::make_unique<SceneDebug>();
		m_sceneTable[SCENE_TITLE] = std::make_unique<SceneTitle>();
		m_sceneTable[SCENE_GAME] = std::make_unique<SceneGame>();
		m_sceneTable[SCENE_RESULT] = std::make_unique<SceneResult>();
		// Add Table

		m_sceneNameList.resize(SCENE_NUM);
		m_sceneNameList[SCENE_TITLE] = "SceneTitle";
		m_sceneNameList[SCENE_GAME] = "SceneGame";
		m_sceneNameList[SCENE_RESULT] = "SceneResult";
		// Add List
	}
	~Scene()
	{
		m_sceneTable.clear();
		m_sceneNameList.clear();
	}

	// データ取得
	SCENE GetStartScene() { return m_startScene; }
	std::string GetSceneName(SCENE _scene) { return m_sceneNameList[_scene]; }
	std::vector<std::string>& GetSceneNameAll() { return m_sceneNameList; }

	SceneBase* GetScene(SCENE _scene) 
	{
#ifdef DEBUG
		assert(m_sceneTable[_scene]);
#endif // DEBUG
		return m_sceneTable[_scene].get();
	}
};