// ===================================================================
// SceneProto.cpp
// Plese Write scene explanation
// ===================================================================
#include "SceneProto.h"
#include "SceneManager.h"
#include "IOManager.h"
#include <iostream>

// Components
#include "SimplePlaneRendererComponent.h"
#include "MapSystemComponent.h"
#include "SunManageComponent.h"
#include "GameSystemComponent.h"

using namespace DirectX::SimpleMath;


void SceneProto::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneProto] Init START" << std::endl;

    // Delete ObjectList
    DeleteObjectList();

    // Make ObjectList
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_PROTO).c_str());

    // 追加コンポーネント
    {
        // マップシステム
        auto* mapSystem = FindGameObjectWithTag("System")->AddComponent<MapSystemComponent>("TestMap.csv");
        mapSystem->MakeMap(m_GameObjects);    // マップの読み込み
        float heightMap = mapSystem->GetMapSizeHeight();
        float widthMap = mapSystem->GetMapSizeWidth();

        // 太陽
        auto* sun = FindGameObjectWithTag("System")->AddComponent<SunManageComponent>(
            widthMap, heightMap, mapSystem->GetMapHeight(), mapSystem->GetMapWidth());
        sun->Init();

        // ゲームシステム
        auto* gameSystem = FindGameObjectWithTag("System")->AddComponent<GameSystemComponent>();
        gameSystem->Init();

        // ここで SunManageComponent をセット
        // ※m_sunSystem は現状 private なので、public setter または friend でアクセス推奨
        // gameSystem->SetSunSystem(sun); // setter を作ると良い
    }  

    // Init Camera
    m_Camera.Init();
    Vector3 newPos = m_Camera.GetPosition();
    newPos.z = -100;
    m_Camera.SetPosition(newPos);

    // Init Data
    m_nextScene = SCENE_NONE;

    // Complete
    m_isInitialized = true;

    std::cout << "[SceneProto] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
}

void SceneProto::UnInit()
{
    std::cout << "[SceneProto] UnInit" << std::endl;
    DeleteObjectList();

    // UnInit Camera
    m_Camera.Uninit();

    // Complete
    m_isInitialized = false;
}

void SceneProto::Update()
{
    // Update Camera
    m_Camera.Update();

    // Update GameObjectList
    UpdateObjectList();

}

void SceneProto::Draw()
{
    // World
    Draw(&m_Camera);

    // Ui
    DrawLayer(&m_Camera, RenderLayer::UI);
}

void SceneProto::Draw(Camera* camera)
{
    DrawLayer(camera, RenderLayer::WORLD);
}

