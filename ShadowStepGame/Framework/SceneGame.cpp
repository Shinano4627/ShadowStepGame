// ===================================================================
// SceneGame.cpp
// ゲームシーン実装
// ===================================================================
#include "SceneGame.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "Game.h"

// コンポーネント
#include "PlayerMoverComponent.h"

#include <iostream>

void SceneGame::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneGame] Init START" << std::endl;

    // 既存オブジェクトを削除
    DeleteObjectList();

    // ゲーム時間初期化
    m_GameTime = 0.0f;

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_GAME).c_str());

    // 追加コンポーネント
    {
        // プレイヤー移動コンポーネント
        auto* mover = FindGameObjectWithTag("Player")->AddComponent<PlayerMoverComponent>(5.0f, 3.0f);

        std::cout << "[SceneGame] Player created" << std::endl;
    }

    // カメラ初期化
    m_Camera.Init();

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneGame] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== GAME SCENE ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D - Move Player" << std::endl;
    std::cout << "  Q/E     - Rotate Player" << std::endl;
    std::cout << "  ENTER   - Go to Result" << std::endl;
    std::cout << "  ESC     - Back to Title" << std::endl;
    std::cout << "" << std::endl;
}

void SceneGame::UnInit()
{
    std::cout << "[SceneGame] UnInit" << std::endl;
    DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    m_isInitialized = false;
}

void SceneGame::Update()
{
    // ゲーム時間更新
    m_GameTime += Game::GetDeltaTime();

    // カメラ更新
    m_Camera.Update();

    // Enterキーでリザルトへ
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneGame] ENTER pressed - Go to Result" << std::endl;
        m_nextScene = SCENE_RESULT;
        return;
    }

    // Escキーでタイトルへ戻る
    if (IO_MANAGER.GetKeyDownKeyBord(VK_ESCAPE))
    {
        std::cout << "[SceneGame] ESC pressed - Back to Title" << std::endl;
        m_nextScene = SCENE_TITLE;
        return;
    }

    // GameObjectリストを更新（プレイヤー移動など）
    UpdateObjectList();
}

void SceneGame::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    DrawLayer(nullptr, RenderLayer::UI);
}

void SceneGame::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}