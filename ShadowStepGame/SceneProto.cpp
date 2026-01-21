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
#include "UnitComponent.h"
#include "GameSystemComponent.h"
#include "OrbitCameraComponent.h"
#include "SunManageComponent.h"
#include "ShadowSystemComponent.h"
#include "UnitSystemComponent.h"

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
        // sun->Init();

        // シャドウシステム
        auto* shadowSystem = FindGameObjectWithTag("System")->AddComponent<ShadowSystemComponent>();

        // ユニットシステム
        auto* unitSystem = FindGameObjectWithTag("System")->AddComponent<UnitSystemComponent>();

        // ゲームシステム
        auto* gameSystem = FindGameObjectWithTag("System")->AddComponent<GameSystemComponent>();
        gameSystem->Init();

        // カメラ
        auto* orbitCamera = FindGameObjectWithTag("System")->AddComponent<OrbitCameraComponent>(&m_Camera);
        orbitCamera->SetGameSystem(gameSystem);
        orbitCamera->SetRotationSpeed(0.02f);

        // ここで SunManageComponent をセット
        // ※m_sunSystem は現状 private なので、public setter または friend でアクセス推奨
        // gameSystem->SetSunSystem(sun); // setter を作ると良い






        // プレイヤー
        auto* playerObj = FindGameObjectWithTag("Player");
        auto* playerUnit = playerObj->AddComponent<UnitComponent>();
        // playerUnit->SetCamp(UnitComponent::UnitCamp::UnitPlayer);

        //エネミー
        auto* enemyObj = FindGameObjectWithTag("Enemy");
        auto* enemyUnit = enemyObj->AddComponent<UnitComponent>();
        // enemyUnit->SetCamp(UnitComponent::UnitCamp::UnitEnemy);


    }  

    // Init Camera
    m_Camera.Init();

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
    // ============================
    // オブジェクト更新
    // ============================
    UpdateObjectList();


    // カメラ更新
    m_Camera.Update();
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

