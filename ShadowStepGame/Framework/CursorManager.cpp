// ===================================================================
// CursorManager.cpp
// 画面内カーソル管理クラス実装
// ===================================================================
#include "CursorManager.h"
#include "Texture2D.h"
#include "IOManager.h"
#include "Application.h"
#include <iostream>

// ===================================================================
// 初期化
// ===================================================================
void CursorManager::Init()
{
	// 既に読み込み済みなら何もしない
	if (m_IsLoaded)
	{
		return;
	}

	// SceneCommon.xmlからカーソルデータを読み込む
	if (LoadCursorDataFromXml())
	{
		m_IsLoaded = true;
		std::cout << "[CursorManager] カーソルデータ読み込み完了" << std::endl;
		std::cout << "  テクスチャ: " << m_CursorData.texture << std::endl;
		std::cout << "  スケール: " << m_CursorData.scl[0] << " x " << m_CursorData.scl[1] << std::endl;
	}
	else
	{
		std::cerr << "[CursorManager] カーソルデータの読み込みに失敗" << std::endl;
	}

	m_Camera.Init();
}

// ===================================================================
// 終了処理
// ===================================================================
void CursorManager::UnInit()
{
	m_Camera.Uninit();
	m_IsLoaded = false;
	std::cout << "[CursorManager] 終了処理完了" << std::endl;
}

// ===================================================================
// 更新処理（メインループ）
// ===================================================================
void CursorManager::Update()
{
	m_Camera.Update();

	// マウスの絶対座標をUI座標に変換してカーソル位置を更新
	// MODE_ABSOLUTE: スクリーン座標(左上原点, Y下向き)を
	// UI座標(中央原点, Y上向き)に変換
	auto mouseState = IO_MANAGER.GetMouseState();

	float uiX = static_cast<float>(mouseState.x) - Application::GetWidth() / 2.0f;
	float uiY = Application::GetHeight() / 2.0f - static_cast<float>(mouseState.y);

	Transform& transform = m_Cursol->GetTransform();
	transform.SetPosition(Vector3(uiX, uiY, 0.0f));

	// Added by Yamanaka: ウィンドウ内外でゲームカーソルの表示を切り替え
	bool inWindow = (mouseState.x >= 0 && mouseState.x < (int)Application::GetWidth() && mouseState.y >= 0 && mouseState.y < (int)Application::GetHeight());
	m_Cursol->SetActive(inWindow);

	//std::cout << "Cursol Position : " << newPos.x << ", " << newPos.y << std::endl;
}

// ===================================================================
// 描画処理（メインループ）
// ===================================================================
void CursorManager::Draw()
{
	Vector3 pos = m_Cursol->GetTransform().GetPosition();

	// カーソルがウィンドウ外にある場合は描画しない
	float halfW = static_cast<float>(Application::GetWidth()) / 2.0f;
	float halfH = static_cast<float>(Application::GetHeight()) / 2.0f;

	if (pos.x < -halfW || pos.x > halfW ||
		pos.y < -halfH || pos.y > halfH)
	{
		return;
	}

	m_Cursol->Draw(&m_Camera);
}
// ===================================================================
// SceneCommon.xmlからカーソルデータを検索して読み込む
// ===================================================================
bool CursorManager::LoadCursorDataFromXml()
{
	XmlRW xml;
	std::vector<ObjectData> objects;

	// SceneCommon.xmlを読み込む
	int ret = xml.GetObjectData("SceneCommon", objects);
	if (ret != 0)
	{
		std::cerr << "[CursorManager] SceneCommon.xmlの読み込みに失敗" << std::endl;
		return false;
	}

	// タグが"Cursol"のオブジェクトを検索
	for (const ObjectData& obj : objects)
	{
		if (obj.objectTag == "Cursol")
		{
			// トランスフォームデータを渡す
			m_Cursol = std::make_unique<GameObject>(Vector3(obj.pos), Vector3(obj.rot), Vector3(obj.scl));
			m_Cursol->SetID(2000);
			m_Cursol->SetName(obj.objectName);
			m_Cursol->SetTag(obj.objectTag);
			auto* renderer = m_Cursol->AddMeshComponent<Texture2D>(
				obj.texture,
				Vector4(obj.color));

			return true;
		}
	}

	std::cerr << "[CursorManager] カーソルオブジェクトが見つかりません" << std::endl;
	return false;
}
