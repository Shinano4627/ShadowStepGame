// ===================================================================
// SceneBase.h シーン基底クラス(GameObjectの管理機能付き)
// ===================================================================

#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>
#include <atomic>

#include "SystemCommon.h"
#include "Camera.h"
#include "GameObjectList.h"

enum SceneReturnCode
{
	SCENE_CONTINUE,
	SCENE_CHANGE,
	SCENE_EXIT,
	SCENE_ERROR,
};

class SceneBase
{
#pragma region 変数
protected:
	std::atomic<bool> m_isInitialized = false;	// スレッドセーフな初期化フラグ
	bool m_isActive = false;
	int m_nextScene = 999;
	int m_lastID = 0;

	// カメラ
	Camera  m_Camera;
	Camera  m_UiCamera;
	
	// オブジェクトリスト
	std::unique_ptr<GameObjectList> m_GameObjectList = nullptr;

private:

#pragma endregion

#pragma region 関数
public:
	// ===================================================================
	// ライフサイクル	(派生クラスで実装必須！)
	// ===================================================================
	virtual void Init() = 0;			// シーン切り替え時に実行されます
	virtual void UnInit() = 0;		// シーン切り替え時に実行されます
	virtual void Update() = 0;	// 毎フレーム実行されます
	virtual void Draw() = 0;		// 毎フレーム実行されます
	virtual void Draw(Camera*) = 0;	// 毎フレーム実行されます。3D用

	// ===================================================================
	// 状態取得
	// ===================================================================
	bool IsInitialized() { return m_isInitialized.load(); }	// 初期化が完了しているか
	bool IsActive() { return m_isActive; }				// 実行中のシーンか
	int GetNextScene() { return m_nextScene;	 }	// 次のシーン
private:

#pragma endregion
};

