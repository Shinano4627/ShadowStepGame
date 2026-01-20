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
#include "TestTurnComponent.h"

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
        auto* mapSystem = FindGameObjectWithTag("MapSystem")->AddComponent<MapSystemComponent>("TestMap.csv");
        mapSystem->MakeMap(m_GameObjects);    // マップの読み込み

        // プレイヤー
        auto* playerObj = FindGameObjectWithTag("Player");
        auto* playerUnit = playerObj->AddComponent<UnitComponent>();
        playerUnit->SetCamp(UnitComponent::UnitCamp::UnitPlayer);

        //エネミー
        auto* enemyObj = FindGameObjectWithTag("Enemy");
        auto* enemyUnit = enemyObj->AddComponent<UnitComponent>();
        enemyUnit->SetCamp(UnitComponent::UnitCamp::UnitEnemy);

        //ターン管理
        auto* turnManagerObj = FindGameObjectWithTag("TurnManager");
        auto* turn = turnManagerObj->AddComponent<TestTurnComponent>();
        turn->AddUnit(playerUnit);
        turn->AddUnit(enemyUnit);
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
    // 1. カメラ更新
    m_Camera.Update();

    // 2. 全GameObject更新
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

