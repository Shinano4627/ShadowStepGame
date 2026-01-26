// ===================================================================
// SceneGame.cpp
// ゲームシーン実装
// ===================================================================
#include "SceneGame.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "Game.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "SimplePlaneRendererComponent.h"
#include "CursorManager.h"

using namespace std;


// コンポーネント
#include "PlayerMoverComponent.h"
#include "Texture2D.h"

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
        // ボタン整列処理
        MakeUIButtons();

        // タイムラインの表示
        MakeUITimeLine();

        // ステータスの表示
        MakeUIStatus();

        // 太陽のルート表示
        MakeUISunRoute();

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

    // ゲーム用カーソルアップデート
    CURSOR_MANAGER.Update();

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
    DrawLayer(nullptr, RenderLayer::UI_2);

    // カーソルを最前面に描画
    CURSOR_MANAGER.Draw();
}

void SceneGame::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}

// ===================================================================
// ボタン整列処理
// ===================================================================
void SceneGame::MakeUIButtons()
{
    // UIAreaButtonsの範囲を取得
    GameObject* buttonArea = FindGameObjectWithName("UIAreaButtons");

    if (!buttonArea)
    {
        std::cout << "[SceneGame] UIAreaButtons not found!" << std::endl;
        return;
    }

    Vector3 areaPos = buttonArea->GetTransform().GetPosition();
    Vector3 areaScale = buttonArea->GetTransform().GetScale();

    // パディング（仮値）
    const float padding = 5.f;

    // 行数: ButtonSub(1行) + Button(2行) = 3行
    const int rowCount = 3;
    float rowHeight = (areaScale.y - padding * (rowCount + 1)) / rowCount;

    // 各行のY座標を計算（上から順に）
    // エリアの上端からスタート
    float topY = areaPos.y + areaScale.y / 2.f;

    // ---------------------------------------------------------------
    // 1行目: ButtonSub（横並び）
    // ---------------------------------------------------------------
    std::vector<GameObject*> buttonSubs = FindGameObjectsWithTag("ButtonSub");
    int subCount = static_cast<int>(buttonSubs.size());

    if (subCount > 0)
    {
        float row1Y = topY - padding - rowHeight / 2.f;
        float subWidth = (areaScale.x - padding * (subCount + 1)) / subCount;
        float startX = areaPos.x - areaScale.x / 2.f + padding + subWidth / 2.f;

        for (int i = 0; i < subCount; i++)
        {
            float posX = startX + i * (subWidth + padding);
            buttonSubs[i]->GetTransform().SetPosition(Vector3(posX, row1Y, 0.f));
            buttonSubs[i]->GetTransform().SetScale(Vector3(subWidth, rowHeight, 1.f));
            // UVを設定
            auto* tex = buttonSubs[i]->GetMeshComponent<Texture2D>();
            tex->SetUV(1, 1, 2, 1);
        }
    }

    // ---------------------------------------------------------------
    // 2行目・3行目: Button（ButtonBuild, ButtonMove）
    // ---------------------------------------------------------------
    // 表示するボタン名のリスト
    std::vector<std::string> buttonNames = { "ButtonBuild", "ButtonMove" };

    // TagがButtonのオブジェクトから該当するものを取得
    std::vector<GameObject*> buttons = FindGameObjectsWithTag("Button");
    std::vector<GameObject*> targetButtons;

    for (const auto& name : buttonNames)
    {
        for (auto* btn : buttons)
        {
            if (btn->GetName() == name)
            {
                targetButtons.push_back(btn);
                break;
            }
        }
    }

    // 不要なButtonは非アクティブに
    for (auto* btn : buttons)
    {
        bool isTarget = false;
        for (auto* target : targetButtons)
        {
            if (btn == target)
            {
                isTarget = true;
                break;
            }
        }
        if (!isTarget)
        {
            btn->SetActive(false);
        }
    }

    // ボタンを配置（2行目、3行目）
    float buttonWidth = areaScale.x - padding * 2;
    for (int i = 0; i < static_cast<int>(targetButtons.size()); i++)
    {
        // 行インデックス（1行目はButtonSubなので、i+1）
        int rowIndex = i + 1;
        float rowY = topY - padding - rowHeight / 2.f - rowIndex * (rowHeight + padding);
        float posX = areaPos.x;

        targetButtons[i]->GetTransform().SetPosition(Vector3(posX, rowY, 0.f));
        targetButtons[i]->GetTransform().SetScale(Vector3(buttonWidth, rowHeight, 1.f));

        // UVを設定
        auto* tex = targetButtons[i]->GetMeshComponent<Texture2D>();
        tex->SetUV(1, 1, 2, 1);
    }

    std::cout << "[SceneGame] Buttons arranged in UIAreaButtons" << std::endl;
}

void SceneGame::MakeUITimeLine()
{
    // UIAreaTimelineを名前で取得
    GameObject* timelineArea = FindGameObjectWithName("UIAreaTimeline");
    if (!timelineArea) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // WindowTemplateを非表示にする
    windowTemplate->SetActive(false);

    // エリアの位置・スケールを取得
    Vector3 areaPos = timelineArea->GetTransform().GetPosition();
    Vector3 areaScale = timelineArea->GetTransform().GetScale();

    // タイムラインウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    windowObj->SetID(m_lastID++);
    windowObj->SetName("WindowTimeline");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    m_GameObjects.push_back(std::move(windowObj));
    std::cout << "[SceneGame] Timeline window created" << std::endl;
}

void SceneGame::MakeUIStatus()
{
    // UIAreaStatusを名前で取得
    GameObject* statusArea = FindGameObjectWithName("UIAreaStatus");
    if (!statusArea) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // エリアの位置・スケールを取得
    Vector3 areaPos = statusArea->GetTransform().GetPosition();
    Vector3 areaScale = statusArea->GetTransform().GetScale();

    // ステータスウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    windowObj->SetID(m_lastID++);
    windowObj->SetName("WindowStatus");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    m_GameObjects.push_back(std::move(windowObj));
    std::cout << "[SceneGame] Status window created" << std::endl;

    // ---------------------------------------------------------------
    // レイアウト計算
    // ---------------------------------------------------------------
    const float padding = 5.f;
    float halfWidth = areaScale.x / 2.f;
    float leftCenterX = areaPos.x - halfWidth / 2.f;   // 左側の中心X
    float rightCenterX = areaPos.x + halfWidth / 2.f;  // 右側の中心X
    float rowHeight = areaScale.y / 3.f;               // 左側3段分割
    float topY = areaPos.y + areaScale.y / 2.f;        // エリア上端

    // ---------------------------------------------------------------
    // 右側: UIStatusImage（縦横比維持）
    // ---------------------------------------------------------------
    {
        std::vector<GameObject*> statusImages = FindGameObjectsWithTag("UIStatusImage");
        if (!statusImages.empty())
        {
            for (size_t i = 0; i < statusImages.size(); i++)
            {
                GameObject* statusImage = statusImages[i];

                // レイヤーセット
                auto* tex = statusImage->GetMeshComponent<Texture2D>();
                tex->SetRenderLayer(RenderLayer::UI_2);

                // 元の縦横比を取得
                Vector3 originalScale = statusImage->GetTransform().GetScale();
                float aspectRatio = originalScale.x / originalScale.y;

                // 右側半分に収まるサイズを計算（縦横比維持）
                float availableWidth = halfWidth - padding * 2.f;
                float availableHeight = areaScale.y - padding * 2.f;

                float finalWidth, finalHeight;
                if (availableWidth / aspectRatio <= availableHeight)
                {
                    // 幅基準
                    finalWidth = availableWidth;
                    finalHeight = availableWidth / aspectRatio;
                }
                else
                {
                    // 高さ基準
                    finalHeight = availableHeight;
                    finalWidth = availableHeight * aspectRatio;
                }

                statusImage->GetTransform().SetPosition(Vector3(rightCenterX, areaPos.y, 0.f));
                statusImage->GetTransform().SetScale(Vector3(finalWidth, finalHeight, 1.f));

                // 一旦すべて非表示
                statusImages[i]->SetActive(false);
            }

            // 仮で１つ目を表示
            statusImages[0]->SetActive(true);
        }
    }
    // ---------------------------------------------------------------
    // 左側2段目: UIStatusJob（1:1、左寄せ）
    // ---------------------------------------------------------------
    {
        std::vector<GameObject*> statusJobs = FindGameObjectsWithTag("UIStatusJob");
        if (!statusJobs.empty())
        {
            // 1:1の大きさ（行の高さより小さめ）
            float jobSize = (rowHeight - padding * 2.f) * 0.8;

            // 2段目のY座標（上から2番目）
            float row2Y = topY - rowHeight * 1.5f;

            // 左寄せ（左側エリアの左端からpadding分離す）
            float leftEdgeX = areaPos.x - halfWidth + padding + jobSize / 2.f;

            for (size_t i = 0; i < statusJobs.size(); i++)
            {
                GameObject* statusJob = statusJobs[i];

                // レイヤーセット
                auto* tex = statusJob->GetMeshComponent<Texture2D>();
                tex->SetRenderLayer(RenderLayer::UI_2);

                statusJob->GetTransform().SetPosition(Vector3(leftEdgeX, row2Y, 0.f));
                statusJob->GetTransform().SetScale(Vector3(jobSize, jobSize, 1.f));

                // 一旦すべて非表示
                statusJobs[i]->SetActive(false);
            }

            // 仮で１つ目を表示
            statusJobs[0]->SetActive(true);
        }
    }
    // ---------------------------------------------------------------
    // 左側3段目: UIStatusSpeed x3（中央整列）
    // ---------------------------------------------------------------
    GameObject* speedTemplate = FindGameObjectWithTag("UIStatusSpeed");
    if (speedTemplate)
    {
        // テンプレートを非表示
        speedTemplate->SetActive(false);
        auto* speedTex = speedTemplate->GetMeshComponent<Texture2D>();
        if (speedTex)
        {
            // 3段目のY座標（上から3番目）
            float row3Y = topY - rowHeight * 2.5f;

            // アイコンサイズ（行の高さに合わせる）
            const int speedCount = 3;
            float iconSize = rowHeight - padding * 2.f;
            float totalWidth = iconSize * speedCount + padding * (speedCount + 1);  // 左右もパディングする

            // 中央整列の開始X座標(左をパディング)
            float startX = leftCenterX - totalWidth / 2.f + iconSize / 2.f + padding;

            for (int i = 0; i < speedCount; i++)
            {
                float posX = startX + i * (iconSize + padding);

                auto speedObj = std::make_unique<GameObject>(
                    Vector3(posX, row3Y, 0.f),
                    Vector3(0, 0, 0),
                    Vector3(iconSize, iconSize, 1.f));
                speedObj->SetID(m_lastID++);
                speedObj->SetName("UIStatusSpeed" + std::to_string(i + 1));
                speedObj->SetTag("UIStatusSpeedIcon");
                auto* tex = speedObj->AddMeshComponent<Texture2D>(speedTex->GetTexturePath(), Color(1, 1, 1, 1));
                // レイヤーセット
                tex->SetRenderLayer(RenderLayer::UI_2);

                m_GameObjects.push_back(std::move(speedObj));
            }
        }
    }

    std::cout << "[SceneGame] Status UI elements arranged" << std::endl;
}

void SceneGame::MakeUISunRoute()
{
    // UIAreaStatusを名前で取得
    GameObject* Area = FindGameObjectWithName("UIAreaSunRoute");
    if (!Area) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // エリアの位置・スケールを取得
    Vector3 areaPos = Area->GetTransform().GetPosition();
    Vector3 areaScale = Area->GetTransform().GetScale();

    // ステータスウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    windowObj->SetID(m_lastID++);
    windowObj->SetName("WindowSunRoute");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    m_GameObjects.push_back(std::move(windowObj));
    std::cout << "[SceneGame] SunRoute window created" << std::endl;
}
