// ===================================================================
// CursorManager.h
// 画面内カーソル管理クラス（シングルトン）
// SceneCommon.xmlからカーソルデータを読み込み、シーン共通で使用する
// ===================================================================
#pragma once

#include "singleton.h"
#include "XmlRW.h"
#include "GameObject.h"
#include "Camera.h"
#include <string>

class CursorManager
{
	// ===================================================================
	// 変数
	// ===================================================================
private:
	ObjectData m_CursorData;		// カーソルオブジェクトデータ
	bool m_IsLoaded = false;		// データ読み込み済みフラグ
	std::unique_ptr<GameObject> m_Cursol;

	// カメラ
	Camera  m_Camera;

	// マウス感度
	float m_mouseSensitivity = 3.f;

	// ===================================================================
	// 関数
	// ===================================================================
public:
	// 初期化（SceneCommon.xmlからカーソルデータを読み込む）
	void Init();

	// 更新処理
	void Update();

	// 更新処理
	void Draw();

	// 終了処理
	void UnInit();

	// データ読み込み済みか
	bool IsLoaded() const { return m_IsLoaded; }

	// カーソルデータ取得
	const ObjectData& GetCursorData() const { return m_CursorData; }

	// テクスチャパス取得
	const std::string& GetTexturePath() const { return m_CursorData.texture; }

	// スケール取得
	float GetScaleX() const { return m_CursorData.scl[0]; }
	float GetScaleY() const { return m_CursorData.scl[1]; }

	// マウス感度
	float GetSensitivity() const { return m_mouseSensitivity; }
	void SetSensitivity(float mouseSensitivity) { m_mouseSensitivity = mouseSensitivity; }

	// ポイントを取得
	DirectX::SimpleMath::Vector2 GetPoint() const
	{
		// 左上をポイントとする
		DirectX::SimpleMath::Vector2 point;
		Transform& tarnsform = m_Cursol->GetTransform();
		point.x = tarnsform.GetPosition().x - tarnsform.GetScale().x / 2;
		point.y = tarnsform.GetPosition().y - tarnsform.GetScale().y / 2;
		return point;
	}

private:
	// SceneCommon.xmlからカーソルデータを検索して読み込む
	bool LoadCursorDataFromXml();
};

// 隠蔽インスタンス取得マクロ
#define CURSOR_MANAGER Singleton<CursorManager>::GetInstance()
