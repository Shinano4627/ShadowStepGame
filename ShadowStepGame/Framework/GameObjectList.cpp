// ===================================================================
// GameObjectをリスト管理
// ===================================================================
#include "GameObjectList.h"
#include "ResourceManager.h"
#include "XmlRW.h"

// コンポーネント
#include "MeshComponentWrapper.h"

// ===================================================================
// xmlファイルからオブジェクトのデータを取得してリストを作成する
// ===================================================================
void GameObjectList::MakeObjectList(const char* _stage)
{
	XmlRW xml;
	std::vector<ObjectData> objects;

	int ret = xml.GetObjectData(_stage, objects);
	assert(ret != -1);	// エラーチェック

	int n = 0;
	//　全データをコンポーネントデータに変換してリストに格納
	for (ObjectData object : objects)
	{
		// トランスフォームデータを渡す
		auto obj = std::make_unique<GameObject>(Vector3(object.pos), Vector3(object.rot), Vector3(object.scl));
		GameObject* newObject = obj.get();
		newObject->SetID(n);
		newObject->SetName(object.objectName);
		newObject->SetTag(object.objectTag);

		if (typeTags.contains(object.objectType))
		{
			auto tag = typeTags[object.objectType];

			// キューブ描画コンポーネント（WORLD層）
			if (tag == &tag_cube)
			{
				newObject->AddMeshComponent<SimpleCubeRendererComponent>(
					Vector4(object.color),
					object.texture);
			}
			// 平面描画コンポーネント（WORLD層）
			else if (tag == &tag_plane)
			{
				newObject->AddMeshComponent<SimplePlaneRendererComponent>(
					Vector4(object.color),
					object.texture);
			}
			// モデルコンポーネント（WORLD層）
			else if (tag == &tag_model)
			{
				MeshRendererComponent* mesh = newObject->AddMeshComponent<MeshRendererComponent>(object.fileName, object.texture);
				newObject->AddMeshComponent<MeshRendererComponent>(object.fileName, object.texture);
				std::cout << "[" << _stage << "] " << "Attempting to load model : " << object.fileName << std::endl;
				mesh->LoadModel(object.animations);
				std::cout << "[" << _stage << "] " << "OBJ Model loaded successfully!" << std::endl;
			}
			else if (tag == &tag_2D)
			{
				auto* renderer = newObject->AddMeshComponent<Texture2D>(
					object.texture,
					Vector4(object.color));
			}
		}

		m_GameObjects.push_back(std::move(obj));

		// 作成完了メッセージ
		std::cout << "[" << _stage << "] " << object.objectName << " created" << std::endl;

		n++;
	}

	m_lastID = n;
}

// ===================================================================
// GameObjectリスト削除
// ===================================================================
void GameObjectList::SaveObjectData(const char* _stage)
{
	XmlRW xml;
	std::vector<ObjectData> objects;

	// 全データをObjectDataに変換してリストに格納
	for (auto& gameObject : m_GameObjects)
	{
		ObjectData object;

		object.id = gameObject->GetID();
		object.objectName = gameObject->GetName();
		object.fileName = "";
		object.texture = "";

		Color color = Color(Vector4(1, 1, 1, 1));

		// タイプはコンポーネントで確認
		if (auto component = gameObject->GetMeshComponent<SimpleCubeRendererComponent>())
		{
			object.objectType = strSimpleCube;
			color = component->GetColor();
			object.texture = component->GetTexturePath();  // テクスチャパス取得
		}
		else if (auto component = gameObject->GetMeshComponent<SimplePlaneRendererComponent>())
		{
			object.objectType = strSimplePlane;
			color = component->GetColor();
			object.texture = component->GetTexturePath();  // テクスチャパス取得
		}
		else if (auto component = gameObject->GetMeshComponent<MeshRendererComponent>())
		{
			object.objectType = strModel;
			object.fileName = component->GetModelPath();     // モデルデータ
			object.texture = component->GetTexturePath();    // テクスチャデータ
		}
		else if (auto component = gameObject->GetMeshComponent<Texture2D>())
		{
			object.objectType = str2D;
			object.texture = component->GetTexturePath();    // テクスチャデータ
			color = component->GetColor();
		}
		else
		{
			object.objectType = "";
		}

		object.objectTag = gameObject->GetTag();

		gameObject->GetTransform().ConvertData(object.pos, object.rot, object.scl);

		object.color[0] = color.x;
		object.color[1] = color.y;
		object.color[2] = color.z;
		object.color[3] = color.w;

		objects.push_back(object);
	}

	xml.UpdateObjectsInXML(objects, _stage);

	// 完了メッセージ
	std::cout << "[" << _stage << "] saved complete" << std::endl;
}

// ===================================================================
// GameObjectリスト削除
// ===================================================================
void GameObjectList::DeleteObjectList()
{
	// すべてのGameObjectの終了処理を呼び出し
	for (auto& obj : m_GameObjects)
	{
		if (obj)
		{
			obj->Uninit();
		}
	}
	// リストをクリア
	m_GameObjects.clear();
}

// ===================================================================
// GameObjectリスト更新
// ===================================================================
void GameObjectList::UpdateObjectList()
{
	// 全ての有効なGameObjectのUpdate()を呼び出し
	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->IsActive())
		{
			obj->Update();
		}
	}
}

// ===================================================================
// GameObjectリスト描画
// ===================================================================
void GameObjectList::DrawObjectList(Camera* camera)
{
	// 全ての有効なGameObjectのDraw()を呼び出し
	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->IsActive())
		{
			obj->Draw(camera);
		}
	}
}

// ===================================================================
// 指定レイヤーのみ描画
// ===================================================================
void GameObjectList::DrawLayer(Camera* camera, RenderLayer layer)
{
	// 全ての有効なGameObjectの指定レイヤーを描画
	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->IsActive())
		{
			obj->DrawLayer(camera, layer);
		}
	}
}

// ===================================================================
// 全レイヤーを順番に描画
// ===================================================================
void GameObjectList::DrawAllLayers(Camera* camera)
{
	// レイヤーを順番に描画
	DrawLayer(camera, RenderLayer::WORLD);   // 3D空間（カメラ使用）
	DrawLayer(camera, RenderLayer::UI);     // UI（カメラ不使用）
}

// ===================================================================
// GameObjectを名前で検索（最初の一つのみ）
// ===================================================================
GameObject* GameObjectList::FindGameObjectWithName(const std::string& name)
{
	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->GetName() == name)
		{
			return obj.get();
		}
	}
	return nullptr;
}

// ===================================================================
// GameObjectをタグで検索（最初の一つのみ）
// ===================================================================
GameObject* GameObjectList::FindGameObjectWithTag(const std::string& tag)
{
	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->GetTag() == tag)
		{
			return obj.get();
		}
	}
	return nullptr;
}

// ===================================================================
// GameObjectをタグで検索（すべて）
// ===================================================================
std::vector<GameObject*> GameObjectList::FindGameObjectsWithTag(const std::string& tag)
{
	std::vector<GameObject*> result;

	for (auto& obj : m_GameObjects)
	{
		if (obj && obj->GetTag() == tag)
		{
			result.push_back(obj.get());
		}
	}
	return result;
}
