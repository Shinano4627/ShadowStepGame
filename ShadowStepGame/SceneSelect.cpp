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

    // Make ObjectList

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