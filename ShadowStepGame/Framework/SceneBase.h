// ===================================================================
// SceneBase.h シーン基底クラス(GameObjectの管理機能付き)
// ===================================================================

#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

#include "SystemCommon.h"
#include "Camera.h"
#include "GameObject.h"
#include "XmlRW.h"

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
	bool m_isInitialized = false;
	bool m_isActive = false;
	int m_nextScene = 999;
	int m_lastID = 0;

	// カメラ
	Camera  m_Camera;
	
	// GameObject リスト
	std::vector<std::unique_ptr<GameObject>> m_GameObjects;
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
	bool IsInitialized() { return m_isInitialized; }	// 初期化が完了しているか
	bool IsActive() { return m_isActive; }				// 実行されるシーンか
	int GetNextScene() { return m_nextScene;	 }	// 次のシーン

protected:
	// ===================================================================
	// GameObject管理
	// ===================================================================
	// xmlファイルからオブジェクトのデータを取得してリストを作成する
	void MakeObjectList(const char* _stage);
	// xmlファイルに更新データを保存する
	void SaveObjectData(const char* _stage);
	// GameObjectリスト削除
	void DeleteObjectList();
	// GameObjectリスト更新
	void UpdateObjectList();
	// GameObjectリスト描画（旧版・互換性のため残す）
	void DrawObjectList(Camera* camera);
	// GameObject指定レイヤーのみ描画
	void DrawLayer(Camera* camera, RenderLayer layer);
	// GameObject全レイヤーを順番に描画
	void DrawAllLayers(Camera* camera);

	// ===================================================================
	// GameObject　タグ検索
	// ===================================================================
	// GameObjectをタグで検索（最初の一つのみ）
	GameObject* FindGameObjectWithTag(const std::string& tag);
	// GameObjectをタグで検索（すべて）
	std::vector<GameObject*> FindGameObjectsWithTag(const std::string& tag);
private:

#pragma endregion
};

