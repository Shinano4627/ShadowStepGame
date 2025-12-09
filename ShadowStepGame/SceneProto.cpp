// ===================================================================
// SceneProto.cpp
// タイトルシーン実装
// ===================================================================
#include "SceneProto.h"
#include "SceneManager.h"
#include "IOManager.h"

// コンポーネント
#include "RotatorComponent.h"

#include <iostream>

void SceneProto::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneProto] Init START" << std::endl;

    // 既存オブジェクトを削除
    DeleteObjectList();

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_TITLE).c_str());

    // 追加コンポーネント
    {


    }

    // カメラ初期化
    m_Camera.Init();

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneProto] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== TITLE PROTO ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ENTER - Start Game" << std::endl;
    std::cout << "  SPACE - Go to TestCube" << std::endl;
    std::cout << "" << std::endl;
}

void SceneProto::UnInit()
{
    std::cout << "[SceneProto] UnInit" << std::endl;
    DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    m_isInitialized = false;
}

void SceneProto::Update()
{
    // カメラ更新
    m_Camera.Update();

    // Enterキーでゲーム開始
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneProto] ENTER pressed - Starting Game" << std::endl;
        m_nextScene = SCENE_GAME;
        return;
    }

    // GameObjectリストを更新
    UpdateObjectList();
}

void SceneProto::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    DrawLayer(nullptr, RenderLayer::UI);
}

void SceneProto::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}