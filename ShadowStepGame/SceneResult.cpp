// ===================================================================
// SceneResult.cpp
// リザルトシーン実装 - カメラ方向に合わせた移動
// ===================================================================
#include "SceneResult.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "ResourceManager.h"
#include "Game.h"

// コンポーネント
#include "CameraRelativeMoverComponent.h"      // カメラ相対移動用

#include <iostream>

void SceneResult::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneResult] Init START" << std::endl;

    // 既存オブジェクトを削除
    DeleteObjectList();

    // 表示時間初期化
    m_DisplayTime = 0.0f;
    m_pCamera = nullptr; // カメラはDraw()で受け取る

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_RESULT).c_str());

    // 追加コンポーネント
    {
        // ===================================================================
        // カメラ相対移動コンポーネントを追加
        // ※ カメラはまだnullptrだが、Update()で使用する前にDraw()が呼ばれてセットされる
        // ===================================================================
        // プレイヤー移動コンポーネント
        auto* mover = FindGameObjectWithTag("ModelObject")->AddComponent<CameraRelativeMoverComponent>(
            nullptr,    // カメラは後でセット
            8.0f,       // 移動速度
            3.0f        // 回転速度
        );

        std::cout << "[SceneResult] ModelObject created with camera-relative movement" << std::endl;
    }

    // カメラ初期化
    m_Camera.Init();

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
    DeleteObjectList();
    m_pCamera = nullptr;

    // カメラ終了処理
    m_Camera.Uninit();

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

    // Escキーでゲームをリトライ
    if (IO_MANAGER.GetKeyDownKeyBord(VK_ESCAPE))
    {
        std::cout << "[SceneResult] ESC pressed - Retry Game" << std::endl;
        m_nextScene = SCENE_GAME;
        return;
    }

    // ===================================================================
    // CameraRelativeMoverComponentにカメラをセット
    // ===================================================================
    if (m_pCamera)
    {
        auto* modelObject = FindGameObjectWithTag("ModelObject");
        if (modelObject)
        {
            auto* mover = modelObject->GetComponent<CameraRelativeMoverComponent>();
            if (mover)
            {
                mover->SetCamera(m_pCamera);
            }
        }
    }

    // GameObjectリストを更新（カメラ相対移動など）
    UpdateObjectList();
}

void SceneResult::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    DrawLayer(nullptr, RenderLayer::UI);
}

void SceneResult::Draw(Camera* camera)
{
    // カメラを保存（Update()で使用）
    m_pCamera = camera;

    // ===================================================================
    // カメラ制御処理
    // ===================================================================
    if (camera)
    {
        // ModelObjectを検索
        auto* modelObject = FindGameObjectWithTag("ModelObject");
        if (modelObject)
        {
            using namespace DirectX::SimpleMath;

            // カメラの回転処理（矢印キー）
            static float cameraAngle = 0.0f;       // カメラの水平角度
            static float cameraDistance = 10.0f;   // カメラの距離
            static float cameraHeight = 5.0f;      // カメラの高さ

            float deltaTime = Game::GetDeltaTime();

            // 左右矢印キーでカメラ回転
            if (IO_MANAGER.GetKeyPressKeyBord(VK_LEFT))
            {
                cameraAngle -= 2.0f * deltaTime; // 左回転
            }
            if (IO_MANAGER.GetKeyPressKeyBord(VK_RIGHT))
            {
                cameraAngle += 2.0f * deltaTime; // 右回転
            }

            // 上下矢印キーでズーム
            if (IO_MANAGER.GetKeyPressKeyBord(VK_UP))
            {
                cameraDistance -= 8.0f * deltaTime; // 近づく
                if (cameraDistance < 3.0f) cameraDistance = 3.0f;
            }
            if (IO_MANAGER.GetKeyPressKeyBord(VK_DOWN))
            {
                cameraDistance += 8.0f * deltaTime; // 遠ざかる
                if (cameraDistance > 30.0f) cameraDistance = 30.0f;
            }

            // モデル位置取得
            Vector3 modelPosition = modelObject->GetTransform().GetPosition();

            // カメラ位置を計算（円周上を回転）
            float x = modelPosition.x + sin(cameraAngle) * cameraDistance;
            float z = modelPosition.z + cos(cameraAngle) * cameraDistance;
            float y = modelPosition.y + cameraHeight;

            Vector3 cameraPosition(x, y, z);

            // カメラ設定
            camera->SetPosition(cameraPosition);
            camera->SetTarget(modelPosition);
        }
    }

    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}