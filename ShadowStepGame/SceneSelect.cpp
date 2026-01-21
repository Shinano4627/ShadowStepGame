// ===================================================================
// SceneSelect.cpp
// Plese Write scene explanation
// ===================================================================
#include "SceneSelect.h"
#include "SceneManager.h"
#include "IOManager.h"
#include <iostream>

// Components

using namespace DirectX::SimpleMath;

void SceneSelect::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneSelect] Init START" << std::endl;

    // Delete ObjectList
    DeleteObjectList();

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_SELECT).c_str());

    // 追加コンポーネント
    {
        std::cout << "[SceneSelect] TitleCube created" << std::endl;
    }

    // Init Camera
    m_Camera.Init();

    // Init Data
    m_nextScene = SCENE_NONE;

    // Complete
    m_isInitialized = true;

    std::cout << "[SceneSelect] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
}

void SceneSelect::UnInit()
{
    std::cout << "[SceneSelect] UnInit" << std::endl;
    DeleteObjectList();

    // UnInit Camera
    m_Camera.Uninit();

    // Complete
    m_isInitialized = false;
}

void SceneSelect::Update()
{
    // Update Camera
    m_Camera.Update();

    // Enterキーで次のシーン（仮実装）
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneSelect] ENTER pressed - Starting Game" << std::endl;
        m_nextScene = SCENE_GAME;
        return;
    }

    // Update GameObjectList
    UpdateObjectList();
}

void SceneSelect::Draw()
{
    // World
    Draw(&m_Camera);

    // Ui
    DrawLayer(&m_Camera, RenderLayer::UI);
}

void SceneSelect::Draw(Camera* camera)
{
    DrawLayer(camera, RenderLayer::WORLD);
}