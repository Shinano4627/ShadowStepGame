#pragma once
#include "GameObject.h"

class GameObjectList
{
#pragma region 変数
protected:
	int m_lastID = 0;

	// GameObject リスト
	std::vector<std::unique_ptr<GameObject>> m_GameObjects;
private:

#pragma endregion

#pragma region 関数
public:
	// ===================================================================
	// GameObject管理
	// ===================================================================
	void AddObject(std::unique_ptr<GameObject> object)
	{
		m_GameObjects.push_back(std::move(object));
		m_lastID++;
	};
	// xmlファイルからオブジェクトのデータを取得してリストを作成する
	void MakeObjectList(const char* _stage);
	// xmlファイルに更新データを保存する
	void SaveObjectData(const char* _stage);
	// GameObjectリスト削除
	void DeleteObjectList();
	// GameObjectリスト更新
	void UpdateObjectList();
	// GameObjectリスト描画（互換・互換性のため残す）
	void DrawObjectList(Camera* camera);
	// GameObject指定レイヤーのみ描画
	void DrawLayer(Camera* camera, RenderLayer layer);
	// GameObject全レイヤーを順番に描画
	void DrawAllLayers(Camera* camera);
	// 削除
	template <typename Predicate>
	void RemoveIf(Predicate predicate)
	{
		m_GameObjects.erase(
			std::remove_if(
				m_GameObjects.begin(),
				m_GameObjects.end(),
				[&](const std::unique_ptr<GameObject>& obj)
				{
					return predicate(*obj);
				}),
			m_GameObjects.end());
		m_GameObjects.shrink_to_fit();
	}

	// ===================================================================
	// GameObject　検索
	// ===================================================================
	// GameObjectを名前で検索（最初の一つのみ）
	GameObject* FindGameObjectWithName(const std::string& name);
	// GameObjectをタグで検索（最初の一つのみ）
	GameObject* FindGameObjectWithTag(const std::string& tag);
	// GameObjectをタグで検索（すべて）
	std::vector<GameObject*> FindGameObjectsWithTag(const std::string& tag);
	// インデックスで指定して取得
	GameObject* FindGameObjectWithIndex(const int& idx) { return m_GameObjects[idx].get(); };
	// 全データ取得（参照用）
	const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return m_GameObjects; };

	int GetLastID(){ return m_lastID; };
private:
#pragma endregion
};

