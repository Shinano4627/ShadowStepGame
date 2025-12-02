// ===================================================================
// SceneDebug.h
// デバッグ用シーン - GameObjectとコンポーネントシステムを使用
// ===================================================================
#pragma once
#include "SceneBase.h"

class SceneDebug : public SceneBase
{
private:
#pragma region 変数
	// デバッグ用
	GameObject* m_activeObjectDebug = nullptr;
	int m_curScene = 0; // 選択中のシーン
	int m_preScene = 0; // 前回の選択シーン
#pragma endregion
#pragma region 関数
	// ===================================================================
	// デバッグ用関数 (Transformの調整)
	// ===================================================================
	// デバッグ用UIの内容
	void DebugUI();
	// 初期化
	void DebugUIInit();
	// 終了処理
	void DebugUIUnInit();
	// 描画
	void DebugUIRender();

	void ReLoadObject();
	// オブジェクトデータの変更をxmlファイルに保存
	void SaveData();
	// デバッグ画面から抜ける
	void ExitDebug();
#pragma endregion
public:
    void Init() override;
    void UnInit() override;
    void Update() override;
    void Draw() override;
    void Draw(Camera*) override;
};

