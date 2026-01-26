// ===================================================================
// SceneTitle.cpp
// タイトルシーン実装
// ===================================================================
#include "SceneTitle.h"
#include "SceneManager.h"
#include "IOManager.h"

// コンポーネント

#include <iostream>

void SceneTitle::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneTitle] Init START" << std::endl;

    if (m_GameObjectList == nullptr)
    {
        // リストクラスのインスタンス作成
        m_GameObjectList = std::make_unique<GameObjectList>();
    }
    // 既存オブジェクトを削除
    m_GameObjectList->DeleteObjectList();

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    m_GameObjectList->MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_TITLE).c_str());

    // 追加コンポーネント
    {
        std::cout << "[SceneTitle] TitleCube created" << std::endl;
    }

    // カメラ初期化
    m_Camera.Init();

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneTitle] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== TITLE SCENE ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ENTER - Start Game" << std::endl;
    std::cout << "  SPACE - Go to TestCube" << std::endl;
    std::cout << "" << std::endl;
}

void SceneTitle::UnInit()
{
    std::cout << "[SceneTitle] UnInit" << std::endl;
    m_GameObjectList->DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    m_isInitialized = false;
}

void SceneTitle::Update()
{
    // カメラ更新
    m_Camera.Update();

    // Enterキーでゲーム開始
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneTitle] ENTER pressed - Starting Game" << std::endl;
        m_nextScene = SCENE_SELECT;
        return;
    }

    // GameObjectリストを更新
    m_GameObjectList->UpdateObjectList();
}

void SceneTitle::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    m_GameObjectList->DrawLayer(&m_Camera, RenderLayer::UI);
}

void SceneTitle::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    m_GameObjectList->DrawLayer(camera, RenderLayer::WORLD);
}