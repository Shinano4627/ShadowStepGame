// ===================================================================
// SceneResult.cpp
// リザルトシーン実装 - カメラ方向に合わせた移動
// ===================================================================
#include "SceneResult.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "ResourceManager.h"
#include "Game.h"
#include "SoundManager.h"

// コンポーネント
#include "CameraRelativeMoverComponent.h"      // カメラ相対移動用

#include <iostream>

void SceneResult::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneResult] Init START" << std::endl;

    if (m_GameObjectList == nullptr)
    {
        // リストクラスのインスタンス作成
        m_GameObjectList = std::make_unique<GameObjectList>();
    }
    // 既存オブジェクトを削除
    m_GameObjectList->DeleteObjectList();

    // 表示時間初期化
    m_DisplayTime = 0.0f;

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    m_GameObjectList->MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_RESULT).c_str());

    // 追加コンポーネント
    {
        auto* m_win = m_GameObjectList->FindGameObjectWithTag("WIN");
        m_win->SetActive(false);
        auto* m_loss = m_GameObjectList->FindGameObjectWithTag("LOSE");
        m_loss->SetActive(false);

        // Player勝利か
        if (SCENE_MANAGER.GetPlayerResult())
        {
            m_win->SetActive(true);
        }
        else {
            m_loss->SetActive(true);
        }
    }

    // カメラ初期化
    m_Camera.Init();

    // BGMの開始
    SOUND_MANAGER.PlayBGM(SOUND_LABEL::SOUND_LABEL_BGM_GAME);

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneResult] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== RESULT SCENE ===" << std::endl;
    std::cout << "OBJ Model Display with Camera-Relative Movement" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D      - Move (Camera-Relative)" << std::endl;
    std::cout << "  Q/E          - Rotate Model" << std::endl;
    std::cout << "  LEFT/RIGHT   - Rotate Camera" << std::endl;
    std::cout << "  UP/DOWN      - Zoom Camera" << std::endl;
    std::cout << "  ENTER        - Back to Title" << std::endl;
    std::cout << "  ESC          - Retry Game" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "NOTE: W moves toward camera direction!" << std::endl;
    std::cout << "" << std::endl;
}

void SceneResult::UnInit()
{
    std::cout << "[SceneResult] UnInit" << std::endl;
    m_GameObjectList->DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    // BGMの停止
    SOUND_MANAGER.Stop(SOUND_LABEL::SOUND_LABEL_BGM_GAME);

    m_isInitialized = false;
}

void SceneResult::Update()
{
    // 表示時間更新
    m_DisplayTime += Game::GetDeltaTime();

    // カメラ更新
    m_Camera.Update();

    // Enterキーでタイトルへ
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneResult] ENTER pressed - Back to Title" << std::endl;
        m_nextScene = SCENE_TITLE;
        return;
    }

    // GameObjectリストを更新（カメラ相対移動など）
    m_GameObjectList->UpdateObjectList();
}

void SceneResult::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画
    m_GameObjectList->DrawLayer(&m_Camera, RenderLayer::UI);
}

void SceneResult::Draw(Camera* camera)
{
    
    // WORLD層を描画（カメラ使用）
    m_GameObjectList->DrawLayer(camera, RenderLayer::WORLD);
}