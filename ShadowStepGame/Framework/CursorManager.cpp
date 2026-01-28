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

	Transform& transform = m_Cursol->GetTransform();
	Vector3 currentPos = transform.GetPosition();
	Vector3 newPos = currentPos;

	newPos.y -= IO_MANAGER.GetMouseDeltaY() * m_mouseSensitivity;
	newPos.x += IO_MANAGER.GetMouseDeltaX() * m_mouseSensitivity;

	// なめらかに移動
	newPos = DirectX::SimpleMath::Vector3::Lerp(currentPos, newPos, m_mouseSensitivity);
	transform.SetPosition(newPos);

	//std::cout << "Cursol Position : " << newPos.x << ", " << newPos.y << std::endl;
}

// ===================================================================
// 描画処理（メインループ）
// ===================================================================
void CursorManager::Draw()
{
	Vector3 pos = m_Cursol->GetTransform().GetPosition();

	// 範囲内のときのみ描画
	if((-1) * Application::GetHeight() / 2 < pos.y || Application::GetHeight()/2 < pos.y
		|| (-1) * Application::GetWidth() / 2 < pos.x || Application::GetWidth() / 2 < pos.x)
	{
		return;
	}

	if (m_Cursol && m_Cursol->IsActive())
	{
		m_Cursol->Draw(&m_Camera);
	}
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
